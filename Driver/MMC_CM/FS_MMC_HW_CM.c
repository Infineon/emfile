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

File        : FS_MMC_HW_CM_PSoC6.c
Purpose     : Generic HW layer for the MMC / SD driver working in card mode.
Literature  :
  [1] SD Specifications Part 1 Physical Layer Simplified Specification Version 2.00
    (\\fileserver\Techinfo\Company\SDCard_org\SDCardPhysicalLayerSpec_V200.pdf)
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "FS.h"
#include "FS_OS.h"
#include "FS_MMC_HW_CM.h"
#include "cy_utils.h"
#include "cyhal_system.h"
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef CY_IP_MXSDHC

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define CY_SD_HOST_SUPPLY_RAMP_UP_TIME_MS   (35UL)  /* The host supply ramp up time in milliseconds. */


/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#define SD_HOST_RESP_LEN_SIX_BYTES          (6UL)
#define SD_HOST_RESPONSE_SIZE               (4U)

#define SD_HOST_BUS_WIDTH_1_BIT             (1U)
#define SD_HOST_BUS_WIDTH_4_BIT             (4U)
#define SD_HOST_BUS_WIDTH_8_BIT             (8U)

#define SD_HOST_INIT_CLK_FREQUENCY_KHZ      (400U)          /* The CLK frequency in kHz during card initialization. */
#define SD_HOST_NCC_MIN_CYCLES              (80U)           /* The period (clock cycles) between an end bit
                                                             * of the command and a start bit of the next command.
                                                             */
#define SD_HOST_NCC_MIN_US                  ((1000U * SD_HOST_NCC_MIN_CYCLES) / SD_HOST_INIT_CLK_FREQUENCY_KHZ)

/*********************************************************************
*
*       Local data types
*
**********************************************************************
*/

/* Structure holding the driver instance-specific variables. */
typedef struct
{
    bool is_init_done;                                      /* Indicates whether the driver is initialized or not. */
    FS_MMC_HW_CM_SDHostConfig_t *config;                    /* User-provided driver configuration. */
    cy_rslt_t send_cmd_status;                              /* Stores the return value of the cyhal_sdhc_send_cmd() function. */
    void *data_ptr;                                         /* Destination/source buffer for the DMA transfers. */
    U16  block_size;                                        /* Size of the block to transfer as set by the upper layer. */
    U16  num_blocks;                                        /* Number of blocks to transfer as set by the upper layer. */
} cy_sd_host_inst_t;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static cy_sd_host_inst_t _sd_host_inst[FS_MMC_NUM_UNITS];

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*******************************************************************************
* Function Name: _reset_cmd_and_data_lines
****************************************************************************//**
*
*  Issues software resets to command and data lines. This function also clears
*  the error interrupt status registers.
*
*  Parameters
*   Unit            Index of the SD / MMC host controller (0-based).
*
*******************************************************************************/
static void _reset_cmd_and_data_lines(U8 Unit)
{
    cyhal_sdhc_software_reset(&_sd_host_inst[Unit].config->Obj);

    /* Clear the error status bits after the reset is complete. */
    cyhal_sdhc_clear_errors(&_sd_host_inst[Unit].config->Obj);

    FS_DEBUG_WARN((FS_MTYPE_DRIVER, "_reset_cmd_and_data_lines: SW reset of CMD & DATA lines!\n"));
}


