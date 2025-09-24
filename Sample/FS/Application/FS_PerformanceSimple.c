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

File    : FS_PerformanceSimple.c
Purpose : Sample program that can be used to measure the performance
          of the file system.

Additional information:
  Preparations:
    Works out-of-the-box with any storage device.

  Expected behavior:
    Measures the speed at witch the file system can write and read blocks
    of data to and from a file. The size of the file as well as the number
    of bytes that have to be written at once are configurable.

    The application always formats the storage device to make sure that
    the measurements are not influenced by the data already stored on the
    file system.

  Sample output:
    Start
    High-level format
    Writing 4 chunks of 2048 KiB.......OK
    Reading 4 chunks of 2048 KiB.......OK

    W Speed: 9183 KiB/s
    R Speed: 18123 KiB/s
    Finished

  Notes:
    The sample application may report a write or read speed of 0
    if a very fast storage device is used for the test such as a RAM disk.
    This is normal behavior and the reason for this is that the accuracy
    of the time base used for the measurements is limited to 1 ms.
    This limitation can be removed by increasing the size of the work buffer
    via the BLOCK_SIZE configuration define.
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
#ifndef   FILE_SIZE
  #define FILE_SIZE             (8192L * 1024L)       // Size of the file in bytes to be used for testing.
#endif

#ifndef   BLOCK_SIZE
  #define BLOCK_SIZE            (8 * 1024L)           // Block size for individual read / write operation in bytes.
#endif

#ifndef   NUM_BLOCKS_MEASURE
  #define NUM_BLOCKS_MEASURE    (64)                  // Number of blocks for individual measurement
#endif

#ifndef   VOLUME_NAME
  #define VOLUME_NAME           ""                    // Name of the volume to be used for testing.
#endif

#ifndef   FILE_NAME
  #define FILE_NAME             "SEGGER.txt"          // Name of the file to be used for testing.
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct {
  const char * sName;
  I32          Min;
  I32          Max;
  I32          Av;
  I32          Sum;
  I32          NumSamples;
  U32          NumBytes;
} RESULT;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32    _aBuffer[BLOCK_SIZE / 4];
static RESULT _aResult[2];
static int    _TestNo;
static char   _ac[512];

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _WriteFile
*
*  Function description
*    Measures the write time.
*/
static I32 _WriteFile(FS_FILE * pFile, const void * pData, U32 NumBytes, U32 NumBlocksMeasure) {
  I32 t;
  U32 i;

  t = (I32)FS_X_OS_GetTime();
  for (i = 0; i < NumBlocksMeasure; i++) {
    (void)FS_Write(pFile, pData, NumBytes);
  }
  return (I32)FS_X_OS_GetTime() - t;
}


/*********************************************************************
*
*       _ReadFile
*
*  Function description
*    Measures the read performance.
*/
static I32 _ReadFile(FS_FILE * pFile, void * pData, U32 NumBytes, U32 NumBlocksMeasure) {
  I32 t;
  U32 i;

  t = (I32)FS_X_OS_GetTime();
  for (i = 0; i < NumBlocksMeasure; i++) {
    (void)FS_Read(pFile, pData, NumBytes);
  }
  return (I32)FS_X_OS_GetTime() - t;
}
/*********************************************************************
*
*       _StartTest
*/
static void _StartTest(const char * sName, U32 NumBytes) {
  RESULT * pResult;

  if ((_TestNo + 1) < (int)SEGGER_COUNTOF(_aResult)) {
    pResult = &_aResult[++_TestNo];
    pResult->sName      = sName;
    pResult->Min        =  0x7fffffff;
    pResult->Max        = -0x7fffffff;
    pResult->NumSamples = 0;
    pResult->Sum        = 0;
    pResult->NumBytes   = NumBytes;
  }
}

/*********************************************************************
*
*       _StoreResult
*/
static void _StoreResult(I32 t) {
  RESULT * pResult;

  pResult = &_aResult[_TestNo];
  if (t > pResult->Max) {
    pResult->Max = t;
  }
  if (t < pResult->Min) {
    pResult->Min = t;
  }
  pResult->NumSamples++;
  pResult->Sum += (I32)t;
  pResult->Av   = pResult->Sum / pResult->NumSamples;
}

