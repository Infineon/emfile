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

File    : FS_Commander.c
Purpose : Interactive application that can be used to test different
          file system operations.

Additional information:
  Preparations:
    Works out-of-the-box with any storage device.

  Expected behavior:
    This application accepts and executes commands interactively. Each
    command performs different file system operations. The commands can
    be typed in via a terminal such as the Debug Terminal of SEGGER Embedded
    Studio. The commands and responses are exchanged either semihosting
    or SEGGER Real Time Transfer (RTT) which is the default. The selection
    of the data exchange mode is realized via the USE_RTT configuration define.

    The list of the supported commands can be queried by entering
    either the "?" or "help" command. The command names are case insensitive.

  Sample output:
    SEGGER FS Commander V5.10.0
    Type "?" or "help" for a list of commands
    emFile-nand:0:>dir
    TESTFILE.BIN               Attr: A--- C: 2021-10-05 18:42:40
                                          M: 2021-10-05 18:42:40 Size: 10000
    emFile-nand:0:>
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "FS.h"
#include "FS_OS.h"

#ifdef _WIN32
  #include <io.h>
#endif

#ifdef linux
  #include <unistd.h>
  #include <dlfcn.h>
#endif

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#ifndef   USE_MAIN_TASK
  #define USE_MAIN_TASK             1                           // Include or exclude MainTask() of this module.
#endif

#ifndef   USE_RTT
  #define USE_RTT                   1                           // Enable or disable the command exchange via RTT.
#endif

/*********************************************************************
*
*       #include section (conditional)
*
**********************************************************************
*/
#if USE_RTT
  #include "SEGGER_RTT.h"
#endif // USE_RTT

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#define APP_ERROR_NONE                  0
#define APP_ERROR_SYNTAX                1

#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)
  #define MAX_RECURSION_LEVEL_DEFAULT   5
  #define MAX_RECURSION_LEVEL           32u
  #define BYTES_PER_LINE                16u
#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

#if defined(_WIN32) || defined(linux)
  #define USE_SIMULATOR                 1
#else
  #define USE_SIMULATOR                 0
#endif

#if       USE_SIMULATOR
  #define BUFFER_SIZE                   (1024 * 1024)
#else
  #define BUFFER_SIZE                   2048
#endif

#if FS_OS_LOCKING
  #define FILE_NAME                     "SEGGER.dat"                // Name of the file used for the performance tests.
  #define FILE_SIZE                     (8uL * 1024uL * 1024uL)     // Size of the file used for the performance tests.
  #define MAX_BLOCK_SIZE                (8uL * 1024uL)              // Number of blocks to access at once during the performance tests.
  #define NUM_BLOCKS_TO_MEASURE         64                          // Number of times to access a block during the performance tests.
  #define MIN_TIME                      (-0x7fffffff)
  #define MAX_TIME                      (0x7fffffff)
#endif // FS_OS_LOCKING

/*********************************************************************
*
*       GET_INPUT_LINE()
*/
#if (USE_SIMULATOR == 0)
  #define GET_INPUT_LINE(pPrompt, pBuffer, BufferSize)          _TARGET_GetInputLine(pPrompt, pBuffer, BufferSize)
#else
#ifdef _WIN32
  #define GET_INPUT_LINE(pPrompt, pBuffer, BufferSize)          _WIN32_GetInputLine(pPrompt, pBuffer, BufferSize)
#endif
#ifdef linux
  #define GET_INPUT_LINE(pPrompt, pBuffer, BufferSize)          _LINUX_GetInputLine(pPrompt, pBuffer, BufferSize)
#endif
#endif // (USE_SIMULATOR == 0)

/*********************************************************************
*
*       IS_CONSOLE_INPUT()
*/
#if (USE_SIMULATOR == 0)
  #define IS_CONSOLE_INPUT()                                    _TARGET_IsConsoleInput()
#else
#ifdef _WIN32
  #define IS_CONSOLE_INPUT()                                    _WIN32_IsConsoleInput()
#endif
#ifdef linux
  #define IS_CONSOLE_INPUT()                                    _LINUX_IsConsoleInput()
#endif
#endif // (USE_SIMULATOR == 0)

/*********************************************************************
*
*       INIT_SYSTEM()
*/
#if (USE_SIMULATOR == 0)
  #define INIT_SYSTEM(sTitle)
#else
#ifdef _WIN32
  #define INIT_SYSTEM(sTitle)                                   _WIN32_InitSystem(sTitle)
#endif
#ifdef linux
  #define INIT_SYSTEM(sTitle)                                   _LINUX_InitSystem(sTitle)
#endif
#endif

/*********************************************************************
*
*       Local data types
*
**********************************************************************
*/

/*********************************************************************
*
*       COMMAND
*
*  Description
*    Command description.
*/
typedef struct {
  int      ( * pfHandler)(const char* s);   // Command handler.
  const char * sShortName;                  // Short name of command.
  const char * sLongName;                   // Long name of command.
  const char * sDescription;                // Description for command.
  const char * sSyntax;                     // Syntax of command.
} COMMAND;

/*********************************************************************
*
*       COMMAND_SET
*
*  Description
*    Command list.
*/
typedef struct {
  const COMMAND  * pCommand;                  // List of command descriptors.
  unsigned         NumItems;                  // Number of command descriptors in the list.
} COMMAND_SET;

#if FS_OS_LOCKING

/*********************************************************************
*
*       RESULT
*
*  Description
*    Performance test results.
*/
typedef struct {
  const char          * sName;
  I32                   Min;
  I32                   Max;
  I32                   Av;
  I32                   Sum;
  I32                   NumSamples;
  U32                   NumBytes;
  FS_STORAGE_COUNTERS   StorageCounter;
} RESULT;

#endif // FS_OS_LOCKING

#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)

/*********************************************************************
*
*       VALUE_TO_TEXT
*
*  Description
*    Maps a value to a text representation and the other way around.
*/
typedef struct {
  U32          Value;
  const char * sText;
} VALUE_TO_TEXT;

#endif // (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)

#if USE_SIMULATOR
#if defined(linux)
typedef char * READ_LINE  (const char *);
typedef int    ADD_HISTORY(const char *);
#endif // defined(linux)
#endif // USE_SIMULATOR

/*********************************************************************
*
*       Static const data
*
**********************************************************************
*/
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)

static const VALUE_TO_TEXT _aMTypeToText[] = {
  {FS_MTYPE_INIT,     "INIT"},
  {FS_MTYPE_API,      "API"},
  {FS_MTYPE_FS,       "FS"},
  {FS_MTYPE_STORAGE,  "STORAGE"},
  {FS_MTYPE_JOURNAL,  "JOURNAL"},
  {FS_MTYPE_CACHE,    "CACHE"},
  {FS_MTYPE_DRIVER,   "DRIVER"},
  {FS_MTYPE_OS,       "OS"},
  {FS_MTYPE_MEM,      "MEM"}
};

#endif // (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const COMMAND_SET * _paCommandSet;                   // List of supported commands.
static char                _acCurrentDir[FS_MAX_PATH];      // Current working directory.
static char                _acCurrentVolume[20];            // Current working volume.
static U32                 _aBuffer[BUFFER_SIZE / 4];       // Generic buffer used with CheckDisk and MakeFile commands.
static char                _acCommandLine[FS_MAX_PATH * 3]; // Buffer for the commands.
static U8                  _IsVerbose;                      // Set to 1 if additional messages should be displayed about the command execution.
static char                _ac[FS_MAX_PATH];                // Working buffer.
#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)
  static char              _acFileName[FS_MAX_PATH];        // Buffer for the file name.
  static U8                _AbortRequested;                 // Set to 1 if the current command should be aborted.
  static U8                _IsAutoRepair;                   // Set to 1 if no user input is required during the check disk operation.
  static U32               _NumFSErrors;                    // Number of file system errors.
#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)
#if USE_SIMULATOR
  static int               _NumCommandErrors;               // Total number of errors reported by the command handling functions.
#if defined(linux)
  static READ_LINE       * _pfReadLine;                     // Function to read the command line.
  static ADD_HISTORY     * _pfAddHistory;                   // List of input commands.
#endif // defined(linux)
#endif // USE_SIMULATOR

/*********************************************************************
*
*       Forward function declarations
*
**********************************************************************
*/
static int _ExecCommand(const char * s);

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _EatWhite
*/
static void _EatWhite(const char ** ps) {
  const char * s;

  s = *ps;
  while ((*s != '\0') && ((*s == ' ') || (*s == '\t') || (*s == '\r') || (*s == '\n'))) {
    s++;
  }
  *ps = s;
}

#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _Hex2Dec
*/
static int _Hex2Dec(int c) {
  if ((c >= (int)'0') && (c <= (int)'9')) {
    return c - (int)'0';
  }
  if ((c >= (int)'a') && (c <= (int)'f')) {
    return c - (int)'a' + 10;
  }
  if ((c >= (int)'A') && (c <= (int)'F')) {
    return c - (int)'A' + 10;
  }
  return -1;
}

/*********************************************************************
*
*       _ParseHex
*/
static int _ParseHex(const char ** ps, U32 * pData) {
  U32 Data;
  int NumDigits;
  int v;

  Data      = 0;
  NumDigits = 0;
  _EatWhite(ps);
  if (**ps == '0') {
    if (*(*ps + 1) == 'x') {
      (*ps) += 2;
    }
  }
  for (;;) {
    v = (int)**ps;
    v = _Hex2Dec(v);
    if (v >= 0) {
      Data = (Data << 4) | (U32)v;
      (*ps)++;
      NumDigits++;
    } else {
      if (NumDigits == 0) {
        return APP_ERROR_SYNTAX;
      }
      *pData = Data;
      return APP_ERROR_NONE;
    }
  }
}

#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _ParseDec
*
*  Function description
*    Converts a text string into a decimal number.
*
*  Parameter
*    ps      [IN]  String to be converted.
*            [OUT] Points to the end of converted string.
*    pData   [OUT] Converted decimal number.
*
*  Return value
*    ==APP_ERROR_NONE       OK, integer parsed successfully.
*    ==APP_ERROR_SYNTAX     Invalid integer string.
*/
static int _ParseDec(const char ** ps, U32 * pData) {
  U32  Data;
  int  NumDigits;
  int  v;
  char c;

  Data      = 0;
  NumDigits = 0;
  _EatWhite(ps);
  for (;;) {
    c = **ps;
    v = ((c >= '0') && (c <= '9')) ? c - '0' : -1;
    if (v >= 0) {
      Data = (Data * 10u) + (U32)v;
      (*ps)++;
      NumDigits++;
    } else {
      if (NumDigits == 0) {
        return APP_ERROR_SYNTAX;
      }
      *pData = Data;
      return APP_ERROR_NONE;
    }
  }
}

/*********************************************************************
*
*       _ParseString
*/
static int _ParseString(const char ** ps, char * pBuffer, unsigned BufferSize) {
  char         c;
  const char * s;
  int          IsQuotation;
  int          r;

  r           = 0;
  s           = *ps;
  IsQuotation = 0;
  if ((pBuffer != NULL) && (BufferSize != 0u)) {
    *pBuffer = '\0';
    _EatWhite(&s);
    if (*s == '"') {
      IsQuotation = 1;
      s++;
    }
    for (;;) {
      c = *s;
      if (c == '\0') {
        break;                          // End of input.
      }
      ++s;
      if (c == '\r') {
        break;
      }
      if (c == '\n') {
        break;
      }
      if (c == '"') {
        if (IsQuotation != 0) {
          IsQuotation = 0;
        } else {
          IsQuotation = 1;
        }
        break;
      }
      if (--BufferSize == 0u) {
        break;                          // Buffer full.
      }
      if (IsQuotation == 0) {
        if (c == ',') {
          break;
        }
        if (c == ' ') {
          break;
        }
      }
      *pBuffer++ = c;
    }
    if (IsQuotation != 0) {
      r = APP_ERROR_SYNTAX;             // Quotation not terminated.
    }
    *pBuffer = '\0';                    // Set the end of string.
    *ps      = s;
  }
  return r;
}

/*********************************************************************
*
*       _Log
*/
static void _Log(const char * s) {
#if USE_SIMULATOR
  printf("%s", s);
#else
  FS_X_Log(s);
#endif
}

#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _ParseAttrMask
*
*  Return value
*    ==APP_ERROR_NONE     OK, attribute string successfully parsed.
*    ==APP_ERROR_SYNTAX   Error, invalid attribute string.
*/
static int _ParseAttrMask(const char ** ps, U8 * pAttrMask) {
  const char * s;
  int          IsQuoted;
  U8           AttrMask;
  int          BitOperation;
  int          r;
  char         c;
  U8           Mask;
  int          NumOperations;

  r             = APP_ERROR_SYNTAX;
  s             = *ps;
  IsQuoted      = 0;
  NumOperations = 0;
  AttrMask      = *pAttrMask;
  _EatWhite(&s);
  if (*s == '"') {
    IsQuoted++;
    s++;
  }
  while ((*s != '\0') && (*s != '\r') && (*s != '\n') && ((*s != ',') || (IsQuoted != 0))) {
    BitOperation = 0;                 // Attribute operation.
    Mask         = 0;
    _EatWhite(&s);
    do {
      c = *s;
      c = (char)toupper((int)c);
      switch (c) {
      case '+':
        BitOperation = 1;             // Attribute will be set.
        break;
      case '-':
        BitOperation = 2;             // Attribute will be cleared.
        break;
      case 'A':
        Mask = FS_ATTR_ARCHIVE;
        ++NumOperations;
        break;
      case 'R':
        Mask = FS_ATTR_READ_ONLY;
        ++NumOperations;
        break;
      case 'H':
        Mask = FS_ATTR_HIDDEN;
        ++NumOperations;
        break;
      case 'S':
        Mask = FS_ATTR_SYSTEM;
        ++NumOperations;
        break;
      default:
        //
        // Unknown attributes are ignored.
        //
        break;
      }
      if (BitOperation == 1) {
        AttrMask |= Mask;
      } else if (BitOperation == 2) {
        AttrMask &= ~Mask;
      } else {
        r = APP_ERROR_SYNTAX;
        break;
      }
      r = APP_ERROR_NONE;
      s++;
    } while (((*s != ' ') && (*s != '\0')) || (r != 0));
  }
  if (NumOperations == 0) {
    r = APP_ERROR_SYNTAX;
  }
  *pAttrMask = AttrMask;
  *ps        = s;
  return r;
}

