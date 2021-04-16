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

File    : FS_SectorCache.c
Purpose : Demonstrates how to configure and use the sector cache.

Additional information:
  Preparations:
    Works out-of-the-box with any storage device.

  Expected behavior:
    Measures the time it takes to create a number of files with
    and without the sector cache enabled. The results are output
    on the debug console.

  Sample output:
    Start
    Cache disabled
    Creation of 50 files took: 60 ms
    Cache enabled
    Creation of 50 files took: 18 ms
    Cache flush took: 0 ms
    Cache disabled
    Creation of 50 files took: 114 ms
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
#include "FS_OS.h"
#include "SEGGER.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define VOLUME_NAME   ""
#define NUM_FILES     32

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32  _aCache[0x400];
static char _aacFileName[NUM_FILES][13];
static char _ac[128];

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _CreateFiles
*/
static void _CreateFiles(void) {
  int       i;
  U32       Time;
  FS_FILE * pFile[NUM_FILES];

  Time = FS_X_OS_GetTime();
  for (i = 0; i < NUM_FILES; i++) {
    pFile[i] = FS_FOpen(&_aacFileName[i][0], "w");
  }
  Time = FS_X_OS_GetTime() - Time;
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "Creation of %d files took: %d ms\n", NUM_FILES, Time);
  FS_X_Log(_ac);
  for (i = 0; i < NUM_FILES; i++) {
    (void)FS_FClose(pFile[i]);
  }
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
  int i;
  U32 Time;

  FS_X_Log("Start\n");
  //
  // Initialize file system
  //
  FS_Init();
  //
  // Check if low-level format is required
  //
  (void)FS_FormatLLIfRequired(VOLUME_NAME);
  //
  // Check if volume needs to be high level formatted.
  //
  if (FS_IsHLFormatted(VOLUME_NAME) == 0) {
    FS_X_Log("High-level formatting\n");
    (void)FS_Format(VOLUME_NAME, NULL);
  }
  //
  // Prepare file names in advance.
  //
  for (i = 0; i < NUM_FILES; i++) {
    SEGGER_snprintf(_aacFileName[i], (int)sizeof(_aacFileName[0]), "file%.2d.txt", i);
  }
  //
  // Create and measure the time used to create the files with the cache enabled.
  //
  FS_X_Log("Cache disabled\n");
  _CreateFiles();
  //
  // Create and measure the time used to create the files.
  //
  (void)FS_AssignCache(VOLUME_NAME, _aCache, (int)sizeof(_aCache), FS_CACHE_RW);
  (void)FS_CACHE_SetMode(VOLUME_NAME, (int)FS_SECTOR_TYPE_MASK_ALL, (int)FS_CACHE_MODE_WB);
  FS_X_Log("Cache enabled\n");
  _CreateFiles();
  Time = FS_X_OS_GetTime();
  FS_CACHE_Clean(VOLUME_NAME);
  Time = FS_X_OS_GetTime() - Time;
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "Cache flush took: %d ms\n", Time);
  FS_X_Log(_ac);
  //
  // Create and measure the time used to create the files with the cache disabled.
  //
  FS_X_Log("Cache disabled\n");
  (void)FS_AssignCache(VOLUME_NAME, NULL, 0, FS_CACHE_NONE);
  _CreateFiles();
  FS_X_Log("Finished\n");
  for(;;) {
    ;
  }
}

/*************************** End of file ****************************/
