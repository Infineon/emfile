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

File        : FS_NOR_HW_SPIFI.h
Purpose     : low-level flash driver for quad SPI.
*/

#ifndef FS_NOR_HW_SPIFI_H     // Avoid recursive and multiple inclusion
#define FS_NOR_HW_SPIFI_H

#include "FS.h"
#include "cyhal_qspi.h"

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
    cyhal_gpio_t IO0;         /* Data/IO pin 0 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t IO1;         /* Data/IO pin 1 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t IO2;         /* Data/IO pin 2 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t IO3;         /* Data/IO pin 3 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t IO4;         /* Data/IO pin 4 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t IO5;         /* Data/IO pin 1 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t IO6;         /* Data/IO pin 2 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t IO7;         /* Data/IO pin 3 connected to the memory, Pass NC when unused. */
    cyhal_gpio_t Sclk;        /* Clock pin connected to the memory. */
    uint8_t NumMem;           /* Number of memory devices used. */
    cyhal_gpio_t Ssel[FS_NOR_HW_SPIFI_MAX_MEM_SUPPORTED]; /* Slave select pins connected to the memories.
                                                           * The index of this array corresponds to the HW
                                                           * unit number used with the emFile APIs.
                                                           */
    uint32_t HFClockFreqHz;   /* HF clock frequency (in hertz) input to the QSPI block. Refer to the
                               * device datasheet for the limitations.
                               */
#if defined(COMPONENT_RTOS_AWARE)
    uint8_t QspiIntrPriority; /* Interrupt priority for the QSPI block. */
#endif

    cyhal_qspi_t Obj;         /* This HW layer passes this object to the HAL APIs.
                               * User should not access the contents of it.
                               * Provided here so that user can call the HAL APIs directly
                               * if required.
                               */
} FS_NOR_HW_SPIFI_Config_t;

typedef enum
{
    FS_NOR_HW_SPIFI_RESULT_OK = 0u,
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
FS_NOR_HW_SPIFI_Result_t FS_NOR_HW_SPIFI_Configure(FS_NOR_HW_SPIFI_Config_t *Config);

#endif  // FS_NOR_HW_SPIFI_H

/*************************** End of file ****************************/