/*******************************************************************************
* Function Name: _clear_interrupt_status_registers
****************************************************************************//**
*
*  Clears Error Interrupt Status registers. Resets CMD and DATA lines if an
*  error has occurred as indicated by the Error Interrupt Status register.
*
*  Parameters
*   Unit            Index of the SD / MMC host controller (0-based).
*
*******************************************************************************/
static void _clear_interrupt_status_registers(U8 Unit)
{
    cyhal_sdhc_error_type_t err = cyhal_sdhc_get_last_command_errors(&_sd_host_inst[Unit].config->Obj);

    /* Check if an error has occurred in the previous transaction. */
    if (err)
    {
        _reset_cmd_and_data_lines(Unit);
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*******************************************************************************
* Function Name: FS_MMC_HW_CM_Configure
****************************************************************************//**
*
*  Configures the SD Host driver.
*
*  Parameters
*   Unit    Index of the SD / MMC host controller (0-based).
*   Config  Driver configuration structure.
*
*  Return Value
*   FS_MMC_HW_CM_RESULT_BADPARAM    Invalid parameters.
*   FS_MMC_HW_CM_RESULT_OK          Configured successfully.
*
*******************************************************************************/
FS_MMC_HW_CM_Result_t FS_MMC_HW_CM_Configure(U8 Unit, FS_MMC_HW_CM_SDHostConfig_t *Config)
{
    FS_MMC_HW_CM_Result_t result = FS_MMC_HW_CM_RESULT_BADPARAM;

    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);

    if(Config != NULL)
    {
        _sd_host_inst[Unit].config = Config;
        result = FS_MMC_HW_CM_RESULT_OK;
    }

    return result;
}

/*********************************************************************
*
*       Public code (via callback)
*
**********************************************************************
*/

/*********************************************************************
*
*       _HW_Init
*
*  Function description
*    Initialize the SD / MMC host controller.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*/
static void _HW_Init(U8 Unit) {
    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);

    cy_rslt_t result = CY_RSLT_SUCCESS;
    FS_MMC_HW_CM_SDHostConfig_t *cfg = _sd_host_inst[Unit].config;

    FS_DEBUG_LOG((FS_MTYPE_DRIVER, "_HW_Init\n"));

    if(!_sd_host_inst[Unit].is_init_done)
    {
        result = cyhal_sdhc_init_hw(&cfg->Obj, &cfg->Config,
                                    cfg->Cmd, cfg->Clk,
                                    cfg->Data0, cfg->Data1, cfg->Data2, cfg->Data3,
                                    cfg->Data4, cfg->Data5, cfg->Data6, cfg->Data7,
                                    cfg->CardDetect, cfg->IoVoltSel, cfg->CardPwrEn,
                                    cfg->CardWriteProt, cfg->LedControl, cfg->EmmcReset);

        _sd_host_inst[Unit].is_init_done = true;
    }

    /* emFile calls this function many times during initialization.
     * The HAL init must be called only once but the bus width and the voltage
     * levels must be reset at every initialization.
     */
    if(CY_RSLT_SUCCESS == result)
    {
        /* Set the initial host bus width */
        result = cyhal_sdhc_set_bus_width(&_sd_host_inst[Unit].config->Obj, SD_HOST_BUS_WIDTH_1_BIT, false);

        if( (CY_RSLT_SUCCESS == result) && (NC != cfg->IoVoltSel) )
        {
            /* Controls IoVoltSel pin. */
            result = cyhal_sdhc_set_io_voltage(&_sd_host_inst[Unit].config->Obj, CYHAL_SDHC_IO_VOLTAGE_3_3V, CYHAL_SDHC_IO_VOLT_ACTION_NONE);
        }

        if( (CY_RSLT_SUCCESS == result) && (NC != cfg->CardPwrEn) )
        {
            /* Controls the CardPwrEn pin */
            cyhal_sdhc_enable_card_power(&_sd_host_inst[Unit].config->Obj, true);
        }

        /* Wait to the stable voltage and the stable clock */
        FS_X_OS_Delay(CY_SD_HOST_SUPPLY_RAMP_UP_TIME_MS);
    }

    CY_ASSERT(CY_RSLT_SUCCESS == result);
}

/*********************************************************************
*
*       _HW_Delay
*
*  Function description
*    Blocks the code execution for the specified time.
*
*  Parameters
*    ms   Number of milliseconds to delay.
*/
static void _HW_Delay(int ms) {
    FS_X_OS_Delay(ms);
}

