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

File    : FS_SD_FailSafety.c
Purpose : Sample program demonstrating the usage of journaling.

Additional information:
  Preparations:
    This sample works only with an SD card as storage device.
    It has to be built with FS_SUPPORT_JOURNAL set to 1.
    The hardware must provide a way to reliably detect the removal
    and insertion of the SD card. Typically, this is realized via
    a mechanical contact built into the SD card slot that is connected
    to a GPIO port.

  Expected behavior:
    The sample initializes the SD card if required by formatting it
    and by creating the journal file. It then enters an endless loop
    in which alternatively deletes and then creates a specified number
    of files. The user is expected to remove and insert the card in order
    to simulate a power failure. Any file system corruption will be reported
    during the disk checking operation that is performed after the insertion
    of the SD card.

  Sample output:
    Start
    Create journal...OK (Created)
    SD card is present
    Check disk...OK
    Perform file system operations
    Delete files
    Create files
    Delete files
    Create files
    Delete files
    Create files
    SD card has been removed
    Wait for the SD card to be inserted
    SD card is present
    Check disk...OK
    Perform file system operations
    Delete files
    Create files
*/

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include <stdarg.h>
#include <string.h>
#include "SEGGER.h"
#include "FS.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define VOLUME_NAME       "mmc:0:"
#define MAX_RECURSION     5
#define BUFFER_SIZE       2048
#define NUM_FILES         20
#define FILE_SIZE         (64u * 1024u)
#define FILE_NAME         VOLUME_NAME"\\Test%d.txt"
#if FS_SUPPORT_JOURNAL
  #define JOURNAL_SIZE    (100u * 512u)
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32  _aBuffer[BUFFER_SIZE / 4];   // The more space is used the faster the disk checking can run.
static char _ac[256];
static int  _NumErrors;

/*********************************************************************
*
*       _CreateFiles
*/
static int _CreateFiles(void) {
  int       i;
  int       Status;
  FS_FILE * pFile;
  U32       FileSize;
  U32       NumBytesAtOnce;

  FS_X_Log("Create files\n");
  for (i = 0; i < NUM_FILES; ++i) {
    Status = FS_GetVolumeStatus(VOLUME_NAME);
    if (Status != FS_MEDIA_IS_PRESENT) {
      return 1;
    }
    SEGGER_snprintf(_ac, (int)sizeof(_ac), FILE_NAME, i);
    memset(_aBuffer, i, sizeof(_aBuffer));
    pFile = FS_FOpen(_ac, "w");
    if (pFile == NULL) {
      return 1;
    }
    FileSize = FILE_SIZE;
    do {
      NumBytesAtOnce = SEGGER_MIN(FileSize, sizeof(_aBuffer));
      (void)FS_Write(pFile, _aBuffer, NumBytesAtOnce);
      FileSize -= NumBytesAtOnce;
    } while (FileSize != 0u);
    (void)FS_FClose(pFile);
  }
  return 0;
}

/*********************************************************************
*
*       _DeleteFiles
*/
static int _DeleteFiles(void) {
  int i;
  int Status;

  FS_X_Log("Delete files\n");
  for (i = 0; i < NUM_FILES; ++i) {
    Status = FS_GetVolumeStatus(VOLUME_NAME);
    if (Status != FS_MEDIA_IS_PRESENT) {
      return 1;
    }
    SEGGER_snprintf(_ac, (int)sizeof(_ac), FILE_NAME, i);
    (void)FS_Remove(_ac);
  }
  return 0;
}

