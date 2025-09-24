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

File    : FS_DirOperations.c
Purpose : Demonstrates the usage of API functions that operate
          on directories.

Additional information:
  Preparations:
    Works out-of-the-box with any storage device.

  Expected behavior:
    This sample creates three directories. In each directory three
    files are created. After creating the directories and files,
    the contents of each directory is shown.

  Sample output:
    Start
    High-level format...OK
    Create directory \Dir00
    Create files ...OK
    Create directory \Dir01
    Create files ...OK
    Create directory \Dir02
    Create files ...OK
    Contents of
    DIR00 (Dir) Attributes: ---- Size: 0
    Contents of \DIR00
      . (Dir) Attributes: ---- Size: 0
      .. (Dir) Attributes: ---- Size: 0
      FILE0000.TXT       Attributes: A--- Size: 19
      FILE0001.TXT       Attributes: A--- Size: 19
      FILE0002.TXT       Attributes: A--- Size: 19

    DIR01 (Dir) Attributes: ---- Size: 0
    Contents of \DIR01
      . (Dir) Attributes: ---- Size: 0
      .. (Dir) Attributes: ---- Size: 0
      FILE0000.TXT       Attributes: A--- Size: 19
      FILE0001.TXT       Attributes: A--- Size: 19
      FILE0002.TXT       Attributes: A--- Size: 19

    DIR02 (Dir) Attributes: ---- Size: 0
    Contents of \DIR02
      . (Dir) Attributes: ---- Size: 0
      .. (Dir) Attributes: ---- Size: 0
      FILE0000.TXT       Attributes: A--- Size: 19
      FILE0001.TXT       Attributes: A--- Size: 19
      FILE0002.TXT       Attributes: A--- Size: 19


    Finished
*/

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include <string.h>
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
#define NUM_DIRS          3
#define NUM_FILES         3

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static char _ac[512];

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
static void _CreateFiles(const char * sPath) {
  int       i;
  FS_FILE * pFile;
  char      acFileName[40];

  FS_X_Log("Create files...");
  for (i = 0; i < NUM_FILES; i++) {
    SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s\\file%.4d.txt", sPath, i);
    pFile = FS_FOpen(acFileName, "w");
    if (pFile != NULL) {
      (void)FS_Write(pFile, acFileName, strlen(acFileName));
      (void)FS_FClose(pFile);
      FS_X_Log(".");
    } else {
      FS_X_Log("ERROR\n");
      return;
    }
  }
  FS_X_Log("OK\n");
}

/*********************************************************************
*
*       _ShowDir
*
*/
static void _ShowDir(const char * sDirName, int MaxRecursion) {
  FS_FIND_DATA fd;
  char         acFileName[20];
  char         acDummy[20];
  int          NumBytes;
  int          r;

  NumBytes = MAX_RECURSION - MaxRecursion;
  memset(acDummy, (int)' ', (unsigned)NumBytes);
  acDummy[NumBytes] = '\0';
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "%sContents of %s \n", acDummy, sDirName);
  FS_X_Log(_ac);
  if (MaxRecursion != 0) {
    r = FS_FindFirstFile(&fd, sDirName, acFileName, (int)sizeof(acFileName));
    if (r == 0) {
      do {
        U8 Attr;

        Attr = fd.Attributes;
        SEGGER_snprintf(_ac, (int)sizeof(_ac), "%s %s %s Attributes: %s%s%s%s Size: %lu\n",
                                               acDummy, fd.sFileName,
                                               ((Attr & FS_ATTR_DIRECTORY) != 0u) ? "(Dir)" : "     ",
                                               ((Attr & FS_ATTR_ARCHIVE)   != 0u) ? "A" : "-",
                                               ((Attr & FS_ATTR_READ_ONLY) != 0u) ? "R" : "-",
                                               ((Attr & FS_ATTR_HIDDEN)    != 0u) ? "H" : "-",
                                               ((Attr & FS_ATTR_SYSTEM)    != 0u) ? "S" : "-",
                                               fd.FileSize);
        FS_X_Log(_ac);
        if ((Attr & FS_ATTR_DIRECTORY) != 0u) {
          char acDirName[256];
          //
          // Show contents of each directory in the root
          //
          if (*fd.sFileName != '.') {
            SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", sDirName, FS_DIRECTORY_DELIMITER, fd.sFileName);
            _ShowDir(acDirName, MaxRecursion - 1);
          }
        }

      } while (FS_FindNextFile(&fd) != 0);
      FS_FindClose(&fd);
    } else if (r == 1) {
      FS_X_Log("Empty directory");
    } else {
      SEGGER_snprintf(_ac, (int)sizeof(_ac), "ERROR: Unable to open directory %s\n", sDirName);
      FS_X_Log(_ac);
    }
    FS_X_Log("\n");
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
  int r;

  FS_X_Log("Start\n");
  FS_Init();
  (void)FS_FormatLLIfRequired(VOLUME_NAME);
  //
  // High level volume format
  //
  FS_X_Log("High-level format...");
#if FS_SUPPORT_FAT
  r = FS_FormatSD(VOLUME_NAME);
#else
  r = FS_Format(VOLUME_NAME, NULL);
#endif
  if (r != 0) {
    FS_X_Log("ERROR: Could not format storage device\n");
  } else {
    FS_X_Log("OK\n");
    //
    //  Create 3 folders
    //
    for (i = 0; i < NUM_DIRS; i++) {
      char acDirName[20];

      (void)SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%cDir%.2d", VOLUME_NAME, FS_DIRECTORY_DELIMITER, i);
      (void)SEGGER_snprintf(_ac, (int)sizeof(_ac), "Create directory %s\n", acDirName);
      FS_X_Log(_ac);
      r = FS_MkDir(acDirName);
      //
      // If directory has been successfully created
      // Create the files in that directory.
      //
      if (r == 0) {
        _CreateFiles(acDirName);
      } else {
        SEGGER_snprintf(_ac, (int)sizeof(_ac), "ERROR: Could not create the directory %s\n", acDirName);
        FS_X_Log(_ac);
      }
    }
    //
    // Show contents of root directory
    //
    _ShowDir(VOLUME_NAME, MAX_RECURSION);
  }
  FS_Unmount(VOLUME_NAME);
  FS_X_Log("Finished\n");
  for (;;) {
    ;
  }
}

/*************************** End of file ****************************/
