# SEGGER emFile for ModusToolbox Release Notes

[emFile](https://www.segger.com/products/file-system/emfile) is a fail-safe filesystem designed for embedded systems by SEGGER Microcontroller GmbH that can be used with different types of storage devices. Cypress has licensed emFile from SEGGER and offers it for free to its customers. This middleware library provides emFile in the form of pre-built libraries and supports FAT 12/16/32 file systems.

**Important Notice Regarding Long File Names (LFN):** If you configure the software to support long file names on FAT file systems, you should determine whether a license from Microsoft is required. Cypress and its suppliers grant no license under Microsoft's intellectual property rights and assume no liability for any use of the software without obtaining any license that may be required.

## What's Included?

- SEGGER emFile (in the form of pre-built static libraries)

- Hardware layer implementations for Card mode MMC/SD and Block Map NOR drivers

- OS layer implementation using the [abstraction-rtos](https://github.com/cypresssemiconductorco/abstraction-rtos) API

- Documentation

## Known Issues and Limitations

- Supported SD bus speed modes are Default speed and High speed. Ultra High Speed (UHS) modes such as SDR 12, SDR25, and SDR50 requiring 1.8-V signaling are not supported. These modes will be supported in future.

- Simultaneous use of MMC/SD (card mode) and NOR flash storage devices is not supported. However, you can use them simultaneously if you use an SDHC0 hardware instance when available. The MMC/SD driver picks an instance based on the data and clock pins provided by the user. See the PSoC&trade; 6 MCU datasheets for information on the pin assignment. This will be addressed in a future release.

- The NOR flash driver does not support connecting multiple memories to different QSPI data lines of the MCU. This means that all memories must be connected to the same set of data line. This will be addressed in a future release.

## Supported Software and Tools

This version was validated for compatibility with the following software and tools:

| Software and Tools                         | Version |
| :---                                       | :----  |
| ModusToolbox&trade; software environment   | 2.2     |
| GCC Compiler                               | 9.3     |
| IAR Compiler                               | 8.4     |
| Arm® Compiler 6                            | 6.13    |

Minimum required ModusToolbox Software Environment: v2.2.

## More information

- [emFile Middleware User Guide](./Doc/User_Guide.md)

- [SEGGER emFile User Guide](./Doc/UM02001_emFile.pdf)

- [README.md](./README.md)

- [mtb-example-psoc6-filesystem-emfile-freertos](https://github.com/cypresssemiconductorco/mtb-example-psoc6-filesystem-emfile-freertos) code example

- [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)

- [ModusToolbox software](https://www.cypress.com/products/modustoolbox-software-environment)

- [PSoC 6 MCU Code Examples using ModusToolbox](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software)

- [PSoC 6 MCU Middleware](https://github.com/cypresssemiconductorco/psoc6-middleware)

- [PSoC 6 MCU Resources - KBA223067](https://community.cypress.com/docs/DOC-14644)


---
© 2021 Cypress Semiconductor Corporation, an Infineon Technologies Company.
