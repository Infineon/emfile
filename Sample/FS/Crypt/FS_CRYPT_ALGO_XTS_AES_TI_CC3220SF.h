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

File    : FS_CRYPT_ALGO_XTS_AES_TI_CC3220SF.h
Purpose : Encryption algorithm for emFile that uses the AES HW module of the TI CC3220SF CPU.
*/

#ifndef FS_CRYPT_ALGO_XTS_AES_TI_CC3220SF_H     // Avoid multiple inclusion.
#define FS_CRYPT_ALGO_XTS_AES_TI_CC3220SF_H

#if defined(__cplusplus)
  extern "C" {                      // Make sure we have C-declarations in C++ programs.
#endif

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "FS.h"

/*********************************************************************
*
*       Types, global
*
**********************************************************************
*/

/*********************************************************************
*
*       FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF
*
*  Description
*    Context for the FS_CRYPT_ALGO_XTS_AES128_TI_CC3220SF encryption algorithm.
*/
typedef struct {
  U32 aKeyData[4];        // Key to encrypt the data.
  U32 aKeyIV[4];          // Key to encrypt the initialization vector (IV).
} FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF;

/*********************************************************************
*
*       FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF
*
*  Description
*    Context for the FS_CRYPT_ALGO_XTS_AES128_TI_CC3220SF encryption algorithm.
*/
typedef struct {
  U32 aKeyData[8];        // Key to encrypt the data.
  U32 aKeyIV[8];          // Key to encrypt the initialization vector (IV).
} FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF;

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
extern const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_XTS_AES128_TI_CC3220SF;   // XTS-AES encryption algorithm (128-bit key) using the AES HW module of the TI CC3220SF CPU
extern const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_XTS_AES256_TI_CC3220SF;   // XTS-AES encryption algorithm (256-bit key) using the AES HW module of the TI CC3220SF CPU

#if defined(__cplusplus)
}                                   // Make sure we have C-declarations in C++ programs.
#endif

#endif                              // Avoid multiple inclusion.

/*************************** End of file ****************************/
