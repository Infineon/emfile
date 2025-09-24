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
-------------------------- END-OF-HEADER -----------------------------

File    : FS_STORAGE_Start.c
Purpose : Start application for the storage layer of the file system.

Additional information:
  Preparations:
    Works out-of-the-box with any storage device type.

  Expected behavior:
    The sample application formats the storage device if required,
    shows the information about the organization of the volume such
    as number of sectors and sector size and then it writes and
    verifies the contents of a logical sector. The index of the
    logical sector that has to be modified can be specified via
    the SECTOR_INDEX configuration define.

  Sample output:
    Start
    Device info:
      Volume name:       ""
      Number of sectors: 7308
      Sector size:       512 bytes
    Write, read and verify sector 0
    Finished
*/

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include <string.h>
#include "FS.h"
#include "SEGGER.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define VOLUME_NAME         ""
#define MAX_SECTOR_SIZE     2048
#define SECTOR_INDEX        0

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32  _aBuffer[MAX_SECTOR_SIZE / 4];
static char _ac[256];

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       MainTask
*/
#ifdef __cplusplus
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif
void MainTask(void);
#ifdef __cplusplus
}
#endif
void MainTask(void) {
  int           r;
  FS_DEV_INFO   DevInfo;
  U32         * pData32;
  U32           Cnt;
  U32           NumLoops;
  U32           BytesPerSector;

  FS_X_Log("Start\n");
  //
  // Initialize file system.
  //
  (void)FS_STORAGE_Init();
  //
  // Perform a low-level format if required.
  //
  if (FS_IsLLFormatted(VOLUME_NAME) == 0) {
    r = FS_FormatLow(VOLUME_NAME);
    if (r < 0) {
      FS_X_Log("ERROR: Low-level format failed.\n");
      goto Done;
    }
    FS_X_Log("Low-level format\n");
  }
  //
  // Get and show information about the storage device.
  //
  memset(&DevInfo, 0, sizeof(DevInfo));
  r = FS_STORAGE_GetDeviceInfo(VOLUME_NAME, &DevInfo);
  if (r != 0) {
    FS_X_Log("ERROR: Could not get device info.\n");
    goto Done;
  }
  BytesPerSector = DevInfo.BytesPerSector;
  FS_X_Log("Device info:\n");
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "  Volume name:       \"%s\"\n",    VOLUME_NAME);
  FS_X_Log(_ac);
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "  Number of sectors: %lu\n",       DevInfo.NumSectors);
  FS_X_Log(_ac);
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "  Sector size:       %lu bytes\n", BytesPerSector);
  FS_X_Log(_ac);
  //
  // Check if the sector buffer is large enough.
  //
  if (BytesPerSector > sizeof(_aBuffer)) {
    FS_X_Log("ERROR: Sector buffer too small.\n");
    goto Done;
  }
  //
  // Write some data to a sector, read it back and verify it.
  //
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "Write, read and verify sector %lu\n", (long unsigned)SECTOR_INDEX);
  FS_X_Log(_ac);
  NumLoops = (U32)DevInfo.BytesPerSector >> 2;
  pData32  = _aBuffer;
  Cnt      = 0;
  do {
    *pData32++ = Cnt++;
  } while (--NumLoops != 0u);
  r = FS_STORAGE_WriteSector(VOLUME_NAME, _aBuffer, SECTOR_INDEX);
  if (r != 0) {
    FS_X_Log("ERROR: Could not write sector data.\n");
    goto Done;
  }
  r = FS_STORAGE_ReadSector(VOLUME_NAME, _aBuffer, SECTOR_INDEX);
  if (r != 0) {
    FS_X_Log("ERROR: Could not read sector data.\n");
    goto Done;
  }
  NumLoops = (U32)DevInfo.BytesPerSector >> 2;
  pData32  = _aBuffer;
  Cnt      = 0;
  do {
    if (*pData32 != Cnt) {
      FS_X_Log("ERROR: Verification failed.\n");
      break;
    }
    ++pData32;
    ++Cnt;
  } while (--NumLoops != 0u);
Done:
  FS_STORAGE_Unmount(VOLUME_NAME);
  FS_X_Log("Finished\n");
  for (;;) {
    ;
  }
}

/*************************** End of file ****************************/
