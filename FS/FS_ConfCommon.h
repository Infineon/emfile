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

File    : FS_ConfCommon.h
Purpose : Provides emFile configuration that is common for all the
          pre-built libraries. This file is included into every
          FS_Conf.h file that was used for building a specific
          pre-built library. You should not change any of the macros.
*/

#ifndef FS_CONFCOMMON_H
#define FS_CONFCOMMON_H

/* See emFile user guide for details on these macros. */

#define FS_DEBUG_LEVEL                      0       // 0: Smallest code, 5: Full debug. See chapter 10 "Debugging" of the emFile manual.

#define FS_OS_LOCKING                       2       // 0: No locking, 1: API locking, 2: Driver locking. See chapter 9 "OS integration" of the emFile manual.

#define FS_SUPPORT_FILE_BUFFER              1
#define FS_SUPPORT_DEINIT                   1

#define FS_MMC_READ_DATA_TIMEOUT            500     // in milliseconds

#define FS_NOR_SUPPORT_CRC                  1
#define FS_NOR_SUPPORT_ECC                  1

#define FS_SUPPORT_JOURNAL                  1
#define FS_SUPPORT_ENCRYPTION               1

#endif /* FS_CONFCOMMON_H */
