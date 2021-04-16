/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2003 - 2021  SEGGER Microcontroller GmbH                 *
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
*       emFile version: V5.6.1                                       *
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
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5, PSoC6)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2010-12-01 - 2022-07-27
Contact to extend SUA:    sales@segger.com
-------------------------- END-OF-HEADER -----------------------------

File    : FS_NOR_SectorInfo.c
Purpose : Displays information about all the physical sectors
          managed by the NOR driver.

Additional information:
  Preparations:
    The volume accessed by this application has to be located on a
    NOR flash device. The NOR flash device has to be accessed via
    the sector map NOR driver (FS_NOR_Driver).

  Expected behavior:
    The application performs the following operations for the number
    of times specified by the NUM_LOOPS configuration define:
    - opens a file in write mode
    - writes some data in it
    - closes the file
    - displays information about all the NOR physical sectors

  Sample output:
    Start
    NOR flash info:
      Physical sectors: 128
      Logical sectors:  14399
      Used sectors:     75
    Loop cycle: 0
    Sector info:
      Physical sector No.:      0
      Offset:                   0
      Size:                     65536
      Erase Count:              1
      Used logical sectors:     0
      Free logical sectors:     0
      Erasable logical sectors: 0
    Sector info:
      Physical sector No.:      1
      Offset:                   65536
      Size:                     65536
      Erase Count:              1
      Used logical sectors:     72
      Free logical sectors:     0
      Erasable logical sectors: 54
    Sector info:
      Physical sector No.:      2
      Offset:                   131072
      Size:                     65536
      Erase Count:              1
      Used logical sectors:     4
      Free logical sectors:     86
      Erasable logical sectors: 36
    Sector info:
      Physical sector No.:      3
      Offset:                   196608
      Size:                     65536
      Erase Count:              1
      Used logical sectors:     0
      Free logical sectors:     126
      Erasable logical sectors: 0

    [...]

    Sector info:
      Physical sector No.:      127
      Offset:                   8323072
      Size:                     65536
      Erase Count:              1
      Used logical sectors:     0
      Free logical sectors:     126
      Erasable logical sectors: 0
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
#define VOLUME_NAME       ""
#define UNIT_NO           0
#define NUM_LOOPS         10

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static char _acBuffer[0x400];
static char _ac[400];

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _ShowDiskInfo
*/
static void _ShowDiskInfo(FS_NOR_DISK_INFO * pDiskInfo) {
  FS_X_Log("NOR flash info: \n");
  (void)FS_NOR_GetDiskInfo(UNIT_NO, pDiskInfo);
  SEGGER_snprintf(_ac, (int)sizeof(_ac),
                  "  Physical sectors: %lu\n"
                  "  Logical sectors:  %lu\n"
                  "  Used sectors:     %lu\n", pDiskInfo->NumPhysSectors,
                                               pDiskInfo->NumLogSectors,
                                               pDiskInfo->NumUsedSectors);
  FS_X_Log(_ac);
}

/*********************************************************************
*
*       _ShowSectorInfo
*/
static void _ShowSectorInfo(FS_NOR_SECTOR_INFO * pSecInfo, U32 PhysSectorIndex) {
  FS_X_Log("Sector info: \n");
  FS_NOR_GetSectorInfo(UNIT_NO, PhysSectorIndex, pSecInfo);
  SEGGER_snprintf(_ac, (int)sizeof(_ac),
                  "  Physical sector No.:      %lu\n"
                  "  Offset:                   %lu\n"
                  "  Size:                     %lu\n"
                  "  Erase Count:              %lu\n"
                  "  Used logical sectors:     %d\n"
                  "  Free logical sectors:     %d\n"
                  "  Erasable logical sectors: %d\n", PhysSectorIndex,
                                                      pSecInfo->Off,
                                                      pSecInfo->Size,
                                                      pSecInfo->EraseCnt,
                                                      pSecInfo->NumUsedSectors,
                                                      pSecInfo->NumFreeSectors,
                                                      pSecInfo->NumEraseableSectors);
  FS_X_Log(_ac);
}

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
  int                  i;
  U32                  j;
  FS_NOR_DISK_INFO     DiskInfo;
  FS_NOR_SECTOR_INFO   SecInfo;
  FS_FILE            * pFile;

  FS_X_Log("Start\n");
  FS_Init();
  (void)FS_FormatLLIfRequired(VOLUME_NAME);
  //
  // Check if volume needs to be high level formatted.
  //
  if (FS_IsHLFormatted(VOLUME_NAME) == 0) {
    FS_X_Log("High level formatting\n");
    (void)FS_Format(VOLUME_NAME, NULL);
  }
  _ShowDiskInfo(&DiskInfo);
  memset(_acBuffer, (int)'a', sizeof(_acBuffer));
  for (i = 0; i < NUM_LOOPS; i++) {
    pFile = FS_FOpen("test.txt", "w");
    if (pFile != NULL) {
      (void)FS_Write(pFile, &_acBuffer[0], sizeof(_acBuffer));
      (void)FS_FClose(pFile);
      SEGGER_snprintf(_ac, (int)sizeof(_ac), "Loop cycle: %d\n", i);
      FS_X_Log(_ac);
      for(j = 0; j < DiskInfo.NumPhysSectors; j++) {
        _ShowSectorInfo(&SecInfo, j);
      }
    }
  }
  FS_X_Log("Finished\n");
  for (;;) {
    ;
  }
}


/*************************** End of file ****************************/