/*********************************************************************
*
*       _HW_IsPresent
*
*  Function description
*    Returns the state of the media.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*
*  Return value
*    FS_MEDIA_STATE_UNKNOWN     The state of the storage device is not known.
*    FS_MEDIA_NOT_PRESENT       The storage device is not present.
*    FS_MEDIA_IS_PRESENT        The storage device is present.
*
*  Additional information
*    If the state is unknown, the function has to FS_MEDIA_STATE_UNKNOWN
*    and the higher layers of the file system will try to figure out
*    if a storage device is present or not.
*/
static int _HW_IsPresent(U8 Unit) {
    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);

    return (cyhal_sdhc_is_card_inserted(&_sd_host_inst[Unit].config->Obj) ? FS_MEDIA_IS_PRESENT : FS_MEDIA_NOT_PRESENT);
}

/*********************************************************************
*
*       _HW_IsWriteProtected
*
*  Function description
*    Returns whether card is write protected or not.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*
*  Return value
*    ==0      The SD / MMC card is writable.
*    ==1      The SD / MMC card is write protected.
*/
static int _HW_IsWriteProtected(U8 Unit) {
    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);

    return (cyhal_sdhc_is_card_mech_write_protected(&_sd_host_inst[Unit].config->Obj) ? 1 : 0);
}

/*********************************************************************
*
*       _HW_SetMaxSpeed
*
*  Function description
*    Configures the frequency of the clock supplied to SD / MMC card.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*    Freq     Requested clock frequency in kHz.
*
*  Return value
*    !=0  OK, actual configured clock frequency in kHz.
*    ==0  An error occurred.
*
*  Additional information
*    This function is called two times:
*    1. During card initialization
*       Initialize the frequency to not more than 400 kHz.
*    2. After card initialization
*       The CSD register of card is read and the max frequency
*       the card can operate is determined.
*       [In most cases: MMC cards 20 MHz, SD cards 25 MHz]
*    The configured clock frequency has to be smaller than or equal to
*    the frequency specified via Freq.
*/
static U16 _HW_SetMaxSpeed(U8 Unit, U16 Freq) {
    uint32_t actual_freq_khz = 0;

    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);
    CY_ASSERT(0u < Freq);

    cy_rslt_t result = cyhal_sdhc_set_frequency(&_sd_host_inst[Unit].config->Obj, (Freq * 1000lu), false);

    if(CY_RSLT_SUCCESS == result)
    {
        actual_freq_khz = cyhal_sdhc_get_frequency(&_sd_host_inst[Unit].config->Obj) / 1000lu;
    }

    FS_DEBUG_LOG((FS_MTYPE_DRIVER, "_HW_SetMaxClock: ReqFreq = %d KHz, Actual Freq = %d KHz\n", Freq, actual_freq_khz));

    return actual_freq_khz;
}

/*********************************************************************
*
*       _HW_SetResponseTimeOut
*
*  Function description
*    Sets the maximum time the SD / MMC host controller
*    has to wait for a response.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*    Value    Number of SD / MMC clock cycles.
*/
static void _HW_SetResponseTimeOut(U8 Unit, U32 Value) {
    FS_USE_PARA(Unit);
    FS_USE_PARA(Value);
}

/*********************************************************************
*
*       _HW_SetReadDataTimeOut
*
*  Function description
*    Sets the maximum time the SD / MMC host controller has to wait
*    for the arrival of data.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*    Value    Number of SD / MMC clock cycles.
*/
static void _HW_SetReadDataTimeOut(U8 Unit, U32 Value) {

    /* This function returns error when it is unable to configure a timeout that
     * is greater than or equal to the delay corresponding to the number of card
     * clock cycles (specified through Value). In that case, reconfigure the
     * delay using the FS_MMC_READ_DATA_TIMEOUT macro in FS_Conf.h.
     */
    cy_rslt_t result = cyhal_sdhc_set_data_read_timeout(&_sd_host_inst[Unit].config->Obj, Value, false);

    CY_ASSERT(CY_RSLT_SUCCESS == result);
    FS_USE_PARA(result);
}

