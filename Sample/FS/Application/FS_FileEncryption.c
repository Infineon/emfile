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

File    : FS_FileEncryption.c
Purpose : Demonstrates how to use the file encryption.

Additional information:
  Preparations:
    This sample has to be built with FS_SUPPORT_ENCRYPTION set to 1.
    It works without modification with any storage device.

  Expected behavior:
    The application performs the following actions:
    - Formats the storage device if required
    - Opens a test file
    - Configures the encryption for the test file
    - Writes some data to the test file
    - Closes the test file

  Sample output:
    Start
    Write encrypted data to file Cipher.txt
    Finished

  The data written to file is encrypted using the DES encryption algorithm.
  The file can be decrypted on a PC using the FSFileEncrypter.exe command
  line utility using the following parameters:

  FSFileEncrypter.exe -d secret Cipher.txt Plain.txt
*/

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include <stdio.h>
#include "FS.h"
#include "SEGGER.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define VOLUME_NAME       ""
#define FILE_NAME         VOLUME_NAME"\\Cipher.txt"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
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
  FS_FILE               * pFile;
  const char            * sPlainText = "This message has been encrypted using emFile";
#if FS_SUPPORT_ENCRYPTION
  const U8                abCryptKey[8] = {(U8)'s', (U8)'e', (U8)'c', (U8)'r', (U8)'e', (U8)'t', 0, 0};
  FS_CRYPT_OBJ            CryptObj;
  static FS_DES_CONTEXT   CryptContext;
#endif // FS_SUPPORT_ENCRYPTION

  FS_X_Log("Start\n");
  //
  // Initialize file system
  //
  FS_Init();
  //
  // Format the storage device if required.
  //
  (void)FS_FormatLLIfRequired(VOLUME_NAME);
  if (FS_IsHLFormatted(VOLUME_NAME) == 0) {
    FS_X_Log("High-level format\n");
    (void)FS_Format(VOLUME_NAME, NULL);
  }
  //
  // Store the sample text encrypted to the test file.
  //
  pFile = FS_FOpen(FILE_NAME, "w");
  if (pFile != NULL) {
#if FS_SUPPORT_ENCRYPTION
    SEGGER_snprintf(_ac, (int)sizeof(_ac), "Write encrypted data to file %s\n", FILE_NAME);
    FS_X_Log(_ac);
    //
    // Configure the encryption operation. This operation has to be performed only once.
    //
    (void)FS_CRYPT_Prepare(&CryptObj, &FS_CRYPT_ALGO_DES, &CryptContext, 512, abCryptKey);
    //
    // Enable the encryption for the test file.
    //
    (void)FS_SetEncryptionObject(pFile, &CryptObj);
#else
    FS_X_Log("ERROR: The support for encryption is not enabled.\n");
#endif // FS_SUPPORT_ENCRYPTION
    //
    // Write the data encrypted to the test file.
    //
    (void)FS_Write(pFile, sPlainText, strlen(sPlainText));
    (void)FS_FClose(pFile);
  } else {
    SEGGER_snprintf(_ac, (int)sizeof(_ac), "ERROR: Could not open file %s for writing.\n", FILE_NAME);
    FS_X_Log(_ac);
  }
  FS_Unmount(VOLUME_NAME);
  FS_X_Log("Finished\n");
  for (;;) {
    ;
  }
}

/*************************** End of file ****************************/
