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
#define FS_NOR_HW_QSPI_RW_TIMEOUT_MS    (500U) /* in milliseconds. */
#endif

/*********************************************************************
*
*       Defines, non-configurable
*
**********************************************************************
*/
#define BUS_WIDTH_ONE               (1U)
#define BUS_WIDTH_TWO               (2U)
#define BUS_WIDTH_FOUR              (4U)

#define NUM_BITS_PER_BYTE           (8U)

#if defined(COMPONENT_RTOS_AWARE)
#define QSPI_SEMA_MAX_COUNT         (1LU)
#define QSPI_SEMA_INIT_COUNT        (0LU)
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static FS_NOR_HW_SPIFI_Config_t *config_nor_spifi;

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
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 8.13', 1,\
'The third-party defines the function interface')

static void qspi_event_callback(void *callback_arg, cyhal_qspi_event_t event)
{
    CY_MISRA_BLOCK_END('MISRA C-2012 Rule 8.13');
    FS_USE_PARA(callback_arg);
    FS_USE_PARA(event);

    (void) cy_rtos_set_semaphore(&qspi_sema, true);
}
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

static cy_rslt_t set_active_ssel(U8 unit)
{
    static uint8_t active_hw_unit = 0U;
    cy_rslt_t result = CY_RSLT_SUCCESS;

    if(active_hw_unit != unit)
    {
        result = cyhal_qspi_select_active_ssel(&config_nor_spifi->Obj, config_nor_spifi->PinSet[unit].ssel);

        if(CY_RSLT_SUCCESS == result)
        {
            active_hw_unit = unit;
        }
    }

    return result;
}

static cyhal_qspi_bus_width_t get_bus_width(U8 bus_width_in)
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
        break;
    }

    return bus_width_out;
}

static inline cyhal_qspi_size_t get_size(uint32_t num_bytes)
{
    cyhal_qspi_size_t qspi_size;

    /* Multiply by 8 to convert num_bytes to number of bits. */    
    switch(num_bytes)
    {
    case 1U:
        qspi_size = CYHAL_QSPI_CFG_SIZE_8;
        break;
    case 2U:
        qspi_size = CYHAL_QSPI_CFG_SIZE_32;
        break;
    case 3U:
        qspi_size = CYHAL_QSPI_CFG_SIZE_24;
        break;
    default:
        qspi_size = CYHAL_QSPI_CFG_SIZE_32;
        break;
    }
    return qspi_size;
}

static void get_qspi_cmd(cyhal_qspi_command_t *qspi_cmd, U8 Cmd, const U8 * pPara, uint32_t NumBytesPara, uint32_t NumBytesAddr, U16 BusWidth, uint32_t *address)
{
    uint32_t num_dummy_bytes = 0U;

    qspi_cmd->instruction.bus_width = get_bus_width((uint8_t)FS_BUSWIDTH_GET_CMD(BusWidth));
    qspi_cmd->instruction.value = Cmd;
    qspi_cmd->instruction.disabled = false;
    qspi_cmd->instruction.two_byte_cmd = false;
    qspi_cmd->instruction.data_rate = config_nor_spifi->DataRate;

    if(NumBytesAddr > 0U)
    {
        const U8 *buf_pPara = pPara;
        qspi_cmd->address.bus_width = get_bus_width((uint8_t)FS_BUSWIDTH_GET_ADDR(BusWidth));
        qspi_cmd->address.size = get_size(NumBytesAddr);
        *address = 0U;

        /* Convert byte array to uint32_t. Value at index 0 of the array is the
         * MSB of the address.
         */
        for(uint32_t count = 0U; count < NumBytesAddr; count++)
        {
            *address <<= NUM_BITS_PER_BYTE;
            *address |= *buf_pPara;
            buf_pPara++;
        }

        qspi_cmd->address.disabled = false;
        qspi_cmd->address.data_rate = config_nor_spifi->DataRate;
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
        qspi_cmd->dummy_cycles.dummy_count = num_dummy_bytes * (NUM_BITS_PER_BYTE / FS_BUSWIDTH_GET_ADDR((uint32_t)BusWidth));
        qspi_cmd->dummy_cycles.data_rate = config_nor_spifi->DataRate;
        qspi_cmd->dummy_cycles.bus_width = CYHAL_QSPI_CFG_BUS_SINGLE; 
    }
    else
    {
        qspi_cmd->dummy_cycles.dummy_count = 0U;
    }

    qspi_cmd->data.bus_width = get_bus_width((uint8_t)FS_BUSWIDTH_GET_DATA(BusWidth));
    qspi_cmd->data.data_rate = config_nor_spifi->DataRate;
}