/*********************************************************************
*
*       _HW_SendCmd
*
*  Function description
*    Sends a command to the card.
*
*  Parameters
*    Unit           Index of the SD / MMC host controller (0-based).
*    Cmd            Command number according to [1]
*    CmdFlags       Additional information about the command to execute
*    ResponseType   Type of response as defined in [1]
*    Arg            Command parameter
*/
static void _HW_SendCmd(U8 Unit, unsigned Cmd, unsigned CmdFlags, unsigned ResponseType, U32 Arg) {
    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);

    /* Unsupported command flags:
     *  FS_MMC_CMD_FLAG_WRITE_BURST_REPEAT
     *  FS_MMC_CMD_FLAG_WRITE_BURST_FILL
     *  FS_MMC_CMD_FLAG_SWITCH_VOLTAGE
     */
    cy_rslt_t result = CY_RSLT_SUCCESS;
    cyhal_sdhc_cmd_config_t cmd_config = { 0u };
    cyhal_sdhc_data_config_t data_config = { 0u };

    if (CmdFlags & FS_MMC_CMD_FLAG_DATATRANSFER)
    {
        data_config.data_ptr             = _sd_host_inst[Unit].data_ptr;
        data_config.block_size           = _sd_host_inst[Unit].block_size,
        data_config.number_of_blocks     = _sd_host_inst[Unit].num_blocks,

        /* data_config.auto_command must be set to AUTO_CMD_NONE since the
         * emFile driver already takes care of issuing CMD23 (SET_BLOCK_COUNT)
         * or CMD12 (STOP_TRANSMISSION) before or after sending a multi-block
         * read (CMD18) or a multi-block write (CMD25) command respectively.
         */
        data_config.auto_command         = CYHAL_SDHC_AUTO_CMD_NONE;
        data_config.is_read              = (CmdFlags & FS_MMC_CMD_FLAG_WRITETRANSFER) ? false : true;

        cmd_config.data_config = &data_config;
        result = cyhal_sdhc_config_data_transfer(&_sd_host_inst[Unit].config->Obj, &data_config);
    }
    else
    {
        cmd_config.data_config = NULL;
    }

    if(CY_RSLT_SUCCESS == result)
    {
        if (CmdFlags & FS_MMC_CMD_FLAG_USE_SD4MODE)
        {
            result = cyhal_sdhc_set_bus_width(&_sd_host_inst[Unit].config->Obj, SD_HOST_BUS_WIDTH_4_BIT, false);
        }
        else if (CmdFlags & FS_MMC_CMD_FLAG_USE_MMC8MODE)
        {
            result = cyhal_sdhc_set_bus_width(&_sd_host_inst[Unit].config->Obj, SD_HOST_BUS_WIDTH_8_BIT, false);
        }

        if(CY_RSLT_SUCCESS == result)
        {
            cmd_config.command_index = (uint32_t)Cmd;
            cmd_config.command_argument = Arg;

            if(cmd_config.command_index == 0)
            {
                FS_DEBUG_LOG((FS_MTYPE_DRIVER, "CMD0!\n"));
            }

            /* CRC Check enable must be set to 0 for the command with no
             * response, R3 response, and R4 response.
             */
            cmd_config.enable_crc_check = (CmdFlags & FS_MMC_CMD_FLAG_NO_CRC_CHECK) ? false : true;

            /* Must be set to false for the command with no response, R2
             * response, R3 response, and R4 response.
             */
            cmd_config.enable_idx_check = true;

            /* FS_MMC_RESPONSE_FORMAT_R6 and FS_MMC_RESPONSE_FORMAT_R7 are same as R1. */
            switch (ResponseType)
            {
                case FS_MMC_RESPONSE_FORMAT_NONE:
                    cmd_config.response_type = CYHAL_SDHC_RESPONSE_NONE;
                    cmd_config.enable_idx_check = false;
                    cmd_config.enable_crc_check = false;
                    break;
                case FS_MMC_RESPONSE_FORMAT_R1:
                    if(CmdFlags & FS_MMC_CMD_FLAG_SETBUSY)
                    {
                        cmd_config.response_type = CYHAL_SDHC_RESPONSE_LEN_48B;
                    }
                    else
                    {
                        cmd_config.response_type = CYHAL_SDHC_RESPONSE_LEN_48;
                    }
                    break;
                case FS_MMC_RESPONSE_FORMAT_R2:
                    cmd_config.response_type = CYHAL_SDHC_RESPONSE_LEN_136;
                    cmd_config.enable_idx_check = false;
                    break;
                case FS_MMC_RESPONSE_FORMAT_R3:
                    cmd_config.response_type = CYHAL_SDHC_RESPONSE_LEN_48;
                    cmd_config.enable_crc_check = false;
                    cmd_config.enable_idx_check = false;
                    break;
                default:
                    break;
            }

            /* FS_MMC_CMD_FLAG_INITIALIZE is set when CMD0 is sent.
             * command_type must be set to ABORT for CMD0.
             *
             * FS_MMC_CMD_FLAG_STOP_TRANS is set when CMD12 is sent.
             */
            if (CmdFlags & (FS_MMC_CMD_FLAG_INITIALIZE | FS_MMC_CMD_FLAG_STOP_TRANS) )
            {
                cmd_config.command_type = CYHAL_SDHC_CMD_ABORT;
            }
            else
            {
                cmd_config.command_type = CYHAL_SDHC_CMD_NORMAL;
            }

            /* Clean all the interrupts before sending commands */
            _clear_interrupt_status_registers(Unit);
            result = cyhal_sdhc_send_cmd(&_sd_host_inst[Unit].config->Obj, &cmd_config);

            /* FS_MMC_CMD_FLAG_INITIALIZE is set when CMD0 is sent.
             * SW reset of CMD and DATA lines must be performed after CMD0 is
             * sent.
             */
            if (CmdFlags & FS_MMC_CMD_FLAG_INITIALIZE)
            {
                cyhal_system_delay_us(SD_HOST_NCC_MIN_US);

                /* Software reset for the CMD line. */
                Cy_SD_Host_SoftwareReset(_sd_host_inst[Unit].config->Obj.base, CY_SD_HOST_RESET_CMD_LINE);
                FS_DEBUG_WARN((FS_MTYPE_DRIVER, "_HW_SendCmd: SW reset of CMD line!\n"));         
            }
            else
            {
                _sd_host_inst[Unit].send_cmd_status = result;

                if (CY_RSLT_SUCCESS == result)
                {
                    FS_DEBUG_LOG((FS_MTYPE_DRIVER, "cyhal_sdhc_send_cmd: success!\n"));
                }
                else
                {
                    /* Software reset for the CMD and DATA lines. */
                    _reset_cmd_and_data_lines(Unit);
                    FS_DEBUG_WARN((FS_MTYPE_DRIVER, "cyhal_sdhc_send_cmd: error 0x%08"PRIx32"\n", result));
                }
            }
        }
    }
}

