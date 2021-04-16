/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************

----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              Cypress Semiconductor Corp, 198 Champion Ct., San Jose, CA 95134, USA
Licensed SEGGER software: emFile
License number:           FS-00227
License model:            Cypress Services and License Agreement, signed November 17th/18th, 2010
                          and Amendment Number One, signed December 28th, 2020 and February 10th, 2021
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5, PSoC6)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2010-12-01 - 2022-07-27
Contact to extend SUA:    sales@segger.com
-------------------------- END-OF-HEADER -----------------------------

File        : FS_NOR_HW_SPIFI.c
Purpose     : low-level flash driver for quad SPI.
*/

#include "cyhal_qspi.h"
#include "cyhal_system.h"
#include "FS_OS.h"
#include "FS_NOR_HW_SPIFI.h"

#if defined(COMPONENT_RTOS_AWARE)
#include "cyabs_rtos.h"
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

/*********************************************************************
*
*      Defines, configurable
*
**********************************************************************
*/
#ifndef FS_NOR_HW_QSPI_RW_TIMEOUT_MS
#define FS_NOR_HW_QSPI_RW_TIMEOUT_MS    (500u) /* in milliseconds. */
#endif

/*********************************************************************
*
*       Defines, non-configurable
*
**********************************************************************
*/
#define BUS_WIDTH_ONE               (1u)
#define BUS_WIDTH_TWO               (2u)
#define BUS_WIDTH_FOUR              (4u)

#define NUM_BITS_PER_BYTE           (8u)

#if defined(COMPONENT_RTOS_AWARE)
#define QSPI_SEMA_MAX_COUNT         (1lu)
#define QSPI_SEMA_INIT_COUNT        (0lu)
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static FS_NOR_HW_SPIFI_Config_t *_config;
static bool _qspi_init_done = false;
static uint8_t active_hw_unit = 0;

#if defined(COMPONENT_RTOS_AWARE)
/* Semaphore used while waiting for the QSPI event to occur. */
static cy_semaphore_t qspi_sema;
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

/*********************************************************************
*
*      Static code
*
**********************************************************************
*/
#if defined(COMPONENT_RTOS_AWARE)
static void _qspi_event_callback(void *callback_arg, cyhal_qspi_event_t event)
{
    FS_USE_PARA(callback_arg);
    FS_USE_PARA(event);

    (void) cy_rtos_set_semaphore(&qspi_sema, true);
}
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

static cy_rslt_t _set_active_ssel(U8 unit)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    if(active_hw_unit != unit)
    {
        result = cyhal_qspi_select_active_ssel(&_config->Obj, _config->Ssel[unit]);

        if(CY_RSLT_SUCCESS == result)
        {
            active_hw_unit = unit;
        }
    }

    return result;
}

static cyhal_qspi_bus_width_t _get_bus_width(U8 bus_width_in)
{
    cyhal_qspi_bus_width_t bus_width_out;

    switch(bus_width_in)
    {
    case BUS_WIDTH_ONE:
        bus_width_out = CYHAL_QSPI_CFG_BUS_SINGLE;
        break;
    case BUS_WIDTH_TWO:
        bus_width_out = CYHAL_QSPI_CFG_BUS_DUAL;
        break;
    case BUS_WIDTH_FOUR:
        bus_width_out = CYHAL_QSPI_CFG_BUS_QUAD;
        break;
    default:
        bus_width_out = CYHAL_QSPI_CFG_BUS_SINGLE;
    }

    return bus_width_out;
}

static inline cyhal_qspi_size_t _get_size(unsigned num_bytes)
{
    /* Multiply by 8 to convert num_bytes to number of bits. */
    return (cyhal_qspi_size_t)(num_bytes << 3u);
}

