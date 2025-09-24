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

File    : FS_CRYPT_ALGO_Scramble.c
Purpose : Scrambling algorithm for the emFile encryption logical driver.
Additional information:
  <Any additional information for this module>
*/

#include "FS.h"
#include "FS_CRYPT_ALGO_Scramble.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32 _aXOR[2048 / 4] = {
  0x4D8D2BEEuL, 0x014B027BuL, 0xDFE8484AuL, 0x86941497uL, 0x85F9FE8EuL, 0xF1FCFA7DuL, 0x39D39B1DuL, 0x5C3BFDA7uL,
  0x99C0A911uL, 0x3F4C7BA4uL, 0xBF916883uL, 0xBB512B89uL, 0x9A4389D1uL, 0x91DEC99EuL, 0x88F4D3ABuL, 0xC88F3F3BuL,
  0x3F053718uL, 0x11BB4786uL, 0xB2124D76uL, 0x900AFD49uL, 0xE7CA8227uL, 0x279886B9uL, 0x2106B69EuL, 0xE1D3B32FuL,
  0x6A2D21D6uL, 0xC260E5A9uL, 0x2E3F941FuL, 0xD9CDE576uL, 0x6BB308BFuL, 0x26BDEB69uL, 0xF34139B0uL, 0x5F25CC52uL,
  0x899E1CA4uL, 0xCDD39AADuL, 0x2C4A43E1uL, 0x0F9AA7A1uL, 0xC7CD550DuL, 0x6FB4B723uL, 0xE118573CuL, 0x187EAB42uL,
  0x404A6312uL, 0x556B9339uL, 0x4F19B3F8uL, 0x042B5A3EuL, 0x34AC8544uL, 0x78A4B23BuL, 0x7F839E9AuL, 0x6F779503uL,
  0x1BCDA849uL, 0x6213EFECuL, 0xFE26324FuL, 0xDD1DAD86uL, 0x9974C44FuL, 0x7DD05F43uL, 0x8FCE23E6uL, 0xBE73CA81uL,
  0x0A6552DAuL, 0x5F6C7F65uL, 0xFDA204B3uL, 0x2892A2B2uL, 0x156B8C3AuL, 0x3D64C17AuL, 0x866AFC2CuL, 0xF93C3CEDuL,
  0x9191A583uL, 0x752FBA21uL, 0xCF416180uL, 0xCC98A107uL, 0x61119F17uL, 0xF8396D58uL, 0x60FAA72FuL, 0x16A30645uL,
  0x75E6A2F3uL, 0x46D4016AuL, 0x811364B4uL, 0xEA349514uL, 0xFDDF1CC9uL, 0x6D9E498BuL, 0x1906D42EuL, 0xFF054769uL,
  0x0CEE1604uL, 0xE720AFEEuL, 0x72CF2DC4uL, 0x3CDA7595uL, 0x50050256uL, 0x7672054AuL, 0x749420DEuL, 0xF25AC6A7uL,
  0x1B756B39uL, 0x3A59A9D5uL, 0x04B01FFEuL, 0x2C24603EuL, 0x93B3256CuL, 0xB61CAFBCuL, 0x05DA9610uL, 0x2227ECD7uL,
  0x14DAEC9CuL, 0xBB3B545BuL, 0x47B1705FuL, 0xC7594878uL, 0x823B4A98uL, 0x7EA92559uL, 0x700D5665uL, 0x32FEA370uL,
  0x1A113565uL, 0x0283952CuL, 0x2CDBA934uL, 0x3C5E038AuL, 0xF0B0D37AuL, 0xCAF6707EuL, 0x2638C3DCuL, 0x4F8F69EDuL,
  0xD721348FuL, 0x0231FF57uL, 0x9ED7FCD2uL, 0xCF2CC1C2uL, 0xEA891394uL, 0x67EAC571uL, 0x23130A46uL, 0xA54A738FuL,
  0x6C4618A5uL, 0x547315D6uL, 0x31A6CB9EuL, 0x7111F23AuL, 0x8FA91A5BuL, 0x4BF3BFABuL, 0xD136081EuL, 0x62E7AF06uL,
  0x2C154B56uL, 0x1E35741DuL, 0x3135D91EuL, 0x8F9515B0uL, 0x05A6805EuL, 0x93AC3C31uL, 0xFF3A6B7DuL, 0xB7A5F888uL,
  0x64F1E738uL, 0xB9F864F8uL, 0xACBCA9B5uL, 0xF20190FEuL, 0x7ECA2FA4uL, 0xF362F54FuL, 0xCB39E639uL, 0x59E9054FuL,
  0x91B8BA78uL, 0x885710A6uL, 0xAE418764uL, 0x75E21329uL, 0xAC5ED51EuL, 0x6B540107uL, 0x68C21F46uL, 0x41EE7A51uL,
  0xA0E1FEB0uL, 0x02D1D3A9uL, 0x4DD6FB65uL, 0x79B6E7D2uL, 0x4D8AB8F8uL, 0xAE71A349uL, 0xE2D9D20EuL, 0x99133D6AuL,
  0x60CCEB69uL, 0x84803C1AuL, 0xFDF0B3B5uL, 0x18A7199CuL, 0x5EF8F0BBuL, 0x7C1BE4A7uL, 0x28DACA90uL, 0xE70F6305uL,
  0xB674C8DEuL, 0x687C63E7uL, 0xB097B6A6uL, 0x5682DE19uL, 0x33CB2179uL, 0x6CAC2974uL, 0x03A0152AuL, 0x93E5839DuL,
  0x71E7EF91uL, 0x69559486uL, 0x456A2D5DuL, 0xAEFB6B39uL, 0x2FF264DCuL, 0x45833D44uL, 0xE914F9ECuL, 0xFC4B7BCFuL,
  0xCEFCA0EDuL, 0xD8BFE1C8uL, 0x904C5407uL, 0x32E9C102uL, 0x092AF503uL, 0x826E7DCDuL, 0xDBEEB5A8uL, 0x0268D8B9uL,
  0x5A32990DuL, 0xF8EDC1C2uL, 0x8BC1F321uL, 0xDB125E84uL, 0x1F292C1FuL, 0xA00CB53DuL, 0xE21191E1uL, 0x6E26832EuL,
  0x5DBD4A0AuL, 0xBE07294EuL, 0x99093E03uL, 0xEC2707B7uL, 0xB3B52B5DuL, 0x22E7D44EuL, 0xB39BF359uL, 0xF91A3027uL,
  0x2458E445uL, 0xF78350B3uL, 0x7F197B93uL, 0xFBBFBB71uL, 0xAB5AF7F1uL, 0x14158191uL, 0xFE72E7EAuL, 0x6F5064C9uL,
  0x09C6BA9AuL, 0xA7C0FB9EuL, 0x25E4F7ECuL, 0x44156625uL, 0x9C90160EuL, 0x34737A72uL, 0xD820EF3EuL, 0x5A435569uL,
  0x571413A0uL, 0xC72D9249uL, 0x2379A530uL, 0x82B33B7DuL, 0xC1501C11uL, 0x554DF234uL, 0xE2B945E0uL, 0x22540B09uL,
  0x174F2A9BuL, 0x616AB288uL, 0x5FADD523uL, 0xE806603BuL, 0xF3FD1AC5uL, 0x964E58D4uL, 0xF40DAF8AuL, 0xA2D1CA78uL,
  0x19F297CDuL, 0x2F7294EBuL, 0xF2999470uL, 0x2D19F58CuL, 0xBE657947uL, 0x4C0C6107uL, 0x0A06D0B4uL, 0xBCBFDB9DuL,
  0x91112EB7uL, 0xFAE3D892uL, 0xC7F76BA7uL, 0xCC255ADBuL, 0x323F7FF2uL, 0x625F325AuL, 0x9143D825uL, 0x29163B8EuL,
  0xD99AE413uL, 0xDCCD67A2uL, 0x13A6875BuL, 0x2EEBBF9DuL, 0x2D353F39uL, 0x664A658EuL, 0x6C830134uL, 0x4C315AC4uL,
  0x0D98BF2CuL, 0x8767E7B9uL, 0x0BDDC9BFuL, 0x6F950681uL, 0x4E156A6DuL, 0xE93136E2uL, 0xDD3828C0uL, 0x36DE829CuL,
  0x4FFB88E6uL, 0x828D8716uL, 0x54696CD9uL, 0xE0A1CB05uL, 0xBC68B98BuL, 0xDD8FBDE6uL, 0x8AD82BA5uL, 0x09C701D1uL,
  0x0CA33786uL, 0x75E5F833uL, 0xBAF81EEBuL, 0x80D05613uL, 0x23487C3BuL, 0x2739AC9FuL, 0xA9744B2EuL, 0xAE417548uL,
  0x6B48B0A2uL, 0x27EF7428uL, 0xD05F0DB6uL, 0x8909217FuL, 0xC636F41FuL, 0xEB75C334uL, 0x6C101228uL, 0x4B4E47FCuL,
  0xEEAF0FD5uL, 0xDE6B33E4uL, 0xDE814F2AuL, 0xED44942AuL, 0x5ECC4057uL, 0x2E5F8AE9uL, 0x29A0793BuL, 0x7AF3AC26uL,
  0xCD19726AuL, 0xB0B2D36DuL, 0x926DC648uL, 0x5CB9344FuL, 0xD9AE9D05uL, 0x09308A65uL, 0x1B04399CuL, 0xFE8CDE70uL,
  0x8305CD39uL, 0x2A9935E6uL, 0xBB27AF24uL, 0x60060476uL, 0xE97C5C7EuL, 0x47C70E87uL, 0x0BBFADE2uL, 0x55BEDD9EuL,
  0xDE7CB10AuL, 0x14EC27C3uL, 0x5533BD9DuL, 0xD957DB0DuL, 0xA63243D7uL, 0x6B40025AuL, 0x7737B2D6uL, 0x8DD8E2E9uL,
  0x791F464CuL, 0xD54520C2uL, 0x6ADE1509uL, 0xF4B838E1uL, 0x6612F8ACuL, 0xBFAC30E3uL, 0xF330D7B3uL, 0xBF586512uL,
  0xB14ECE9BuL, 0x82FA38FEuL, 0x3E7C700AuL, 0xE9BA37B3uL, 0xB238ADC5uL, 0xDF3805FBuL, 0x56AB1287uL, 0x77E0045FuL,
  0x03DB756AuL, 0x91FC5FF9uL, 0x8B86FFB6uL, 0xC4C15C71uL, 0xC329935DuL, 0xBB3F1A03uL, 0xB6A1EBA5uL, 0x4D2A5A3EuL,
  0x3143F16BuL, 0x5A9F5626uL, 0xC4D3391EuL, 0xA451AFB6uL, 0x0ADB0E0DuL, 0x7B788A24uL, 0x69EDCE4DuL, 0xE9643F43uL,
  0x10AE75E9uL, 0x3DF17AFBuL, 0x7CD70F5FuL, 0x038C5C86uL, 0xAAB48E50uL, 0xCE45E36DuL, 0x07B711B1uL, 0x2F9ED0E2uL,
  0xA6B24C44uL, 0x34887422uL, 0x342D1E9BuL, 0x427E6826uL, 0x5BF6DC59uL, 0x466F903DuL, 0x35CFE73CuL, 0x66336314uL,
  0x68ED5374uL, 0x457E223FuL, 0xC0FA9A6CuL, 0x599732BCuL, 0x374D4F06uL, 0xC10356BDuL, 0x9650AFB5uL, 0x05899EB2uL,
  0xD1A52B90uL, 0x29024828uL, 0xFC3DBF34uL, 0xCDAD7C03uL, 0xCE9CAEE0uL, 0x75E5209AuL, 0xFA2DFA8BuL, 0x6054F40CuL,
  0x343A6711uL, 0xA447CC30uL, 0xD671AFFAuL, 0x2BCA7E90uL, 0x9C0B74B7uL, 0x347D58ADuL, 0xB514CDB1uL, 0x02DCD804uL,
  0xBC19CAA8uL, 0x14D3EFF1uL, 0xB9200C76uL, 0x062A9CF9uL, 0x8ACC3A60uL, 0x643B88E2uL, 0x303D7381uL, 0x5A5C94DBuL,
  0x5046BAF7uL, 0xCCD50799uL, 0xDDBD21A7uL, 0xE270954FuL, 0xE41DFAB8uL, 0xDB667598uL, 0x3AFA5CF1uL, 0x60718348uL,
  0xB409CA6FuL, 0xA334E679uL, 0xCB5504AAuL, 0x9F4D0A7CuL, 0x2B78C30EuL, 0xAA96BD93uL, 0xA0BBE819uL, 0x1229C864uL,
  0x896B8460uL, 0xB9A3763FuL, 0xD419934AuL, 0x4D93C08DuL, 0xE78F0AC8uL, 0x618FD9AAuL, 0x215D3504uL, 0x566171B4uL,
  0x52E66458uL, 0x9EB482C6uL, 0x488526B7uL, 0xE9DDCEF9uL, 0xDCF914DFuL, 0xD57E29C2uL, 0x46F97AC8uL, 0x6ACE6062uL,
  0xF9F597CEuL, 0x58E1A5FDuL, 0xA535184DuL, 0x113242DEuL, 0x6AB7DC09uL, 0xDA38967EuL, 0x48387689uL, 0x95857EA6uL,
  0x5FB5E5F5uL, 0x549759CDuL, 0xAC5A375FuL, 0x0E148C1AuL, 0xBC01F63BuL, 0x751EE01AuL, 0x3090AFE9uL, 0xC1697D7EuL,
  0x627AF1A6uL, 0xA08B077FuL, 0x82A21CD8uL, 0x26ACC3A3uL, 0x15637A7EuL, 0x10AD5484uL, 0x377DCBD6uL, 0xACADCECAuL,
  0x8C4C3CB6uL, 0x40F3BFA7uL, 0xB443B739uL, 0xCDB2D2FBuL, 0x0A2BD067uL, 0x2C0C0617uL, 0x7C48304CuL, 0x734EC5F0uL,
  0x87D002ADuL, 0x62D69032uL, 0x8E1F3133uL, 0x86FFB5D6uL, 0xFACBA278uL, 0x2E48616CuL, 0xBC819AD0uL, 0x1D2ED2D3uL,
  0xEAA02C66uL, 0x5DC7D723uL, 0x316DD680uL, 0x8BA07B29uL, 0x8DCC04A1uL, 0x5EE7248CuL, 0xCED4E867uL, 0x58781802uL,
  0x63F8848FuL, 0x11E377DDuL, 0x515B1AC7uL, 0xB9394EA4uL, 0x878B1897uL, 0xACA29FB8uL, 0xF2EF560AuL, 0x51B0A5ADuL,
  0x2FF32BB7uL, 0xAD400E5AuL, 0x68A40274uL, 0xA05C88EAuL, 0xA1A864C3uL, 0xC5D47402uL, 0x4E09B938uL, 0x71969140uL,
  0xF50F7183uL, 0x33BC15F0uL, 0x61EE7935uL, 0xE907B75CuL, 0xE4271FBDuL, 0x2C885FE8uL, 0x4944D3A0uL, 0xC1CD7701uL
};

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _xor
*/
static void _xor(U32 * pDest, const U32 * pSrc, const U32 * pXOR, U32 NumItems) {
  //
  // Manual loop unrolling.
  //
  while (NumItems >= 16) {
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    *pDest++ = *pSrc++ ^ *pXOR++;
    NumItems -= 16;
  }
  //
  // Last block, if any, handled by XOR-ing single items.
  //
  while (NumItems > 0) {
    *pDest++ = *pSrc++ ^ *pXOR++;
    --NumItems;
  }
}