/*********************************************************************
*
*       _HW_GetResponse
*
*  Function description
*    Waits for a response to be received.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*    pBuffer  User allocated buffer where the response is stored.
*    Size     Size of the buffer in bytes.
*
*  Return values
*    FS_MMC_CARD_NO_ERROR                 Success
*    FS_MMC_CARD_RESPONSE_CRC_ERROR       CRC error in response
*    FS_MMC_CARD_RESPONSE_TIMEOUT         No response received
*    FS_MMC_CARD_RESPONSE_GENERIC_ERROR   Any other error
*
*  Notes
*    (1) The response data has to be stored at byte offset 1 when
*        the SD host controller does not provide the first byte
*        of response message, that is the byte that includes the
*        start bit.
*/
static int _HW_GetResponse(U8 Unit, void * pBuffer, U32 Size) {
    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);
    CY_ASSERT(pBuffer != NULL);

    cyhal_sdhc_error_type_t err;
    uint8_t *pResponse = (uint8_t *) pBuffer;
    uint8_t i;
    uint32_t response[SD_HOST_RESPONSE_SIZE] = { 0u };
    int ret = FS_MMC_CARD_RESPONSE_GENERIC_ERROR;

    if(CY_RSLT_SUCCESS == _sd_host_inst[Unit].send_cmd_status)
    {
        err = cyhal_sdhc_get_last_command_errors(&_sd_host_inst[Unit].config->Obj);
        FS_DEBUG_LOG((FS_MTYPE_DRIVER, "\terror = 0x%04"PRIx32"\n", err));

        if(CYHAL_SDHC_NO_ERR == err)
        {
            ret = FS_MMC_CARD_NO_ERROR;
        }
        else if(err & CYHAL_SDHC_CMD_CRC_ERR)
        {
            ret = FS_MMC_CARD_RESPONSE_CRC_ERROR;
        }
        else if(err & CYHAL_SDHC_CMD_TOUT_ERR)
        {
            ret = FS_MMC_CARD_RESPONSE_TIMEOUT;
        }
    }
    else
    {
         ret = FS_MMC_CARD_RESPONSE_TIMEOUT;
    }

    if(ret == FS_MMC_CARD_NO_ERROR)
    {
        /* Size is the length of the actual response in bytes.
         * Size = 6 when response length is 48-bits
         * Size = 17 when response length is 136-bits
         */

        /* Check the response. */
        cy_rslt_t result = cyhal_sdhc_get_response(&_sd_host_inst[Unit].config->Obj,
                                         (uint32_t *)&response,
                                         Size > SD_HOST_RESP_LEN_SIX_BYTES ? true : false);

        if(CY_RSLT_SUCCESS == result)
        {
            /* Read the necessary number of response words from the response register. */
            if (Size <= SD_HOST_RESP_LEN_SIX_BYTES)
            {
                pResponse[4] = (uint8_t) (response[0]);
                pResponse[3] = (uint8_t) (response[0] >> 8u);
                pResponse[2] = (uint8_t) (response[0] >> 16u);
                pResponse[1] = (uint8_t) (response[0] >> 24u);
            }
            else
            {
                i = Size - 2u;

                for (uint32_t index = 0; index < ((Size - 1) / sizeof(uint32_t)); index++)
                {
                    pResponse[i - 0u] = (uint8_t) (response[index]);
                    pResponse[i - 1u] = (uint8_t) (response[index] >> 8u);
                    pResponse[i - 2u] = (uint8_t) (response[index] >> 16u);
                    pResponse[i - 3u] = (uint8_t) (response[index] >> 24u);

                    i = i - sizeof(uint32_t);
                }
            }
        }
        else
        {
            ret = FS_MMC_CARD_RESPONSE_GENERIC_ERROR;
        }
    }

    return ret;
}

