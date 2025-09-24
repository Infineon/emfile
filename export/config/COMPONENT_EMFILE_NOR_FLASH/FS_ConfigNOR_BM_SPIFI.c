/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2003 - 2023  SEGGER Microcontroller GmbH                 *
*                                                                    *
*       www.segger.com     Support: support_emfile@segger.com        *
*                                                                    *
**********************************************************************
*                                                                    *
*       emFile * File system for embedded applications               *
*                                                                    *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product for in-house use.         *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       emFile version: V5.22.0                                      *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              Cypress Semiconductor Corp, 198 Champion Ct., San Jose, CA 95134, USA
Licensed SEGGER software: emFile
License number:           FS-00227
License model:            Cypress Services and License Agreement, signed November 17th/18th, 2010
                          and Amendment Number One, signed December 28th, 2020 and February 10th, 2021
                          and Amendment Number Three, signed May 2nd, 2022 and May 5th, 2022
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5, PSoC6)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2010-12-01 - 2023-07-27
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : FS_ConfigNOR_BM_SPIFI.c
Purpose     : Configuration file for quad SPI NOR flashes.
-------------------------- END-OF-HEADER -----------------------------
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "FS.h"
#include "FS_NOR_HW_SPIFI.h"
#include "mtb_hal_memoryspi.h"
#include "cybsp.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define ALLOC_SIZE                  (0x2000u)        // Size of memory dedicated to the file system. This value should be fine tuned according for your system.
#define FLASH_BASE_ADDR             (0x00000000u)    // Base address of the NOR flash device to be used as storage.
#define FLASH_START_ADDR            (0x00A00000u)    // The start address of the first sector be used as storage. If the entire chip is used for the file system, the start address is identical to the base address.
#if defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION < 2u)
#define FS_FLASH_SIZE               (0x04000000u)    // The number of bytes to use for the storage.
#else /* CY_IP_MXS22SRSS */
#define FS_FLASH_SIZE               (0x00300000u)    // The number of bytes to use for the storage.
//
// The erase sector size for S25FS128S
//
#define FS_FLASH_ERASE_SECTOR_SIZE  (0x10000u)
#endif /* #if defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION < 2u) */
#define BYTES_PER_SECTOR            (512u)           // The size of the logical sector.

/*********************************************************************
*
*       XIP support section. The implementation support S25FS128S
*       non-volatile memory but can be adapted for others.
*
**********************************************************************
*/
#if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH)

/* Timeout for operations with memory like erase, program, read.
 * Select it as the worst case number for longest operation.
  */
#define TIMEOUT_US                              (1000000U)

/* Command to read the Status Register 1 */
#define S25FS128S_READ_STATUS_REG_1_CMD         (0x05)
/* The position of WIP bit in Register */
#define REG_1_STS_WIP                           (1U << 0U)

/* Buffer to store the Register value */
static uint8_t RxBuffer[1U];

/* Command configuration for reading Status Register 1 */
static mtb_hal_memoryspi_command_t s25fs128sReadReg1 =
{
    .instruction.bus_width      = MTB_HAL_MEMORYSPI_CFG_BUS_SINGLE,
    .instruction.data_rate      = MTB_HAL_MEMORYSPI_DATARATE_SDR,

    .instruction.two_byte_cmd   = false,
    .instruction.value          = S25FS128S_READ_STATUS_REG_1_CMD,
    .instruction.disabled       = false,

    .address.disabled           = true,

    .mode_bits.disabled         = true,

    .dummy_cycles.dummy_count   = 0,
    .data.bus_width             = MTB_HAL_MEMORYSPI_CFG_BUS_SINGLE,
    .data.data_rate             = MTB_HAL_MEMORYSPI_DATARATE_SDR,
};

/*********************************************************************
*
*       wipCallback
*
*  Function description
*    The function blocks the code execution until the non-volatile memory completes
*    all operations (for example, write, erase, etc.) or a timeout occurs.
*
*/
void wipCallback(mtb_hal_memoryspi_t * halObj)
{
    for (uint32_t i = 0U; i < TIMEOUT_US; i++)
    {
        (void)mtb_hal_memoryspi_transfer(halObj, &s25fs128sReadReg1, 0U, NULL, 0U, RxBuffer, 1U);

        if ((RxBuffer[0U] & REG_1_STS_WIP) == 0U)
        {
            break;
        }

        /* The PDL function is used, as it is stored in the internal memory */
        Cy_SysLib_DelayUs(1U);
    }
}
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

/*********************************************************************
*
*       Memory pool used for semi-dynamic allocation.
*/
static U32 _aMemBlock[ALLOC_SIZE / 4u];
static mtb_hal_memoryspi_t memspi_obj;
#if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH)
static U8 DataBuffer[BYTES_PER_SECTOR];
static U8 ParamBuffer[8U];
#endif /* #if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH) */

