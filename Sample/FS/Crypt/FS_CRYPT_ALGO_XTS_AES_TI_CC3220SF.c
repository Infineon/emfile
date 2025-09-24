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

File    : FS_CRYPT_ALGO_XTS_AES_TI_CC3220SF.c
Purpose : Encryption algorithm for emFile that uses the AES HW module of the TI CC3220SF CPU.
Literature:
  [1] CC3220 SimpleLink Wi-Fi and Internet of Things Technical Reference Manual
    (\\FILESERVER\Techinfo\Company\TI\MCU\CC3220\CC3220_TechnicalReferenceManual_swru465.pdf)
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "FS_CRYPT_ALGO_XTS_AES_TI_CC3220SF.h"

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

/*********************************************************************
*
*       AES HW module
*/
#define AES_BASE_ADDR                 0x44037000uL
#define AES_KEY2_6                    (*(volatile U32 *)(AES_BASE_ADDR + 0x000))    // Key register
#define AES_KEY2_7                    (*(volatile U32 *)(AES_BASE_ADDR + 0x004))    // Key register
#define AES_KEY2_4                    (*(volatile U32 *)(AES_BASE_ADDR + 0x008))    // Key register
#define AES_KEY2_5                    (*(volatile U32 *)(AES_BASE_ADDR + 0x00C))    // Key register
#define AES_KEY2_2                    (*(volatile U32 *)(AES_BASE_ADDR + 0x010))    // Key register
#define AES_KEY2_3                    (*(volatile U32 *)(AES_BASE_ADDR + 0x014))    // Key register
#define AES_KEY2_0                    (*(volatile U32 *)(AES_BASE_ADDR + 0x018))    // Key register
#define AES_KEY2_1                    (*(volatile U32 *)(AES_BASE_ADDR + 0x01C))    // Key register
#define AES_KEY1_6                    (*(volatile U32 *)(AES_BASE_ADDR + 0x020))    // Key register
#define AES_KEY1_7                    (*(volatile U32 *)(AES_BASE_ADDR + 0x024))    // Key register
#define AES_KEY1_4                    (*(volatile U32 *)(AES_BASE_ADDR + 0x028))    // Key register
#define AES_KEY1_5                    (*(volatile U32 *)(AES_BASE_ADDR + 0x02C))    // Key register
#define AES_KEY1_2                    (*(volatile U32 *)(AES_BASE_ADDR + 0x030))    // Key register
#define AES_KEY1_3                    (*(volatile U32 *)(AES_BASE_ADDR + 0x034))    // Key register
#define AES_KEY1_0                    (*(volatile U32 *)(AES_BASE_ADDR + 0x038))    // Key register
#define AES_KEY1_1                    (*(volatile U32 *)(AES_BASE_ADDR + 0x03C))    // Key register
#define AES_IV_IN_0                   (*(volatile U32 *)(AES_BASE_ADDR + 0x040))
#define AES_IV_IN_1                   (*(volatile U32 *)(AES_BASE_ADDR + 0x044))
#define AES_IV_IN_2                   (*(volatile U32 *)(AES_BASE_ADDR + 0x048))
#define AES_IV_IN_3                   (*(volatile U32 *)(AES_BASE_ADDR + 0x04C))
#define AES_CTRL                      (*(volatile U32 *)(AES_BASE_ADDR + 0x050))
#define AES_C_LENGTH_0                (*(volatile U32 *)(AES_BASE_ADDR + 0x054))
#define AES_C_LENGTH_1                (*(volatile U32 *)(AES_BASE_ADDR + 0x058))
#define AES_AUTH_LENGTH               (*(volatile U32 *)(AES_BASE_ADDR + 0x05C))
#define AES_DATA_IN_0                 (*(volatile U32 *)(AES_BASE_ADDR + 0x060))    // Data register
#define AES_DATA_IN_1                 (*(volatile U32 *)(AES_BASE_ADDR + 0x064))    // Data register
#define AES_DATA_IN_2                 (*(volatile U32 *)(AES_BASE_ADDR + 0x068))    // Data register
#define AES_DATA_IN_3                 (*(volatile U32 *)(AES_BASE_ADDR + 0x06C))    // Data register
#define AES_TAG_OUT_0                 (*(volatile U32 *)(AES_BASE_ADDR + 0x070))
#define AES_TAG_OUT_1                 (*(volatile U32 *)(AES_BASE_ADDR + 0x074))
#define AES_TAG_OUT_2                 (*(volatile U32 *)(AES_BASE_ADDR + 0x078))
#define AES_TAG_OUT_3                 (*(volatile U32 *)(AES_BASE_ADDR + 0x07C))
#define AES_REVISION                  (*(volatile U32 *)(AES_BASE_ADDR + 0x080))
#define AES_SYSCONFIG                 (*(volatile U32 *)(AES_BASE_ADDR + 0x084))
#define AES_IRQSTATUS                 (*(volatile U32 *)(AES_BASE_ADDR + 0x08C))
#define AES_IRQENABLE                 (*(volatile U32 *)(AES_BASE_ADDR + 0x090))
#define DTHE_AES_IM                   (*(volatile U32 *)(AES_BASE_ADDR + 0x820))    // AES Interrupt Mask Set register
#define DTHE_AES_RIS                  (*(volatile U32 *)(AES_BASE_ADDR + 0x824))    // AES Interrupt Raw Interrupt Status register
#define DTHE_AES_MIS                  (*(volatile U32 *)(AES_BASE_ADDR + 0x828))    // AES Interrupt Masked interrupt Status register
#define DTHE_AES_IC                   (*(volatile U32 *)(AES_BASE_ADDR + 0x82C))    // AES Interrupt Clear Interrupt Status register