static void _get_qspi_cmd(cyhal_qspi_command_t *qspi_cmd, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, U16 BusWidth)
{
    uint32_t num_dummy_bytes = 0;

    qspi_cmd->instruction.bus_width = _get_bus_width(FS_BUSWIDTH_GET_CMD(BusWidth));
    qspi_cmd->instruction.value = Cmd;
    qspi_cmd->instruction.disabled = false;

    if(NumBytesAddr > 0)
    {
        qspi_cmd->address.bus_width = _get_bus_width(FS_BUSWIDTH_GET_ADDR(BusWidth));
        qspi_cmd->address.size = _get_size(NumBytesAddr);
        qspi_cmd->address.value = 0;

        /* Convert byte array to uint32_t. Value at index 0 of the array is the
         * MSB of the address.
         */
        for(uint32_t count = 0; count < NumBytesAddr; count++)
        {
            qspi_cmd->address.value <<= NUM_BITS_PER_BYTE;
            qspi_cmd->address.value |= *pPara++;
        }

        qspi_cmd->address.disabled = false;
    }
    else
    {
        qspi_cmd->address.disabled = true;
    }

    qspi_cmd->mode_bits.disabled = true;

    if(NumBytesPara > NumBytesAddr)
    {
        num_dummy_bytes = NumBytesPara - NumBytesAddr;

        /* Dummy bytes have the same bus width as the address bytes.
         * dummy_count is specified in number of clock cycles.
         */
        qspi_cmd->dummy_count = num_dummy_bytes * (NUM_BITS_PER_BYTE / FS_BUSWIDTH_GET_ADDR(BusWidth));
    }
    else
    {
        qspi_cmd->dummy_count = 0;
    }

    qspi_cmd->data.bus_width = _get_bus_width(FS_BUSWIDTH_GET_DATA(BusWidth));
}

