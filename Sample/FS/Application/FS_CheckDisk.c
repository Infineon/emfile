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

File    : FS_CheckDisk.c
Purpose : Sample program demonstrating disk checking functionality.

Additional information:
  Preparations:
    The file system has to be formatted first otherwise the checking
    operation fails with an error.

  Expected behavior:
    Checks the consistency of the file system structure and displays
    a message indicating the result of operation. In case of an error
    the user interaction is required to decide how the error has to
    be handled. The checking will always fail on a RAMDISK volume since
    the format information is lost when the application is started and
    the RAM is initialized.

  Sample output:
    Start
    File system structure is consistent.
    Finished
*/

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "SEGGER.h"
#include "FS.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define VOLUME_NAME       ""
#define MAX_RECURSION     5
#define BUFFER_SIZE       2048      // The more space is reserved the faster the disk checking can run.

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32  _aBuffer[BUFFER_SIZE / 4];
static char _ac[256];
static int  _NumErrors;

/*********************************************************************
*
*       _OnError
*/
static int _OnError(int ErrCode, ...) {
  va_list      ParamList;
  const char * sFormat;
  char         c;

  //lint --e{438} -esym(530, ParamList)
  sFormat = FS_CheckDisk_ErrCode2Text(ErrCode);
  if (sFormat != NULL) {
    va_start(ParamList, ErrCode);
    SEGGER_vsnprintf(_ac, (int)sizeof(_ac), sFormat, ParamList);
    va_end(ParamList);
    FS_X_Log(_ac);
    FS_X_Log("\n");
  }
  if (ErrCode != FS_CHECKDISK_ERRCODE_CLUSTER_UNUSED) {
    FS_X_Log("  Do you want to repair this error? (y/n/a) ");
  } else {
    FS_X_Log("  * Convert lost cluster chain into file (y)\n"
             "  * Delete cluster chain                 (d)\n"
             "  * Do not repair                        (n)\n"
             "  * Abort                                (a) ");
    FS_X_Log("\n");
  }
  ++_NumErrors;
  c = (char)getchar();
  FS_X_Log("\n");
  if ((c == 'y') || (c == 'Y')) {
    return FS_CHECKDISK_ACTION_SAVE_CLUSTERS;
  } else if ((c == 'a') || (c == 'A')) {
    return FS_CHECKDISK_ACTION_ABORT;
  } else if ((c == 'd') || (c == 'D')) {
    return FS_CHECKDISK_ACTION_DELETE_CLUSTERS;
  } else {
    return FS_CHECKDISK_ACTION_DO_NOT_REPAIR;
  }
}

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
  int r;

  FS_X_Log("Start\n");
  FS_Init();
  _NumErrors = 0;
  //
  // Call the function repeatedly until all errors are fixed.
  //
  for (;;) {
    r = FS_CheckDisk(VOLUME_NAME, _aBuffer, sizeof(_aBuffer), MAX_RECURSION, _OnError);
    if (r != FS_CHECKDISK_RETVAL_RETRY) {
      if (r < 0) {
        FS_X_Log("File system access error.\n");
        _NumErrors++;
      }
      break;
    }
  }
  if (_NumErrors == 0) {
    FS_X_Log("File system structure is consistent.\n");
  }
  FS_X_Log("Finished\n");
  for (;;) {
    ;
  }
}

/*************************** End of file ****************************/

