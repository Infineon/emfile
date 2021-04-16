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

File    : FS_CRYPT_ALGO_XTS_AES.c
Purpose : Encryption algorithm for emFile that uses emCrypt XTS-AES.
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "CRYPTO.h"
#include "FS_CRYPT_ALGO_XTS_AES.h"

/*********************************************************************
*
*       Static functions (public via callback)
*
**********************************************************************
*/

/*********************************************************************
*
*       _XTS_AES128_Prepare
*
*  Function description
*    Saves the key to the encryption context.
*
*  Parameters
*    pContext     Encryption context
*    pKey         Encryption key (for data and IV)
*
*  Additional information
*    pKey contains the key for encrypting the data and the key for
*    the encryption of the  initialization vector:
*    pKey[0..15] - data key, pKey[16..31] - IV key.
*    Therefore, pKey as to be at least 32 bytes large.
*/
static void _XTS_AES128_Prepare(void * pContext, const U8 * pKey) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES128 * pCryptContext;
  unsigned                           KeyLen;
  U8                               * pKeyData;
  U8                               * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES128 *)pContext;
  pKeyData      = pCryptContext->abKeyData;
  pKeyIV        = pCryptContext->abKeyIV;
  KeyLen        = 16;
  FS_MEMCPY(pKeyData, pKey,          KeyLen);
  FS_MEMCPY(pKeyIV,   pKey + KeyLen, KeyLen);
}

/*********************************************************************
*
*       _XTS_AES128_Encrypt
*
*  Function description
*    Encrypts the data using XTS-AES and an 128-bit key.
*/
static void _XTS_AES128_Encrypt(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES128 * pCryptContext;
  unsigned                           KeyLen;
  U8                               * pKeyData;
  U8                               * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES128 *)pContext;
  KeyLen        = sizeof(pCryptContext->abKeyData);
  pKeyData      = pCryptContext->abKeyData;
  pKeyIV        = pCryptContext->abKeyIV;
  CRYPTO_XTS_AES_Encrypt(pDest, (U64)BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen);
}

/*********************************************************************
*
*       _XTS_AES128_Decrypt
*
*  Function description
*    Decrypts the data using XTS-AES and an 128-bit key.
*/
static void _XTS_AES128_Decrypt(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES128 * pCryptContext;
  unsigned                           KeyLen;
  U8                               * pKeyData;
  U8                               * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES128 *)pContext;
  KeyLen        = sizeof(pCryptContext->abKeyData);
  pKeyData      = pCryptContext->abKeyData;
  pKeyIV        = pCryptContext->abKeyIV;
  CRYPTO_XTS_AES_Decrypt(pDest, (U64)BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen);
}

/*********************************************************************
*
*       _XTS_AES256_Prepare
*
*  Function description
*    Saves the key to the encryption context.
*
*  Parameters
*    pContext     Encryption context
*    pKey         Encryption key (for data and IV)
*
*  Additional information
*    pKey contains the key for encrypting the data and the key for
*    the encryption of the  initialization vector:
*    pKey[0..31] - data key, pKey[32..63] - IV key.
*    Therefore, pKey as to be at least 63 bytes large.
*/
static void _XTS_AES256_Prepare(void * pContext, const U8 * pKey) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES256 * pCryptContext;
  unsigned                           KeyLen;
  U8                               * pKeyData;
  U8                               * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES256 *)pContext;
  pKeyData      = pCryptContext->abKeyData;
  pKeyIV        = pCryptContext->abKeyIV;
  KeyLen        = 32;
  FS_MEMCPY(pKeyData, pKey,          KeyLen);
  FS_MEMCPY(pKeyIV,   pKey + KeyLen, KeyLen);
}

/*********************************************************************
*
*       _XTS_AES256_Encrypt
*
*  Function description
*    Encrypts the data using XTS-AES and an 256-bit key.
*/
static void _XTS_AES256_Encrypt(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES256 * pCryptContext;
  unsigned                           KeyLen;
  U8                               * pKeyData;
  U8                               * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES256 *)pContext;
  KeyLen        = sizeof(pCryptContext->abKeyData);
  pKeyData      = pCryptContext->abKeyData;
  pKeyIV        = pCryptContext->abKeyIV;
  CRYPTO_XTS_AES_Encrypt(pDest, (U64)BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen);
}

/*********************************************************************
*
*       _XTS_AES256_Decrypt
*
*  Function description
*    Decrypts the data using XTS-AES and an 256-bit key.
*/
static void _XTS_AES256_Decrypt(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES256 * pCryptContext;
  unsigned                           KeyLen;
  U8                               * pKeyData;
  U8                               * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES256 *)pContext;
  KeyLen        = sizeof(pCryptContext->abKeyData);
  pKeyData      = pCryptContext->abKeyData;
  pKeyIV        = pCryptContext->abKeyIV;
  CRYPTO_XTS_AES_Decrypt(pDest, (U64)BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen);
}

/*********************************************************************
*
*       Public API structures
*
**********************************************************************
*/

/*********************************************************************
*
*       FS_CRYPT_ALGO_XTS_AES128
*
*  Description
*    Encryption algorithm using XTS-AES with a 128-bit key.
*/
const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_XTS_AES128 = {
  _XTS_AES128_Prepare,
  _XTS_AES128_Encrypt,
  _XTS_AES128_Decrypt,
  128
};

/*********************************************************************
*
*       FS_CRYPT_ALGO_XTS_AES256
*
*  Description
*    Encryption algorithm using XTS-AES with a 256-bit key.
*/
const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_XTS_AES256 = {
  _XTS_AES256_Prepare,
  _XTS_AES256_Encrypt,
  _XTS_AES256_Decrypt,
  128
};

/*************************** End of file ****************************/