/*********************************************************************
*
*       _OnError
*/
static int _OnError(int ErrCode, ...) {
  va_list      ParamList;
  const char * sFormat;

  //lint --e{438} -esym(530, ParamList)
  sFormat = FS_CheckDisk_ErrCode2Text(ErrCode);
  if (sFormat != NULL) {
    va_start(ParamList, ErrCode);
    SEGGER_vsnprintf(_ac, (int)sizeof(_ac), sFormat, ParamList);
    va_end(ParamList);
    FS_X_Log(_ac);
    FS_X_Log("\n");
  }
  ++_NumErrors;
  //
  // Stop the checking after the first error to make the sample application more responsive.
  //
  return FS_CHECKDISK_ACTION_ABORT;
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
  int Status;

  FS_X_Log("Start\n");
  FS_Init();
  //
  // The journal works correctly only when the file write mode is set to SAFE.
  //
  FS_SetFileWriteMode(FS_WRITEMODE_SAFE);
  //
  // Wait here for the SD card to be inserted.
  //
  Status = FS_GetVolumeStatus(VOLUME_NAME);
  if (Status != FS_MEDIA_IS_PRESENT) {
    FS_X_Log("Wait for the SD card to be inserted\n");
    for (;;) {
      Status = FS_GetVolumeStatus(VOLUME_NAME);
      if (Status == FS_MEDIA_IS_PRESENT) {
        break;
      }
    }
  }
  //
  // Format the storage device if required.
  //
  if (FS_IsHLFormatted(VOLUME_NAME) == 0) {
    FS_X_Log("High-level format...");
    r = FS_Format(VOLUME_NAME, NULL);
    if (r == 0) {
      FS_X_Log("OK\n");
    } else {
      SEGGER_snprintf(_ac, (int)sizeof(_ac), "ERROR (%s)\n", FS_ErrorNo2Text(r));
      FS_X_Log(_ac);
    }
  }
#if FS_SUPPORT_JOURNAL
  FS_X_Log("Create journal...");
  r = FS_JOURNAL_Create(VOLUME_NAME, JOURNAL_SIZE);
  if (r < 0) {
    SEGGER_snprintf(_ac, (int)sizeof(_ac), "ERROR (%s)\n", FS_ErrorNo2Text(r));
    FS_X_Log(_ac);
  } else {
    if (r == 0) {
      FS_X_Log("OK (Created)\n");
    } else {
      FS_X_Log("OK (Already exists)\n");
    }
  }
#else
  FS_X_Log("ERROR: The support for journal is not enabled.\n");
#endif // FS_SUPPORT_JOURNAL
  //
  // Error and warning messages are generated when the SD card
  // is removed while the file system is trying to access it.
  // Disable all the debug messages to avoid confusion.
  //
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)
  FS_SetErrorFilter(0);
#endif
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS)
  FS_SetWarnFilter(0);
#endif
  //
  // Stay in this loop forever and wait for the SD card to be removed
  // and inserted again.
  //
  for (;;) {
    //
    // Wait here for the SD card to be inserted.
    //
    Status = FS_GetVolumeStatus(VOLUME_NAME);
    if (Status == FS_MEDIA_IS_PRESENT) {
      //
      // Make sure that the SD card can be accessed.
      //
      r = FS_Mount(VOLUME_NAME);
      if (r <= 0) {
        Status = FS_MEDIA_NOT_PRESENT;
      }
    }
    if (Status != FS_MEDIA_IS_PRESENT) {
      FS_X_Log("Wait for the SD card to be inserted\n");
      for (;;) {
        Status = FS_GetVolumeStatus(VOLUME_NAME);
        if (Status == FS_MEDIA_IS_PRESENT) {
          //
          // Make sure that the SD card can be accessed.
          //
          r = FS_Mount(VOLUME_NAME);
          if (r > 0) {
            break;
          }
          FS_UnmountForced(VOLUME_NAME);
        }
      }
    }
    FS_X_Log("SD card is present\n");
    //
    // Check the consistency of the file system structure.
    //
    FS_X_Log("Check disk...");
    _NumErrors = 0;
    for (;;) {
      r = FS_CheckDisk(VOLUME_NAME, _aBuffer, sizeof(_aBuffer), MAX_RECURSION, _OnError);
      if (r != FS_CHECKDISK_RETVAL_RETRY) {
        if (r < 0) {
          _NumErrors++;
        }
        break;
      }
    }
    if (_NumErrors == 0) {
      FS_X_Log("OK\n");
    } else {
      FS_UnmountForced(VOLUME_NAME);
      SEGGER_snprintf(_ac, (int)sizeof(_ac), "ERROR (%d detected)\n", _NumErrors);
      FS_X_Log(_ac);
    }
    //
    // Perform operations that modify the file system structure
    // such as creating and deleting files. Quit the loop when
    // the SD card is removed.
    //
    FS_X_Log("Perform file system operations\n");
    for (;;) {
      r = _DeleteFiles();
      if (r == 0) {
        r = _CreateFiles();
      }
      if (r != 0) {
        FS_X_Log("SD card has been removed\n");
        FS_UnmountForced(VOLUME_NAME);
        break;
      }
    }
  }
}

/*************************** End of file ****************************/