/*********************************************************************
*
*       _FillBuffer
*/
static void _FillBuffer(void * pBuffer, unsigned NumBytes, U32 StartValue) {
  U32 * pBuffer32;
  U32   NumItems;
  U32   i;

  pBuffer32 = SEGGER_PTR2PTR(U32, pBuffer);
  NumItems  = NumBytes >> 2;
  for (i = 0; i < NumItems; i++) {
    *pBuffer32++ = i + StartValue;
  }
}

/*********************************************************************
*
*       _ShowData
*/
static void _ShowData(U32 Off, const U8 * pData, unsigned NumBytes) {
  unsigned i;
  char     ac[16];
  U8       c;
  unsigned NumBytesPerLine;

  while (NumBytes > 0u) {
    SEGGER_snprintf(ac, (int)sizeof(ac), "%.8lX ", Off);
    _Log(ac);
    NumBytesPerLine = (NumBytes > BYTES_PER_LINE) ? BYTES_PER_LINE : NumBytes;
    NumBytes -= NumBytesPerLine;
    Off      += NumBytesPerLine;
    for (i = 0; i < NumBytesPerLine; i++) {
     SEGGER_snprintf(ac, (int)sizeof(ac), "%.2X ", *(pData + i));
     _Log(ac);
    }
    if (NumBytesPerLine < BYTES_PER_LINE) {
      for (i = 0; i < (BYTES_PER_LINE - NumBytesPerLine); i++) {
        _Log("   ");
      }
    }
    for (i = 0; i < NumBytesPerLine; i++) {
      c = *(pData + i);
      if ((c >= 0x20u) && (c < 0x80u)) {
        SEGGER_snprintf(ac, (int)sizeof(ac), "%c", c);
        _Log(ac);
      } else {
        _Log(".");
      }
    }
    _Log("\n");
    pData += NumBytesPerLine;
  }
}

#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _ParseCompareString
*
*  Function description
*    Parses a string and compares it for equality with the specified string.
*
*  Parameters
*    ps       String to be parsed.
*    s        String to be compared.
*
*  Return value
*   ==0     Strings match.
*   ==1     Strings do not match.
*/
static int _ParseCompareString(const char ** ps, const char * s) {
  const char * s0;
  char         c0;
  char         c1;

  s0 = *ps;
  _EatWhite(&s0);                           // Ignore leading white spaces.
  for (;;) {
    c0 = *s0;
    c0 = (char)toupper((int)c0);
    c1 = *s++;
    c1 = (char)toupper((int)c1);
    if (c1 == '\0') {
      if ((c0 == ' ') || (c0 == '\0')) {
        *ps = s0;
        return 0;                           // Strings match.
      }
      return 1;                             // Strings do not match.
    }
    if (c0 != c1) {
      return 1;                             // Strings do not match.
    }
    s0++;
  }
}

/*********************************************************************
*
*       _ParseBool
*
*  Function description
*    Converts a text string into a boolean value.
*
*  Parameter
*    ps      [IN]  String to be converted.
*            [OUT] Points to the end of converted string.
*    pData   [OUT] Converted boolean value.
*
*  Return value
*    ==APP_ERROR_NONE       OK, integer parsed successfully.
*    ==APP_ERROR_SYNTAX     Invalid integer string.
*/
static int _ParseBool(const char ** ps, int * pData) {
  int          r;
  int          Data;
  U32          v;
  const char * s;

  r    = APP_ERROR_NONE;
  s    = *ps;
  Data = 0;
  if (_ParseCompareString(&s, "on") == 0) {
    Data = 1;
    goto Done;
  }
  if (_ParseCompareString(&s, "off") == 0) {
    Data = 0;
    goto Done;
  }
  if (_ParseCompareString(&s, "true") == 0) {
    Data = 1;
    goto Done;
  }
  if (_ParseCompareString(&s, "false") == 0) {
    Data = 0;
    goto Done;
  }
  if (_ParseCompareString(&s, "enable") == 0) {
    Data = 1;
    goto Done;
  }
  if (_ParseCompareString(&s, "disable") == 0) {
    Data = 0;
    goto Done;
  }
  r = _ParseDec(&s, &v);
  if (r != 0) {
    goto Done;
  }
  if (v == 1u) {
    Data = 1;
    goto Done;
  }
  if (v == 0u) {
    Data = 0;
    goto Done;
  }
  r = APP_ERROR_SYNTAX;
Done:
  *pData = Data;
  *ps    = s;
  return r;
}

#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _GetPath
*/
static const char * _GetPath(const char * sPath) {
  if (strchr(sPath, (int)':') != NULL) {
    sPath = "";
  } else {
    sPath = _acCurrentDir;
  }
  return sPath;
}

#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _GetVolumeName
*/
static const char * _GetVolumeName(const char * s) {
  if (strchr(s, (int)':') == NULL) {            // Check if s points to a volume name.
    s = _acCurrentVolume;
  }
  return s;
}

/*********************************************************************
*
*       _UpdateCurrentDir
*/
static void _UpdateCurrentDir(const char * s) {
  unsigned   NumCharsToCopy;
  char     * sDelimiter;

  (void)strncpy(_acCurrentDir, s, sizeof(_acCurrentDir) - 1u);
  _acCurrentDir[sizeof(_acCurrentDir) - 1u] = '\0';
  sDelimiter = strrchr(_acCurrentDir, (int)':');
  if (sDelimiter != NULL) {
    NumCharsToCopy = (U32)SEGGER_PTR_DISTANCE(sDelimiter, _acCurrentDir) + 1u;
    (void)strncpy(_acCurrentVolume, _acCurrentDir, NumCharsToCopy);
    _acCurrentVolume[NumCharsToCopy] = '\0';
  }
}

/*********************************************************************
*
*       _ChangeToRootDir
*/
static void _ChangeToRootDir(void) {
  (void)strncpy(_acCurrentDir, _acCurrentVolume, sizeof(_acCurrentDir) - 1u);
  _acCurrentDir[sizeof(_acCurrentDir) - 1u] = '\0';
}

/*********************************************************************
*
*       _LogV
*/
static void _LogV(const char * s) {
  if (_IsVerbose != 0u) {
    _Log(s);
  }
}

/*********************************************************************
*
*       _LogOK
*/
static void _LogOK(int r) {
  if (r == 0) {
    _LogV("OK\n");
  }
}

#if (FS_OS_LOCKING != 0)

/*********************************************************************
*
*       _InitResult
*/
static void _InitResult(RESULT * pResult, const char * sName, U32 NumBytes) {
  pResult->sName      = sName;
  pResult->Min        = MAX_TIME;
  pResult->Max        = MIN_TIME;
  pResult->NumSamples = 0;
  pResult->Sum        = 0;
  pResult->NumBytes   = NumBytes;
}

/*********************************************************************
*
*       _StoreResult
*/
static void _StoreResult(RESULT * pResult, I32 t) {
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
static float _GetAverage(RESULT * pResult) {
  float v;

  v = (float)pResult->Av;
  if ((U32)v == 0) {
    return 0;
  }
  v = (float)1000.0 / v;
  v = v * (float)(pResult->NumBytes / 1024.0);
  return v;
}

/*********************************************************************
*
*       _ShowPerformanceResult
*/
static void _ShowPerformanceResult(RESULT * pResult) {
  char ac[128];

  SEGGER_snprintf(ac, (int)sizeof(ac),"Test: %s\n", pResult->sName);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac),"  Time:      %d/%d/%d ms (Min/Max/Avg)\n", (int)pResult->Min, (int)pResult->Max,(int)pResult->Av);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac),"  Speed:     %d KB/s\n", (int)_GetAverage(pResult));
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac),"  Counters:  ReadOperationCnt:    %d\n", (int)pResult->StorageCounter.ReadOperationCnt);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac),"             ReadSectorCnt:       %d\n", (int)pResult->StorageCounter.ReadSectorCnt);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac),"             ReadSectorCachedCnt: %d\n", (int)pResult->StorageCounter.ReadSectorCachedCnt);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac),"             WriteOperationCnt:   %d\n", (int)pResult->StorageCounter.WriteOperationCnt);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac),"             WriteSectorCnt:      %d\n", (int)pResult->StorageCounter.WriteSectorCnt);
  _Log(ac);
}

#endif // (FS_OS_LOCKING != 0)

#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _AttrToString
*/
static void _AttrToString(U8 Attr, char * sAttr, unsigned NumBytes) {
  SEGGER_snprintf(sAttr, (int)NumBytes, "%c%c%c%c%c",
    ((Attr & FS_ATTR_DIRECTORY) != 0u) ? 'D' : '-',
    ((Attr & FS_ATTR_ARCHIVE) != 0u)   ? 'A' : '-',
    ((Attr & FS_ATTR_READ_ONLY) != 0u) ? 'R' : '-',
    ((Attr & FS_ATTR_HIDDEN) != 0u)    ? 'H' : '-',
    ((Attr & FS_ATTR_SYSTEM) != 0u)    ? 'S' : '-');
}

#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

#if (USE_SIMULATOR == 0)

/*********************************************************************
*
*       _TARGET_GetInputLine
*
*  Function description
*    Reads one input line.
*
*  Parameters
*    sPrompt        Text to be displayed. Can be NULL.
*    pBuffer        [OUT] Input line (0-terminated)
*    BufferSize     Number of bytes in pBuffer.
*
*  Return value
*    > 0      OK, no more input.
*    ==0      OK, input line read.
*    < 0      An error occurred.
*/
static int _TARGET_GetInputLine(const char * pPrompt, char * pBuffer, unsigned BufferSize) {
  unsigned NumBytes;
  int      c;
  char     ac[32];

  //
  // Display a prompt if required.
  //
  NumBytes = 0;
  if (pPrompt != NULL) {
    SEGGER_snprintf(ac, (int)sizeof(ac), "%s", pPrompt);
    _Log(ac);
  }
  //
  // Wait for input.
  //
  do {
#if USE_RTT
    c = SEGGER_RTT_WaitKey();
#else
    c = getchar();
#endif
    if (NumBytes >= BufferSize) {
      break;                            // OK, buffer full.
    }
    pBuffer[NumBytes++] = (char)c;
  } while (c != (int)'\n');
  pBuffer[NumBytes - 1u] = '\0';
  return 0;
}

/*********************************************************************
*
*       _TARGET_IsConsoleInput
*/
static int _TARGET_IsConsoleInput(void) {
  //
  // The input is always coming from console when
  // the application is running on a target hardware.
  //
  return 1;
}

#endif // USE_SIMULATOR == 0

#if USE_SIMULATOR

#ifdef linux

/*********************************************************************
*
*       _LINUX_GetSymbol
*/
static void * _LINUX_GetSymbol(void * pLib, const char * pName) {
  void * pSymbol;
  char * pError;

  dlerror();
  pSymbol = dlsym(pLib, pName);
  pError = dlerror();
  if (pError) {
    pSymbol = NULL;
  }
  return pSymbol;
}

/*********************************************************************
*
*       _LINUX_IsConsoleInput
*/
static int _LINUX_IsConsoleInput(void) {
  int r;

  r = 0;
  if (isatty(fileno(stdin))) {
    r = 1;
  }
  return r;
}

/*********************************************************************
*
*       _LINUX_GetInputLine
*
*  Function description
*    Reads one input line.
*
*  Parameters
*    sPrompt        Text to be displayed. Can be NULL.
*    pBuffer        [OUT] Input line (0-terminated)
*    BufferSize     Number of bytes in pBuffer.
*
*  Return value
*    > 0      OK, no more input.
*    ==0      OK, input line read.
*    < 0      An error occurred.
*/
static int _LINUX_GetInputLine(const char * sPrompt, char * pBuffer, U32 BufferSize) {
  char     * s;
  unsigned   NumBytes;
  char     * pError;
  char       ac[32];

  if ((_LINUX_IsConsoleInput() != 0) && (_pfReadLine != NULL) && (_pfAddHistory != NULL)) {
    //
    // Found the library. Use the functions implemented in it.
    //
    for (;;) {
      s = (*_pfReadLine)(sPrompt);
      if (s != NULL) {
        NumBytes = strlen(s);
        NumBytes = SEGGER_MIN(NumBytes, BufferSize - 1);
        (void)strncpy(pBuffer, s, NumBytes);
        pBuffer[NumBytes] = '\0';
        if (NumBytes != 0u) {
          (*_pfAddHistory)(s);
        } else {
          free(s);
        }
        break;
      }
    }
  } else {
    //
    // libedit library not found. Use standard I/O functions.
    // Display a prompt if required.
    //
    if (sPrompt != NULL) {
      printf("%s", sPrompt);
    }
    //
    // Wait for input.
    //
    s = fgets(pBuffer, BufferSize, stdin);
    if (s == NULL) {
      if (feof(stdin) != 0) {
        return 1;                 // OK, no more input.
      }
      if (ferror(stdin) != 0) {
        return -1;                // Error, could not read data.
      }
    }
    //
    // Remove the new line character if present.
    //
    NumBytes = strlen(pBuffer);
    if (NumBytes != 0) {
      s = &pBuffer[NumBytes - 1];
      if (*s == '\n') {
        *s = '\0';
      }
    }
  }
  return 0;                       // OK, input line read.
}

/*********************************************************************
*
*       _LINUX_InitSystem
*/
static void _LINUX_InitSystem(const char * sTitle) {
  void * pLib;

  (void)sTitle;
  _pfReadLine   = NULL;
  _pfAddHistory = NULL;
  pLib = dlopen("libedit.so.2", RTLD_LAZY);
  if (pLib != NULL) {
    _pfReadLine   = (READ_LINE   *)_LINUX_GetSymbol(pLib, "readline");
    _pfAddHistory = (ADD_HISTORY *)_LINUX_GetSymbol(pLib, "add_history");
  }
}

#endif // linux

#ifdef _WIN32

/*********************************************************************
*
*       _WIN32_GetInputLine
*
*  Function description
*    Reads one input line.
*
*  Parameters
*    sPrompt        Text to be displayed. Can be NULL.
*    pBuffer        [OUT] Input line (0-terminated)
*    BufferSize     Number of bytes in pBuffer.
*
*  Return value
*    > 0      OK, no more input.
*    ==0      OK, input line read.
*    < 0      An error occurred.
*/
static int _WIN32_GetInputLine(const char * sPrompt, char * pBuffer, U32 BufferSize) {
  char     * s;
  unsigned   NumBytes;

  //
  // Display a prompt if required.
  //
  if (sPrompt != NULL) {
    printf("%s", sPrompt);
  }
  //
  // Wait for input.
  //
  s = fgets(pBuffer, BufferSize, stdin);
  if (s == NULL) {
    if (feof(stdin) != 0) {
      return 1;                 // OK, no more input.
    }
    if (ferror(stdin) != 0) {
      return -1;                // Error, could not read data.
    }
  }
  //
  // Remove the new line character if present.
  //
  NumBytes = (U32)strlen(pBuffer);
  if (NumBytes != 0) {
    s = &pBuffer[NumBytes - 1];
    if (*s == '\n') {
      *s = '\0';
    }
  }
  return 0;                     // OK, input line read.
}