/*********************************************************************
*
*       _Scramble
*
*  Function description
*    XORs sector data with a random data pattern.
*
*  Parameters
*    pContext       [IN]  pContext parameter passed to FS_CRYPT_Configure().
*    pDest          [OUT] XORed sector data.
*    pSrc           [IN]  Sector data to be XORed.
*    NumBytes       Number of bytes to process (logical sector size)
*    BlockIndex     Logical sector index.
*/
static void _Scramble(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex) {
  U32         NumItemsAtOnce;
  U32         NumItems;
  U32       * pDest32;
  const U32 * pSrc32;

  FS_USE_PARA(pContext);
  FS_USE_PARA(BlockIndex);
  //
  // We know that the sector data is aligned at a 4-byte boundary
  // and that the sector size is multiple of 4 bytes.
  //
  pDest32  = (U32 *)(void *)pDest;
  pSrc32   = (U32 *)(void *)pSrc;
  NumItems = NumBytes >> 2;
  if (NumItems != 0u) {
    do {
      NumItemsAtOnce = SEGGER_MIN(NumItems, SEGGER_COUNTOF(_aXOR));
      _xor(pDest32, pSrc32, _aXOR, NumItemsAtOnce);
      pDest32  += NumItemsAtOnce;
      pSrc32   += NumItemsAtOnce;
      NumItems -= NumItemsAtOnce;
    } while (NumItems != 0u);
  }
}

/*********************************************************************
*
*       Public API structures
*
**********************************************************************
*/

/*********************************************************************
*
*       FS_CRYPT_ALGO_Scramble
*/
const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_Scramble = {
  NULL,
  _Scramble,
  _Scramble,
  0
};

/*************************** End of file ****************************/