/*********************************************************************
*
*       _HW_ReadData
*
*  Function description
*    Reads data from the card using the SD / MMC host controller.
*
*  Parameters
*    Unit       Index of the SD / MMC host controller (0-based).
*    pBuffer    [OUT] Data received from storage device.
*    NumBytes   Number of bytes in one read data block.
*    NumBlocks  Number of data blocks to be read.
*
*  Return values
*    FS_MMC_CARD_NO_ERROR             Success
*    FS_MMC_CARD_READ_CRC_ERROR       CRC error in received data
*    FS_MMC_CARD_READ_TIMEOUT         No data received
*    FS_MMC_CARD_READ_GENERIC_ERROR   Any other error
*
*  Additional information
*    The total number of bytes to be transferred is NumBytes * NumBlocks.
*/
static int _HW_ReadData(U8 Unit, void * pBuffer, unsigned NumBytes, unsigned NumBlocks) {
    FS_USE_PARA(pBuffer);
    FS_USE_PARA(NumBytes);
    FS_USE_PARA(NumBlocks);

    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);

    int ret = FS_MMC_CARD_READ_GENERIC_ERROR;
    cyhal_sdhc_error_type_t err;

    if(CY_RSLT_SUCCESS == cyhal_sdhc_wait_transfer_complete(&_sd_host_inst[Unit].config->Obj))
    {
        err = cyhal_sdhc_get_last_command_errors(&_sd_host_inst[Unit].config->Obj);
        FS_DEBUG_LOG((FS_MTYPE_DRIVER, "\terror = 0x%04"PRIx32"\n", err));

        if(CYHAL_SDHC_NO_ERR == err)
        {
            ret = FS_MMC_CARD_NO_ERROR;
        }
        else if(err & CYHAL_SDHC_DATA_CRC_ERR)
        {
            ret = FS_MMC_CARD_READ_CRC_ERROR;
        }
        else if(err & CYHAL_SDHC_DATA_TOUT_ERR)
        {
            ret = FS_MMC_CARD_READ_TIMEOUT;
        }
    }
    else
    {
        ret = FS_MMC_CARD_READ_TIMEOUT;
    }

    if(FS_MMC_CARD_NO_ERROR != ret)
    {
        FS_DEBUG_WARN((FS_MTYPE_DRIVER, "_HW_ReadData, ret = %d\n", ret));
    }

    return ret;
}

