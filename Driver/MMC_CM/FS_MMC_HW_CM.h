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
----------------------------------------------------------------------
File        : FS_MMC_HW_CM.h
Purpose     : Configuration file for FS with MMC / SD card
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef FS_MMC_HW_CM_H     // Avoid recursive and multiple inclusion
#define FS_MMC_HW_CM_H

/*********************************************************************
*
*       Includes
*
**********************************************************************
*/
#include "FS.h"
#include "mtb_hal_sdhc.h"

/*********************************************************************
*
*       Public types
*
**********************************************************************
*/
typedef struct
{
    mtb_hal_sdhc_t * Obj;      /* This HW layer passes this object to the HAL APIs.
                               * User should not access the contents of it.
                               * Provided here so that user can call the HAL APIs directly
                               * if required.
                               */
    bool IoVoltSelEn;
    bool CardPwrEn;
} FS_MMC_HW_CM_SDHostConfig_t;

typedef enum
{
    FS_MMC_HW_CM_RESULT_OK = 0U,
    FS_MMC_HW_CM_RESULT_BADPARAM,
} FS_MMC_HW_CM_Result_t;

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
extern const FS_MMC_HW_TYPE_CM FS_MMC_HW_CM;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
FS_MMC_HW_CM_Result_t FS_MMC_HW_CM_Configure(U8 Unit, FS_MMC_HW_CM_SDHostConfig_t *UserConfig);

#endif  // FS_MMC_HW_CM_H

/*************************** End of file ****************************/