static void _transfer_data(bool is_read, U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, U8 * pData, unsigned NumBytesData, U16 BusWidth)
{
    cyhal_qspi_command_t qspi_cmd;

    cy_rslt_t result = _set_active_ssel(Unit);

    if(CY_RSLT_SUCCESS == result)
    {
        _get_qspi_cmd(&qspi_cmd, Cmd, pPara, NumBytesPara, NumBytesAddr, BusWidth);

        /* Use cyhal_qspi_transfer() when pData is NULL and NumBytesData is 0 since
         * only command needs to be exchanged. Also, cyhal_qspi_transfer()
         * terminates the transfer for a command-only transfer whereas
         * cyhal_qspi_read() does not.
         */
        if(pData == NULL || NumBytesData == 0)
        {
            result = cyhal_qspi_transfer(&_config->Obj, &qspi_cmd, NULL, 0, NULL, 0);
        }
        else
        {
#if defined(COMPONENT_RTOS_AWARE)
            if(is_read)
            {
                result = cyhal_qspi_read_async(&_config->Obj, &qspi_cmd, pData, &NumBytesData);
            }
            else
            {
                result = cyhal_qspi_write_async(&_config->Obj, &qspi_cmd, pData, &NumBytesData);
            }

            if(CY_RSLT_SUCCESS == result)
            {
                /* Wait until the semaphore is set in the callback. */
                result = cy_rtos_get_semaphore(&qspi_sema, FS_NOR_HW_QSPI_RW_TIMEOUT_MS, false);
            }
#else
            if(is_read)
            {
                result = cyhal_qspi_read(&_config->Obj, &qspi_cmd, pData, &NumBytesData); /* Blocking read. */
            }
            else
            {
                result = cyhal_qspi_write(&_config->Obj, &qspi_cmd, pData, &NumBytesData); /* Blocking write. */
            }
#endif /* #if defined(COMPONENT_RTOS_AWARE) */
        }
    }

    CY_ASSERT(CY_RSLT_SUCCESS == result);
    FS_USE_PARA(result);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*******************************************************************************
* Function Name: FS_NOR_HW_SPI_Configure
****************************************************************************//**
*
*  Configures the SD Host driver.
*
*  Parameters
*   Config  Driver configuration structure.
*
*  Return Value
*   FS_NOR_HW_SPI_RESULT_BADPARAM   Invalid parameters.
*   FS_NOR_HW_SPI_RESULT_OK         Configured successfully.
*
*******************************************************************************/
FS_NOR_HW_SPIFI_Result_t FS_NOR_HW_SPIFI_Configure(FS_NOR_HW_SPIFI_Config_t *Config)
{
    FS_NOR_HW_SPIFI_Result_t result = FS_NOR_HW_SPIFI_RESULT_BADPARAM;

    if( Config != NULL)
    {
        _config = Config;
        result = FS_NOR_HW_SPIFI_RESULT_OK;
    }

    return result;
}

/*********************************************************************
*
*      Public code (via callback)
*
**********************************************************************
*/

/*********************************************************************
*
*       _HW_Init
*
*  Function description
*    HW layer function. It is called before any other function of the physical layer.
*    It should configure the HW so that the other functions can access the NOR flash.
*
*  Return value
*    Frequency of the SPI clock in Hz.
*/
static int _HW_Init(U8 Unit) {
    int freq_hz = 0;

    if(!_qspi_init_done)
    {

      cy_rslt_t result = cyhal_qspi_init(&_config->Obj, _config->IO0, _config->IO1, _config->IO2, _config->IO3,
                                         _config->IO4, _config->IO5, _config->IO6, _config->IO7,
                                         _config->Sclk, _config->Ssel[Unit], _config->HFClockFreqHz, 0u);

      if(CY_RSLT_SUCCESS == result)
      {
          for(uint32_t mem_slot = 0; mem_slot < _config->NumMem; mem_slot++)
          {
              if(NC != _config->Ssel[mem_slot])
              {
                  result = cyhal_qspi_slave_select_config(&_config->Obj, _config->Ssel[mem_slot]);

                  if(CY_RSLT_SUCCESS != result)
                  {
                      break;
                  }
              }
          }

#if defined(COMPONENT_RTOS_AWARE)
          if(CY_RSLT_SUCCESS == result)
          {
              cyhal_qspi_register_callback(&_config->Obj, _qspi_event_callback, NULL);
              cyhal_qspi_enable_event(&_config->Obj, (cyhal_qspi_event_t) (CYHAL_QSPI_IRQ_TRANSMIT_DONE | CYHAL_QSPI_IRQ_RECEIVE_DONE), _config->QspiIntrPriority, true);
              result = cy_rtos_init_semaphore(&qspi_sema, QSPI_SEMA_MAX_COUNT, QSPI_SEMA_INIT_COUNT);
          }
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

          if(CY_RSLT_SUCCESS == result)
          {
              freq_hz = cyhal_qspi_get_frequency(&_config->Obj);
              _qspi_init_done = true;
          }
      }
    }
    else
    {
        /* QSPI is already initialized. Just return the frequency value. */
        freq_hz = cyhal_qspi_get_frequency(&_config->Obj);
    }

    return freq_hz;
}

/*********************************************************************
*
*       _HW_SetCmdMode
*
*  Function description
*    HW layer function. It enables the direct access to NOR flash via SPI.
*    This function disables the memory-mapped mode.
*/
static void _HW_SetCmdMode(U8 Unit) {
    FS_USE_PARA(Unit);

    /* No implementation is required since switching between cmd & mem modes is
     * not done by this driver.
     */
}

/*********************************************************************
*
*       _HW_ExecCmd
*
*  Function description
*    HW layer function. It requests the NOR flash to execute a simple command.
*    The HW has to be in SPI mode.
*/
static void _HW_ExecCmd(U8 Unit, U8 Cmd, U8 BusWidth) {
    cyhal_qspi_command_t qspi_cmd;

    cy_rslt_t result = _set_active_ssel(Unit);

    if(CY_RSLT_SUCCESS == result)
    {
        qspi_cmd.instruction.bus_width = _get_bus_width(BusWidth);
        qspi_cmd.instruction.value = Cmd;
        qspi_cmd.instruction.disabled = false;
        qspi_cmd.address.disabled = true;
        qspi_cmd.mode_bits.disabled = true;
        qspi_cmd.dummy_count = 0;

        result = cyhal_qspi_transfer(&_config->Obj, &qspi_cmd, NULL, 0, NULL, 0);
    }

    CY_ASSERT(CY_RSLT_SUCCESS == result);
    FS_USE_PARA(result);
}

/*********************************************************************
*
*       _HW_ReadData
*
*  Function description
*    HW layer function. It transfers data from NOR flash to MCU.
*    The HW has to be in SPI mode.
*/
static void _HW_ReadData(U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, U8 * pData, unsigned NumBytesData, U16 BusWidth) {
    _transfer_data(true, Unit, Cmd, pPara, NumBytesPara, NumBytesAddr, pData, NumBytesData, BusWidth);
}

/*********************************************************************
*
*       _HW_WriteData
*
*  Function description
*    HW layer function. It transfers data from MCU to NOR flash.
*    The HW has to be in SPI mode.
*/
static void _HW_WriteData(U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, const U8 * pData, unsigned NumBytesData, U16 BusWidth) {
    _transfer_data(false, Unit, Cmd, pPara, NumBytesPara, NumBytesAddr, (U8 *) pData, NumBytesData, BusWidth);
}

/*********************************************************************
*
*       _HW_Poll
*
*  Function description
*    HW layer function. Sends a command repeatedly and checks the
*    response for a specified condition.
*
*  Return value
*    > 0    Timeout occurred.
*    ==0    OK, bit set to specified value.
*    < 0    Feature not supported.
*
*  Additional information
*    The function executes periodically a command and waits
*    until specified bit in the response returned by the NOR flash
*    is set to a value specified by BitValue. The position of the bit
*    that has to be checked is specified by BitPos where 0 is the
*    position of the least significant bit in the byte.
*/
static int _HW_Poll(U8 Unit, U8 Cmd, U8 BitPos, U8 BitValue, U32 Delay, U32 TimeOut_ms, U16 BusWidth) {
  int r;

  /* This function is not implemented since the QSPI block does not support HW
   * based polling of register bits of a QSPI memory. emFile uses SW based
   * polling when this function returns a negative value.
   */

  FS_USE_PARA(Unit);
  FS_USE_PARA(Cmd);
  FS_USE_PARA(BitPos);
  FS_USE_PARA(BitValue);
  FS_USE_PARA(Delay);
  FS_USE_PARA(TimeOut_ms);
  FS_USE_PARA(BusWidth);
  r = -1;                 // Set to indicate that the feature is not supported.
  return r;
}

/*********************************************************************
*
*       _HW_Delay
*
*  Function description
*    HW layer function. Blocks the execution for the specified number
*    of milliseconds.
*
*  Return value
*    ==0    Delay executed.
*    < 0    Feature not supported.
*/
static int _HW_Delay(U8 Unit, U32 ms) {
  FS_USE_PARA(Unit);

  FS_X_OS_Delay(ms);

  return 0;
}

/*********************************************************************
*
*       _HW_Lock
*
*  Function description
*    HW layer function. Requests exclusive access to the SPI bus.
*/
static void _HW_Lock(U8 Unit) {
  FS_USE_PARA(Unit);
}

/*********************************************************************
*
*       _HW_Unlock
*
*  Function description
*    HW layer function. Releases the exclusive access of the SPI bus.
*
*  Additional information
*    The configuration of the quad SPI controller has to be saved and
*    later restored in the _HW_Lock() function if the application
*    changes it.
*/
static void _HW_Unlock(U8 Unit) {
  FS_USE_PARA(Unit);
}

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
const FS_NOR_HW_TYPE_SPIFI FS_NOR_HW_SPIFI = {
  _HW_Init,
  _HW_SetCmdMode,
   NULL,
  _HW_ExecCmd,
  _HW_ReadData,
  _HW_WriteData,
  _HW_Poll,
  _HW_Delay,
  _HW_Lock,
  _HW_Unlock
};

/*************************** End of file ****************************/