/*********************************************************************
*
*       _HW_WriteData
*
*  Function description
*    Writes the data to SD / MMC card using the SD / MMC host controller.
*
*  Parameters
*    Unit       Index of the SD / MMC host controller (0-based).
*    pBuffer    [OUT] Data to be sent to storage device.
*    NumBytes   Number of bytes in one written data block.
*    NumBlocks  Number of data blocks to be written.
*
*  Return values
*    FS_MMC_CARD_NO_ERROR      Success
*    FS_MMC_CARD_READ_TIMEOUT  No data received
*
*  Additional information
*    The total number of bytes to be transferred is NumBytes * NumBlocks.
*/
static int _HW_WriteData(U8 Unit, const void * pBuffer, unsigned NumBytes, unsigned NumBlocks) {
    FS_USE_PARA(pBuffer);
    FS_USE_PARA(NumBytes);
    FS_USE_PARA(NumBlocks);

    CY_ASSERT(FS_MMC_NUM_UNITS > Unit);

    int ret = FS_MMC_CARD_WRITE_GENERIC_ERROR;
    cyhal_sdhc_error_type_t err;

    if(CY_RSLT_SUCCESS == cyhal_sdhc_wait_transfer_complete(&_sd_host_inst[Unit].config->Obj))
    {
        err = cyhal_sdhc_get_last_command_errors(&_sd_host_inst[Unit].config->Obj);
        FS_DEBUG_LOG((FS_MTYPE_DRIVER, "\terror = 0x%04"PRIx32"\n", err));

        if(CYHAL_SDHC_NO_ERR == err)
        {
            ret = FS_MMC_CARD_NO_ERROR;
        }
        else if(err & CYHAL_SDHC_DATA_CRC_ERR)
        {
            ret = FS_MMC_CARD_WRITE_CRC_ERROR;
        }
        else if(err & CYHAL_SDHC_DATA_TOUT_ERR)
        {
            ret = FS_MMC_CARD_RESPONSE_TIMEOUT;
        }
    }
    else
    {
        ret = FS_MMC_CARD_RESPONSE_TIMEOUT;
    }

    if(FS_MMC_CARD_NO_ERROR != ret)
    {
        FS_DEBUG_WARN((FS_MTYPE_DRIVER, "_HW_WriteData, ret = %d\n", ret));
    }

    return ret;
}

/*********************************************************************
*
*       _HW_SetDataPointer
*
*  Function description
*    Tells the hardware layer where to read data from
*    or write data to.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*    p        Data buffer.
*
*  Additional information
*    This function is optional. It is required to be implemented for
*    some SD / MMC host controllers that need to know the address of
*    the data before sending the command to the card,
*    eg. when transferring the data via DMA.
*/
static void _HW_SetDataPointer(U8 Unit, const void * p) {
    FS_USE_PARA(Unit);
    _sd_host_inst[Unit].data_ptr = (void *) p;
}

/*********************************************************************
*
*       _HW_SetBlockLen
*
*  Function description
*    Sets the block size (sector size) that has to be transferred.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*/
static void _HW_SetBlockLen(U8 Unit, U16 BlockSize) {
    FS_USE_PARA(Unit);
    _sd_host_inst[Unit].block_size = BlockSize;
}