/*********************************************************************
*
*       MCU reset clock manager
*/
#define PRCM_BASE_ADDR                0x44025000uL
#define PRCM_CRYPTOCLKEN              (*(volatile U32 *)(PRCM_BASE_ADDR + 0x0B8))

/*********************************************************************
*
*       Control register defines
*/
#define CTRL_OUTPUT_READY_BIT         0
#define CTRL_INPUT_READY_BIT          1
#define CTRL_DIRECTION_BIT            2
#define CTRL_KEY_SIZE_128BIT          1uL
#define CTRL_KEY_SIZE_256BIT          3uL
#define CTRL_KEY_SIZE_BIT             3
#define CTRL_XTS_KEY2IJ0              3uL
#define CTRL_XTS_BIT                  11

/*********************************************************************
*
*       Misc. defines
*/
#define CRYPTOCLKEN_RUNCLKEN_BIT      0
#define LD_NUM_BITS_BLOCK             7           // AES works with 128 bit blocks.
#define NUM_BITS_BLOCK                (1u << LD_NUM_BITS_BLOCK)
#define LD_NUM_BYTES_BLOCK            (LD_NUM_BITS_BLOCK - 3)
#define NUM_BYTES_BLOCK               (1u << LD_NUM_BYTES_BLOCK)

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _LoadU32LE
*
*  Function description
*    Reads a 32 bit little endian from a char array.
*/
static U32 _LoadU32LE(const U8 *pBuffer) {
  U32 r;
  r = (U32)pBuffer[3] & 0x000000FFu;
  r <<= 8;
  r += (U32)pBuffer[2] & 0x000000FFu;
  r <<= 8;
  r += (U32)pBuffer[1] & 0x000000FFu;
  r <<= 8;
  r += (U32)pBuffer[0] & 0x000000FFu;
  return r;
}

/*********************************************************************
*
*       _StoreU32LE
*
*  Function description
*    Stores 32 bits little endian into memory.
*/
static void _StoreU32LE(U8 *pBuffer, U32 Data) {
  *pBuffer++ = (U8)Data;
  Data >>= 8;
  *pBuffer++ = (U8)Data;
  Data >>= 8;
  *pBuffer++ = (U8)Data;
  Data >>= 8;
  *pBuffer   = (U8)Data;
}

