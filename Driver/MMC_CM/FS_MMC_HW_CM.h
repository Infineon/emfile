/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
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

File        : FS_MMC_HW_CM.h
Purpose     : Configuration file for FS with MMC / SD card
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
#include "cyhal_sdhc.h"

/*********************************************************************
*
*       Public types
*
**********************************************************************
*/
typedef struct
{
    cyhal_sdhc_config_t Config;
    cyhal_gpio_t Cmd;
    cyhal_gpio_t Clk;
    cyhal_gpio_t Data0;
    cyhal_gpio_t Data1;
    cyhal_gpio_t Data2;
    cyhal_gpio_t Data3;
    cyhal_gpio_t Data4;
    cyhal_gpio_t Data5;
    cyhal_gpio_t Data6;
    cyhal_gpio_t Data7;
    cyhal_gpio_t CardDetect;
    cyhal_gpio_t IoVoltSel;
    cyhal_gpio_t CardPwrEn;
    cyhal_gpio_t CardWriteProt;
    cyhal_gpio_t LedControl;
    cyhal_gpio_t EmmcReset;
    cyhal_clock_t *BlockClk;
    cyhal_sdhc_t Obj;         /* This HW layer passes this object to the HAL APIs.
                               * User should not access the contents of it.
                               * Provided here so that user can call the HAL APIs directly
                               * if required.
                               */
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
