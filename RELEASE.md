# SEGGER emFile for ModusToolbox Release Notes

[emFile](https://www.segger.com/products/file-system/emfile) is a fail-safe filesystem designed for embedded systems by SEGGER Microcontroller GmbH that can be used with different types of storage devices. Cypress has licensed emFile from SEGGER and offers it for free to its customers. This middleware library provides emFile in the form of pre-built libraries and supports FAT 12/16 and FAT 32 file systems.

**Important Notice Regarding Long File Names (LFN):** If you configure the software to support long file names on FAT file systems, you should determine whether a license from Microsoft is required. Cypress and its suppliers grant no license under Microsoft's intellectual property rights and assume no liability for any use of the software without obtaining any license that may be required.

## What's Included?

- The emFile is migrated to HAL-Next flow

## Known Issues and Limitations
- Supported SD bus speed modes are Default speed and High speed. Ultra High Speed (UHS) modes such as SDR 12, SDR25, and SDR50 requiring 1.8-V signaling are not supported. These modes will be supported in future.

- emFile always selects the 256kB erase sector size for S25FS128S. If S25FS128S is configured to use another erase sector size, this parameter can be set by the FS_NOR_SPIFI_SetSectorSize() function inside FS_X_AddDevices(). By default, for S25FS128S, the erase sector size is 64 kB.

- Octal SPI is not supported for NOR flash storage.

- emFile does not support working with memories that need configuration in the secure core from the non-secure core for Edge devices

## Supported software and tools

This version was validated for compatibility with the following software and tools:

| Software and Tools                        | Version  |
| :---------------------------------------- | :------- |
| ModusToolbox™ Software Environment        | 3.6      |
| MTB Device Support package for PSE84      | 1.0.0    |
| abstraction-rtos                          | 1.11.0   |
| GCC Compiler                              | 14.2.1   |
| IAR Compiler                              | 9.50.2   |
| Arm® Compiler 6                           | 6.22     |
| LLVM ARM Compiler                         | 19.1.5   |

## More information

- [emFile Middleware User Guide](https://infineon.github.io/emfile/html/index.html)

- [SEGGER emFile User Guide](./docs/UM02001_emFile.pdf)

- [README.md](./README.md)

- [Infineon Semiconductor GitHub](https://github.com/infineon)

- [ModusToolbox software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)

---
© 2021-2025 Cypress Semiconductor Corporation, an Infineon Technologies Company.