/*********************************************************************
*
*       _HW_SetNumBlocks
*
*  Function description
*    Sets the number of blocks (sectors) to be transferred.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*/
static void _HW_SetNumBlocks(U8 Unit, U16 NumBlocks) {
    FS_USE_PARA(Unit);
    _sd_host_inst[Unit].num_blocks = NumBlocks;
}

/*********************************************************************
*
*       _HW_GetMaxReadBurst
*
*  Function description
*    Returns the number of block (sectors) that can be read at once
*    with a single READ_MULTIPLE_SECTOR command.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*
*  Return value
*    Number of sectors that can be read at once.
*/
static U16 _HW_GetMaxReadBurst(U8 Unit) {
    FS_USE_PARA(Unit);

    /* Block count is 32-bit register since HOST_CTRL2_R.HOST_VER4_ENABLE bit is
     * set in Cy_SD_Host_Init() but the return type of this function is only
     * 16-bits.
     */
    return UINT16_MAX;
}

/*********************************************************************
*
*       _HW_GetMaxWriteBurst
*
*  Function description
*    Returns the number of block (sectors) that can be written at once
*    with a single WRITE_MULTIPLE_SECTOR command.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*
*  Return value
*    Number of sectors that can be written at once.
*/
static U16 _HW_GetMaxWriteBurst(U8 Unit) {
    FS_USE_PARA(Unit);

    /* Block count is 32-bit register since HOST_CTRL2_R.HOST_VER4_ENABLE bit is
     * set in Cy_SD_Host_Init() but the return type of this function is only
     * 16-bits.
     */
    return UINT16_MAX;
}

/*********************************************************************
*
*       _HW_GetMaxWriteBurstRepeat
*
*  Function description
*    Returns the number of block (sectors) that can be written at once
*    with a single WRITE_MULTIPLE_SECTOR command and that contain the
*    same data.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*
*  Return value
*    Number of sectors that can be written at once. The function has
*    to return 0 if the feature is not supported.
*/
static U16 _HW_GetMaxWriteBurstRepeat(U8 Unit) {
    FS_USE_PARA(Unit);
    return 0;
}

/*********************************************************************
*
*       _HW_GetMaxWriteBurstFill
*
*  Function description
*    Returns the number of block (sectors) that can be written at once
*    with a single WRITE_MULTIPLE_SECTOR command. The contents of the
*    sectors is filled with the same 32-bit pattern.
*
*  Parameters
*    Unit     Index of the SD / MMC host controller (0-based).
*
*  Return value
*    Number of sectors that can be written at once. The function has
*    to return 0 if the feature is not supported.
*/
static U16 _HW_GetMaxWriteBurstFill(U8 Unit) {
    FS_USE_PARA(Unit);
    return 0;
}

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
const FS_MMC_HW_TYPE_CM FS_MMC_HW_CM = {
  _HW_Init,
  _HW_Delay,
  _HW_IsPresent,
  _HW_IsWriteProtected,
  _HW_SetMaxSpeed,
  _HW_SetResponseTimeOut,
  _HW_SetReadDataTimeOut,
  _HW_SendCmd,
  _HW_GetResponse,
  _HW_ReadData,
  _HW_WriteData,
  _HW_SetDataPointer,
  _HW_SetBlockLen,
  _HW_SetNumBlocks,
  _HW_GetMaxReadBurst,
  _HW_GetMaxWriteBurst,
  _HW_GetMaxWriteBurstRepeat,
  _HW_GetMaxWriteBurstFill,
  NULL, /* 1.8v signaling (UHS-I mode) is not supported. */
  NULL,
  NULL, /* Required to be implemented when UHS speed modes are supported. */

  /* Tuning functions need not be implemented since SDR50 mode does not require
   * tuning and other modes requiring tuning are not supported by SDHC block.
   */
  NULL,
  NULL,
  NULL,
  NULL
};

/*************************** End of file ****************************/

#endif /* #ifdef CY_IP_MXSDHC */