/*********************************************************************
*
*       _GetAverage
*/
static double _GetAverage(int Index) {
  RESULT   * pResult;
  double     v;
  unsigned   NumKBytes;

  pResult = &_aResult[Index];
  v = (double)pResult->Av;
  if ((U32)v == 0u) {
    return (double)0.0;
  }
  v = (double)1000.0 / v;
  NumKBytes = pResult->NumBytes >> 10;
  v = v * (double)NumKBytes;
  return v;
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
  int       i;
  U32       Space;
  unsigned  NumLoops;
  U32       NumBytes;
  U32       NumBytesAtOnce;
  FS_FILE * pFile;
  I32       t;
  int       r;
  U32       NumBlocksMeasure;
  char      acFileName[128];

  FS_X_Log("Start\n");
  FS_Init();
  _TestNo = -1;
  //
  // Check if we need to low-level format the volume
  //
  if (FS_IsLLFormatted(VOLUME_NAME) == 0) {
    FS_X_Log("Low-level format\n");
    (void)FS_FormatLow(VOLUME_NAME);
  }
  //
  // Volume is always high level formatted
  // before doing any performance tests.
  //
  FS_X_Log("High-level format\n");
#if FS_SUPPORT_FAT
  r = FS_FormatSD(VOLUME_NAME);
#else
  r = FS_Format(VOLUME_NAME, NULL);
#endif
  if (r == 0) {
    //
    // Configure the file system so that the
    // directory entry and the allocation table
    // are updated when the file is closed.
    //
    FS_SetFileWriteMode(FS_WRITEMODE_FAST);
    //
    // Fill the buffer with data.
    //
    memset(_aBuffer, (int)'a', sizeof(_aBuffer));
    //
    // Get some general info.
    //
    Space            = FS_GetVolumeFreeSpace(VOLUME_NAME);
    Space            = SEGGER_MIN(Space, (U32)FILE_SIZE);
    NumBytesAtOnce   = BLOCK_SIZE;
    NumBlocksMeasure = NUM_BLOCKS_MEASURE;
    for (;;) {
      NumBytes = NumBytesAtOnce * NumBlocksMeasure;
      if (NumBytes <= Space) {
        break;
      }
      NumBytesAtOnce   >>= 1;
      NumBlocksMeasure >>= 1;
    }
    NumLoops = Space / NumBytes;
    if (NumLoops != 0u) {
      //
      // Create file of full size.
      //
      _StartTest("W", NumBytes);
      SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", VOLUME_NAME, FS_DIRECTORY_DELIMITER, FILE_NAME);
      pFile = FS_FOpen(acFileName, "w");
      //
      // Preallocate the file, setting the file pointer to the highest position
      // and declare it as the end of the file.
      //
      (void)FS_FSeek(pFile, (I32)Space, FS_SEEK_SET);
      (void)FS_SetEndOfFile(pFile);
      //
      // Set file position to the beginning.
      //
      (void)FS_FSeek(pFile, 0, FS_SEEK_SET);
      //
      // Check write performance with clusters/file size preallocated.
      //
      SEGGER_snprintf(_ac, (int)sizeof(_ac), "Writing %d chunks of %lu KiB...", NumLoops, NumBytes >> 10);
      FS_X_Log(_ac);
      for (i = 0; i < (int)NumLoops ; i++) {
        t = _WriteFile(pFile, _aBuffer, NumBytesAtOnce, NumBlocksMeasure);
        _StoreResult(t);
        FS_X_Log(".");
      }
      FS_X_Log("OK\n");
      (void)FS_FClose(pFile);
      //
      // Check read performance.
      //
      _StartTest("R", NumBytes);
      SEGGER_snprintf(_ac, (int)sizeof(_ac), "Reading %d chunks of %lu KiB..." , NumLoops, NumBytes >> 10);
      FS_X_Log(_ac);
      pFile = FS_FOpen(acFileName, "r");
      for (i = 0; i < (int)NumLoops; i++) {
        t = _ReadFile(pFile, _aBuffer, NumBytesAtOnce, NumBlocksMeasure);
        _StoreResult(t);
        FS_X_Log(".");
      }
      FS_X_Log("OK\n\n");
      (void)FS_FClose(pFile);
      (void)FS_Remove(acFileName);
      //
      // Show results for performance list.
      //
      for (i = 0; i <= _TestNo; i++) {
        SEGGER_snprintf(_ac, (int)sizeof(_ac), "%s Speed: %d KiB/s\n", _aResult[i].sName, (int)_GetAverage(i));
        FS_X_Log(_ac);
      }
    } else {
      FS_X_Log("ERROR: Not enough free space available on the storage.\n");
    }
    FS_Unmount(VOLUME_NAME);
  } else {
    FS_X_Log("ERROR: Volume could not be formatted!\n");
  }
  FS_X_Log("Finished\n");
  for (;;) {
    ;
  }
}

/*************************** End of file ****************************/