/*********************************************************************
*
*       _ProcessOneBlock
*
*  Function description
*    Encrypts or decrypts a single block of data (16 bytes).
*/
static void _ProcessOneBlock(U8 * pDest, const U8 * pSrc) {
  const U32 * pSrc32;
  U32       * pDest32;

  //
  // Wait for the AES HW module to become ready.
  //
  for (;;) {
    if ((AES_CTRL & (1uL << CTRL_INPUT_READY_BIT)) != 0u) {
      break;
    }
  }
  //
  // Store the data to be processed.
  //
  if ((SEGGER_PTR2ADDR(pSrc) & 3u) == 0u) {
    pSrc32 = SEGGER_PTR2PTR(const U32, pSrc);
    AES_DATA_IN_3 = *pSrc32++;
    AES_DATA_IN_2 = *pSrc32++;
    AES_DATA_IN_1 = *pSrc32++;
    AES_DATA_IN_0 = *pSrc32++;
  } else {
    AES_DATA_IN_3 = _LoadU32LE(pSrc);
    pSrc += 4;
    AES_DATA_IN_2 = _LoadU32LE(pSrc);
    pSrc += 4;
    AES_DATA_IN_1 = _LoadU32LE(pSrc);
    pSrc += 4;
    AES_DATA_IN_0 = _LoadU32LE(pSrc);
  }
  //
  // Wait for the AES HW module to process the data.
  //
  for (;;) {
    if ((AES_CTRL & (1uL << CTRL_OUTPUT_READY_BIT)) != 0u) {
      break;
    }
  }
  //
  // Load the processed data.
  //
  if ((SEGGER_PTR2ADDR(pDest) & 3u) == 0u) {
    pDest32 = SEGGER_PTR2PTR(U32, pDest);
    *pDest32++ = AES_DATA_IN_3;
    *pDest32++ = AES_DATA_IN_2;
    *pDest32++ = AES_DATA_IN_1;
    *pDest32++ = AES_DATA_IN_0;
  } else {
     _StoreU32LE(pDest, AES_DATA_IN_3);
     pDest += 4;
     _StoreU32LE(pDest, AES_DATA_IN_2);
     pDest += 4;
     _StoreU32LE(pDest, AES_DATA_IN_1);
     pDest += 4;
     _StoreU32LE(pDest, AES_DATA_IN_0);
  }
}

/*********************************************************************
*
*       _Process
*
*  Function description
*    Encrypts or decrypts a block of data.
*/
static void _Process(U8 * pDest, U32 BlockIndex, const U8 * pSrc, U32 NumBytes, const U32 * pKeyData, const U32 * pKeyIV, unsigned KeyLen, int DoEncrypt) {
  U32 KeySize;
  U32 v;
  U32 NumBlocks;

  //
  // If required, enable the clock of the AES HW module.
  //
  if ((PRCM_CRYPTOCLKEN & (1uL << CRYPTOCLKEN_RUNCLKEN_BIT)) == 0) {
    PRCM_CRYPTOCLKEN |= 1uL << CRYPTOCLKEN_RUNCLKEN_BIT;
  }
  //
  // Set the key size and store the keys.
  //
  KeySize = CTRL_KEY_SIZE_128BIT;
  if (KeyLen == 32) {
    KeySize = CTRL_KEY_SIZE_256BIT;
  }
  AES_CTRL = KeySize << CTRL_KEY_SIZE_BIT;
  AES_KEY1_0 = *pKeyData++;
  AES_KEY1_1 = *pKeyData++;
  AES_KEY1_2 = *pKeyData++;
  AES_KEY1_3 = *pKeyData++;
  if (KeyLen == 32) {
    AES_KEY1_4 = *pKeyData++;
    AES_KEY1_5 = *pKeyData++;
    AES_KEY1_6 = *pKeyData++;
    AES_KEY1_7 = *pKeyData++;
  }
  AES_KEY2_0 = *pKeyIV++;
  AES_KEY2_1 = *pKeyIV++;
  AES_KEY2_2 = *pKeyIV++;
  AES_KEY2_3 = *pKeyIV++;
  if (KeyLen == 32) {
    AES_KEY2_4 = *pKeyIV++;
    AES_KEY2_5 = *pKeyIV++;
    AES_KEY2_6 = *pKeyIV++;
    AES_KEY2_7 = *pKeyIV++;
  }
  //
  // Set the initialization vector.
  //
  AES_IV_IN_0 = BlockIndex;
  AES_IV_IN_1 = 0;
  AES_IV_IN_2 = 0;
  AES_IV_IN_3 = 0;
  //
  // Enable the the XTS mode.
  //
  v  = AES_CTRL;
  v |= CTRL_XTS_KEY2IJ0 << CTRL_XTS_BIT;
  if (DoEncrypt != 0) {
    v |= 1uL << CTRL_DIRECTION_BIT;
  }
  AES_CTRL = v;
  //
  // Set the number of bytes to process.
  //
  AES_C_LENGTH_0 = NumBytes;
  AES_C_LENGTH_1 = 0;
  //
  // Loop until all the blocks are processed. Each block is 16 bytes large.
  //
  NumBlocks = NumBytes >> LD_NUM_BYTES_BLOCK;
  if (NumBlocks != 0u) {
    do {
      _ProcessOneBlock(pDest, pSrc);
      pDest += NUM_BYTES_BLOCK;
      pSrc  += NUM_BYTES_BLOCK;
    } while (--NumBlocks != 0u);
  }
}

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
  FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF * pCryptContext;
  unsigned                                       KeyLen;
  U32                                          * pKeyData;
  U32                                          * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF *)pContext;
  pKeyData      = pCryptContext->aKeyData;
  pKeyIV        = pCryptContext->aKeyIV;
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
  FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF * pCryptContext;
  unsigned                                       KeyLen;
  U32                                          * pKeyData;
  U32                                          * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF *)pContext;
  KeyLen        = sizeof(pCryptContext->aKeyData);
  pKeyData      = pCryptContext->aKeyData;
  pKeyIV        = pCryptContext->aKeyIV;
  _Process(pDest, BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen, 1);       // 1 means perform encryption.
}