static void transfer_data(bool is_read, U8 Unit, U8 Cmd, const U8 * pPara, uint32_t NumBytesPara, uint32_t NumBytesAddr, U8 * pData, size_t NumBytesData, U16 BusWidth)
{
    cyhal_qspi_command_t qspi_cmd;
    uint32_t address = 0U;

    cy_rslt_t result = set_active_ssel(Unit);

    if(CY_RSLT_SUCCESS == result)
    {
        get_qspi_cmd(&qspi_cmd, Cmd, pPara, NumBytesPara, NumBytesAddr, BusWidth, &address);

        /* Use cyhal_qspi_transfer() when pData is NULL and NumBytesData is 0 since
         * only command needs to be exchanged. Also, cyhal_qspi_transfer()
         * terminates the transfer for a command-only transfer whereas
         * cyhal_qspi_read() does not.
         */
        if((pData == NULL) || (NumBytesData == 0U))
        {
            result = cyhal_qspi_transfer(&config_nor_spifi->Obj, &qspi_cmd, address, NULL, 0, NULL, 0);
        }
        else
        {
#if defined(COMPONENT_RTOS_AWARE)
            if(is_read)
            {
                 result = cyhal_qspi_read_async(&config_nor_spifi->Obj, &qspi_cmd, address, pData, &NumBytesData);
            }
            else
            {
                result = cyhal_qspi_write_async(&config_nor_spifi->Obj, &qspi_cmd, address, pData, &NumBytesData);
            }

            if(CY_RSLT_SUCCESS == result)
            {
                /* Wait until the semaphore is set in the callback. */
                result = cy_rtos_get_semaphore(&qspi_sema, FS_NOR_HW_QSPI_RW_TIMEOUT_MS, false);
            }
#else
            if(is_read)
            {
                result = cyhal_qspi_read(&config_nor_spifi->Obj, &qspi_cmd, address, pData, &NumBytesData); /* Blocking read. */
            }
            else
            {
                result = cyhal_qspi_write(&config_nor_spifi->Obj, &qspi_cmd, address, pData, &NumBytesData); /* Blocking write. */
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
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Directive 4.6', 8,\
'The third-party defines the function interface with basic numeral type')
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 5.9', 4,\
'The third-party defines the names of the API: _HW_Init, _HW_ReadData, _HW_WriteData, _HW_Delay. Both drivers must have instances of these API')
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 21.2', 9,\
'The third-party defines the names of the functions: _HW_Init, _HW_SetCmdMode, _HW_ExecCmd, _HW_ReadData, _HW_WriteData, _HW_Poll, _HW_Delay, _HW_Lock, _HW_Unlock. The underscore should be part of the names of these functions ')
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
FS_NOR_HW_SPIFI_Result_t FS_NOR_HW_SPIFI_Configure(FS_NOR_HW_SPIFI_Config_t *UserConfig)
{
    FS_NOR_HW_SPIFI_Result_t result = FS_NOR_HW_SPIFI_RESULT_BADPARAM;

    if( UserConfig != NULL)
    {
        config_nor_spifi = UserConfig;
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
    uint32_t freq_hz = 0U;
    cy_rslt_t result;
    static bool qspi_init_done = false;

    if(!qspi_init_done)
    {
        result = cyhal_qspi_init(&config_nor_spifi->Obj, config_nor_spifi->Sclk, &config_nor_spifi->PinSet[0U],
                        config_nor_spifi->HFClockFreqHz, 0U, NULL );

        if(CY_RSLT_SUCCESS == result)
        {
            for(uint32_t mem_slot = 1U; mem_slot < config_nor_spifi->NumMem; mem_slot++)
            {
                if(NC != config_nor_spifi->PinSet[mem_slot].ssel)
                {
                    result = cyhal_qspi_slave_configure(&config_nor_spifi->Obj, &config_nor_spifi->PinSet[mem_slot]);

                    if(CY_RSLT_SUCCESS != result)
                    {
                        break;
                    }
                }
            }
#if defined(COMPONENT_RTOS_AWARE)
          if(CY_RSLT_SUCCESS == result)
          {
              cyhal_qspi_register_callback(&config_nor_spifi->Obj, qspi_event_callback, NULL);
              CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 10.5','In cyhal_qspi_enable_event essentially() enum type cast to essentially unsigned type');
              cyhal_qspi_enable_event(&config_nor_spifi->Obj, (cyhal_qspi_event_t) ((uint32_t)CYHAL_QSPI_IRQ_TRANSMIT_DONE | (uint32_t)CYHAL_QSPI_IRQ_RECEIVE_DONE), config_nor_spifi->QspiIntrPriority, true);
              result = cy_rtos_init_semaphore(&qspi_sema, QSPI_SEMA_MAX_COUNT, QSPI_SEMA_INIT_COUNT);
          }
#endif /* #if defined(COMPONENT_RTOS_AWARE) */
            result = set_active_ssel(Unit);
            if(CY_RSLT_SUCCESS == result)
            {
                freq_hz = cyhal_qspi_get_frequency(&config_nor_spifi->Obj);
                qspi_init_done = true;
            }
        }
    }
    else
    {
        result = set_active_ssel(Unit);
        /* QSPI is already initialized. Just return the frequency value. */
        if(CY_RSLT_SUCCESS == result)
        {        
            freq_hz = cyhal_qspi_get_frequency(&config_nor_spifi->Obj);
        }
    }

    return (int32_t) freq_hz;
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
    uint32_t address = 0U;

    cy_rslt_t result = set_active_ssel(Unit);

    if(CY_RSLT_SUCCESS == result)
    {
        qspi_cmd.instruction.bus_width = get_bus_width(BusWidth);
        qspi_cmd.instruction.value = Cmd;
        qspi_cmd.instruction.disabled = false;
        qspi_cmd.instruction.data_rate = config_nor_spifi->DataRate; 
        qspi_cmd.instruction.two_byte_cmd = false; 

        qspi_cmd.address.disabled = true;

        qspi_cmd.mode_bits.disabled = true;
        
        qspi_cmd.dummy_cycles.bus_width = CYHAL_QSPI_CFG_BUS_SINGLE;
        qspi_cmd.dummy_cycles.data_rate = config_nor_spifi->DataRate;
        qspi_cmd.dummy_cycles.dummy_count = 0U;
        
        qspi_cmd.data.data_rate = config_nor_spifi->DataRate; 
        qspi_cmd.data.bus_width = get_bus_width(BusWidth);

        result = cyhal_qspi_transfer(&config_nor_spifi->Obj, &qspi_cmd, address, NULL, 0, NULL, 0);
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
    transfer_data(true, Unit, Cmd, pPara, NumBytesPara, NumBytesAddr, pData, NumBytesData, BusWidth);
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
    CY_MISRA_DEVIATE_LINE('MISRA C-2012 Rule 11.8','The third-party defines the function interface');
    transfer_data(false, Unit, Cmd, pPara, NumBytesPara, NumBytesAddr, (U8 *) pData, NumBytesData, BusWidth);
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
  int32_t r;

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

  FS_X_OS_Delay((int32_t)ms);

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

CY_MISRA_BLOCK_END('MISRA C-2012 Directive 4.6')
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 5.9')
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 21.2')
/*************************** End of file ****************************/
