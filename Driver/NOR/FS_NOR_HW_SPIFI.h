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
File        : FS_NOR_HW_SPIFI.h
Purpose     : low-level flash driver for quad SPI.
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef FS_NOR_HW_SPIFI_H     // Avoid recursive and multiple inclusion
#define FS_NOR_HW_SPIFI_H

#include "FS.h"
#include "mtb_hal_memoryspi.h"

/*********************************************************************
*
*       Defines, non-configurable
*
**********************************************************************
*/
/* Maximum number of memories supported */
#define FS_NOR_HW_SPIFI_MAX_MEM_SUPPORTED       0x04

/*********************************************************************
*
*       Public types
*
**********************************************************************
*/
#if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH)
typedef void (* FS_NOR_HW_SPIFI_XIP_SUPPORT)(mtb_hal_memoryspi_t * halObj);
#endif /* #if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH) */

/* The QSPI HAL supports up to four memories using four slave select pins but
 * all the memories need to be connected to the same data lines. i.e. The QSPI
 * HAL currently does not support different data lines for different slaves.
 *
 * Number of I/Os depend on the type of memory interface. Pass NC when an
 * I/O is unused. See the device datasheet for the actual pins connected to
 * these I/O lines.
 *
 * Single-SPI/Dual-SPI : (io0, io1) or (io2, io3) or (io4, io5) or (io6, io7)
 * Quad SPI            : (io0, io1, io2, io3) or (io4, io5, io6, io7)
 *
 * Note: Though the QSPI HAL supports octal SPI connection, emFile does not
 * support it.
 */

typedef struct
{
    mtb_hal_memoryspi_data_select_t PinSet [FS_NOR_HW_SPIFI_MAX_MEM_SUPPORTED]; /* Data/IO pins connected to the memory */
    uint8_t NumMem;                                                           /* Number of memory devices used. */

    mtb_hal_memoryspi_datarate_t    DataRate;                                           /* Field added for future updates */ 
    mtb_hal_memoryspi_chip_select_t ChipSelect [FS_NOR_HW_SPIFI_MAX_MEM_SUPPORTED];

    mtb_hal_memoryspi_t *Obj;                                                  /* This HW layer passes this object to the HAL APIs.
                                                                               * The user must not access the object contents.
                                                                               * Here so the user can call the HAL APIs directly.
                                                                               */
#if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH)
    FS_NOR_HW_SPIFI_XIP_SUPPORT WipCallback;                                  /* Pointer to function which blocks code execution until the all
                                                                               * the memory operation completes.
                                                                               */
    uint8_t *pDataBuffer;                                                     /* The pointer to the buffer to store data from one logic sector. */
    uint8_t *pParamBuffer;                                                    /* The pointer to the buffer to store parameters - always 8-bytes size */
#endif /* #if defined(ENABLE_XIP_EMFILE_ON_SAME_NOR_FLASH) */
} FS_NOR_HW_SPIFI_Config_t;

typedef enum
{
    FS_NOR_HW_SPIFI_RESULT_OK = 0U,
    FS_NOR_HW_SPIFI_RESULT_BADPARAM,
} FS_NOR_HW_SPIFI_Result_t;

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
extern const FS_NOR_HW_TYPE_SPIFI FS_NOR_HW_SPIFI;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
FS_NOR_HW_SPIFI_Result_t FS_NOR_HW_SPIFI_Configure(FS_NOR_HW_SPIFI_Config_t *UserConfig);

#endif  // FS_NOR_HW_SPIFI_H

/*************************** End of file ****************************/