static FS_NOR_HW_SPIFI_Config_t MemConfig =
{
    .PinSet = {MTB_HAL_MEMORYSPI_DATA_SELECT_0, MTB_HAL_MEMORYSPI_DATA_SELECT_0, MTB_HAL_MEMORYSPI_DATA_SELECT_0, MTB_HAL_MEMORYSPI_DATA_SELECT_0},
    .NumMem = 1u,
    .DataRate = MTB_HAL_MEMORYSPI_DATARATE_SDR,
    .Obj = &memspi_obj,
    .ChipSelect = {MTB_HAL_MEMORYSPI_CHIP_SELECT_1, MTB_HAL_MEMORYSPI_CHIP_SELECT_0 , MTB_HAL_MEMORYSPI_CHIP_SELECT_0, MTB_HAL_MEMORYSPI_CHIP_SELECT_0},

#if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH)
    .WipCallback = wipCallback,
    .pDataBuffer = DataBuffer,
    .pParamBuffer = ParamBuffer,
#endif /* #if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH) */
};

/*********************************************************************
*
*       HW initialization weak function
*
**********************************************************************
*/
void FS_NOR_HW_SPIFI_ConfigureHw(mtb_hal_memoryspi_t *memspi_obj);

/*********************************************************************
*
*       FS_NOR_HW_SPIFI_ConfigureHw
*
*  Function description
*    Call this function in FS_X_AddDevices. This function must
*    configure memory SPI HW using PDL and HAL APIs.
*/
__WEAK void FS_NOR_HW_SPIFI_ConfigureHw(mtb_hal_memoryspi_t *memspi_obj)
{
    (void) memspi_obj;
}


/*********************************************************************
*
*       FS_X_AddDevices
*
*  Function description
*    This function is called by the FS during FS_Init().
*    It is supposed to add all devices, using primarily FS_AddDevice().
*
*  Note
*    (1) Other API functions may NOT be called, since this function is called
*        during initialization. The devices are not yet ready at this point.
*/
void FS_X_AddDevices(void) {
  //
  // Give file system memory to work with.
  //
  FS_AssignMemory(&_aMemBlock[0], sizeof(_aMemBlock));
  //
  // Configure the size of the logical sector and activate the file buffering.
  //
  FS_SetMaxSectorSize(BYTES_PER_SECTOR);
#if FS_SUPPORT_FILE_BUFFER
  FS_ConfigFileBufferDefault(BYTES_PER_SECTOR, FS_FILE_BUFFER_WRITE);
#endif
  //
  // Add and configure the NOR driver.
  //
  FS_AddDevice(&FS_NOR_BM_Driver);
  FS_NOR_BM_SetPhyType(0, &FS_NOR_PHY_SPIFI);
  FS_NOR_BM_Configure(0, FLASH_BASE_ADDR, FLASH_START_ADDR, FS_FLASH_SIZE);
  FS_NOR_BM_SetSectorSize(0, BYTES_PER_SECTOR);

#if FS_NOR_SUPPORT_FAIL_SAFE_ERASE
  FS_NOR_BM_SetFailSafeErase(0, 1);
#endif // FS_NOR_SUPPORT_FAIL_SAFE_ERASE

  //
  // Configure the NOR physical layer.
  //
  FS_NOR_SPIFI_Allow2bitMode(0, 1);
  FS_NOR_SPIFI_Allow4bitMode(0, 1);

  /* Initialize HW here */
  FS_NOR_HW_SPIFI_ConfigureHw(&memspi_obj);

  // Configure the HW layer and add it.
  (void) FS_NOR_HW_SPIFI_Configure(&MemConfig);
  FS_NOR_SPIFI_SetHWType(0, &FS_NOR_HW_SPIFI);
#if defined (CY_IP_MXS22SRSS)
  FS_NOR_SPIFI_SetSectorSize(0U, FS_FLASH_ERASE_SECTOR_SIZE);
#endif /* #if defined (CY_IP_MXS22SRSS) */

  FS_NOR_SPIFI_SetDeviceList(0, &FS_NOR_SPI_DeviceList_All);
}

/*********************************************************************
*
*       FS_X_GetTimeDate
*
*  Function description
*    Current time and date in a format suitable for the file system.
*
*  Additional information
*    Bit 0-4:   2-second count (0-29)
*    Bit 5-10:  Minutes (0-59)
*    Bit 11-15: Hours (0-23)
*    Bit 16-20: Day of month (1-31)
*    Bit 21-24: Month of year (1-12)
*    Bit 25-31: Count of years from 1980 (0-127)
*/
U32 FS_X_GetTimeDate(void) {
  U32 r;
  U16 Sec, Min, Hour;
  U16 Day, Month, Year;

  Sec   = 0;        // 0 based.  Valid range: 0..59
  Min   = 0;        // 0 based.  Valid range: 0..59
  Hour  = 0;        // 0 based.  Valid range: 0..23
  Day   = 1;        // 1 based.    Means that 1 is 1. Valid range is 1..31 (depending on month)
  Month = 1;        // 1 based.    Means that January is 1. Valid range is 1..12.
  Year  = 0;        // 1980 based. Means that 2007 would be 27.
  r   = Sec / 2 + (Min << 5) + (Hour  << 11);
  r  |= (U32)(Day + (Month << 5) + (Year  << 9)) << 16;
  return r;
}

/*************************** End of file ****************************/
