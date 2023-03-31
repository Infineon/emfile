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

File        : FS_ConfigNOR_BM_SPIFI.c
Purpose     : Configuration file for quad SPI NOR flashes.
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "FS.h"
#include "FS_NOR_HW_SPIFI.h"
#include "cybsp.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define ALLOC_SIZE                  0x2000        // Size of memory dedicated to the file system. This value should be fine tuned according for your system.
#define FLASH_BASE_ADDR             0x00000000    // Base address of the NOR flash device to be used as storage.
#define FLASH_START_ADDR            0x00000000    // Start address of the first sector be used as storage. If the entire chip is used for file system, it is identical to the base address.
#define FLASH_SIZE                  0x04000000    // Number of bytes to be used for storage
#define BYTES_PER_SECTOR            512           // Logical sector size

#define QSPI_BUS_FREQ_HZ            50000000      // QSPI bus frequency in Hz
#define QSPI_DEFAULT_INTR_PRIORITY  7             // Default interrupt priority for QSPI

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
static U32 _aMemBlock[ALLOC_SIZE / 4];

static FS_NOR_HW_SPIFI_Config_t QspiConfig =
{
    .PinSet [0u] =
    {
        .io[0u] = CYBSP_QSPI_D0,
        .io[1u] = CYBSP_QSPI_D1,
        .io[2u] = CYBSP_QSPI_D2,
        .io[3u] = CYBSP_QSPI_D3,
        .io[4u] = NC,
        .io[5u] = NC,
        .io[6u] = NC,
        .io[7u] = NC,
        .ssel  = CYBSP_QSPI_SS,
    },
    .PinSet [1u] = { .io[0u] = NC, .io[1u] = NC, .io[2u] = NC, .io[3u] = NC, .io[4u] = NC, .io[5u] = NC, .io[6u] = NC, .io[7u] = NC, .ssel  = NC, },
    .PinSet [2u] = { .io[0u] = NC, .io[1u] = NC, .io[2u] = NC, .io[3u] = NC, .io[4u] = NC, .io[5u] = NC, .io[6u] = NC, .io[7u] = NC, .ssel  = NC, },
    .PinSet [3u] = { .io[0u] = NC, .io[1u] = NC, .io[2u] = NC, .io[3u] = NC, .io[4u] = NC, .io[5u] = NC, .io[6u] = NC, .io[7u] = NC, .ssel  = NC, },
    .Sclk = CYBSP_QSPI_SCK,
    .Clk = NULL,
    .NumMem = 1,
    .HFClockFreqHz = QSPI_BUS_FREQ_HZ,
    .DataRate = CYHAL_QSPI_DATARATE_SDR,

#if defined(COMPONENT_RTOS_AWARE)
    .QspiIntrPriority = QSPI_DEFAULT_INTR_PRIORITY  /* Interrupt priority for the QSPI block. */
#endif
};

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

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
  FS_NOR_BM_Configure(0, FLASH_BASE_ADDR, FLASH_START_ADDR, FLASH_SIZE);
  FS_NOR_BM_SetSectorSize(0, BYTES_PER_SECTOR);

#if FS_NOR_SUPPORT_FAIL_SAFE_ERASE
  FS_NOR_BM_SetFailSafeErase(0, 1);
#endif // FS_NOR_SUPPORT_FAIL_SAFE_ERASE

  //
  // Configure the NOR physical layer.
  //
  FS_NOR_SPIFI_Allow2bitMode(0, 1);
  FS_NOR_SPIFI_Allow4bitMode(0, 1);

  // Configure the HW layer and add it.
  (void) FS_NOR_HW_SPIFI_Configure(&QspiConfig);
  FS_NOR_SPIFI_SetHWType(0, &FS_NOR_HW_SPIFI);

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
