# SEGGER emFile for ModusToolbox

## Overview

[emFile](https://www.segger.com/products/file-system/emfile) is a fail-safe filesystem designed for embedded systems by SEGGER Microcontroller GmbH that can be used with different types of storage devices. It is a high-performance library optimized for high speed, versatility, and a minimal memory footprint of both RAM and Flash.

Cypress has licensed emFile from SEGGER and offers it for free to its customers. This middleware library provides emFile in the form of pre-built libraries and supports FAT 12/16/32 file systems.

**Important Notice Regarding Long File Names (LFN):** If you configure the software to support long file names on FAT file systems, you should determine whether a license from Microsoft is required. Cypress and its suppliers grant no license under Microsoft's intellectual property rights and assume no liability for any use of the software without obtaining any license that may be required.

## Features

- SEGGER emFile library v5.22.0

- Supports FAT 12/16 and FAT 32 file systems

    - COMPONENT `EMFILE_FAT16` supports FAT12 and FAT 16

    - COMPONENT `EMFILE_FAT32` extends FAT12/16 and supports functionality FAT 32

- Supports Long File Name (LFN)

- Cache support via RAM for optimized performance

- Supports fail-safety through the Journaling component

- Supports encryption: DES (56-bit key length)

- Supports thread safety for use with multi-threaded RTOS environments using the [abstraction-rtos](https://github.com/infineon/abstraction-rtos) library

- Supports memory card devices such as MMC, SD, SDHC, and eMMC using SD bus mode (card mode)

    - The SD/MMC HW layer uses DMA and waits on a semaphore until data transfer completes when enabled

    - The SD/MMC driver supports up to 2 instances (`FS_MMC_NUM_UNITS=2`)

- Supports Single-SPI/Dual-DSPI/Quad-SPI based NOR flash memories

- Supports wear leveling for use with NOR flash memories

## Quick Start

The [Quick Start section of the emFile Middleware API Reference Guide]
describes step-by-step instructions to set up an emFile application.

## Supported Devices

- PSoC™ Edge E84 MCUs

## More information

- [SEGGER emFile User Guide](./docs/UM02001_emFile.pdf)

- [Release Notes](./RELEASE.md)

- [Infineon GitHub](https://github.com/infineon)

- [ModusToolbox&trade; software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)


---
© 2021-2025 Cypress Semiconductor Corporation, an Infineon Technologies Company.
