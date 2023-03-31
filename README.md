# SEGGER emFile for ModusToolbox

## Overview

[emFile](https://www.segger.com/products/file-system/emfile) is a fail-safe filesystem designed for embedded systems by SEGGER Microcontroller GmbH that can be used with different types of storage devices. It is a high-performance library optimized for high speed, versatility, and a minimal memory footprint of both RAM and Flash.

Cypress has licensed emFile from SEGGER and offers it for free to its customers. This middleware library provides emFile in the form of pre-built libraries and supports 
FAT 12/16/32 file systems.

**Important Notice Regarding Long File Names (LFN):** If you configure the software to support long file names on FAT file systems, you should determine whether a license from Microsoft is required. Cypress and its suppliers grant no license under Microsoft's intellectual property rights and assume no liability for any use of the software without obtaining any license that may be required.

## Features

- SEGGER emFile library v5.6.1

- Supports FAT 12/16/32 file systems

- Supports Long File Name (LFN)

- Cache support via RAM for optimized performance

- Supports fail-safety through the Journaling component

- Supports encryption: DES (56-bit key length)

- Supports thread safety for use with multi-threaded RTOS environments using the [abstraction-rtos](https://github.com/Infineon/abstraction-rtos) library

- Supports memory card devices such as MMC, SD, SDHC, and eMMC using SD bus mode (card mode)

    - The SD/MMC HW layer uses DMA and waits on a semaphore until data transfer completes when enabled

    - The SD/MMC driver supports up to 2 instances (`FS_MMC_NUM_UNITS=2`)

- Supports Single-SPI/Dual-DSPI/Quad-SPI based NOR flash memories

- Supports wear leveling for use with NOR flash memories

## Quick Start

See the Quick Start section in the [emFile Middleware User Guide](./Doc/User_Guide.md) or see the [mtb-example-psoc6-filesystem-emfile-freertos](https://github.com/Infineon/mtb-example-psoc6-filesystem-emfile-freertos) code example that shows implementing emFile filesystem on SD card.

## Usage Instructions

When used in an RTOS environment, add `COMPONENTS=RTOS_AWARE` in the Makefile to enable waiting on a semaphore until read/write transfer completes. You need to also add `FREERTOS` to the components list if you are using [FreeRTOS](https://github.com/Infineon/freertos).

## Dependencies

- [abstraction-rtos](https://github.com/Infineon/abstraction-rtos) library if RTOS support (`COMPONENTS=RTOS_AWARE`) is required

    **Note:** *abstraction-rtos* is automatically pulled in when you add FreeRTOS using the Library Manager; otherwise, you need to add it manually.


## Supported Toolchains

- GNU Arm® Embedded Compiler v10.3.1 (`GCC_ARM`)

- Arm compiler v6.16 (`ARM`)

- IAR C/C++ compiler v9.30.1 (`IAR`)

## Supported Devices

- All PSoC&trade; 6 MCUs

## More information

- [emFile Middleware User Guide](./Doc/User_Guide.md)

- [SEGGER emFile User Guide](./Doc/UM02001_emFile.pdf)

- [Release Notes](./RELEASE.md)

- [mtb-example-psoc6-filesystem-emfile-freertos](https://github.com/Infineon/mtb-example-psoc6-filesystem-emfile-freertos) code example

- [Infineon GitHub](https://github.com/Infineon)

- [ModusToolbox&trade; software](https://www.Infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)

- [PSoC 6 MCU Code Examples using ModusToolbox](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software)

- [PSoC 6 MCU Middleware](https://github.com/Infineon/psoc6-middleware)

- [PSoC 6 MCU Resources - KBA223067](https://community.Infineon.com/t5/Knowledge-Base-Articles/How-to-Design-with-PSoC-6-MCU-KBA223067)

---
© 2021-2023 Cypress Semiconductor Corporation, an Infineon Technologies Company.