/*********************************************************************
*
*       _XTS_AES128_Decrypt
*
*  Function description
*    Decrypts the data using XTS-AES and an 128-bit key.
*/
static void _XTS_AES128_Decrypt(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF * pCryptContext;
  unsigned                                       KeyLen;
  U32                                          * pKeyData;
  U32                                          * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES128_TI_CC3220SF *)pContext;
  KeyLen        = sizeof(pCryptContext->aKeyData);
  pKeyData      = pCryptContext->aKeyData;
  pKeyIV        = pCryptContext->aKeyIV;
  _Process(pDest, BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen, 0);       // 0 means perform decryption.
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
*    Therefore, pKey as to be at least 64 bytes large.
*/
static void _XTS_AES256_Prepare(void * pContext, const U8 * pKey) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF * pCryptContext;
  unsigned                                       KeyLen;
  U32                                          * pKeyData;
  U32                                          * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF *)pContext;
  pKeyData      = pCryptContext->aKeyData;
  pKeyIV        = pCryptContext->aKeyIV;
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
  FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF * pCryptContext;
  unsigned                                       KeyLen;
  U32                                          * pKeyData;
  U32                                          * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF *)pContext;
  KeyLen        = sizeof(pCryptContext->aKeyData);
  pKeyData      = pCryptContext->aKeyData;
  pKeyIV        = pCryptContext->aKeyIV;
  _Process(pDest, BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen, 1);  // 1 means perform encryption.
}

/*********************************************************************
*
*       _XTS_AES256_Decrypt
*
*  Function description
*    Decrypts the data using XTS-AES and an 256-bit key.
*/
static void _XTS_AES256_Decrypt(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex) {
  FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF * pCryptContext;
  unsigned                                       KeyLen;
  U32                                          * pKeyData;
  U32                                          * pKeyIV;

  pCryptContext = (FS_CRYPT_ALGO_CONTEXT_XTS_AES256_TI_CC3220SF *)pContext;
  KeyLen        = sizeof(pCryptContext->aKeyData);
  pKeyData      = pCryptContext->aKeyData;
  pKeyIV        = pCryptContext->aKeyIV;
  _Process(pDest, BlockIndex, pSrc, NumBytes, pKeyData, pKeyIV, KeyLen, 0);  // 0 means perform decryption.
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
const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_XTS_AES128_TI_CC3220SF = {
  _XTS_AES128_Prepare,
  _XTS_AES128_Encrypt,
  _XTS_AES128_Decrypt,
  NUM_BITS_BLOCK
};

/*********************************************************************
*
*       FS_CRYPT_ALGO_XTS_AES256_TI_CC3220SF
*
*  Description
*    Encryption algorithm using XTS-AES with a 256-bit key.
*/
const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_XTS_AES256_TI_CC3220SF = {
  _XTS_AES256_Prepare,
  _XTS_AES256_Encrypt,
  _XTS_AES256_Decrypt,
  NUM_BITS_BLOCK
};

/*************************** End of file ****************************/