/*********************************************************************
*
*       _WIN32_ConsoleHandler
*/
static BOOL WINAPI _WIN32_ConsoleHandler(DWORD Event) {
  switch(Event) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
#if FS_SUPPORT_FAT
    FS_FAT_AbortCheckDisk();
#endif // FS_SUPPORT_FAT
    _AbortRequested = 1;
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

/*********************************************************************
*
*       _WIN32_IsConsoleInput
*/
static int _WIN32_IsConsoleInput(void) {
  int r;

  r = 0;
  if (_isatty(_fileno(stdin))) {
    r = 1;
  }
  return r;
}

/*********************************************************************
*
*       _WIN32_InitSystem
*/
static void _WIN32_InitSystem(const char * sTitle) {
  (void)GetLastError();
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCtrlHandler(_WIN32_ConsoleHandler, TRUE);
  SetConsoleTitle(sTitle);
}

#endif // _WIN32

#endif // USE_SIMULATOR

#if (FS_OS_LOCKING != 0)

/*********************************************************************
*
*       _WriteFile
*/
static I32 _WriteFile(FS_FILE * pFile, const void * pData, U32 NumBytesToWrite, U32 NumBlocksMeasure) {
  I32 t;
  U32 i;

  t = (I32)FS_OS_GetTime();
  for (i = 0; i < NumBlocksMeasure; i++) {
    (void)FS_Write(pFile, pData, NumBytesToWrite);
  }
  return FS_OS_GetTime() - t;
}

/*********************************************************************
*
*       _ReadFile
*/
static I32 _ReadFile(FS_FILE * pFile, void * pData, U32 NumBytesToWrite, U32 NumBlocksMeasure) {
  I32 t;
  U32 i;

  t = (I32)FS_OS_GetTime();
  for (i = 0; i < NumBlocksMeasure; i++) {
    (void)FS_Read(pFile, pData, NumBytesToWrite);
  }
  return FS_OS_GetTime() - t;
}

#endif // (FS_OS_LOCKING != 0)

#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _CalcKBytes
*
*  Function description
*    Multiplies 2 values and returns the result divided by 1024.
*
*  Parameters
*    NumItems      Number of items such as number of sectors
*    BytesPerItem  Bytes per item, such as number of bytes is a sector or cluster. MUST BE A POWER OF 2!
*
*  Additional information
*    The reason for using a routine is that with a simple multiplication,
*    the 4GB range that a U32 can hold is typically exceeded.
*/
static U32 _CalcKBytes(U32 NumItems, U32 BytesPerItem) {
  int i;
  U32 v;
  U32 Shift;

  //
  // Compute number of bits to shift. 512: -1, 1024:0, 2048: 1
  //
  i = -11;
  while (BytesPerItem != 0u) {
    BytesPerItem >>= 1;
    i++;
  }
  //
  // Shift NumItems to compute the result
  //
  if (i > 0) {
    Shift = (U32)i;
    v = NumItems << Shift;
  } else if (i < 0) {
    Shift = (U32)(-i);
    v = NumItems >> Shift;
  } else {
    v = NumItems;
  }
  return v;
}

/*********************************************************************
*
*       _IsVolumeName
*/
static int _IsVolumeName(const char * sVolumeName) {
  int      i;
  int      NumVolumes;
  char     acVolumeName[20];
  int      r;
  unsigned NumBytes;

  r = 0;
  NumBytes = (U32)strlen(sVolumeName);
  if (NumBytes > 0u) {
    if (sVolumeName[NumBytes - 1u] == ':') {
      NumVolumes = FS_GetNumVolumes();
      for (i = 0; i < NumVolumes; i++) {
        memset(acVolumeName, 0, sizeof(acVolumeName));
        (void)FS_GetVolumeName(i, acVolumeName, (int)sizeof(acVolumeName));
        if (strstr(acVolumeName, sVolumeName) == acVolumeName) {
          r = 1;
          break;
        }
      }
    }
  }
  return r;
}

/*********************************************************************
*
*       _OnError
*/
static int _OnError(int ErrCode, ...) {
  va_list      ParamList;
  const char * sFormat;
  char         c;
  char         ac[512];
  int          IsConsoleInput;

  //lint -esym(530, ParamList)
  _NumFSErrors++;
  sFormat = FS_FAT_CheckDisk_ErrCode2Text(ErrCode);
  if (_IsAutoRepair == 0u) {
    if ((_NumFSErrors == 1u) && (_IsVerbose != 0u)) {
      _Log("\n");
    }
    if (sFormat != NULL) {
      va_start(ParamList, ErrCode);
      (void)vsprintf(ac, sFormat, ParamList);
      va_end(ParamList);
      _Log(ac);
      _Log("\n");
    }
    IsConsoleInput = IS_CONSOLE_INPUT();
    if (IsConsoleInput == 0) {
      return FS_CHECKDISK_ACTION_DO_NOT_REPAIR;
    }
    if (ErrCode != FS_ERRCODE_CLUSTER_UNUSED) {
      _Log("  Do you want to repair this? (y[es]/n[o]/a[lways]/c[ancel]) ");
    } else {
      _Log("  * Convert lost cluster chain into file (y[es])\n");
      _Log("  * Delete cluster chain                 (d[elete])\n");
      _Log("  * Do not repair                        (n[o])\n");
      _Log("  * Abort                                (c[ancel])\n");
    }
    ac[0] = '\0';
    (void)GET_INPUT_LINE("", ac, sizeof(ac));
    c = ac[0];
    if ((c == 'a') || (c == 'A')) {
      _IsAutoRepair = 1;
      return FS_CHECKDISK_ACTION_SAVE_CLUSTERS;
    }
    if ((c == 'y') || (c == 'Y')) {
      return FS_CHECKDISK_ACTION_SAVE_CLUSTERS;
    } else if ((c == 'c') || (c == 'C')) {
      return FS_CHECKDISK_ACTION_ABORT;
    } else if ((c == 'd') || (c == 'D')) {
      return FS_CHECKDISK_ACTION_DELETE_CLUSTERS;
    } else {
      //
      // Ignore any other characters.
      //
    }
  } else {
    return FS_CHECKDISK_ACTION_SAVE_CLUSTERS;
  }
  return FS_CHECKDISK_ACTION_DO_NOT_REPAIR;
}

#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

#if FS_SUPPORT_JOURNAL

/*********************************************************************
*
*       _RemoveJournal
*/
static int _RemoveJournal(const char * sVolumeName) {
  int  r;
  char acFileName[FS_MAX_PATH];
  int  NumChars;

  //
  // Suspend the journal activity.
  //
  r = FS_JOURNAL_Disable(sVolumeName);
  if (r != 0) {
    return r;
  }
  //
  // Build the fully qualified path to journal file.
  //
  memset(acFileName, 0, sizeof(acFileName));
  if (sVolumeName[0] != '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", FS_JOURNAL_FILE_NAME);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sVolumeName, FS_DIRECTORY_DELIMITER, FS_JOURNAL_FILE_NAME);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Remove the journal file.
  //
  r = FS_Remove(acFileName);
  if (r != 0) {
    return r;
  }
  //
  // Deinitialize the journal.
  //
  if (FS_GetVolumeStatus(sVolumeName) != FS_MEDIA_IS_PRESENT) {
    FS_UnmountForced(sVolumeName);
  } else {
    FS_Unmount(sVolumeName);
  }
  return 0;             // OK, journal removed.
}

#endif // FS_SUPPORT_JOURNAL

#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _ShowDirTree
*/
static int _ShowDirTree(char * sDirName, unsigned SizeOfDirName, char * sPrefix, unsigned SizeOfPrefix, unsigned MaxRecursionLevel) {
  FS_FIND_DATA   fd;
  int            r;
  U8             Attr;
  char         * s;
  unsigned       NumBytes;
  unsigned       NumBytesPath;
  unsigned       NumBytesPrefix;
  const char   * sFileName;
  const char   * sPrefixNotLast = "|  ";
  const char   * sPrefixLast    = "   ";
  const char   * sPrefixToAdd;
  unsigned       NumEntries;
  int            IsSystemDir;

  memset(&fd, 0, sizeof(fd));
  //
  // Count the number of file and directories.
  //
  NumEntries = 0;
  r = FS_FindFirstFile(&fd, sDirName, _acFileName, (int)sizeof(_acFileName));
  if (r == 0) {
    for (;;) {
      ++NumEntries;
      r = FS_FindNextFileEx(&fd);
      if (r < 0) {
        break;                                    // Error, could not find the next file or directory.
      }
      if (r == 1) {
        r = 0;                                    // OK, end of directory reached.
        break;
      }
    }
    FS_FindClose(&fd);
  }
  if (r != 0) {
    if (r == 1) {
      r = 0;                                      // Empty directory.
    }
    return r;                                     // Error, could not calculate the number of files and directories.
  }
  r = FS_FindFirstFile(&fd, sDirName, _acFileName, (int)sizeof(_acFileName));
  if (r == 0) {
    for (;;) {
      --NumEntries;
      Attr      = fd.Attributes;
      sFileName = fd.sFileName;
      IsSystemDir = 0;
      if (strcmp(sFileName, ".") == 0) {
        IsSystemDir = 1;
      } else {
        if (strcmp(sFileName, "..") == 0) {
          IsSystemDir = 1;
        }
      }
      if (IsSystemDir == 0) {                     // Do not show system directory entries.
        //
        // Show the name of the file or directory.
        //
        _Log(sPrefix);
        _Log("+--");
        _Log(sFileName);
        _Log("\n");
        if ((Attr & FS_ATTR_DIRECTORY) != 0u) {
          if (MaxRecursionLevel != 0u) {
            //
            // Check that the directory name fits into the path.
            //
            NumBytes      = 0;
            NumBytesPath  = (U32)strlen(sDirName);
            NumBytes     += NumBytesPath;
            NumBytes     += (U32)strlen(sFileName);
            NumBytes     += 1u + 1u;              // One character for the directory delimiter and one for the 0-terminator.
            if (NumBytes >= SizeOfDirName) {
              r = FS_ERRCODE_PATH_TOO_LONG;
              break;
            }
            //
            // Check that the prefix fits into the buffer.
            //
            sPrefixToAdd = sPrefixNotLast;
            if (NumEntries == 0u) {
              sPrefixToAdd = sPrefixLast;
            }
            NumBytes        = 0;
            NumBytesPrefix  = (U32)strlen(sPrefix);
            NumBytes       += NumBytesPrefix;
            NumBytes       += (U32)strlen(sPrefixToAdd);
            NumBytes       += 1u;                 // One character for the 0-terminator.
            if (NumBytes >= SizeOfPrefix) {
              r = FS_ERRCODE_BUFFER_TOO_SMALL;
              break;
            }
            //
            // Add the name of the directory to the path.
            //
            s = &sDirName[NumBytesPath];
            *s++ = FS_DIRECTORY_DELIMITER;
            (void)strcpy(s, sFileName);
            //
            // Add the prefix to the buffer.
            //
            s = &sPrefix[NumBytesPrefix];
            (void)strcpy(s, sPrefixToAdd);
            //
            // Show the contents of the directory.
            //
            r = _ShowDirTree(sDirName, SizeOfDirName, sPrefix, SizeOfPrefix, MaxRecursionLevel - 1u);
            if (r != 0) {
              break;
            }
            //
            // Remove the name of the directory from the path and the prefix.
            //
            sDirName[NumBytesPath]  = '\0';
            sPrefix[NumBytesPrefix] = '\0';
          }
        }
      }
      //
      // Stop showing the directory tree if the user requests it.
      //
      if (_AbortRequested != 0u) {
        _AbortRequested = 0;
        break;
      }
      //
      // Get the name of the next file or directory.
      //
      r = FS_FindNextFileEx(&fd);
      if (r < 0) {
        break;                                  // Error, could not find the next file or directory.
      }
      if (r == 1) {
        r = 0;                                  // OK, end of directory reached.
        break;
      }
    }
    FS_FindClose(&fd);
  } else {
    if (r == 1) {
      r = 0;                                    // OK, empty directory.
    }
  }
  return r;
}

/*********************************************************************
*
*       _ExecChangeDir
*/
static int _ExecChangeDir(const char * s) {
  char           acDirName[FS_MAX_PATH];
  char         * sPath;
  FS_FIND_DATA   fd;
  int            r;
  char           c;
  const char     acDirDelim[] = {FS_DIRECTORY_DELIMITER, '\0'};
  int            NumChars;

  c = '\0';
  //
  // Parse the name of the directory (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (strchr(_ac, (int)':') != NULL) {
    sPath = _ac;
    (void)strcpy(acDirName, sPath);
    _ac[0] = '\0';
  } else {
    //
    // No directory name specified. Show the contents of the current directory.
    //
    if (_ac[0] == '\0') {
      _Log(_acCurrentDir);
      _Log("\n");
      return APP_ERROR_NONE;
    }
    sPath = _acCurrentDir;
    (void)strcpy(acDirName, sPath);
  }
  if (strcmp(_ac, "..") == 0) {
    //
    // Change to parent directory.
    //
    sPath = strrchr(acDirName, (int)FS_DIRECTORY_DELIMITER);
    if (sPath != NULL) {
      *sPath = '\0';
    } else {
      return APP_ERROR_NONE;
    }
  } else if (strcmp(_ac, acDirDelim) == 0) {
    //
    // Change to root directory.
    //
    sPath = strchr(acDirName, (int)FS_DIRECTORY_DELIMITER);
    if (sPath != NULL) {
      *sPath = '\0';
    } else {
      return APP_ERROR_NONE;
    }
  } else if (strcmp(_ac, ".") == 0) {
    //
    // Stay in current directory.
    //
  } else {
    //
    // Change to the specified directory.
    //
    if ((_ac[0] == '\0') && (acDirName[0] == '\0')) {
      return APP_ERROR_NONE;
    }
    //
    // Build the fully qualified path to directory.
    //
    NumChars = 0;
    if (_ac[0] != '\0') {
      NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", acDirName, FS_DIRECTORY_DELIMITER, _ac);
    }
    if ((unsigned)NumChars >= sizeof(acDirName)) {
      return FS_ERRCODE_PATH_TOO_LONG;
    }
  }
  //
  // Set the volume without checking if it is formatted.
  //
  r = 0;
  if (_IsVolumeName(acDirName) != 0) {
    _UpdateCurrentDir(acDirName);
  } else {
    //
    // Check that the directory actually exists.
    //
    r = FS_FindFirstFile(&fd, acDirName, &c, (int)sizeof(c));
    if (r >= 0) {
      _UpdateCurrentDir(acDirName);
    }
  }
  return r;
}

/*********************************************************************
*
*       _ExecCreateDir
*/
static int _ExecCreateDir(const char * s) {
  char         acDirName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  int          NumChars;

  //
  // Parse the name of the directory (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acDirName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  _LogV("Creating directory \"");
  _LogV(acDirName);
  _LogV("\"...");
  r = FS_CreateDir(acDirName);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecShowDir
*/
static int _ExecShowDir(const char * s) {
  FS_FIND_DATA   fd;
  char           acDirName[FS_MAX_PATH];
  const char   * sPath;
  int            r;
  FS_FILETIME    FileTimeCreate;
  FS_FILETIME    FileTimeModify;
  char           ac[128];
  U8             Attr;
  int            NumChars;

  //
  // Parse the name of the directory (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  //
  // Create the fully qualified path to directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acDirName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  r = FS_FindFirstFile(&fd, acDirName, _acFileName, (int)sizeof(_acFileName));
  if (r == 0) {
    for (;;) {
      Attr = fd.Attributes;
      FS_TimeStampToFileTime(fd.CreationTime, &FileTimeCreate);
      FS_TimeStampToFileTime(fd.LastWriteTime, &FileTimeModify);
      SEGGER_snprintf(ac, (int)sizeof(ac), "%-28s %s Attr: %c%c%c%c C: %02u-%02u-%02u %02u:%02u:%02u\n",
        fd.sFileName,
        ((Attr & FS_ATTR_DIRECTORY) != 0u) ? "(Dir)" : "     ",
        ((Attr & FS_ATTR_ARCHIVE) != 0u)   ? 'A' : '-',
        ((Attr & FS_ATTR_READ_ONLY) != 0u) ? 'R' : '-',
        ((Attr & FS_ATTR_HIDDEN) != 0u)    ? 'H' : '-',
        ((Attr & FS_ATTR_SYSTEM) != 0u)    ? 'S' : '-',
        FileTimeCreate.Year, FileTimeCreate.Month,  FileTimeCreate.Day,
        FileTimeCreate.Hour, FileTimeCreate.Minute, FileTimeCreate.Second);
      _Log(ac);
      SEGGER_snprintf(ac, (int)sizeof(ac), "%-28s                  M: %02u-%02u-%02u %02u:%02u:%02u ",
        " ",
        FileTimeModify.Year, FileTimeModify.Month,  FileTimeModify.Day,
        FileTimeModify.Hour, FileTimeModify.Minute, FileTimeModify.Second);
      _Log(ac);
      if ((Attr & FS_ATTR_DIRECTORY) != 0u) {
        _Log("\n");
      } else {
        SEGGER_snprintf(ac, (int)sizeof(ac), "Size: %lu\n", fd.FileSize);
        _Log(ac);
      }
      if (_AbortRequested != 0u) {
        _AbortRequested = 0;
        break;
      }
      r = FS_FindNextFileEx(&fd);
      if (r < 0) {
        break;                      // Error, could not find the next file or directory.
      }
      if (r == 1) {
        r = 0;                      // OK, end of directory reached.
        break;
      }
    }
    FS_FindClose(&fd);
  } else {
    if (r == 1) {
      _Log("Empty directory\n");
      r = 0;
    }
  }
  return r;
}

/*********************************************************************
*
*       _ExecShowDirTree
*/
static int _ExecShowDirTree(const char * s) {
  char         acDirName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  int          NumChars;
  U32          MaxRecursionLevel;
  char         acPrefix[MAX_RECURSION_LEVEL * 3u + 1u];
  unsigned     Attr;

  MaxRecursionLevel = MAX_RECURSION_LEVEL_DEFAULT;
  memset(acDirName, 0, sizeof(acDirName));
  memset(acPrefix, 0, sizeof(acPrefix));
  //
  // Parse the name of the directory (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  //
  // Parse the maximum recursion level (optional)
  //
  (void)_ParseDec(&s, &MaxRecursionLevel);
  if (MaxRecursionLevel > MAX_RECURSION_LEVEL) {
    return FS_ERRCODE_INVALID_PARA;
  }
  //
  // Create the fully qualified path to directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acDirName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  Attr = FS_GetFileAttributes(acDirName);
  if (Attr == 0xFFu) {
    return FS_ERRCODE_FILE_DIR_NOT_FOUND;     // Error, directory does not exist.
  }
  if ((Attr & FS_ATTR_DIRECTORY) == 0u) {
    return FS_ERRCODE_NOT_A_DIR;              // Error, this is not a directory.
  }
  _Log(acDirName);
  _Log("\n");
  r = _ShowDirTree(acDirName, sizeof(acDirName), acPrefix, sizeof(acPrefix), MaxRecursionLevel);
  return r;
}

/*********************************************************************
*
*       _ExecMkDir
*/
static int _ExecMkDir(const char * s) {
  char         acDirName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  int          NumChars;

  //
  // Parse the name of the directory (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Create the fully qualified name to directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acDirName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  _LogV("Creating directory \"");
  _LogV(acDirName);
  _LogV("\"...");
  r = FS_MkDir(acDirName);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecRemoveDir
*/
static int _ExecRemoveDir(const char * s) {
  char         acDirName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  int          NumChars;

  //
  // Parse the name of the directory (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Create the fully qualified name to directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acDirName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  _LogV("Removing directory \"");
  _LogV(acDirName);
  _LogV("\"...");
  r = FS_RmDir(acDirName);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecDeleteDir
*/
static int _ExecDeleteDir(const char * s) {
  char         acDirName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  int          NumChars;
  U32          MaxRecursionLevel;
  char         ac[16];

  MaxRecursionLevel = MAX_RECURSION_LEVEL_DEFAULT;
  //
  // Parse the name of the directory (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Parse the maximum recursion level (optional)
  //
  (void)_ParseDec(&s, &MaxRecursionLevel);
  if (MaxRecursionLevel > MAX_RECURSION_LEVEL) {
    return FS_ERRCODE_INVALID_PARA;
  }
  //
  // Create the fully qualified name to directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acDirName, (int)sizeof(acDirName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acDirName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  SEGGER_snprintf(ac, (int)sizeof(ac), "%lu", MaxRecursionLevel);
  _LogV("Deleting directory \"");
  _LogV(acDirName);
  _LogV("\" up to ");
  _LogV(ac);
  _LogV(" level(s) deep...");
  r = FS_DeleteDir(acDirName, (int)MaxRecursionLevel);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecMakeFile
*/
static int _ExecMakeFile(const char* s) {
  char         acFileName[FS_MAX_PATH];
  int          r;
  const char * sPath;
  FS_FILE    * pFile;
  U32          NumBytes;
  U32          NumBytesToWrite;
  U32          NumReps;
  U32          BufferSize;
  U8         * pData;
  U32          StartValue;
  int          DoFill;
  char         ac[64];
  U32          NumBytesWritten;
  int          NumChars;
  U32          NumBytesAtOnce;
  int          Result;

  NumBytesToWrite = 0;
  NumReps         = 1;
  StartValue      = 0;
  DoFill          = 0;
  //
  // Parse the file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Parse the number of bytes to write at once, number of repetitions and
  // the flag that indicates if any data has to be written to file.
  // All these parameters are optional.
  //
  (void)_ParseDec(&s, &NumBytesToWrite);
  (void)_ParseDec(&s, &NumReps);
  (void)_ParseBool(&s, &DoFill);
  (void)_ParseHex(&s, &StartValue);
  //
  // Create the fully qualified path to file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  if (NumBytesToWrite == 0u) {
    NumBytesToWrite = (U32)strlen(acFileName);
    BufferSize      = NumBytesToWrite;
    pData           = (U8 *)acFileName;
  } else {
    _FillBuffer(_aBuffer, sizeof(_aBuffer), StartValue);
    pData           = (U8 *)_aBuffer;
    BufferSize      = sizeof(_aBuffer);
  }
  pFile = NULL;
  r = FS_FOpenEx(acFileName, "w", &pFile);
  NumBytes = NumBytesToWrite;
  if (r == 0) {
    SEGGER_snprintf(ac, (int)sizeof(ac), "%lu x %lu", NumReps, NumBytesToWrite);
    _LogV("Creating file \"");
    _LogV(acFileName);
    _LogV(" of ");
    _LogV(ac);
    _LogV(" bytes...");
    //
    // Do we want to write data to the file
    //
    if (DoFill != 0) {
      if (NumReps == 0u) {
        NumReps = 1;
      }
      do {
        if (NumBytesToWrite == 0u) {
          NumBytesToWrite = NumBytes;
        }
        do {
          NumBytesAtOnce = SEGGER_MIN(BufferSize, NumBytesToWrite);
          NumBytesWritten = FS_Write(pFile, pData, NumBytesAtOnce);
          if (NumBytesWritten != NumBytesAtOnce) {
            r = FS_FError(pFile);
            (void)FS_FClose(pFile);
            return r;
          }
          NumBytesToWrite -= NumBytesAtOnce;
          StartValue     += NumBytesAtOnce;
          _FillBuffer(_aBuffer, sizeof(_aBuffer), StartValue);
          _LogV(".");
        } while(NumBytesToWrite != 0u);
      } while (--NumReps != 0u);
    } else {
      //
      // Simply grow the file without writing to the file.
      //
      (void)FS_FSeek(pFile, (FS_FILE_OFF)NumBytesToWrite, FS_SEEK_SET);
      r = FS_SetEndOfFile(pFile);
    }
    Result = FS_FClose(pFile);
    if (Result != 0) {
      r = Result;
    }
    _LogOK(r);
  }
  return r;
}

/*********************************************************************
*
*       _ExecType
*/
static int _ExecType(const char * s) {
  char         acFileName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  FS_FILE    * pFile;
  U32          NumBytes;
  int          ErrCode;
  char       * sText;
  int          NumChars;

  //
  // Parse the file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  pFile = NULL;
  r = FS_FOpenEx(acFileName, "r", &pFile);
  if (r == 0) {
    _LogV("Contents of file \"");
    _LogV(acFileName);
    _LogV("\":\n");
    while (FS_FEof(pFile) == 0) {
      NumBytes = FS_Read(pFile, _aBuffer, sizeof(_aBuffer) - 1u);
      if (NumBytes == 0u) {
        break;
      }
      ErrCode = FS_FError(pFile);
      if ((ErrCode != FS_ERRCODE_OK) && (ErrCode != FS_ERRCODE_EOF)) {
        return ErrCode;
      }
      sText = (char *)_aBuffer;
      sText[NumBytes] = '\0';
      _Log(sText);
      if (_AbortRequested != 0u) {
        _AbortRequested = 0;
        break;
      }
    }
    r = FS_FClose(pFile);
    _Log("\n");
  }
  return r;
}

/*********************************************************************
*
*       _ExecTypeHex
*/
static int _ExecTypeHex(const char * s) {
  char         acFileName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  FS_FILE    * pFile;
  unsigned     Off;
  int          ErrCode;
  U32          NumBytes;
  int          NumChars;

  //
  // Parse the file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  pFile = NULL;
  r = FS_FOpenEx(acFileName, "r", &pFile);
  if (r == 0) {
    _LogV("Contents of file \"");
    _LogV(acFileName);
    _LogV("\":\n");
    Off = 0;
    while (FS_FEof(pFile) == 0) {
      NumBytes = FS_Read(pFile, _aBuffer, sizeof(_aBuffer));
      if (NumBytes == 0u) {
        break;
      }
      ErrCode = FS_FError(pFile);
      if ((ErrCode != FS_ERRCODE_OK) && (ErrCode != FS_ERRCODE_EOF)) {
        return ErrCode;
      }
      _ShowData(Off, (const U8 *)_aBuffer, NumBytes);
      Off += NumBytes;
    }
    r = FS_FClose(pFile);
    _Log("\n");
  }
  return r;
}

/*********************************************************************
*
*       _ExecDeleteFile
*/
static int _ExecDeleteFile(const char * s) {
  char         acFileName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  int          NumChars;

  //
  // Parse the file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  _LogV("Removing file \"");
  _LogV(acFileName);
  _LogV("\"...");
  r = FS_Remove(acFileName);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecRename
*/
static int _ExecRename(const char * s) {
  const char * sPath;
  char         acFileNameSrc[FS_MAX_PATH];
  char         acFileNameDest[FS_MAX_PATH];
  int          r;
  int          NumChars;

  //
  // Parse the actual file or directory name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the actual file or directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileNameSrc, (int)sizeof(acFileNameSrc), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileNameSrc, (int)sizeof(acFileNameSrc), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileNameSrc)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Parse the new file or directory name (mandatory)
  //
  r = _ParseString(&s, acFileNameDest, sizeof(acFileNameDest));
  if (r != 0) {
    return r;
  }
  if (acFileNameDest[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Execute the operation.
  //
  _LogV("Renaming \"");
  _LogV(acFileNameSrc);
  _LogV("\" to \"");
  _LogV(acFileNameDest);
  _LogV("\"...");
  r = FS_Rename(acFileNameSrc, acFileNameDest);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecMove
*/
static int _ExecMove(const char * s) {
  const char * sPath;
  char         acFileNameSrc[FS_MAX_PATH];
  char         acFileNameDest[FS_MAX_PATH];
  int          r;
  int          NumChars;

  //
  // Parse the source file or directory name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the source file or directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileNameSrc, (int)sizeof(acFileNameSrc), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileNameSrc, (int)sizeof(acFileNameSrc), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileNameSrc)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Parse the destination file or directory name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the destination file or directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileNameDest, (int)sizeof(acFileNameDest), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileNameDest, (int)sizeof(acFileNameDest), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileNameDest)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  _LogV("Moving \"");
  _LogV(acFileNameSrc);
  _LogV("\" to \"");
  _LogV(acFileNameDest);
  _LogV("\"...");
  r = FS_Move(acFileNameSrc, acFileNameDest);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecSetGetAttr
*/
static int _ExecSetGetAttr(const char * s) {
  char         acFileName[FS_MAX_PATH];
  char         acAttr[6];
  int          r;
  U8           Attr;
  const char * sPath;
  int          NumChars;

  //
  // Parse the name of the file or directory (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the file or directory.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Get the current attributes of the file or directory.
  //
  Attr = FS_GetFileAttributes(acFileName);
  if (Attr == 0xFFu) {
    return FS_ERRCODE_FILE_DIR_NOT_FOUND;
  }
  _EatWhite(&s);
  if (*s == '\0') {
    //
    // Convert the attributes to text format.
    //
    _AttrToString(Attr, acAttr, sizeof(acAttr));
    _LogV(acAttr);
    _LogV("     ");
    _LogV(acFileName);
    _LogV("\n");
    return APP_ERROR_NONE;
  }
  //
  // Parse the attribute values.
  //
  r = _ParseAttrMask(&s, &Attr);
  if (r != 0) {
    return r;
  }
  //
  // Set the new attributes.
  //
  _LogV("Setting attributes of \"");
  _LogV(acFileName);
  _LogV("\"...");
  r = FS_SetFileAttributes(acFileName, Attr);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecCopy
*/
static int _ExecCopy(const char * s) {
  const char * sPath;
  char         acFileNameSrc[FS_MAX_PATH];
  char         acFileNameDest[FS_MAX_PATH];
  int          r;
  int          NumChars;

  //
  // Parse the source file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the source file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileNameSrc, (int)sizeof(acFileNameSrc), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileNameSrc, (int)sizeof(acFileNameSrc), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileNameSrc)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Parse the destination file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the destination file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileNameDest, (int)sizeof(acFileNameDest), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileNameDest, (int)sizeof(acFileNameDest), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileNameDest)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  _LogV("Copying \"");
  _LogV(acFileNameSrc);
  _LogV("\" to \"");
  _LogV(acFileNameDest);
  _LogV("\"...");
  r = FS_CopyFile(acFileNameSrc, acFileNameDest);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecTruncate
*/
static int _ExecTruncate(const char * s) {
  char         acFileName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  FS_FILE    * pFile;
  U32          FileSize;
  int          Result;
  int          NumChars;
  char         ac[16];

  //
  // Parse the file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Parse the new file size (optional).
  //
  FileSize = 0;
  (void)_ParseDec(&s, &FileSize);
  //
  // Build the fully qualified path to the file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  SEGGER_snprintf(ac, (int)sizeof(ac), "%lu", FileSize);
  _LogV("Truncating \"");
  _LogV(acFileName);
  _LogV("\" to ");
  _LogV(ac);
  _LogV(" bytes...");
  pFile = NULL;
  r = FS_FOpenEx(acFileName, "r+", &pFile);
  if (r == 0) {
    r = FS_Truncate(pFile, FileSize);
    Result = FS_FClose(pFile);
    if (Result != 0) {
      r = Result;
    }
  }
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecWipeFile
*/
static int _ExecWipeFile(const char* s) {
  char         acFileName[FS_MAX_PATH];
  const char * sPath;
  int          r;
  int          NumChars;

  //
  // Parse the file name (mandatory)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  if (_ac[0] == '\0') {
    return APP_ERROR_SYNTAX;
  }
  //
  // Build the fully qualified path to the file.
  //
  sPath = _GetPath(_ac);
  if (*sPath == '\0') {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s", _ac);
  } else {
    NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sPath, FS_DIRECTORY_DELIMITER, _ac);
  }
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Execute the operation.
  //
  _LogV("Wiping \"");
  _LogV(acFileName);
  _LogV("\"...");
  r = FS_WipeFile(acFileName);
  _LogOK(r);
  return r;
}

/*********************************************************************
*
*       _ExecDiskFree
*/
static int _ExecDiskFree(const char * s) {
  U32            FreeSpace;
  const char   * sVolumeName;
  char           ac[64];
  FS_DISK_INFO   DiskInfo;
  int            r;
  U32            BytesPerCluster;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  memset(&DiskInfo, 0, sizeof(DiskInfo));
  r = FS_GetVolumeInfo(sVolumeName, &DiskInfo);
  if (r == 0) {
    BytesPerCluster = (U32)DiskInfo.BytesPerSector * (U32)DiskInfo.SectorsPerCluster;
    SEGGER_snprintf(ac, (int)sizeof(ac), "Available free space on \"%s\" is ", sVolumeName);
    _Log(ac);
    if (BytesPerCluster >= 1024u) {
      FreeSpace  = BytesPerCluster >> 10;
      FreeSpace *= DiskInfo.NumFreeClusters;
      if (FreeSpace >= 1024u) {
        FreeSpace >>= 10;
        SEGGER_snprintf(ac, (int)sizeof(ac), "%lu MB\n", FreeSpace);
      } else {
        SEGGER_snprintf(ac, (int)sizeof(ac), "%lu KB\n", FreeSpace);
      }
    } else {
      FreeSpace = (U32)BytesPerCluster * (U32)DiskInfo.NumFreeClusters;
      SEGGER_snprintf(ac, (int)sizeof(ac), "%lu bytes\n", FreeSpace);
    }
    _Log(ac);
  }
  return APP_ERROR_NONE;
}

/*********************************************************************
*
*       _ExecDiskInfo
*/
static int _ExecDiskInfo(const char * s) {
  FS_DISK_INFO   DiskInfo;
  const char   * sVolumeName;
  char           ac[64];
  U32            DiskSpace;
  U32            FreeSpace;
  U32            BytesPerCluster;
  const char   * sFSType;
  int            r;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  r = FS_GetVolumeInfo(sVolumeName, &DiskInfo);
  if (r == 0) {
    switch (DiskInfo.FSType) {
    case FS_TYPE_FAT12:
      sFSType = "FAT12";
      break;
    case FS_TYPE_FAT16:
      sFSType = "FAT16";
      break;
    case FS_TYPE_FAT32:
      sFSType = "FAT32";
      break;
    case FS_TYPE_EFS:
      sFSType = "EFS";
      break;
    default:
      sFSType = "Unknown";
      break;
    }
    BytesPerCluster = (U32)DiskInfo.BytesPerSector * (U32)DiskInfo.SectorsPerCluster;
    DiskSpace       = _CalcKBytes(DiskInfo.NumTotalClusters, BytesPerCluster);
    FreeSpace       = _CalcKBytes(DiskInfo.NumFreeClusters, BytesPerCluster);
    SEGGER_snprintf(ac, (int)sizeof(ac), "Information about \"%s\":\n", sVolumeName);
    _Log(ac);
    SEGGER_snprintf(ac, (int)sizeof(ac), "  File system type: %s\n", sFSType);
    _Log(ac);
    SEGGER_snprintf(ac, (int)sizeof(ac), "  %10lu KB total available space\n", DiskSpace);
    _Log(ac);
    SEGGER_snprintf(ac, (int)sizeof(ac), "  %10lu KB free available space\n", FreeSpace);
    _Log(ac);
    SEGGER_snprintf(ac, (int)sizeof(ac), "  %10lu bytes per cluster\n", BytesPerCluster);
    _Log(ac);
    SEGGER_snprintf(ac, (int)sizeof(ac), "  %10lu total available clusters\n", DiskInfo.NumTotalClusters);
    _Log(ac);
    SEGGER_snprintf(ac, (int)sizeof(ac), "  %10lu free available clusters\n", DiskInfo.NumFreeClusters);
    _Log(ac);
  }
  return r;
}

/*********************************************************************
*
*       _ExecCheckDisk
*/
static int _ExecCheckDisk(const char * s) {
  int          r;
  int          rCheck;
  const char * sVolumeName;
  char         ac[32];
  const char * sBegin;
  U32          MaxRecursionLevel;

  MaxRecursionLevel = MAX_RECURSION_LEVEL_DEFAULT;
  //
  // Parse the volume name (optional)
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Parse the maximum recursion level (optional)
  //
  (void)_ParseDec(&s, &MaxRecursionLevel);
  if (MaxRecursionLevel > MAX_RECURSION_LEVEL) {
    return FS_ERRCODE_INVALID_PARA;
  }
  //
  // Execute the operation.
  //
  _IsAutoRepair = 0;
  _NumFSErrors  = 0;
  SEGGER_snprintf(ac, (int)sizeof(ac), "%lu", MaxRecursionLevel);
  _LogV("Running check disk on \"");
  _LogV(sVolumeName);
  _LogV("\" up to ");
  _LogV(ac);
  _LogV(" level(s) deep...");
  for (;;) {
    rCheck = FS_CheckDisk(sVolumeName, _aBuffer, sizeof(_aBuffer), (int)MaxRecursionLevel, _OnError);
    if (rCheck != FS_CHECKDISK_RETVAL_RETRY) {
      break;
    }
    _LogV(".");
  }
  if (rCheck == FS_CHECKDISK_RETVAL_OK) {
    if (_NumFSErrors != 0u) {
      SEGGER_snprintf(ac, (int)sizeof(ac), "Error (%lu found)\n", _NumFSErrors);
      _LogV(ac);
    } else {
      _LogV("OK (No errors found)\n");
    }
  } else {
    if (rCheck == FS_CHECKDISK_RETVAL_ABORT) {
      _LogV("OK (Aborted)\n");
    } else {
      if (rCheck == FS_CHECKDISK_RETVAL_MAX_RECURSE) {
        _LogV("OK (Maximum recursion level reached)\n");
      }
    }
  }
  r = APP_ERROR_NONE;
  if (rCheck < 0) {
    r = rCheck;
  }
  _IsAutoRepair = 0;
  return r;
}

/*********************************************************************
*
*       _ExecFormat
*/
static int _ExecFormat(const char * s) {
  int              r;
  const char     * sVolumeName;
  const char     * sBegin;
  U32              SectorsPerCluster;
  U32              NumDirEntries;
  FS_FORMAT_INFO   FormatInfo;
  FS_FORMAT_INFO * pFormatInfo;

  SectorsPerCluster = 0;
  NumDirEntries     = 0;
  //
  // Parse the volume name (optional)
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Parse the number of sectors per cluster (optional)
  //
  (void)_ParseDec(&s, &SectorsPerCluster);
  //
  // Parse the number of directory entries in the root directory (optional)
  //
  (void)_ParseDec(&s, &NumDirEntries);
  //
  // Execute the operation.
  //
  _LogV("High-level formatting \"");
  _LogV(sVolumeName);
  _LogV("\"...");
  pFormatInfo = NULL;
  if (SectorsPerCluster != 0u) {
    (void)memset(&FormatInfo, 0, sizeof(FormatInfo));
    FormatInfo.SectorsPerCluster = (U16)SectorsPerCluster;
    FormatInfo.NumRootDirEntries = (U16)NumDirEntries;
    pFormatInfo = &FormatInfo;
  }
  r = FS_Format(sVolumeName, pFormatInfo);
  _LogOK(r);
  if (r == 0) {
    _ChangeToRootDir();
  }
  return r;
}

/*********************************************************************
*
*       _ExecIsHLFormatted
*/
static int _ExecIsHLFormatted(const char * s) {
  int          r;
  const char * sVolumeName;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  r = FS_IsHLFormatted(sVolumeName);
  if (r == 1) {
    _Log("Volume \"");
    _Log(sVolumeName);
    _Log("\" is high-level formatted\n");
    r = 0;
  } else {
    if (r == 0) {
      _Log("Volume \"");
      _Log(sVolumeName);
      _Log("\" is not high-level formatted\n");
    }
  }
  return r;
}

/*********************************************************************
*
*       _ExecMount
*/
static int _ExecMount(const char * s) {
  int          r;
  const char * sVolumeName;
  const char * sBegin;
  unsigned     MountType;

  MountType = 0;            // Mount the volume as configured via FS_SetAutoMount().
  //
  // Parse the volume name (optional)
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Parse the mount type (optional)
  //
  if (_ParseCompareString(&s, "auto") == 0) {
    MountType = 0;
  } else {
    if (_ParseCompareString(&s, "ro") == 0) {
     MountType = FS_MOUNT_RO;
    } else {
      if (_ParseCompareString(&s, "rw") == 0) {
       MountType = FS_MOUNT_RW;
      } else {
        _EatWhite(&s);
        if (*s != '\0') {
          return APP_ERROR_SYNTAX;                      // Error, invalid parameter.
        }
      }
    }
  }
  //
  // Execute the operation.
  //
  _LogV("Mounting \"");
  _LogV(sVolumeName);
  _LogV("\"...");
  if (MountType == 0u) {
    r = FS_Mount(sVolumeName);
  } else {
    r = FS_MountEx(sVolumeName, (U8)MountType);
  }
  if ((unsigned)r == FS_MOUNT_RO) {
    _LogV("OK (RO mode)\n");
    r = 0;
  } else {
    if ((unsigned)r == FS_MOUNT_RW) {
      _LogV("OK (RW mode)\n");
      r = 0;
    }
  }
  return r;
}

/*********************************************************************
*
*       _ExecUnmount
*/
static int _ExecUnmount(const char * s) {
  const char * sVolumeName;
  int          r;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  _LogV("Unmounting \"");
  _LogV(sVolumeName);
  _LogV("\"...");
  FS_Unmount(sVolumeName);
  _LogOK(0);
  return APP_ERROR_NONE;
}

/*********************************************************************
*
*       _ExecSync
*/
static int _ExecSync(const char * s) {
  int          r;
  const char * sVolumeName;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  _LogV("Synchronizing \"");
  _LogV(sVolumeName);
  _LogV("\"...");
  r = FS_Sync(sVolumeName);
  _LogOK(r);
  return r;
}

#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)

/*********************************************************************
*
*       _ExecFormatLow
*/
static int _ExecFormatLow(const char * s) {
  const char * sVolumeName;
  int          r;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  _LogV("Low-level formatting \"");
  _LogV(sVolumeName);
  _LogV("\"...");
  r = FS_FormatLow(sVolumeName);
  _LogOK(r);
  if (r == 0) {
    _ChangeToRootDir();
  }
  return r;
}

/*********************************************************************
*
*       _ExecIsLLFormatted
*/
static int _ExecIsLLFormatted(const char * s) {
  int          r;
  const char * sVolumeName;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  r = FS_IsLLFormatted(sVolumeName);
  if (r == 1) {
    _Log("Volume \"");
    _Log(sVolumeName);
    _Log("\" is low-level formatted\n");
    r = 0;
  } else {
    if (r == 0) {
      _Log("Volume \"");
      _Log(sVolumeName);
      _Log("\" is not low-level formatted\n");
    }
  }
  return r;
}

/*********************************************************************
*
*       _ExecGetStatus
*/
static int _ExecGetStatus(const char * s) {
  int          r;
  const char * sVolumeName;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  _Log("Volume \"");
  _Log(sVolumeName);
  _Log("\" ");
  r = FS_GetVolumeStatus(sVolumeName);
  if (r == FS_MEDIA_NOT_PRESENT){
    _Log("is not present\n");
  } else {
    if (r == FS_MEDIA_IS_PRESENT){
      _Log("is present\n");
    } else {
      _Log("presence status unknown\n");
    }
  }
  return APP_ERROR_NONE;
}

/*********************************************************************
*
*       _ExecListVolumes
*/
static int _ExecListVolumes(const char * s) {
  int  i;
  int  NumVolumes;
  char acVolumeName[20];
  int  r;
  char ac[32];

  FS_USE_PARA(s);
  _Log("Available volumes:\n");
  NumVolumes = FS_GetNumVolumes();
  for (i = 0; i < NumVolumes; i++) {
    memset(ac, 0, sizeof(ac));
    memset(acVolumeName, 0, sizeof(acVolumeName));
    (void)FS_GetVolumeName(i, acVolumeName, (int)sizeof(acVolumeName));
    r = FS_GetMountType(acVolumeName);
    _Log("  ");
    SEGGER_snprintf(ac, (int)sizeof(ac), "%-10s", acVolumeName);
    _Log(ac);
    if (r == 0) {
      (void)strncpy(ac, "not mounted", (int)(sizeof(ac) - 1u));
    } else {
      if ((unsigned)r == FS_MOUNT_RO) {
        (void)strncpy(ac, "mounted in RO mode", (int)(sizeof(ac) - 1u));
      } else {
        if ((unsigned)r == FS_MOUNT_RW) {
          (void)strncpy(ac, "mounted in RW mode", (int)(sizeof(ac) - 1u));
        } else {
          (void)strncpy(ac, FS_ErrorNo2Text(r), (int)(sizeof(ac) - 1u));
        }
      }
    }
    _Log(" (");
    _Log(ac);
    _Log(")");
    _Log("\n");
  }
  return APP_ERROR_NONE;
}

/*********************************************************************
*
*       _ExecShowMemUsage
*/
static int _ExecShowMemUsage(const char * s) {
  char        ac[64];
  FS_MEM_INFO MemInfo;
  int         r;
  unsigned    Percent;

  FS_USE_PARA(s);
  memset(ac, 0, sizeof(ac));
  memset(&MemInfo, 0, sizeof(MemInfo));
  r = FS_GetMemInfo(&MemInfo);
  if (r == 0) {
    if (MemInfo.IsExternal != 0u) {
      SEGGER_snprintf(ac, (int)sizeof(ac), "The file system uses %lu bytes\n", MemInfo.NumBytesAllocated);
    } else {
      Percent = MemInfo.NumBytesAllocated * 100u / MemInfo.NumBytesTotal;
      SEGGER_snprintf(ac, (int)sizeof(ac), "The file system uses %lu of %lu bytes (%u%%)\n", MemInfo.NumBytesAllocated, MemInfo.NumBytesTotal, Percent);
    }
    _Log(ac);
  }
  return r;
}

#if FS_SUPPORT_JOURNAL

/*********************************************************************
*
*       _ExecCreateJournal
*/
static int _ExecCreateJournal(const char * s) {
  int          r;
  U32          NumBytes;
  const char * sVolumeName;
  const char * sBegin;
  char         ac[16];
  int          SupportFreeSector;

  SupportFreeSector = 0;
  //
  // Parse the volume name (optional).
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Parse the journal size (mandatory)
  //
  if (_ParseDec(&s, &NumBytes) != 0) {
    return APP_ERROR_SYNTAX;
  }
  //
  // Parse the free sector support activation (optional)
  //
  (void)_ParseBool(&s, &SupportFreeSector);
  //
  // Execute the operation.
  //
  r = FS_JOURNAL_IsPresent(sVolumeName);
  if (r < 0) {
    return r;                                 // Error, could not get journal presence status.
  }
  if (r == 1) {
    r = _RemoveJournal(sVolumeName);
  }
  if (r == 0) {
    SEGGER_snprintf(ac, (int)sizeof(ac), "%lu", NumBytes);
    _LogV("Creating journal on \"");
    _LogV(sVolumeName);
    _LogV("\" of ");
    _LogV(ac);
    _LogV(" bytes...");
    r = FS_JOURNAL_CreateEx(sVolumeName, NumBytes, (U8)SupportFreeSector);
    _LogOK(r);
  }
  return r;
}

/*********************************************************************
*
*       _ExecRemoveJournal
*/
static int _ExecRemoveJournal(const char * s) {
  int          r;
  const char * sVolumeName;

  //
  // Parse the volume name (optional).
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  r = FS_JOURNAL_IsPresent(sVolumeName);
  if (r < 0) {
    return r;                                 // Error, could not get journal presence status.
  }
  if (r == 0) {
    _LogV("Journal not present on \"");
    _LogV(sVolumeName);
    _LogV("\"\n");
  } else {
    _LogV("Removing journal from \"");
    _LogV(sVolumeName);
    _LogV("\"...");
    r = _RemoveJournal(sVolumeName);
    _LogOK(r);
  }
  return r;
}

#endif // FS_SUPPORT_JOURNAL

#if FS_SUPPORT_FAT

/*********************************************************************
*
*       _ExecFormatSD
*/
static int _ExecFormatSD(const char * s) {
  int          r;
  const char * sVolumeName;

  //
  // Parse the volume name (optional)
  //
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  //
  // Execute the operation.
  //
  _LogV("High-level formatting \"");
  _LogV(sVolumeName);
  _LogV("\"...");
  r = FS_FAT_FormatSD(sVolumeName);
  _LogOK(r);
  if (r == 0) {
    _ChangeToRootDir();
  }
  return r;
}

/*********************************************************************
*
*       _ExecSetLFNSupport
*/
static int _ExecSetLFNSupport(const char * s) {
  int           r;
  int           ShowStatus;
  int           Status;
  FS_FAT_CONFIG Config;

  ShowStatus = 0;
  Status     = 0;
  //
  // Parse the activation status (optional)
  //
  r = _ParseBool(&s, &Status);
  if (r == 0) {
    if (Status != 0) {
      FS_FAT_SupportLFN();
    } else {
      FS_FAT_DisableLFN();
    }
  } else {
    _EatWhite(&s);
    if (*s != '\0') {
      r = APP_ERROR_SYNTAX;                       // Error, invalid parameter.
    } else {
      ShowStatus = 1;
      r = 0;
    }
  }
  if (ShowStatus != 0) {
    //
    // Show the actual status.
    //
    memset(&Config, 0, sizeof(Config));
    (void)FS_FAT_GetConfig(&Config);
    _Log("Support for LFN is ");
    if (Config.IsLFNSupported != 0u) {
      _Log("enabled\n");
    } else {
      _Log("disabled\n");
    }
  }
  return r;
}

/*********************************************************************
*
*       _ExecSetVolumeLabel
*/
static int _ExecSetVolumeLabel(const char * s) {
  int          r;
  char         acLabel[16];
  const char * sVolumeName;
  const char * sBegin;

  //
  // Get the volume name (optional).
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Get the volume label (optional).
  //
  r = _ParseString(&s, acLabel, sizeof(acLabel));
  if (r != 0) {
    return r;
  }
  //
  // Execute the operation.
  //
  if (acLabel[0] == '\0') {
    //
    // Display the current volume label.
    //
    r = FS_GetVolumeLabel(sVolumeName, acLabel, sizeof(acLabel));
    if (r == 0) {
      _Log("Label of \"");
      _Log(sVolumeName);
      _Log("\" is set to \"");
      _Log(acLabel);
      _Log("\"\n");
    } else {
      if (r == FS_ERRCODE_FILE_DIR_NOT_FOUND) {
        _Log("Label of \"");
        _Log(sVolumeName);
        _Log("\" is not set\n");
        r = 0;
      }
    }
  } else {
    //
    // Set a new volume label.
    //
    _LogV("Setting label of \"");
    _LogV(sVolumeName);
    _LogV("\" to \"");
    _LogV(acLabel);
    _LogV("\"...");
    r = FS_SetVolumeLabel(sVolumeName, acLabel);
    _LogOK(r);
  }
  return r;
}

#endif //FS_SUPPORT_FAT

#if (FS_OS_LOCKING != 0)

/*********************************************************************
*
*       _ExecMeasureWriteSpeedPA
*/
static int _ExecMeasureWriteSpeedPA(const char * s) {
  RESULT       Result;
  U32          FreeSpace;
  U32          NumLoops;
  U32          NumBytesToWrite;
  FS_FILE    * pFile;
  char         acFileName[FS_MAX_PATH];
  U32          t;
  U32          i;
  U32          NumBlocksToMeasure;
  U32          BytesPerBlock;
  U32          FileSize;
  const char * sVolumeName;
  char         ac[64];
  const char * sBegin;
  int          r;
  int          NumChars;

  //
  // Set the command parameters to default values.
  //
  NumBlocksToMeasure = NUM_BLOCKS_TO_MEASURE;
  BytesPerBlock      = SEGGER_MIN(sizeof(_aBuffer), MAX_BLOCK_SIZE);
  FileSize           = FILE_SIZE;
  //
  // Parse the volume name (optional).
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Parse the file size (optional).
  //
  (void)_ParseDec(&s, &FileSize);
  //
  // Parse the number of bytes per read block (optional).
  //
  (void)_ParseDec(&s, &BytesPerBlock);
  BytesPerBlock = SEGGER_MIN(BytesPerBlock, sizeof(_aBuffer));
  //
  // Parse the number of blocks to be read (optional).
  //
  (void)_ParseDec(&s, &NumBlocksToMeasure);
  if (NumBlocksToMeasure == 0) {
    NumBlocksToMeasure = 1;
  }
  //
  // Create the name of the file to be used for testing.
  //
  NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sVolumeName, FS_DIRECTORY_DELIMITER, FILE_NAME);
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Verify that there is sufficient free space on the storage device for the test file.
  //
  FreeSpace = FS_GetVolumeFreeSpace(sVolumeName);
  if (FreeSpace < FileSize) {
    return FS_ERRCODE_VOLUME_FULL;
  }
  //
  // Fill the write buffer with a pattern and calculate the how many bytes we have to write at once and for how many times.
  //
  FS_MEMSET((void*)_aBuffer, 'a', sizeof(_aBuffer));
  NumBytesToWrite = BytesPerBlock * NumBlocksToMeasure;
  NumLoops        = FileSize / NumBytesToWrite;
  if (NumLoops == 0) {
    return FS_ERRCODE_INVALID_PARA;
  }
  //
  // Open the file and allocate the storage for it.
  //
  _InitResult(&Result, "Write (Preallocated file)", NumBytesToWrite);
  pFile = NULL;
  r = FS_FOpenEx(acFileName, "w", &pFile);
  if (r != 0) {
    return r;               // Error, could not open file.
  }
  FS_SetFilePos(pFile, FileSize, FS_SEEK_SET);
  r = FS_SetEndOfFile(pFile);
  if (r != 0) {
    (void)FS_FClose(pFile);
    return r;               // Error, could not set the file size.
  }
  FS_SetFilePos(pFile, 0, FS_SEEK_SET);
  //
  // Write to file and measure the performance.
  //
  SEGGER_snprintf(ac, (int)sizeof(ac), "Writing %ld chunks of %ld bytes", NumLoops, NumBytesToWrite);
  _LogV(ac);
  FS_STORAGE_ResetCounters();
  for (i = 0; i < NumLoops ; i++) {
    t = _WriteFile(pFile, _aBuffer, BytesPerBlock, NumBlocksToMeasure);
    _StoreResult(&Result, t);
    _LogV(".");
  }
  _LogOK(0);
  FS_STORAGE_GetCounters(&Result.StorageCounter);
  (void)FS_FClose(pFile);
  _ShowPerformanceResult(&Result);
  return APP_ERROR_NONE;
}

/*********************************************************************
*
*       _ExecMeasureWriteSpeedDA
*/
static int _ExecMeasureWriteSpeedDA(const char * s) {
  RESULT       Result;
  U32          FreeSpace;
  U32          NumLoops;
  U32          NumBytesToWrite;
  FS_FILE    * pFile;
  char         acFileName[FS_MAX_PATH];
  U32          t;
  U32          i;
  U32          NumBlocksToMeasure;
  U32          BytesPerBlock;
  U32          FileSize;
  const char * sVolumeName;
  char         ac[64];
  const char * sBegin;
  int          r;
  int          NumChars;

  //
  // Set the command parameters to default values.
  //
  NumBlocksToMeasure = NUM_BLOCKS_TO_MEASURE;
  BytesPerBlock      = SEGGER_MIN(sizeof(_aBuffer), MAX_BLOCK_SIZE);
  FileSize           = FILE_SIZE;
  //
  // Parse the volume name (optional).
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Parse the file size (optional).
  //
  (void)_ParseDec(&s, &FileSize);
  //
  // Parse the number of bytes per read block (optional).
  //
  (void)_ParseDec(&s, &BytesPerBlock);
  BytesPerBlock = SEGGER_MIN(BytesPerBlock, sizeof(_aBuffer));
  //
  // Parse the number of blocks to be read (optional).
  //
  (void)_ParseDec(&s, &NumBlocksToMeasure);
  if (NumBlocksToMeasure == 0) {
    NumBlocksToMeasure = 1;
  }
  //
  // Create the name of the file to be used for testing.
  //
  NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sVolumeName, FS_DIRECTORY_DELIMITER, FILE_NAME);
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Verify that there is sufficient free space on the storage device for the test file.
  //
  FreeSpace = FS_GetVolumeFreeSpace(sVolumeName);
  if (FreeSpace < FileSize) {
    return FS_ERRCODE_VOLUME_FULL;
  }
  //
  // Fill the write buffer with a pattern and calculate the how many bytes we have to write at once and for how many times.
  //
  FS_MEMSET((void*)_aBuffer, 'a', sizeof(_aBuffer));
  NumBytesToWrite = BytesPerBlock * NumBlocksToMeasure;
  NumLoops        = FileSize / NumBytesToWrite;
  if (NumLoops == 0) {
    return FS_ERRCODE_INVALID_PARA;
  }
  //
  // Open the file.
  //
  _InitResult(&Result, "Write (Dynamically allocated file)", NumBytesToWrite);
  pFile = NULL;
  r = FS_FOpenEx(acFileName, "w", &pFile);
  if (r != 0) {
    return r;               // Error, could not open file.
  }
  //
  // Write to file and measure the performance.
  //
  SEGGER_snprintf(ac, (int)sizeof(ac), "Writing %ld chunks of %ld bytes", NumLoops, NumBytesToWrite);
  _LogV(ac);
  FS_STORAGE_ResetCounters();
  for (i = 0; i < NumLoops ; i++) {
    t = _WriteFile(pFile, _aBuffer, BytesPerBlock, NumBlocksToMeasure);
    _StoreResult(&Result, t);
    _LogV(".");
  }
  _LogOK(0);
  FS_STORAGE_GetCounters(&Result.StorageCounter);
  (void)FS_FClose(pFile);
  _ShowPerformanceResult(&Result);
  return APP_ERROR_NONE;
}

/*********************************************************************
*
*       _ExecMeasureReadSpeed
*/
static int _ExecMeasureReadSpeed(const char * s) {
  RESULT       Result;
  U32          FreeSpace;
  U32          NumLoops;
  U32          NumBytesToRead;
  FS_FILE    * pFile;
  char         acFileName[FS_MAX_PATH];
  U32          t;
  U32          i;
  U32          NumBlocksToMeasure;
  U32          BytesPerBlock;
  U32          FileSize;
  const char * sVolumeName;
  char         ac[64];
  const char * sBegin;
  int          r;
  int          NumChars;

  //
  // Set the command parameters to default values.
  //
  NumBlocksToMeasure = NUM_BLOCKS_TO_MEASURE;
  BytesPerBlock      = SEGGER_MIN(sizeof(_aBuffer), MAX_BLOCK_SIZE);
  FileSize           = FILE_SIZE;
  //
  // Parse the volume name (optional).
  //
  sBegin = s;
  r = _ParseString(&s, _ac, sizeof(_ac));
  if (r != 0) {
    return r;
  }
  sVolumeName = _GetVolumeName(_ac);
  if (sVolumeName != _ac) {
    s = sBegin;
  }
  //
  // Parse the file size (optional).
  //
  (void)_ParseDec(&s, &FileSize);
  //
  // Parse the number of bytes per read block (optional).
  //
  (void)_ParseDec(&s, &BytesPerBlock);
  BytesPerBlock = SEGGER_MIN(BytesPerBlock, sizeof(_aBuffer));
  //
  // Parse the number of blocks to be read (optional).
  //
  (void)_ParseDec(&s, &NumBlocksToMeasure);
  if (NumBlocksToMeasure == 0) {
    NumBlocksToMeasure = 1;
  }
  //
  // Create the name of the file to be used for testing.
  //
  NumChars = SEGGER_snprintf(acFileName, (int)sizeof(acFileName), "%s%c%s", sVolumeName, FS_DIRECTORY_DELIMITER, FILE_NAME);
  if ((unsigned)NumChars >= sizeof(acFileName)) {
    return FS_ERRCODE_PATH_TOO_LONG;
  }
  //
  // Verify that there is sufficient free space on the storage device for the test file.
  //
  FreeSpace = FS_GetVolumeFreeSpace(sVolumeName);
  if (FreeSpace < FileSize) {
    return FS_ERRCODE_VOLUME_FULL;
  }
  //
  // Calculate the how many bytes we have to write at once and for how many times.
  //
  NumBytesToRead = BytesPerBlock * NumBlocksToMeasure;
  NumLoops       = FileSize / NumBytesToRead;
  if (NumLoops == 0) {
    return FS_ERRCODE_INVALID_PARA;
  }
  //
  // Open the file and fill the file with data.
  //
  _InitResult(&Result, "Read", NumBytesToRead);
  pFile = NULL;
  r = FS_FOpenEx(acFileName, "w+", &pFile);
  if (r != 0) {
    return r;               // Error, could not open file.
  }
  memset(_aBuffer, 0, sizeof(_aBuffer));
  for (i = 0; i < NumLoops; i++) {
    (void)_WriteFile(pFile, _aBuffer, BytesPerBlock, NumBlocksToMeasure);
  }
  FS_FSeek(pFile, 0, FS_SEEK_SET);
  //
  // Read from the file and measure the performance.
  //
  SEGGER_snprintf(ac, (int)sizeof(ac), "Reading %ld chunks of %ld bytes", NumLoops, NumBytesToRead);
  _LogV(ac);
  FS_STORAGE_ResetCounters();
  for (i = 0; i < NumLoops ; i++) {
    t = _ReadFile(pFile, _aBuffer, BytesPerBlock, NumBlocksToMeasure);
    _StoreResult(&Result, t);
    _LogV(".");
  }
  _LogOK(0);
  FS_STORAGE_GetCounters(&Result.StorageCounter);
  (void)FS_FClose(pFile);
  _ShowPerformanceResult(&Result);
  return APP_ERROR_NONE;
}

#endif // (FS_OS_LOCKING != 0)

#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)

/*********************************************************************
*
*       _ExecSetLogFilter
*/
static int _ExecSetLogFilter(const char * s) {
  U32                   Mask;
  int                   r;
  char                  ac[16];
  unsigned              NumChars;
  unsigned              i;
  char                  c;
  const VALUE_TO_TEXT * pMTypeToText;
  const char          * sText;
  U32                   Value;
  int                   ShowMask;

  r        = APP_ERROR_NONE;
  Mask     = 0;
  ShowMask = 0;
  memset(ac, 0, sizeof(ac));
  _EatWhite(&s);
  if (*s == '\0') {
    ShowMask = 1;
  } else {
    for (;;) {
      r = _ParseString(&s, ac, sizeof(ac));
      if (r != 0) {
        break;
      }
      if (ac[0] == '\0') {
        break;                              // OK, end of list reached.
      }
      NumChars = (U32)strlen(ac);
      if (NumChars == 0) {
        break;
      }
      for (i = 0; i < NumChars; ++i) {
        c = ac[i];
        c = toupper(c);
        ac[i] = c;
      }
      pMTypeToText = _aMTypeToText;
      for (i = 0; i < SEGGER_COUNTOF(_aMTypeToText); ++i) {
        sText = pMTypeToText->sText;
        Value = pMTypeToText->Value;
        if (strstr(ac, sText) != NULL) {
          Mask |= Value;
          break;
        }
        ++pMTypeToText;
      }
      if (i == SEGGER_COUNTOF(_aMTypeToText)) {
        if (strstr(ac, "DISABLEALL") != NULL) {
          Mask = 0;
        } else {
          if (strstr(ac, "ENABLEALL") != NULL) {
            Mask = 0xFFFFFFFFu;
          } else {
            return APP_ERROR_SYNTAX;
          }
        }
      }
    }
    FS_SetLogFilter(Mask);
  }
  if (ShowMask != 0) {
    Mask = FS_GetLogFilter();
    if (Mask == 0) {
      _Log("All log messages are disabled\n");
    } else {
      _Log("The following log messages are enabled:");
      pMTypeToText = _aMTypeToText;
      for (i = 0; i < SEGGER_COUNTOF(_aMTypeToText); ++i) {
        sText = pMTypeToText->sText;
        Value = pMTypeToText->Value;
        if ((Mask & Value) != 0) {
          _Log(" ");
          _Log(sText);
        }
        ++pMTypeToText;
      }
      _Log("\n");
    }
  }
  return r;
}

#endif // FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL

#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS)

/*********************************************************************
*
*       _ExecSetWarnFilter
*/
static int _ExecSetWarnFilter(const char * s) {
  U32                   Mask;
  int                   r;
  char                  ac[16];
  unsigned              NumChars;
  unsigned              i;
  char                  c;
  const VALUE_TO_TEXT * pMTypeToText;
  const char          * sText;
  U32                   Value;
  int                   ShowMask;

  r        = APP_ERROR_NONE;
  Mask     = 0;
  ShowMask = 0;
  memset(ac, 0, sizeof(ac));
  _EatWhite(&s);
  if (*s == '\0') {
    ShowMask = 1;
  } else {
    for (;;) {
      r = _ParseString(&s, ac, sizeof(ac));
      if (r != 0) {
        break;
      }
      if (ac[0] == '\0') {
        break;                              // OK, end of list reached.
      }
      NumChars = (U32)strlen(ac);
      if (NumChars == 0) {
        break;
      }
      for (i = 0; i < NumChars; ++i) {
        c = ac[i];
        c = toupper(c);
        ac[i] = c;
      }
      pMTypeToText = _aMTypeToText;
      for (i = 0; i < SEGGER_COUNTOF(_aMTypeToText); ++i) {
        sText = pMTypeToText->sText;
        Value = pMTypeToText->Value;
        if (strstr(ac, sText) != NULL) {
          Mask |= Value;
          break;
        }
        ++pMTypeToText;
      }
      if (i == SEGGER_COUNTOF(_aMTypeToText)) {
        if (strstr(ac, "DISABLEALL") != NULL) {
          Mask = 0;
        } else {
          if (strstr(ac, "ENABLEALL") != NULL) {
            Mask = 0xFFFFFFFFu;
          } else {
            return APP_ERROR_SYNTAX;
          }
        }
      }
    }
    FS_SetWarnFilter(Mask);
  }
  if (ShowMask != 0) {
    Mask = FS_GetWarnFilter();
    if (Mask == 0) {
      _Log("All warning messages are disabled\n");
    } else {
      _Log("The following warning messages are enabled:");
      pMTypeToText = _aMTypeToText;
      for (i = 0; i < SEGGER_COUNTOF(_aMTypeToText); ++i) {
        sText = pMTypeToText->sText;
        Value = pMTypeToText->Value;
        if ((Mask & Value) != 0) {
          _Log(" ");
          _Log(sText);
        }
        ++pMTypeToText;
      }
      _Log("\n");
    }
  }
  return r;
}

#endif // FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS

#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)

/*********************************************************************
*
*       _ExecSetErrorFilter
*/
static int _ExecSetErrorFilter(const char * s) {
  U32                   Mask;
  int                   r;
  char                  ac[16];
  unsigned              NumChars;
  unsigned              i;
  char                  c;
  const VALUE_TO_TEXT * pMTypeToText;
  const char          * sText;
  U32                   Value;
  int                   ShowMask;

  r        = APP_ERROR_NONE;
  Mask     = 0;
  ShowMask = 0;
  memset(ac, 0, sizeof(ac));
  _EatWhite(&s);
  if (*s == '\0') {
    ShowMask = 1;
  } else {
    for (;;) {
      r = _ParseString(&s, ac, sizeof(ac));
      if (r != 0) {
        break;
      }
      if (ac[0] == '\0') {
        break;                              // OK, end of list reached.
      }
      NumChars = (U32)strlen(ac);
      if (NumChars == 0) {
        break;
      }
      for (i = 0; i < NumChars; ++i) {
        c = ac[i];
        c = toupper(c);
        ac[i] = c;
      }
      pMTypeToText = _aMTypeToText;
      for (i = 0; i < SEGGER_COUNTOF(_aMTypeToText); ++i) {
        sText = pMTypeToText->sText;
        Value = pMTypeToText->Value;
        if (strstr(ac, sText) != NULL) {
          Mask |= Value;
          break;
        }
        ++pMTypeToText;
      }
      if (i == SEGGER_COUNTOF(_aMTypeToText)) {
        if (strstr(ac, "DISABLEALL") != NULL) {
          Mask = 0;
        } else {
          if (strstr(ac, "ENABLEALL") != NULL) {
            Mask = 0xFFFFFFFFu;
          } else {
            return APP_ERROR_SYNTAX;
          }
        }
      }
    }
    FS_SetErrorFilter(Mask);
  }
  if (ShowMask != 0) {
    Mask = FS_GetErrorFilter();
    if (Mask == 0) {
      _Log("All errors messages are disabled\n");
    } else {
      _Log("The following errors messages are enabled:");
      pMTypeToText = _aMTypeToText;
      for (i = 0; i < SEGGER_COUNTOF(_aMTypeToText); ++i) {
        sText = pMTypeToText->sText;
        Value = pMTypeToText->Value;
        if ((Mask & Value) != 0) {
          _Log(" ");
          _Log(sText);
        }
        ++pMTypeToText;
      }
      _Log("\n");
    }
  }
  return r;
}

#endif // FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS

/*********************************************************************
*
*       _ExecSetVerbosity
*/
static int _ExecSetVerbosity(const char * s) {
  int r;
  int ShowStatus;
  int Status;

  ShowStatus = 0;
  Status     = 0;
  //
  // Parse the activation status (optional)
  //
  r = _ParseBool(&s, &Status);
  if (r == 0) {
    _IsVerbose = (U8)Status;
  } else {
    _EatWhite(&s);
    if (*s != '\0') {
      r = APP_ERROR_SYNTAX;                         // Error, invalid parameter.
    } else {
      ShowStatus = 1;
      r = 0;
    }
  }
  if (ShowStatus != 0) {
    //
    // Show the actual status.
    //
    _Log("Verbosity is ");
    if (_IsVerbose != 0u) {
      _Log("enabled\n");
    } else {
      _Log("disabled\n");
    }
  }
  return r;
}

/*********************************************************************
*
*       Lost of basic command descriptors
*/
static const COMMAND _aCommand[] = {
  {_ExecGetStatus,            "gs",         "GetStatus",          "Shows the presence status of a volume",                          "[<VolumeName>]"                                                     },
  {_ExecListVolumes,          "lv",         "ListVolumes",        "Shows names of the available volumes",                           ""                                                                   },
  {_ExecShowMemUsage,         "mem",        "MemUsage",           "Shows the amount of memory used",                                ""                                                                   },
  {_ExecFormatLow,            "fmtl",       "FormatLow",          "Performs a low-level format of a volume",                        "[<VolumeName>]"                                                     },
  {_ExecIsLLFormatted,        "isllf",      "IsLLFormatted",      "Checks if a volume is low-level formatted",                      "[<VolumeName>]"                                                     },
#if (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)
  {_ExecChangeDir,            "cd",         "ChangeDir",          "Changes the current directory or displays its name",             "[<PathToDir>] or [..]"                                              },
  {_ExecCreateDir,            "crd",        "CreateDir",          "Creates a directory recursively",                                "<PathToDir>"                                                        },
  {_ExecShowDir,              "dir",        "ls",                 "Shows the content of a directory",                               "[<PathToDir>]"                                                      },
  {_ExecShowDirTree,          "tree",       "ShowDirTree",        "Shows the content of a directory recursively",                   "[<PathToDir>] [<MaxRecursionLevel>]"                                },
  {_ExecMkDir,                "md",         "MakeDir",            "Creates a directory",                                            "<PathToDir>"                                                        },
  {_ExecRemoveDir,            "rd",         "RemoveDir",          "Removes a directory",                                            "<PathToDir>"                                                        },
  {_ExecDeleteDir,            "dld",        "DeleteDir",          "Removes a directory recursively",                                "<PathToDir> [<MaxRecursionLevel>]"                                  },
  {_ExecMakeFile,             "mf",         "MakeFile",           "Creates a file with a defined size",                             "<PathToFile> [<BlockSize> [<NumBlocks> [<DoFill> [<StartValue>]]]]" },
  {_ExecType,                 "type",       "ShowFile",           "Shows the contents of a file",                                   "<PathToFile>"                                                       },
  {_ExecTypeHex,              "th",         "ShowFileHex",        "Shows the contents of a file as hexadecimal",                    "<PathToFile>"                                                       },
  {_ExecDeleteFile,           "rm",         "RemoveFile",         "Deletes a file",                                                 "<PathToFile>"                                                       },
  {_ExecRename,               "ren",        "Rename",             "Renames a file or directory",                                    "<PathToFile> <NewFileName> | <PathToDir> <NewDirName>"              },
  {_ExecMove,                 "mv",         "Move",               "Moves a file or directory",                                      "<PathToFileSrc> <PathToFileDest> | <PathToDirSrc> <PathToDirDest>"  },
  {_ExecSetGetAttr,           "attr",       "Attributes",         "Shows or changes the file or directory attributes",              "<PathToFile> <[+|-][ahrs]>"                                         },
  {_ExecCopy,                 "cp",         "Copy",               "Copies a file",                                                  "<PathToFileSrc> <PathToFileDest>"                                   },
  {_ExecTruncate,             "tr",         "TruncateFile",       "Truncates a file",                                               "<PathToFile> [<NumBytes>]"                                          },
  {_ExecWipeFile,             "wipe",       "WipeFile",           "Overwrites the contents of a file with random data",             "<PathToFile>"                                                       },
  {_ExecDiskFree,             "df",         "DiskFree",           "Shows the available free space on a volume",                     "[<VolumeName>]"                                                     },
  {_ExecDiskInfo,             "di",         "DiskInfo",           "Shows information about a volume",                               "[<VolumeName>]"                                                     },
  {_ExecCheckDisk,            "chkdsk",     "CheckDisk",          "Checks the file system consistency",                             "[<VolumeName>] [<MaxRecursionLevel>]"                               },
  {_ExecFormat,               "fmt",        "Format",             "Performs a high-level format of volume",                         "[<VolumeName>] [<SectorsPerCluster> [<NumRootDirEntries>]]"         },
  {_ExecIsHLFormatted,        "ishlf",      "IsHLFormatted",      "Checks if a volume is high-level formatted",                     "[<VolumeName>]"                                                     },
  {_ExecMount,                "mnt",        "Mount",              "Mounts a volume",                                                "[<VolumeName>] [MountType]"                                         },
  {_ExecUnmount,              "umnt",       "Unmount",            "Unmounts a volume",                                              "[<VolumeName>]"                                                     },
  {_ExecSync,                 "sync",       "Sync",               "Saves cached information to storage",                            "[<VolumeName>]"                                                     },
#endif // (FS_SUPPORT_FAT != 0) || (FS_SUPPORT_EFS != 0)
#if FS_SUPPORT_JOURNAL
  {_ExecCreateJournal,        "cj",         "CreateJournal",      "Creates and enables the journal on a volume",                    "[<VolumeName>] <NumBytes> [<SupportFreeSector>]"                    },
  {_ExecRemoveJournal,        "rj",         "RemoveJournal",      "Disables and removes the journal from a volume",                 "[<VolumeName>]"                                                     },
#endif //FS_SUPPORT_JOURNAL
#if FS_SUPPORT_FAT
  {_ExecFormatSD,             "fmtsd",      "FormatSD",           "Performs a high-level format of volume (SD specification)",      "[<VolumeName>]"                                                     },
  {_ExecSetLFNSupport,        "lfn",        "SetLFNSupport",      "Enables or disables the support for LFN or shows its status",    "[ON|OFF]"                                                           },
  {_ExecSetVolumeLabel,       "svl",        "SetVolumeLabel",     "Sets the label of a volume or shows it",                         "[<VolumeName>] [<VolumeLabel>]"                                     },
#endif //FS_SUPPORT_FAT
#if (FS_OS_LOCKING != 0)
  {_ExecMeasureWriteSpeedPA,  "mwspa",      "MeasureWSpeedPA",    "Measures the write speed (preallocated file)",                   "[<VolumeName>] [<FileSize> [<BytesPerBlock> [<NumBlocksMeasure>]]]" },
  {_ExecMeasureWriteSpeedDA,  "mwsda",      "MeasureWSpeedDA",    "Measures the write speed (dynamically allocated file)",          "[<VolumeName>] [<FileSize> [<BytesPerBlock> [<NumBlocksMeasure>]]]" },
  {_ExecMeasureReadSpeed,     "mrs",        "MeasureRSpeed",      "Measures the read speed",                                        "[<VolumeName>] [<FileSize> [<BytesPerBlock> [<NumBlocksMeasure>]]]" },
#endif // (FS_OS_LOCKING != 0)
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
  {_ExecSetLogFilter,         "slf",        "SetLogFilter",       "Enables and disables file system log messages",                  "[<MessageTypeList>]"                                                },
#endif // (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS)
  {_ExecSetWarnFilter,        "swf",        "SetWarnFilter",      "Enables and disables file system warning messages",              "[<MessageTypeList>]"                                                },
#endif // (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS)
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)
  {_ExecSetErrorFilter,       "sef",        "SetErrorFilter",     "Enables and disables file system error messages",                "[<MessageTypeList>]"                                                },
#endif // (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)
  {_ExecSetVerbosity,         "sv",         "SetVerbosity",       "Enables or disables application log messages",                   "[ON|OFF]"                                                           }
};

/*********************************************************************
*
*       _ShowCommandHelp
*
*  Function description
*    Shows information about the usage of a command.
*/
static void _ShowCommandHelp(const COMMAND * pCommand) {
  char ac[128];

  SEGGER_snprintf(ac, (int)sizeof(ac), "%-20s", pCommand->sShortName);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac), "%s\n",  pCommand->sDescription);
  _Log(ac);
  SEGGER_snprintf(ac, (int)sizeof(ac), "%-20s %s %s\n", " ", pCommand->sShortName, pCommand->sSyntax);
  _Log(ac);
}

/*********************************************************************
*
*       _ShowCommandDescription
*
*  Function description
*    Shows command description.
*/
static void _ShowCommandDescription(const COMMAND * pCommand) {
  char ac[128];

  SEGGER_snprintf(ac, (int)sizeof(ac), "%-12s %s\n", "Description:", pCommand->sDescription);
  _Log(ac);
}

/*********************************************************************
*
*       _ShowCommandUsage
*
*  Function description
*    Shows information about the usage of a command.
*/
static void _ShowCommandUsage(const COMMAND * pCommand) {
  char ac[128];

  SEGGER_snprintf(ac, (int)sizeof(ac), "%-12s %s %s\n", "Usage:", pCommand->sShortName, pCommand->sSyntax);
  _Log(ac);
}

/*********************************************************************
*
*       _ShowCommandUnknown
*/
static void _ShowCommandUnknown(const char * s) {
  _Log("Unknown command \"");
  _Log(s);
  _Log("\". Type '?' or 'help' for a list of commands.\n");
}

/*********************************************************************
*
*       _ShowHelp
*
*  Function description
*    Shows information about all the supported commands.
*/
static void _ShowHelp(void){
  const COMMAND_SET * pCommandSet;
  const COMMAND     * pCommand;
  unsigned            NumItems;
  unsigned            i;

  pCommandSet = _paCommandSet;
  for (;;) {
    pCommand = pCommandSet->pCommand;
    NumItems = pCommandSet->NumItems;
    if ((pCommand == NULL) || (NumItems == 0u)) {
      break;                    // End of list reached.
    }
    for (i = 0; i < NumItems; ++i) {
      _ShowCommandHelp(pCommand);
      ++pCommand;
    }
    ++pCommandSet;
  }
}

/*********************************************************************
*
*       _HandleCommandError
*/
static void _HandleCommandError(int ExitCode, const COMMAND * pCommand){
  char ac[128];

  if (ExitCode < 0) {
    SEGGER_snprintf(ac, (int)sizeof(ac), "Error (%s)\n", FS_ErrorNo2Text(ExitCode));
    _Log(ac);
#if USE_SIMULATOR
    _NumCommandErrors++;
#endif // USE_SIMULATOR
  } else {
    if (ExitCode == APP_ERROR_SYNTAX) {
      _ShowCommandUsage(pCommand);
#if USE_SIMULATOR
      _NumCommandErrors++;
#endif // USE_SIMULATOR
    }
  }
}

/*********************************************************************
*
*       _FindCommand
*/
static const COMMAND * _FindCommand(const char ** ps) {
  const COMMAND_SET * pCommandSet;
  const COMMAND     * pCommand;
  unsigned            NumItems;
  unsigned            i;
  int                 IsMatch;

  pCommandSet = _paCommandSet;
  for (;;) {
    pCommand = pCommandSet->pCommand;
    NumItems = pCommandSet->NumItems;
    if ((pCommand == NULL) || (NumItems == 0u)) {
      break;                    // End of list reached.
    }
    for (i = 0; i < NumItems; ++i) {
      IsMatch = 0;
      if (_ParseCompareString(ps, pCommand->sLongName) == 0) {
        IsMatch = 1;
      } else {
        if (_ParseCompareString(ps, pCommand->sShortName) == 0) {
          IsMatch = 1;
        }
      }
      if (IsMatch != 0) {
        return pCommand;        // Command found.
      }
      ++pCommand;
    }
    ++pCommandSet;
  }
  return pCommand;
}

/*********************************************************************
*
*       _ExecCommand
*/
static int _ExecCommand(const char * s) {
  int             r;
  const COMMAND * pCommand;
  int             IsMatch;

  pCommand = _FindCommand(&s);
  if (pCommand != NULL) {
    r = pCommand->pfHandler(s);
    _HandleCommandError(r, pCommand);
    return 1;                                                                             // Continue processing commands.
  }
#if USE_SIMULATOR
  IsMatch = 0;
  if (_ParseCompareString(&s, "q") == 0) {
    IsMatch = 1;
  } else {
    if (_ParseCompareString(&s, "exit") == 0) {
      IsMatch = 1;
    }
  }
  if (IsMatch != 0) {
    return 0;                                                                             // Quit the processing loop.
  }
#endif // USE_SIMULATOR
  IsMatch = 0;
  if (_ParseCompareString(&s, "?") == 0) {
    IsMatch = 1;
  } else {
    if (_ParseCompareString(&s, "help") == 0) {
      IsMatch = 1;
    }
  }
  if (IsMatch != 0) {
    _EatWhite(&s);
    if (*s == '\0') {
      _ShowHelp();                                                                        // Show information about all supported commands.
    } else {
      pCommand = _FindCommand(&s);
      if (pCommand != NULL) {
        _ShowCommandDescription(pCommand);
        _ShowCommandUsage(pCommand);
      } else {
        _ShowCommandUnknown(s);
      }
    }
  } else {
    if (*s != '\0') {
      _ShowCommandUnknown(s);
#if USE_SIMULATOR
      _NumCommandErrors++;
#endif // USE_SIMULATOR
    }
  }
  return 1;                                                                               // Continue processing commands.
}

/*********************************************************************
*
*       _ProcessInput
*/
static void _ProcessInput(void) {
  char         acPrompt[128];
  const char * pPrompt;
  const char * s;
  int          r;
  int          IsConsoleInput;
  char       * sComment;

  IsConsoleInput = IS_CONSOLE_INPUT();
  for (;;) {
    //
    // Build the prompt string. We display a prompt only if
    // the input is coming from the console. If the input
    // is coming from a file we output the prompt together
    // with the command to the output file. This is used
    // for automated testing.
    //
    if (_acCurrentDir[0] == '\0') {
      (void)strncpy(acPrompt, "emFile>", sizeof(acPrompt) - 1u);
      acPrompt[sizeof(acPrompt) - 1u] = '\0';
    } else {
      SEGGER_snprintf(acPrompt, (int)sizeof(acPrompt), "emFile-%s>", _acCurrentDir);
    }
    pPrompt = NULL;
    if (IsConsoleInput != 0) {
      pPrompt = acPrompt;
    }
    //
    // Wait for a command.
    //
    r = GET_INPUT_LINE(pPrompt, _acCommandLine, sizeof(_acCommandLine));
    if (r != 0) {
      break;                  // End of input or error.
    }
    if (IsConsoleInput == 0) {
      //
      // Output the received command if the input is coming from a file.
      //
      _Log(acPrompt);
      _Log(_acCommandLine);
      _Log("\n");
    }
    //
    // Remove comments.
    //
    sComment = strchr(_acCommandLine, (int)'#');
    if (sComment != NULL) {
      *sComment = '\0';
    }
    //
    // Execute the command.
    //
    s = _acCommandLine;
    _EatWhite(&s);
    r = _ExecCommand(s);
    if (r == 0) {
      break;
    }
  }
}

/*********************************************************************
*
*       _Init
*/
static void _Init(const COMMAND_SET * pCommandSet) {
  _paCommandSet = pCommandSet;         // Configure the list of commands supported.
  SEGGER_snprintf(_ac, (int)sizeof(_ac), "SEGGER FS Commander V%d.%d.%d", FS_VERSION_MAJOR, FS_VERSION_MINOR, FS_VERSION_PATCH);
  _Log(_ac);
  _Log("\n");
  _Log("Type \"?\" or \"help\" for a list of commands\n");
  memset(_aBuffer, 0, sizeof(_aBuffer));
  INIT_SYSTEM(_ac);
  FS_Init();
  //
  // Setup the current directory/volume.
  //
  (void)FS_GetVolumeName(0, _ac, (int)sizeof(_ac));
  _UpdateCurrentDir(_ac);
}

#if USE_MAIN_TASK

/*********************************************************************
*
*       MainTask
*/
void MainTask(void);
void MainTask(void) {
  const COMMAND_SET aCommandSet[] = {
    {_aCommand, SEGGER_COUNTOF(_aCommand)},
    {NULL,      0}
  };

  _Init(aCommandSet);
  _ProcessInput();
#if FS_SUPPORT_DEINIT
  FS_DeInit();
#endif // FS_SUPPORT_DEINIT
#if USE_SIMULATOR
  exit(_NumCommandErrors);
#else
  for (;;) {
    ;
  }
#endif
}

#endif // USE_MAIN_TASK

/*************************** End of file ****************************/
