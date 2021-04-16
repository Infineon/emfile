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
----------------------------------------------------------------------
File        : FS.h
Purpose     : Define global functions and types to be used by an
              application using the file system.

              This file needs to be included by any module using the
              file system.
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef FS_H               // Avoid recursive and multiple inclusion
#define FS_H

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include "FS_ConfDefaults.h"
#include "FS_Types.h"
#include "FS_Storage.h"

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif // _WIN32

#if defined(__cplusplus)
extern "C" {     // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

/*********************************************************************
*
*       File system version
*/
#define FS_VERSION                              50601u
#define FS_VERSION_MAJOR                        (FS_VERSION / 10000u)
#define FS_VERSION_MINOR                        ((FS_VERSION / 100u) % 100u)
#define FS_VERSION_PATCH                        (FS_VERSION % 100u)

/*********************************************************************
*
*       Error codes
*
*  Description
*    Values returned by the API functions to indicate the reason of an error.
*
*  Additional information
*    The last error code of an file handle can be checked using FS_FError().
*/
#define FS_ERRCODE_OK                           0         // No error
#define FS_ERRCODE_EOF                          (-1)      // End of file reached
#define FS_ERRCODE_PATH_TOO_LONG                (-2)      // Path to file or directory is too long
#define FS_ERRCODE_INVALID_PARA                 (-3)      // Invalid parameter passed
#define FS_ERRCODE_WRITE_ONLY_FILE              (-5)      // File can only be written
#define FS_ERRCODE_READ_ONLY_FILE               (-6)      // File can not be written
#define FS_ERRCODE_READ_FAILURE                 (-7)      // Error while reading from storage medium
#define FS_ERRCODE_WRITE_FAILURE                (-8)      // Error while writing to storage medium
#define FS_ERRCODE_FILE_IS_OPEN                 (-9)      // Trying to delete a file which is open
#define FS_ERRCODE_PATH_NOT_FOUND               (-10)     // Path to file or directory not found
#define FS_ERRCODE_FILE_DIR_EXISTS              (-11)     // File or directory already exists
#define FS_ERRCODE_NOT_A_FILE                   (-12)     // Trying to open a directory instead of a file
#define FS_ERRCODE_TOO_MANY_FILES_OPEN          (-13)     // Exceeded number of files opened at once (trial version)
#define FS_ERRCODE_INVALID_FILE_HANDLE          (-14)     // The file handle has been invalidated
#define FS_ERRCODE_VOLUME_NOT_FOUND             (-15)     // The volume name specified in a path is does not exist
#define FS_ERRCODE_READ_ONLY_VOLUME             (-16)     // Trying to write to a volume mounted in read-only mode
#define FS_ERRCODE_VOLUME_NOT_MOUNTED           (-17)     // Trying access a volume which is not mounted
#define FS_ERRCODE_NOT_A_DIR                    (-18)     // Trying to open a file instead of a directory
#define FS_ERRCODE_FILE_DIR_NOT_FOUND           (-19)     // File or directory not found
#define FS_ERRCODE_NOT_SUPPORTED                (-20)     // Functionality not supported
#define FS_ERRCODE_CLUSTER_NOT_FREE             (-21)     // Trying to allocate a cluster which is not free
#define FS_ERRCODE_INVALID_CLUSTER_CHAIN        (-22)     // Detected a shorter than expected cluster chain
#define FS_ERRCODE_STORAGE_NOT_PRESENT          (-23)     // Trying to access a removable storage which is not inserted
#define FS_ERRCODE_BUFFER_NOT_AVAILABLE         (-24)     // No more sector buffers available
#define FS_ERRCODE_STORAGE_TOO_SMALL            (-25)     // Not enough sectors on the storage medium
#define FS_ERRCODE_STORAGE_NOT_READY            (-26)     // Storage device can not be accessed
#define FS_ERRCODE_BUFFER_TOO_SMALL             (-27)     // Sector buffer smaller than sector size of storage medium
#define FS_ERRCODE_INVALID_FS_FORMAT            (-28)     // Storage medium is not formatted or the format is not valid
#define FS_ERRCODE_INVALID_FS_TYPE              (-29)     // Type of file system is invalid or not configured
#define FS_ERRCODE_FILENAME_TOO_LONG            (-30)     // The name of the file is too long
#define FS_ERRCODE_VERIFY_FAILURE               (-31)     // Data verification failure
#define FS_ERRCODE_VOLUME_FULL                  (-32)     // No more space on storage medium
#define FS_ERRCODE_DIR_NOT_EMPTY                (-33)     // Trying to delete a directory which is not empty
#define FS_ERRCODE_IOCTL_FAILURE                (-34)     // Error while executing a driver control command
#define FS_ERRCODE_INVALID_MBR                  (-35)     // Invalid information in the Master Boot Record
#define FS_ERRCODE_OUT_OF_MEMORY                (-36)     // Could not allocate memory
#define FS_ERRCODE_UNKNOWN_DEVICE               (-37)     // Storage device is not configured
#define FS_ERRCODE_ASSERT_FAILURE               (-38)     // FS_DEBUG_ASSERT() macro failed
#define FS_ERRCODE_TOO_MANY_TRANSACTIONS_OPEN   (-39)     // Maximum number of opened journal transactions exceeded
#define FS_ERRCODE_NO_OPEN_TRANSACTION          (-40)     // Trying to close a journal transaction which is not opened
#define FS_ERRCODE_INIT_FAILURE                 (-41)     // Error while initializing the storage medium
#define FS_ERRCODE_FILE_TOO_LARGE               (-42)     // Trying to write to a file which is larger than 4 Gbytes
#define FS_ERRCODE_HW_LAYER_NOT_SET             (-43)     // The HW layer is not configured
#define FS_ERRCODE_INVALID_USAGE                (-44)     // Trying to call a function in an invalid state
#define FS_ERRCODE_TOO_MANY_INSTANCES           (-45)     // Trying to create one instance more than maximum configured
#define FS_ERRCODE_TRANSACTION_ABORTED          (-46)     // A journal transaction has been stopped by the application
#define FS_ERRCODE_INVALID_CHAR                 (-47)     // Invalid character in the name of a file

/*********************************************************************
*
*       File positioning reference
*
*  Description
*    Reference point when changing the file position.
*/
#define FS_SEEK_SET                     0     // The reference is the beginning of the file.
#define FS_SEEK_CUR                     1     // The reference is the current position of the file pointer.
#define FS_SEEK_END                     2     // The reference is the end-of-file position.

/*********************************************************************
*
*       I/O commands for driver (internal)
*/
#define FS_CMD_REQUIRES_FORMAT          1003
#define FS_CMD_GET_DEVINFO              1004
#define FS_CMD_FORMAT_LOW_LEVEL         1005            // Used internally by FS_FormatLow() to command the driver to perform low-level format
#define FS_CMD_FREE_SECTORS             1006            // Used internally: Allows the FS layer to inform driver about free sectors
#define FS_CMD_SET_DELAY                1007            // Used in the simulation to simulate a slow device with RAM driver
#define FS_CMD_UNMOUNT                  1008            // Used internally by FS_STORAGE_Unmount() to inform the driver. Driver invalidates caches and all other information about medium.
#define FS_CMD_UNMOUNT_FORCED           1009            // Used internally by FS_STORAGE_UnmountForced() to inform the driver about an unforced remove of the device.
                                                        // Driver invalidates caches and all other information about medium.
#define FS_CMD_SYNC                     1010            // Tells the driver to clean caches. Typically, all dirty sectors are written.
#define FS_CMD_UNMOUNT_VOLUME           FS_CMD_UNMOUNT  // Obsolete: FS_CMD_UNMOUNT shall be used instead of FS_CMD_UNMOUNT_VOLUME.
#define FS_CMD_DEINIT                   1011            // Frees the resources allocated by the file system.
#define FS_CMD_CLEAN_ONE                1012            // Tells the driver to perform one clean operation. Usually this operation erases some data block on the storage medium which stores invalid information.
                                                        // Aux                    Not used
                                                        // pBuffer  [*int, OUT]   ==0 A second command will do nothing
                                                        //                        ==1 A second command will perform a clean operation
#define FS_CMD_CLEAN                    1013            // Executes the clean operation until all the invalid data on the storage medium has been erased
#define FS_CMD_GET_SECTOR_USAGE         1014            // Queries the driver about the usage of a logical sector.
                                                        // Aux      [int,  IN]    Index of the sector to be queried
                                                        // pBuffer  [*int, OUT]   ==0 Sector in use
                                                        //                        ==1 Sector not used
                                                        //                        ==2 Unknown
#define FS_CMD_ENABLE_READ_AHEAD        1015            // Requests the driver to start reading sectors in advance
#define FS_CMD_DISABLE_READ_AHEAD       1016            // Requests the driver to stop reading sectors in advance
#define FS_CMD_GET_CLEAN_CNT            1017            // Returns the number of clean operations which should be performed before all the invalid data on the storage medium has been erased
                                                        // pBuffer  [*int, OUT]   Number of clean operations
#define FS_CMD_SET_READ_ERROR_CALLBACK  1018            // Registers a callback function which should be invoked by a driver when a read error occurs.
                                                        // Aux      not used
                                                        // pBuffer  [*FS_ON_READ_ERROR_CALLBACK, IN]  Pointer to callback function
#define FS_CMD_SYNC_SECTORS             1019            // Informs the driver about the sectors which must be synchronized. Typically, the sectors are written to storage if cached.
                                                        // Aux      [int,  IN]    Index of the fist sector to be synchronized
                                                        // pBuffer  [*int, IN]    Number of sectors to synchronize

/*********************************************************************
*
*       CACHE Commands (internal)
*/
#define FS_CMD_CACHE_SET_MODE           6000L
#define FS_CMD_CACHE_CLEAN              6001L   // Write out all dirty sectors
#define FS_CMD_CACHE_SET_QUOTA          6002L
#define FS_CMD_CACHE_FREE_SECTORS       6003L
#define FS_CMD_CACHE_INVALIDATE         6004L   // Invalidate all sectors in cache
#define FS_CMD_CACHE_SET_ASSOC_LEVEL    6005L   // Sets the associativity level for the multi-way cache
#define FS_CMD_CACHE_GET_NUM_SECTORS    6006L   // Returns the number of sectors the cache is able to store
#define FS_CMD_CACHE_GET_TYPE           6007L   // Returns the type of the cache configured

/*********************************************************************
*
*       File attributes
*
*  Description
*    Attributes of files and directories.
*/
#define FS_ATTR_READ_ONLY                             0x01u     // The file is read-only. Applications can read the file
                                                                // but cannot write to it.
#define FS_ATTR_HIDDEN                                0x02u     // The file or directory is marked as hidden. Most of operating systems
                                                                // do not include these files in an ordinary directory listing.
                                                                // This flag is not evaluated by emFile.
#define FS_ATTR_SYSTEM                                0x04u     // The file or directory is part of, or is used exclusively by, the
                                                                // operating system. This flag is not evaluated by emFile.
#define FS_ATTR_ARCHIVE                               0x20u     // The file or directory is an archive file or directory.
                                                                // Applications can use this attribute to mark files for backup or removal.
                                                                // This flag is not evaluated by emFile.
#define FS_ATTR_DIRECTORY                             0x10u     // The file is actually a directory.

/*********************************************************************
*
*       File time types
*
*  Description
*    Types of timestamps available for a file or directory.
*/
#define FS_FILETIME_CREATE                            0         // Date and time when the file or directory was created.
#define FS_FILETIME_ACCESS                            1         // Date and time of the last read access to file or directory.
#define FS_FILETIME_MODIFY                            2         // Date and time of the last write access to file or directory.

/*********************************************************************
*
*       Volume mounting flags (internal)
*/
#define FS_MOUNT_R                                    (1uL << 0)
#define FS_MOUNT_W                                    (1uL << 1)

/*********************************************************************
*
*       Volume mounting modes
*
*  Description
*    Modes for mounting a volume.
*/
#define FS_MOUNT_RO                                   FS_MOUNT_R                  // Read-only. Data can only be read from storage device.
#define FS_MOUNT_RW                                   (FS_MOUNT_R | FS_MOUNT_W)   // Read / Write. Data can be read form and written to storage device.

/*********************************************************************
*
*       File system types
*
*  Description
*    Types of file systems supported by emFile.
*/
#define FS_FAT                                        0         // File Allocation Table (FAT).
#define FS_EFS                                        1         // SEGGER's Embedded File System (EFS).

/*********************************************************************
*
*       Format types
*
*  Description
*    Types of format supported by the emFile.
*/
#define FS_TYPE_FAT12                                 0x000Cu   // FAT with 12-bit allocation table entries.
#define FS_TYPE_FAT16                                 0x0010u   // FAT with 16-bit allocation table entries.
#define FS_TYPE_FAT32                                 0x0020u   // FAT with 32-bit allocation table entries.
#define FS_TYPE_EFS                                   0x0120u   // SEGGER's Embedded File System (EFS).

/*********************************************************************
*
*       Disk checking error codes
*
*  Description
*    Error codes reported by FS_CheckDisk() during operation.
*
*  Additional information
*    The error codes are reported via the ErrCode parameter of
*    the callback function.
*/
#define FS_CHECKDISK_ERRCODE_0FILE                    0x10      // A cluster chain is assigned to a file that do not contain data (file size is 0)
#define FS_CHECKDISK_ERRCODE_SHORTEN_CLUSTER          0x11      // The cluster chain assigned to a file is longer than the size of the file.
#define FS_CHECKDISK_ERRCODE_CROSSLINKED_CLUSTER      0x12      // A cluster is assigned to more than one file or directory.
#define FS_CHECKDISK_ERRCODE_FEW_CLUSTER              0x13      // The size of the file is larger than the cluster chain assigned to file.
#define FS_CHECKDISK_ERRCODE_CLUSTER_UNUSED           0x14      // A cluster is marked as in use, but not assigned to any file or directory.
#define FS_CHECKDISK_ERRCODE_CLUSTER_NOT_EOC          0x15      // A cluster is does not have end-of-chain marker.
#define FS_CHECKDISK_ERRCODE_INVALID_CLUSTER          0x16      // Invalid cluster id.
#define FS_CHECKDISK_ERRCODE_INVALID_DIRECTORY_ENTRY  0x17      // Invalid directory entry.
#define FS_CHECKDISK_ERRCODE_SECTOR_NOT_IN_USE        0x18      // A logical sector that stores data is not marked as in use in the device driver.

/*********************************************************************
*
*       Disk checking return values
*
*  Description
*    Error codes returned by FS_CheckDisk().
*/
#define FS_CHECKDISK_RETVAL_OK                        0         // OK, file system not in corrupted state
#define FS_CHECKDISK_RETVAL_RETRY                     1         // An error has be found and repaired, retry is required.
#define FS_CHECKDISK_RETVAL_ABORT                     2         // User aborted operation via callback or API call
#define FS_CHECKDISK_RETVAL_MAX_RECURSE               3         // Maximum recursion level reached, operation aborted
#define FS_CHECKDISK_RETVAL_CONTINUE                  4         // FS_CheckAT() returns this value to indicate that the allocation table has not been entirely checked.
#define FS_CHECKDISK_RETVAL_SKIP                      5         // FS_CheckDir() returns this value to indicate that the directory entry does not have to be checked.

/*********************************************************************
*
*       Disk checking action codes
*
*  Description
*    Values returned by the FS_CheckDisk() callback function.
*
*  Additional information
*    These values indicate FS_CheckDisk() how to handle a file
*    system error.
*/
#define FS_CHECKDISK_ACTION_DO_NOT_REPAIR             0         // The error does not have to be repaired.
#define FS_CHECKDISK_ACTION_SAVE_CLUSTERS             1         // The data stored in the clusters of a faulty cluster chain has to be saved to a file.
#define FS_CHECKDISK_ACTION_ABORT                     2         // The disk checking operation has to be aborted.
#define FS_CHECKDISK_ACTION_DELETE_CLUSTERS           3         // The data stored in the clusters of a faulty cluster chain has to be deleted.

/*********************************************************************
*
*       Volume information flags
*
*  Description
*    Flags that control the information returned by FS_GetVolumeInfoEx().
*/
#define FS_DISKINFO_FLAG_FREE_SPACE                   0x01       // Returns the available free space on the storage medium

/*********************************************************************
*
*       Debug message types
*
*  Description
*    Flags that control the output of debug messages.
*/
#define FS_MTYPE_INIT                                 (1uL << 0)  // Initialization log messages.
#define FS_MTYPE_API                                  (1uL << 1)  // Log messages from API functions.
#define FS_MTYPE_FS                                   (1uL << 2)  // Log messages from file system.
#define FS_MTYPE_STORAGE                              (1uL << 3)  // Storage layer log messages.
#define FS_MTYPE_JOURNAL                              (1uL << 4)  // Journal log messages
#define FS_MTYPE_CACHE                                (1uL << 5)  // Cache log messages
#define FS_MTYPE_DRIVER                               (1uL << 6)  // Log messages from device and logical drivers.
#define FS_MTYPE_OS                                   (1uL << 7)  // Log messages from OS integration layer.
#define FS_MTYPE_MEM                                  (1uL << 8)  // Log messages from internal memory allocator.

/*********************************************************************
*
*       Sector usage
*
*  Description
*    Usage status of logical sectors.
*/
#define FS_SECTOR_IN_USE                              0           // Logical sector is used to store data.
#define FS_SECTOR_NOT_USED                            1           // Logical sector is available.
#define FS_SECTOR_USAGE_UNKNOWN                       2           // The usage status is unknown to file system.

/*********************************************************************
*
*       OS locking
*
*  Description
*    Types of locking for multitasking access.
*
*  Additional information
*    These values can be assigned to FS_OS_LOCKING compile-time
*    configuration define.
*/
#define FS_OS_LOCKING_NONE                            0           // No locking against concurrent access.
#define FS_OS_LOCKING_API                             1           // Locking is performed at API function level (coarse locking).
#define FS_OS_LOCKING_DRIVER                          2           // Locking is performed at device driver level (fine locking).

/*********************************************************************
*
*       File open flags
*
*  Description
*    Specify how to open a file.
*
*  Additional information
*    These flags can be used as values for the Flags parameter
*    of FS_BIGFILE_Open(). More than one flag can be specified
*    and in this case they have to be separated by a bitwise OR
*    operator ('|').
*/
#define FS_BIGFILE_OPEN_FLAG_READ                     0x01u       // The application can read from the opened file.
#define FS_BIGFILE_OPEN_FLAG_WRITE                    0x02u       // The application can write to the opened file.
#define FS_BIGFILE_OPEN_FLAG_CREATE                   0x04u       // The file is created if it does not exists or it is truncated to 0 if it exists.
#define FS_BIGFILE_OPEN_FLAG_APPEND                   0x08u       // The application can write only at the end of the file.

/*********************************************************************
*
*       Public types
*
**********************************************************************
*/

/*********************************************************************
*
*       FS_CHECKDISK_ON_ERROR_CALLBACK
*
*  Function description
*    Type of function called by FS_CheckDisk() to report an error.
*
*  Parameters
*    ErrCode    Code of the reported error.
*
*  Return value
*    Value that indicates FS_CheckDisk() how the error should be
*    handled.
*
*  Additional information
*    ErrCode is one of the \tt{FS_CHECKDISK_ERRCODE_...} defines.
*    The value returned by FS_CHECKDISK_ON_ERROR_CALLBACK()
*    can be one of the \tt{FS_CHECKDISK_ACTION_...} defines.
*
*    In addition to ErrCode FS_CheckDisk() can pass other parameters
*    FS_CHECKDISK_ON_ERROR_CALLBACK() providing information about
*    the reported error. These parameters can be directly passed
*    to a printf()-style function along with the format returned
*    by FS_CheckDisk_ErrCode2Text() to create a text description
*    of the error in human-readable format.
*/
typedef int FS_CHECKDISK_ON_ERROR_CALLBACK(int ErrCode, ...);

/*********************************************************************
*
*       FS_BUSY_LED_CALLBACK
*
*  Function description
*    Type of function called by the file system to report a
*    change in the busy status of a volume.
*
*  Parameters
*    OnOff      Indicates if the volume is busy or not.
*               * 1   The volume is busy.
*               * 0   The volume is ready.
*
*  Additional information
*    A function of this type can be registered with the file system
*    via FS_SetBusyLEDCallback(). FS_BUSY_LED_CALLBACK() is called
*    by the file system each time the volume changes the busy status
*    from busy to ready and reverse. Therefore, an application
*    can use FS_BUSY_LED_CALLBACK()to indicate the busy / ready
*    status of a volume via LED or by other means.
*
*    The file system calls FS_BUSY_LED_CALLBACK() with OnOff set to 1
*    when the volume goes busy. When the volume becomes ready
*    FS_BUSY_LED_CALLBACK() is called again with OnOff set to 0.
*/
typedef void FS_BUSY_LED_CALLBACK(U8 OnOff);

/*********************************************************************
*
*       FS_MEM_CHECK_CALLBACK
*
*  Function description
*    Type of function called by the file system to check if a memory
*    region can be used in a 0-copy operation.
*
*  Parameters
*    pMem       Points to the first byte in the memory area
*               to be checked.
*    NumBytes   Size of the memory area in bytes.
*
*  Return value
*    !=0    The memory region can be used in a 0-copy operation.
*    ==0    The memory region cannot be used in a 0-copy operation.
*
*  Additional information
*    A function of this type is called by the file system before
*    any read or write operation performed by the application.
*    The function has to check if the memory region defined
*    by pMem and NumBytes can be passed directly to the device
*    driver during a 0-copy operation.
*
*    The callback function is typically required on a system where
*    the device driver uses DMA to transfer data to and from the
*    storage device and where not all the memory regions can be
*    accessed by the DMA. If the memory region cannot be accessed
*    by DMA the callback function has to return 0. The file system
*    copies then the data to an internal buffer that is accessible
*    to DMA and performs the data transfer. The callback has to
*    return a value different than 0 if the DMA can access the
*    specified memory region. In this case the memory region is
*    passed directly to device driver to perform the data transfer
*    and the internal copy operation of the file system is skipped.
*/
typedef int FS_MEM_CHECK_CALLBACK(void * pMem, U32 NumBytes);

/*********************************************************************
*
*       FS_MEM_FREE_CALLBACK
*
*  Function description
*    Type of function called by the file system to release dynamically
*    allocated memory.
*
*  Parameters
*    pData        Memory block to be released.
*
*  Additional information
*    The callback function has the same signature as the free() standard C
*    function. The application can register the callback function via
*    FS_SetMemHandler(). pMem points to a memory block that was allocated
*    using a call to pfAlloc callback function registered via FS_SetMemHandler().
*/
typedef void FS_MEM_FREE_CALLBACK(void * pData);

/*********************************************************************
*
*       FS_MEM_ALLOC_CALLBACK
*
*  Function description
*    Type of function called by the file system to allocate memory
*    dynamically.
*
*  Parameters
*    NumBytes     Number of bytes to be allocated.
*
*  Return value
*    !=NULL       OK, pointer to the allocated memory block.
*    ==NULL       Error, could not allocate memory.
*
*  Additional information
*    The callback function has the same signature as the malloc() standard C
*    function. The application can register the callback function via
*    FS_SetMemHandler().
*/
typedef void * FS_MEM_ALLOC_CALLBACK(U32 NumBytes);

/*********************************************************************
*
*       FS_TIME_DATE_CALLBACK
*
*  Description
*    Type of function called by the file system to get the actual
*    time an date.
*
*  Return value
*    Current time and date in a format suitable for the file system.
*
*  Additional information
*    The time and date have to be encoded as follows:
*      Bit 0-4:   2-second count (0-29)
*      Bit 5-10:  Minutes (0-59)
*      Bit 11-15: Hours (0-23)
*      Bit 16-20: Day of month (1-31)
*      Bit 21-24: Month of year (1-12)
*      Bit 25-31: Count of years from 1980 (0-127)
*    The callback function can be registered via FS_SetTimeDateCallback().
*    FS_X_GetTimeDate() is set as default callback function at the
*    file system initialization. The application has to provide
*    the implementation of FS_X_GetTimeDate().
*/
typedef U32 FS_TIME_DATE_CALLBACK(void);

/*********************************************************************
*
*       FS_DEVICE_TYPE
*/
typedef struct {
  const char * (*pfGetName)    (U8 Unit);
  int          (*pfAddDevice)  (void);                                                  // Called from AddDevice. Usually the first call to the driver
  int          (*pfRead)       (U8 Unit, U32 SectorIndex,       void * pBuffer, U32 NumSectors);
  int          (*pfWrite)      (U8 Unit, U32 SectorIndex, const void * pBuffer, U32 NumSectors, U8 RepeatSame);
  int          (*pfIoCtl)      (U8 Unit, I32 Cmd, I32 Aux, void * pBuffer);
  int          (*pfInitMedium) (U8 Unit);                                               // Called when formatting or mounting a device
  int          (*pfGetStatus)  (U8 Unit);
  int          (*pfGetNumUnits)(void);
} FS_DEVICE_TYPE;

/*********************************************************************
*
*       FS_ON_READ_ERROR_CALLBACK
*
*  Function description
*    The function is called when a driver encounters an error while
*    reading sector data. Typically called by the NAND driver to get
*    corrected sector data when a bit error occurs.
*
*  Parameters
*    pDeviceType    Type of storage device which encountered the read error
*    DeviceUnit     Unit number of the storage device where the read error occurred
*    SectorIndex    Index of the sector where the read error occurred
*    pBuffer        [OUT] Corrected sector data
*    NumSectors     Number of sectors on which the read error occurred
*
*  Return value
*    ==0    OK, sector data returned
*    !=0    An error occurred
*/
typedef struct {
  int (*pfCallback)(const FS_DEVICE_TYPE * pDeviceType, U32 DeviceUnit, U32 SectorIndex, void * pBuffer, U32 NumSectors);
} FS_ON_READ_ERROR_CALLBACK;

/*********************************************************************
*
*       FS_DIRENT
*/
typedef struct {
  char DirName[FS_MAX_PATH];
  U8   Attributes;
  U32  Size;
  U32  TimeStamp;
} FS_DIRENT;

/*********************************************************************
*
*       FS_DIR_POS
*/
typedef struct {
  U32 ClusterId;                  // Id of the current cluster.
  U32 FirstClusterId;             // Id of the first cluster assigned to directory.
  U32 DirEntryIndex;              // Index of the current directory entry (first directory entry has index 0)
  U32 ClusterIndex;               // Index of the current cluster relative to beginning of directory (0-based).
} FS_DIR_POS;

/*********************************************************************
*
*       FS_DIRENTRY_POS_FAT
*/
typedef struct {
  U32 SectorIndex;                // Index of the logical sector that stores the directory entry.
  U32 DirEntryIndex;              // Position of the directory entry relative to the beginning of the logical sector.
} FS_DIRENTRY_POS_FAT;

/*********************************************************************
*
*       FS_DIRENTRY_POS_EFS
*/
typedef struct {
  U32 FirstClusterId;             // Id of the first cluster in the parent directory that stores the directory entry.
  U32 DirEntryPos;                // Position of the directory entry relative to the beginning of the parent directory.
} FS_DIRENTRY_POS_EFS;

/*********************************************************************
*
*       FS_DIRENTRY_POS
*
*  Description
*    Position of a directory entry on the storage device.
*/
typedef struct {
  FS_DIRENTRY_POS_FAT fat;
  FS_DIRENTRY_POS_EFS efs;
} FS_DIRENTRY_POS;

/*********************************************************************
*
*       FS_DIR_OBJ
*/
typedef struct {
  FS_VOLUME       * pVolume;      // Volume on which the directory is stored.
  FS_DIR_POS        DirPos;       // Stores the current position in directory.
  FS_DIRENTRY_POS   ParentDirPos; // Position of the directory entry of the parent directory.
} FS_DIR_OBJ;

/*********************************************************************
*
*       FS_DIR
*/
struct FS_DIR {                       //lint -esym(9058, FS_DIR) tag unused outside of typedefs. Rationale: the typedef is used as forward declaration.
  FS_DIR_OBJ   DirObj;
  FS_DIRENT  * pDirEntry;
  U8           InUse;
};

/*********************************************************************
*
*       FS_FIND_DATA
*
*  Description
*    Information about a file or directory.
*
*  Additional information
*    This structure contains also the context for the file listing
*    operation. These members are considered internal and should
*    not be used by the application. FS_FIND_DATA is used as context
*    by the FS_FindFirstFile() and FS_FindNextFile() pair of functions.
*/
typedef struct {
  U8           Attributes;          // Attributes of the file or directory.
  U32          CreationTime;        // Date and time when the file or directory was created.
  U32          LastAccessTime;      // Date and time when the file or directory was accessed last.
  U32          LastWriteTime;       // Date and time when the file or directory was modified last.
  U32          FileSize;            // Size of the file. Set to 0 for a directory.
  char       * sFileName;           // Name of the file or directory as 0-terminated string.
                                    // It points to the buffer passed as argument to
                                    // FS_FindFirstFile().
  //
  // Private elements. Not to be used by the application.
  //
  int          SizeofFileName;      // Internal. Do not use.
  FS_DIR       Dir;                 // Internal. Do not use.
} FS_FIND_DATA;

/*********************************************************************
*
*       FS_DISK_INFO
*
*  Description
*    Information about a volume.
*
*  Additional information
*    IsDirty can be used to check if the volume formatted as FAT
*    has been correctly unmounted before a system reset. IsDirty
*    is set to 1 at file system initialization if the file system
*    was not properly unmounted.
*/
typedef struct {
  U32          NumTotalClusters;    // Total number of clusters on the storage device.
  U32          NumFreeClusters;     // Number of clusters that are not in use.
  U16          SectorsPerCluster;   // Number of sectors in a cluster.
  U16          BytesPerSector;      // Size of the logical sector in bytes.
  U16          NumRootDirEntries;   // Number of directory entries in the root directory.
                                    // This member is valid only for volumes formatted
                                    // as FS_TYPE_FAT12 or FS_TYPE_FAT16.
  U16          FSType;              // Type of file system. One of FS_TYPE_... defines.
  U8           IsSDFormatted;       // Set to 1 if the volume has been formatted according
                                    // to SD specification. This member is valid only for
                                    // volumes formatted as FAT.
  U8           IsDirty;             // Set to 1 if the volume was not unmounted correctly
                                    // or the file system modified the storage. This member
                                    // is valid only for volumes formatted as FAT.
  const char * sAlias;              // Alternative name of the volume (0-terminated).
                                    // Set to NULL if the volume alias feature is disabled.
} FS_DISK_INFO;

/*********************************************************************
*
*       FS_FORMAT_INFO
*
*  Description
*    Parameters for the high-level format.
*
*  Additional information
*    This structure is passed as parameter to FS_Format() to specify
*    additional information about how the storage device has to be
*    formatted.
*
*    A cluster is the minimal unit size a file system can handle.
*    Sectors are combined together to form a cluster.
*    SectorsPerCluster has to be a power of 2 value, for example 1,
*    2, 4, 8, 16, 32, 64. Larger values lead to a higher read / write
*    performance when working with large files while low values (1)
*    make more efficient use of disk space.
*    * Allowed values for FAT: 1--128
*    * Allowed values for EFS: 1--32768
*
*    NumRootDirEntries represents the number of directory entries in
*    the root directory should have. This is only a proposed value.
*    The actual value  depends on the FAT type. This value is typically
*    used for FAT12 or FAT16 formatted volume that have a fixed number
*    of entries in the root directory. On FAT32 formatted volume the
*    root directory can grow dynamically. The file system uses a
*    default value of 256 if NumRootDirEntries is set to 0.
*
*    pDevInfo should be typically set to NULL unless some specific
*    information about the storage device has to be passed to
*    format function. The file system requests internally the
*    information from storage device if pDevInfo is set to NULL.
*/
typedef struct {
  U16           SectorsPerCluster;    // Number of sectors in a cluster.
  U16           NumRootDirEntries;    // Number of directory entries in the root directory.
  FS_DEV_INFO * pDevInfo;             // Information about the storage device.
} FS_FORMAT_INFO;

/*********************************************************************
*
*       FS_FILETIME
*
*  Description
*    Time and date representation.
*
*  Additional information
*    FS_FILETIME represents a timestamp using individual members for
*    the month, day, year, weekday, hour, minute, and second values.
*    This can be useful for getting or setting a timestamp of a file
*    or directory. The conversion between timestamp and FS_FILETIME
*    can be done using FS_FileTimeToTimeStamp() and
*    FS_TimeStampToFileTime()
*/
typedef struct {
  U16 Year;         // Year (The value has to be greater than 1980.)
  U16 Month;        // Month (1--12, 1: January, 2: February, etc.)
  U16 Day;          // Day of the month (1--31)
  U16 Hour;         // Hour (0--23)
  U16 Minute;       // Minute (0--59)
  U16 Second;       // Second (0--59)
} FS_FILETIME;

/*********************************************************************
*
*       FS_WRITEMODE
*
*  Description
*    Modes of writing to file.
*/
typedef enum {
  FS_WRITEMODE_SAFE,        // Allows maximum fail-safe behavior. The allocation table and
                            // the directory entry are updated after each write access to file.
                            // This write mode provides the slowest performance.
  FS_WRITEMODE_MEDIUM,      // Medium fail-safe. The allocation table is updated after each
                            // write access to file. The directory entry is updated only
                            // if file is synchronized that is when FS_Sync(), FS_FClose(),
                            // or FS_SyncFile() is called.
  FS_WRITEMODE_FAST,        // This write mode provided the maximum performance. The directory entry
                            // is updated only if the file is synchronized that is when FS_Sync(),
                            // FS_FClose(), or FS_SyncFile() is called. The allocation table is
                            // modified only if necessary.
  FS_WRITEMODE_UNKNOWN      // End of enumeration (for internal use only)
} FS_WRITEMODE;

/*********************************************************************
*
*       FS_CHS_ADDR
*
*  Description
*    Address of physical block on a mechanical drive.
*/
typedef struct {
  U16 Cylinder;           // Cylinder number (0-based)
  U8  Head;               // Read / write head (0-based)
  U8  Sector;             // Index of the sector on a cylinder.
} FS_CHS_ADDR;

/*********************************************************************
*
*       FS_PARTITION_INFO
*
*  Description
*    Information about a MBR partition.
*/
typedef struct {
  U32         NumSectors;       // Total number of sectors in the partition.
  U32         StartSector;      // Index of the first sector in the partition
                                // relative to the beginning of the storage device.
  FS_CHS_ADDR StartAddr;        // Address of the first sector in the partition in CHS format.
  FS_CHS_ADDR EndAddr;          // Address of the last sector in the partition in CHS format.
  U8          Type;             // Type of the partition.
  U8          IsActive;         // Set to 1 if the partition is bootable.
} FS_PARTITION_INFO;

/*********************************************************************
*
*       FS_FILE_INFO
*
*  Description
*    Information about a file or directory.
*
*  Additional information
*    The Attributes member is an or-combination of the following
*    values: FS_ATTR_READ_ONLY, FS_ATTR_HIDDEN, FS_ATTR_SYSTEM,
*    FS_ATTR_ARCHIVE, or FS_ATTR_DIRECTORY.
*
*    For directories the FileSize member is always 0.
*/
typedef struct {
  U8  Attributes;       // File or directory attributes.
  U32 CreationTime;     // Date and time when the file was created.
  U32 LastAccessTime;   // Date and time when the file was accessed last.
  U32 LastWriteTime;    // Date and time when the file was written to last.
  U32 FileSize;         // Size of the file in bytes.
} FS_FILE_INFO;

#if FS_SUPPORT_DEINIT
  struct FS_ON_EXIT_CB;

  typedef struct FS_ON_EXIT_CB {
    struct FS_ON_EXIT_CB * pNext;
    void                (* pfOnExit)(void);
  } FS_ON_EXIT_CB;
#endif

/*********************************************************************
*
*       FS_FREE_SPACE_DATA
*
*  Description
*    Information the number of free space available on a volume.
*
*  Additional information
*    This structure stores the result and context of the operation
*    that calculates the amount of free space available on a volume.
*    The amount of free space is returned as a number of clusters
*    via the NumClustersFree member. The members of the search
*    context are considered internal and should not be used by
*    the application.
*    FS_FREE_SPACE_DATA is used by the FS_GetVolumeFreeSpaceFirst()
*    FS_GetVolumeFreeSpaceNext() pair of API functions.
*/
typedef struct {
  U32         NumClustersFree;      // Number of unallocated clusters found.
  //
  // Context information. Not to be used by the application.
  //
  int         SizeOfBuffer;         // Internal. Do not use. Number of bytes in the work buffer.
  void      * pBuffer;              // Internal. Do not use. Work buffer.
  FS_VOLUME * pVolume;              // Internal. Do not use. Volume information.
  U32         FirstClusterId;       // Internal. Do not use. Id of the first cluster to be checked.
} FS_FREE_SPACE_DATA;

/*********************************************************************
*
*       FS_CLUSTER_MAP
*
*  Description
*    Information about the cluster usage.
*/
typedef struct {
  U8  * pData;                      // Cluster usage (one bit per cluster). 1 -> in use, 0 -> not in use.
  U32   FirstClusterId;             // First usage information in pData is related to this cluster id.
  I32   NumClusters;                // pData stores usage information about this number of clusters.
} FS_CLUSTER_MAP;

/*********************************************************************
*
*       FS_CHECK_DATA
*
*  Description
*    Context for the disk checking operation.
*
*  Additional information
*    All the information in this structure is internal and it should
*    not be accessed or modified in any way by the application.
*/
typedef struct {
  FS_VOLUME                      * pVolume;       // Volume to be checked.
  FS_CHECKDISK_ON_ERROR_CALLBACK * pfOnError;     // Callback function to be invoked on error.
  U32                              WriteCntAT;    // Saved value of the write counter of the allocation table.
  FS_CLUSTER_MAP                   ClusterMap;    // Information about the cluster usage.
} FS_CHECK_DATA;

/*********************************************************************
*
*       FS_BIGFILE_INFO
*
*  Description
*    Information about a big file.
*
*  Additional information
*    The Attributes member is an or-combination of the following
*    values: FS_ATTR_READ_ONLY, FS_ATTR_HIDDEN, FS_ATTR_SYSTEM,
*    FS_ATTR_ARCHIVE, or FS_ATTR_DIRECTORY.
*/
typedef struct {
  U8  Attributes;       // File or directory attributes.
  U32 CreationTime;     // Date and time when the file was created.
  U32 LastAccessTime;   // Date and time when the file was accessed last.
  U32 LastWriteTime;    // Date and time when the file was written to last.
  U64 FileSize;         // Size of the file in bytes.
} FS_BIGFILE_INFO;

/*********************************************************************
*
*       FS_BIGFILE_OBJ
*
*  Description
*    Identifies an opened file.
*
*  Additional information
*    The application has to allocate a structure of this type for
*    each file it opens. The structure is filled with data by
*    FS_BIGFILE_Open() and it has to stay valid until the application
*    calls FS_BIGFILE_Close() with the same structure as parameter.
*
*    Index points to the fragment file that is currently opened.
*    The base file has the index 0, the first extension file has
*    the index 1 and so on. The value of the Index member is valid
*    only when pFile member is different than NULL.
*/
typedef struct {
  char      acName[FS_MAX_LEN_FULL_FILE_NAME];      // Fully qualified name of the base file (0-terminated string)
  U8        Flags;                                  // Stores the value passed to Flags parameter in the call to FS_BIGFILE_Open().
  U64       Size;                                   // Size of the big file in bytes.
  U64       Pos;                                    // Read and write position inside the big file (0-based byte offset).
  U16       Index;                                  // Index of the current regular file opened (0-based)
  FS_FILE * pFile;                                  // Handle to the current regular file opened.
} FS_BIGFILE_OBJ;

/*********************************************************************
*
*       FS_BIGFILE_FIND_DATA
*
*  Description
*    Information about a file or directory.
*
*  Additional information
*    This structure contains also the context for the file listing
*    operation. These members are considered internal and should
*    not be used by the application. FS_BIGFILE_FIND_DATA is used as context
*    by the FS_BIGFILE_FindFirst() and FS_BIGFILE_FindNext() pair of functions.
*/
typedef struct {
  U8               Attributes;          // Attributes of the file or directory.
  U32              CreationTime;        // Date and time when the file or directory was created.
  U32              LastAccessTime;      // Date and time when the file or directory was accessed last.
  U32              LastWriteTime;       // Date and time when the file or directory was modified last.
  U64              FileSize;            // Size of the file in bytes.
  char           * sFileName;           // Name of the file or directory as 0-terminated string.
                                        // It points to the buffer passed as argument to
                                        // FS_BIGFILE_FindFirst().
  U16              SizeOfFileName;      // Internal. Not to be used by the application.
  FS_FIND_DATA     FindData;            // Internal. Not to be used by the application.
  FS_BIGFILE_OBJ   FileObj;             // Internal. Not to be used by the application.
} FS_BIGFILE_FIND_DATA;

/*********************************************************************
*
*       FS_MEM_INFO
*
*  Description
*    Information about the memory management.
*
*  Additional information
*    The information can be queried via FS_GetMemInfo().
*
*    IsExternal contains the value of the FS_SUPPORT_EXT_MEM_MANAGER
*    configuration define.
*
*    If the file system is configured to use the internal memory allocator
*    (FS_SUPPORT_EXT_MEM_MANAGER set to 0) then NumBytesTotal stores the value
*    passed as second parameter to FS_AssignMemory(). NumBytesTotal is set to
*    0 if the file system is configured to use an external memory allocator
*    (FS_SUPPORT_EXT_MEM_MANAGER set to 1) because this value is not known to
*    the file system.
*
*    The value of NumBytesAllocated stores the number of bytes allocated by the
*    file system at the time FS_GetMemInfo() is called. In emFile versions older
*    than 5.xx this information was stored in the global variable FS_NumBytesAllocated.
*/
typedef struct {
  U8  IsExternal;                       // Memory allocator type.
  U32 NumBytesTotal;                    // Size of the memory pool in bytes.
  U32 NumBytesAllocated;                // Number of bytes allocated from the memory pool.
} FS_MEM_INFO;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       "Standard" file I/O functions
*/
FS_FILE * FS_FOpen  (const char * sFileName, const char * sMode);
int       FS_FOpenEx(const char * sFileName, const char * sMode, FS_FILE ** ppFile);
int       FS_FClose (FS_FILE    * pFile);
U32       FS_FRead  (      void * pData, U32 ItemSize, U32 NumItems, FS_FILE * pFile);
U32       FS_FWrite (const void * pData, U32 ItemSize, U32 NumItems, FS_FILE * pFile);

/*********************************************************************
*
*       Non-standard file I/O functions
*/
U32 FS_Read (FS_FILE * pFile,       void * pData, U32 NumBytes);
U32 FS_Write(FS_FILE * pFile, const void * pData, U32 NumBytes);

/*********************************************************************
*
*       File pointer handling
*/
int FS_FSeek       (FS_FILE * pFile, I32 Offset, int Origin);
int FS_SetEndOfFile(FS_FILE * pFile);
int FS_SetFileSize (FS_FILE * pFile, U32 NumBytes);
I32 FS_FTell       (FS_FILE * pFile);

#define FS_GetFilePos(pFile)                                FS_FTell(pFile)
#define FS_SetFilePos(pFile, DistanceToMove, MoveMethod)    FS_FSeek(pFile, DistanceToMove, MoveMethod)

/*********************************************************************
*
*       I/O error handling
*/
int          FS_FEof                     (FS_FILE * pFile);
I16          FS_FError                   (FS_FILE * pFile);
void         FS_ClearErr                 (FS_FILE * pFile);
const char * FS_ErrorNo2Text             (int       ErrCode);
#if FS_SUPPRESS_EOF_ERROR
  void       FS_ConfigEOFErrorSuppression(int OnOff);
#endif

/*********************************************************************
*
*       File functions
*/
int          FS_CopyFile   (const char     * sFileNameSrc,  const char * sFileNameDest);
int          FS_CopyFileEx (const char     * sFileNameSrc,  const char * sFileNameDest, void * pBuffer, U32 NumBytes);
U32          FS_GetFileSize(const FS_FILE  * pFile);
int          FS_Move       (const char     * sNameSrc,      const char * sNameDest);
int          FS_Remove     (const char     * sFileName);
int          FS_Rename     (const char     * sNameOld,      const char * sNameNew);
int          FS_Truncate   (      FS_FILE  * pFile,         U32 NewFileSize);
int          FS_Verify     (      FS_FILE  * pFile,         const void * pData, U32 NumBytes);
int          FS_SyncFile   (      FS_FILE  * pFile);
int          FS_WipeFile   (const char     * sFileName);

/*********************************************************************
*
*       IOCTL
*/
int         FS_IoCtl(const char *pVolumeName, I32 Cmd, I32 Aux, void * pBuffer);

/*********************************************************************
*
*       Volume related functions
*/
int            FS_GetVolumeName          (int VolumeIndex, char * sVolumeName, int VolumeNameSize);
U32            FS_GetVolumeSize          (const char * sVolumeName);
U32            FS_GetVolumeSizeKB        (const char * sVolumeName);
U32            FS_GetVolumeFreeSpace     (const char * sVolumeName);
U32            FS_GetVolumeFreeSpaceKB   (const char * sVolumeName);
int            FS_GetNumVolumes          (void);
FS_VOLUME    * FS_AddDevice              (const FS_DEVICE_TYPE * pDevType);
int            FS_AddPhysDevice          (const FS_DEVICE_TYPE * pDevType);
void           FS_Unmount                (const char * sVolumeName);
int            FS_Mount                  (const char * sVolumeName);
int            FS_MountEx                (const char * sVolumeName, U8 MountType);
int            FS_GetVolumeInfo          (const char * sVolumeName, FS_DISK_INFO * pInfo);
int            FS_GetVolumeInfoEx        (const char * sVolumeName, FS_DISK_INFO * pInfo, int Flags);
int            FS_IsVolumeMounted        (const char * sVolumeName);
int            FS_GetVolumeLabel         (const char * sVolumeName, char * sVolumeLabel, unsigned VolumeLabelSize);
int            FS_SetVolumeLabel         (const char * sVolumeName, const char * sVolumeLabel);
void           FS_UnmountForced          (const char * sVolumeName);
void           FS_SetAutoMount           (const char * sVolumeName, U8 MountType);
int            FS_GetVolumeStatus        (const char * sVolumeName);
void           FS_RemoveDevice           (const char * sVolumeName);
int            FS_Sync                   (const char * sVolumeName);
int            FS_FreeSectors            (const char * sVolumeName);
int            FS_GetVolumeFreeSpaceFirst(FS_FREE_SPACE_DATA * pFSD, const char * sVolumeName, void * pBuffer, int SizeOfBuffer);
int            FS_GetVolumeFreeSpaceNext (FS_FREE_SPACE_DATA * pFSD);
#if FS_SUPPORT_VOLUME_ALIAS
  int          FS_SetVolumeAlias         (const char * sVolumeName, const char * sVolumeAlias);
  const char * FS_GetVolumeAlias         (const char * sVolumeName);
#endif // FS_SUPPORT_VOLUME_ALIAS
int            FS_GetMountType           (const char * sVolumeName);

/*********************************************************************
*
*       File write mode
*/
void         FS_SetFileWriteMode  (FS_WRITEMODE WriteMode);
void         FS_SetFileWriteModeEx(FS_WRITEMODE WriteMode, const char * sVolumeName);
FS_WRITEMODE FS_GetFileWriteMode  (void);
FS_WRITEMODE FS_GetFileWriteModeEx(const char * sVolumeName);

/*********************************************************************
*
*       Journaling / transaction safety
*/

/*********************************************************************
*
*       FS_JOURNAL_STAT_COUNTERS
*
*  Description
*    Journal statistical counters.
*
*  Additional information
*    The statistical counters can be queried via FS_JOURNAL_GetStatCounters().
*    The application can use FS_JOURNAL_ResetStatCounters() to set all
*    the statistical counters to 0.
*
*    MaxWriteSectorCnt can be used to fine tune the size of the journal file.
*/
typedef struct {
  U32 WriteSectorCnt;         // Number of sectors written by the file system to journal.
  U32 NumTransactions;        // Number of journal transactions performed.
  U32 FreeSectorCnt;          // Number of sectors freed.
  U32 OverflowCnt;            // Number of times the journal has been cleaned before the end of a transaction.
  U32 WriteSectorCntStorage;  // Number of sectors written by the journal to the storage device.
  U32 ReadSectorCntStorage;   // Number of sectors read by the journal from the storage device.
  U32 MaxWriteSectorCnt;      // Maximum number of sectors written by the file system to journal file in any transaction.
} FS_JOURNAL_STAT_COUNTERS;

/*********************************************************************
*
*       FS_JOURNAL_OVERFLOW_INFO
*
*  Description
*    Information about a journal overflow event.
*
*  Additional information
*    This type of information is returned via a callback of type
*    FS_JOURNAL_ON_OVERFLOW_EX_CALLBACK when a journal overflow
*    event occurs.
*/
typedef struct {
  U8 VolumeIndex;             // Index of the volume on which the journal overflow occurred.
} FS_JOURNAL_OVERFLOW_INFO;

/*********************************************************************
*
*       FS_JOURNAL_ON_OVERFLOW_CALLBACK
*
*  Function description
*    Prototype of the function that is called on a journal overflow event.
*
*  Parameters
*    sVolumeName      Name of the volume on which the overflow event occurred.
*
*  Additional information
*    This type of callback function can be registered using
*    FS_JOURNAL_SetOnOverflowCallback().
*/
typedef void (FS_JOURNAL_ON_OVERFLOW_CALLBACK)(const char * sVolumeName);

/*********************************************************************
*
*       FS_JOURNAL_ON_OVERFLOW_EX_CALLBACK
*
*  Function description
*    Prototype of the function that is called on a journal overflow event.
*
*  Parameters
*    pOvrflowInfo     Information related to the overflow event.
*
*  Return value
*    ==0      The journal transaction has to continue to completion.
*    ==1      The journal transaction has to be aborted with an error.
*
*  Additional information
*    This type of callback function can be registered using
*    FS_JOURNAL_SetOnOverflowExCallback().
*
*    The return value indicates the journal module how to proceed when
*    an overflow event occurs.
*/
typedef int (FS_JOURNAL_ON_OVERFLOW_EX_CALLBACK)(const FS_JOURNAL_OVERFLOW_INFO * pOverflowInfo);

/*********************************************************************
*
*       FS_JOURNAL_INFO
*
*  Description
*    Information about the journal.
*
*  Additional information
*    The information about the journal can be queried via FS_JOURNAL_GetInfo()
*
*    NumSectorsFree is always smaller than or equal to NumSectors.
*    The difference between NumSectors and NumSectorsFree represents
*    the number of logical sectors that can still be stored to journal
*    without causing a journal overflow.
*
*    IsEnabled and OpenCnt values are identical to the values returned
*    by FS_JOURNAL_IsEnabled() and FS_JOURNAL_GetOpenCnt() respectively.
*/
typedef struct {
  U8  IsEnabled;                // Set to 1 if the journal is used to protect the integrity of the file system structure.
  U8  IsFreeSectorSupported;    // Set to 1 if the journal has been configured to forward "free sector" requests to device driver.
  U16 OpenCnt;                  // Number of times the current transaction has been opened.
  U32 NumSectors;               // Number of logical sectors that can be stored in the journal.
  U32 NumSectorsFree;           // Number of logical sectors that do not store any file system data.
} FS_JOURNAL_INFO;

#if FS_SUPPORT_JOURNAL

/*********************************************************************
*
*       Journal API functions
*/
int  FS_JOURNAL_Begin                  (const char * sVolumeName);
int  FS_JOURNAL_Create                 (const char * sVolumeName, U32 NumBytes);
int  FS_JOURNAL_CreateEx               (const char * sVolumeName, U32 NumBytes, U8 SupportFreeSector);
int  FS_JOURNAL_Disable                (const char * sVolumeName);
int  FS_JOURNAL_Enable                 (const char * sVolumeName);
int  FS_JOURNAL_End                    (const char * sVolumeName);
int  FS_JOURNAL_GetInfo                (const char * sVolumeName, FS_JOURNAL_INFO * pInfo);
int  FS_JOURNAL_GetOpenCnt             (const char * sVolumeName);
int  FS_JOURNAL_GetStatCounters        (const char * sVolumeName, FS_JOURNAL_STAT_COUNTERS * pStat);
int  FS_JOURNAL_Invalidate             (const char * sVolumeName);
int  FS_JOURNAL_IsEnabled              (const char * sVolumeName);
int  FS_JOURNAL_IsPresent              (const char * sVolumeName);
int  FS_JOURNAL_ResetStatCounters      (const char * sVolumeName);
int  FS_JOURNAL_SetFileName            (const char * sVolumeName, const char * sFileName);
void FS_JOURNAL_SetOnOverflowExCallback(FS_JOURNAL_ON_OVERFLOW_EX_CALLBACK * pfOnOverflow);
void FS_JOURNAL_SetOnOverflowCallback  (FS_JOURNAL_ON_OVERFLOW_CALLBACK    * pfOnOverflow);

#endif // FS_SUPPORT_JOURNAL

/*********************************************************************
*
*       File/directory attribute functions
*/
int  FS_SetFileAttributes   (const char * sName, U8 AttrMask);
U8   FS_GetFileAttributes   (const char * sName);
U8   FS_ModifyFileAttributes(const char * sName, U8 SetMask, U8 ClrMask);

/*********************************************************************
*
*       File/directory time stamp functions
*/
void FS_FileTimeToTimeStamp(const FS_FILETIME * pFileTime, U32         * pTimeStamp);
int  FS_GetFileTime        (const char        * sName,     U32         * pTimeStamp);
int  FS_GetFileTimeEx      (const char        * sName,     U32         * pTimeStamp, int TimeType);
int  FS_SetFileTime        (const char        * sName,     U32           TimeStamp);
int  FS_SetFileTimeEx      (const char        * sName,     U32           TimeStamp,  int TimeType);
void FS_TimeStampToFileTime(U32                 TimeStamp, FS_FILETIME * pFileTime);
void FS_SetTimeDateCallback(FS_TIME_DATE_CALLBACK * pfTimeDate);

/*********************************************************************
*
*       File/directory information
*/
int  FS_GetFileInfo(const char * sName, FS_FILE_INFO * pInfo);

/*********************************************************************
*
*       File system misc. functions
*/
int  FS_GetNumFilesOpen  (void);
int  FS_GetNumFilesOpenEx(const char * sVolumeName);
U32  FS_GetMaxSectorSize (void);

/*********************************************************************
*
*       File system directory functions
*/
int  FS_CreateDir      (const char * sDirName);
int  FS_MkDir          (const char * sDirName);
int  FS_RmDir          (const char * sDirName);
int  FS_DeleteDir      (const char * sDirName, int MaxRecursionLevel);
int  FS_FindFirstFile  (FS_FIND_DATA * pFD, const char * sDirName, char * sFileName, int SizeOfFileName);
int  FS_FindNextFile   (FS_FIND_DATA * pFD);
int  FS_FindNextFileEx (FS_FIND_DATA * pFD);
void FS_FindClose      (FS_FIND_DATA * pFD);

/*********************************************************************
*
*       Obsolete directory functions
*/
void        FS_DirEnt2Attr(FS_DIRENT  * pDirEnt, U8   * pAttr);
void        FS_DirEnt2Name(FS_DIRENT  * pDirEnt, char * pBuffer);
U32         FS_DirEnt2Size(FS_DIRENT  * pDirEnt);
U32         FS_DirEnt2Time(FS_DIRENT  * pDirEnt);
U32         FS_GetNumFiles(FS_DIR     * pDir);
FS_DIR    * FS_OpenDir    (const char * sDirName);
int         FS_CloseDir   (FS_DIR     * pDir);
FS_DIRENT * FS_ReadDir    (FS_DIR     * pDir);
void        FS_RewindDir  (FS_DIR     * pDir);

/*********************************************************************
*
*       File system control functions
*/
void     FS_Init            (void);
#if FS_SUPPORT_DEINIT
  void   FS_DeInit          (void);
  void   FS_AddOnExitHandler(FS_ON_EXIT_CB * pCB, void (*pfOnExit)(void));
#endif

/*********************************************************************
*
*       Formatting
*/
int          FS_FormatLLIfRequired(const char * sVolumeName);
int          FS_FormatLow         (const char * sVolumeName);
int          FS_Format            (const char * sVolumeName, const FS_FORMAT_INFO * pFormatInfo);
int          FS_IsLLFormatted     (const char * sVolumeName);
int          FS_IsHLFormatted     (const char * sVolumeName);

/*********************************************************************
*
*       Partitioning
*/
#define FS_NUM_PARTITIONS  4
int          FS_CreateMBR       (const char * sVolumeName, FS_PARTITION_INFO * pPartInfo, int NumPartitions);
int          FS_GetPartitionInfo(const char * sVolumeName, FS_PARTITION_INFO * pPartInfo, U8 PartIndex);

/*********************************************************************
*
*       CheckDisk functionality
*/
int          FS_CheckAT               (FS_CHECK_DATA * pCheckData);
const char * FS_CheckDisk_ErrCode2Text(int ErrCode);
int          FS_CheckDir              (FS_CHECK_DATA * pCheckData, const char * sPath);
int          FS_CheckDisk             (const char * sVolumeName, void * pBuffer, U32 BufferSize, int MaxRecursionLevel, FS_CHECKDISK_ON_ERROR_CALLBACK * pfOnError);
void         FS_FAT_AbortCheckDisk    (void);
int          FS_InitCheck             (FS_CHECK_DATA * pCheckData, const char * sVolumeName, void * pBuffer, U32 BufferSize, FS_CHECKDISK_ON_ERROR_CALLBACK * pfOnError);

/*********************************************************************
*
*       Configuration functions.
*/

/*********************************************************************
*
*       General runtime configuration functions
*/
int  FS_SetMaxSectorSize         (unsigned MaxSectorSize);
void FS_ConfigOnWriteDirUpdate   (char OnOff);
#if (FS_SUPPORT_EXT_MEM_MANAGER == 0)
void FS_AssignMemory             (U32 * pData, U32 NumBytes);
#else
void FS_SetMemHandler            (FS_MEM_ALLOC_CALLBACK * pfAlloc, FS_MEM_FREE_CALLBACK * pfFree);
#endif // FS_SUPPORT_EXT_MEM_MANAGER == 0
int  FS_GetMemInfo               (FS_MEM_INFO * pMemInfo);
#if FS_SUPPORT_POSIX
  void FS_ConfigPOSIXSupport     (int OnOff);
#endif
#if FS_VERIFY_WRITE
  void FS_ConfigWriteVerification(int OnOff);
#endif

/*********************************************************************
*
*       File name conversion
*/

/*********************************************************************
*
*       FS_CHARSET_TYPE
*
*  Description
*    Operations on file names specific to a character set.
*/
typedef struct {
  FS_WCHAR (*pfGetChar)(const U8 * pChar, unsigned NumBytes, unsigned * pNumBytes);
  FS_WCHAR (*pfToUpper)(FS_WCHAR oemChar);
  FS_WCHAR (*pfToLower)(FS_WCHAR oemChar);
  int      (*pfIsUpper)(FS_WCHAR oemChar);
  int      (*pfIsLower)(FS_WCHAR oemChar);
  FS_WCHAR (*pfToOEM)  (FS_WCHAR UnicodeChar);
} FS_CHARSET_TYPE;

/*********************************************************************
*
*       FS_UNICODE_CONV_INFO
*
*  Description
*    Information about the Unicode conversion.
*/
typedef struct {
  U8 IsOEMEncoding;           // Set to 1 if the string to be converted to Unicode is in OEM format. For the UTF-8 converter this value is set to 0.
  U8 MaxBytesPerChar;         // Maximum number of bytes used to encode a character. 1 for single-byte character sets such as CP1252 and 2 for CP932 (Shift JIS) character set.
} FS_UNICODE_CONV_INFO;

/*********************************************************************
*
*       FS_UNICODE_CONV
*
*  Description
*    Operations for Unicode conversion of file names.
*/
typedef struct {
  FS_WCHAR (*pfDecodeChar) (const U8 * pChar, unsigned NumBytes, unsigned * pNumBytes);
  int      (*pfEncodeChar) (      U8 * pChar, unsigned MaxNumBytes, FS_WCHAR UnicodeChar);
  int      (*pfGetNumChars)(const U8 * pChar, unsigned NumBytes);
  int      (*pfGetCharOff) (const U8 * pChar, unsigned NumBytes, unsigned   CharPos);
  FS_WCHAR (*pfGetChar)    (const U8 * pChar, unsigned NumBytes, unsigned * pNumBytes);
  void     (*pfGetInfo)    (FS_UNICODE_CONV_INFO * pInfo);
} FS_UNICODE_CONV;

void FS_SetCharSetType(const FS_CHARSET_TYPE * pCharSetType);

/*********************************************************************
*
*       Supported character sets
*/
extern const FS_CHARSET_TYPE FS_CHARSET_CP437;            // Latin characters
extern const FS_CHARSET_TYPE FS_CHARSET_CP932;            // Japanese characters (encoded as Shift JIS)

/*********************************************************************
*
*       Supported Unicode converters
*/
extern const FS_UNICODE_CONV FS_UNICODE_CONV_CP437;       // Unicode <-> CP437 (DOS Latin US) converter
extern const FS_UNICODE_CONV FS_UNICODE_CONV_CP932;       // Unicode <-> CP932 (Shift JIS) converter
extern const FS_UNICODE_CONV FS_UNICODE_CONV_UTF8;        // Unicode <-> UTF-8 converter

/*********************************************************************
*
*       FAT specific functions.
*/
#if FS_SUPPORT_FAT

int    FS_FAT_FormatSD                  (const char * sVolumeName);
U32    FS_FAT_GrowRootDir               (const char * sVolumeName, U32 NumAddEntries);
void   FS_FAT_SupportLFN                (void);
void   FS_FAT_DisableLFN                (void);
#if FS_SUPPORT_FILE_NAME_ENCODING
  void FS_FAT_SetLFNConverter           (const FS_UNICODE_CONV * pUnicodeConv);
#endif
#if FS_FAT_USE_FSINFO_SECTOR
  void FS_FAT_ConfigFSInfoSectorUse     (int OnOff);
#endif
#if FS_MAINTAIN_FAT_COPY
  void FS_FAT_ConfigFATCopyMaintenance  (int OnOff);
#endif
#if FS_FAT_PERMIT_RO_FILE_MOVE
  void FS_FAT_ConfigROFileMovePermission(int OnOff);
#endif
#if FS_FAT_UPDATE_DIRTY_FLAG
  void FS_FAT_ConfigDirtyFlagUpdate     (int OnOff);
#endif

#endif // FS_SUPPORT_FAT

/*********************************************************************
*
*       EFS runtime configuration functions.
*/
#if FS_SUPPORT_EFS

#if FS_EFS_SUPPORT_STATUS_SECTOR
  void FS_EFS_ConfigStatusSectorSupport (int OnOff);
#endif
#if FS_EFS_CASE_SENSITIVE
  void FS_EFS_ConfigCaseSensitivity     (int OnOff);
#endif
#if FS_SUPPORT_FILE_NAME_ENCODING
  void FS_EFS_SetFileNameConverter      (const FS_UNICODE_CONV * pUnicodeConv);
#endif // FS_SUPPORT_FILE_NAME_ENCODING

#endif // FS_SUPPORT_EFS

/*********************************************************************
*
*       BigFile API
*/

/*********************************************************************
*
*       API functions that work with file objects
*/
int FS_BIGFILE_Close      (FS_BIGFILE_OBJ * pFileObj);
int FS_BIGFILE_GetPos     (FS_BIGFILE_OBJ * pFileObj, U64 * pPos);
int FS_BIGFILE_GetSize    (FS_BIGFILE_OBJ * pFileObj, U64 * pSize);
int FS_BIGFILE_Open       (FS_BIGFILE_OBJ * pFileObj, const char * sFileName, unsigned Flags);
int FS_BIGFILE_Read       (FS_BIGFILE_OBJ * pFileObj,       void * pData, U32 NumBytesToRead, U32 * pNumBytesRead);
int FS_BIGFILE_SetPos     (FS_BIGFILE_OBJ * pFileObj, U64 Pos);
int FS_BIGFILE_SetSize    (FS_BIGFILE_OBJ * pFileObj, U64 Size);
int FS_BIGFILE_Sync       (FS_BIGFILE_OBJ * pFileObj);
int FS_BIGFILE_Write      (FS_BIGFILE_OBJ * pFileObj, const void * pData, U32 NumBytesToWrite, U32 * pNumBytesWritten);

/*********************************************************************
*
*       API functions that work with file names
*/
int FS_BIGFILE_Copy      (const char * sFileNameSrc, const char * sFileNameDest, void * pBuffer, U32 NumBytes);
int FS_BIGFILE_FindFirst (FS_BIGFILE_FIND_DATA * pFD, const char * sDirName, char * sFileName, int SizeOfFileName);
int FS_BIGFILE_FindNext  (FS_BIGFILE_FIND_DATA * pFD);
int FS_BIGFILE_FindClose (FS_BIGFILE_FIND_DATA * pFD);
int FS_BIGFILE_GetInfo   (const char * sFileName, FS_BIGFILE_INFO * pInfo);
int FS_BIGFILE_ModifyAttr(const char * sFileName, unsigned SetMask, unsigned ClrMask);
int FS_BIGFILE_Move      (const char * sFileNameSrc, const char * sFileNameDest);
int FS_BIGFILE_Remove    (const char * sFileName);

/*********************************************************************
*
*       File buffer related functions.
*/

/*********************************************************************
*
*       File buffer flags
*
*  Description
*    File buffer operating options.
*/
#define FS_FILE_BUFFER_WRITE              (1u << 0)         // Cache written data.
#define FS_FILE_BUFFER_ALIGNED            (1u << 1)         // Keep cached data aligned to logical sector boundary.

#define FS_SIZEOF_FILE_BUFFER_STRUCT      sizeof(FS_FILE_BUFFER)

/*********************************************************************
*
*       File buffer size
*
*  Description
*    Calculates the file buffer size.
*
*  Additional information
*    This define can be used in an application to calculate the number
*    of bytes that have to be allocated in order to store the specified
*    number of bytes in the file buffer. \tt{NumBytes} is typically
*    a multiple of logical sector size.
*/
#define FS_SIZEOF_FILE_BUFFER(NumBytes)   (FS_SIZEOF_FILE_BUFFER_STRUCT + (NumBytes))

/*********************************************************************
*
*       File buffer API
*/
#if FS_SUPPORT_FILE_BUFFER
  int FS_ConfigFileBufferDefault(int          BufferSize,  int Flags);
  int FS_SetFileBufferFlags     (FS_FILE *    pFile,       int Flags);
  int FS_SetFileBufferFlagsEx   (const char * sVolumeName, int Flags);
  int FS_SetFileBuffer          (FS_FILE * pFile, void * pData, I32 NumBytes, int Flags);
#endif // FS_SUPPORT_FILE_BUFFER

/*********************************************************************
*
*       Memory accessible support
*/

/*********************************************************************
*
*       Misc. functions
*/
int FS_GetFileId             (const char * sFileName, U8 * pId);
U16 FS_GetVersion            (void);
#if FS_SUPPORT_BUSY_LED
  int FS_SetBusyLEDCallback (const char * sVolumeName, FS_BUSY_LED_CALLBACK * pfBusyLED);
#endif // FS_SUPPORT_BUSY_LED
#if FS_SUPPORT_CHECK_MEMORY
  int FS_SetMemCheckCallback(const char * sVolumeName, FS_MEM_CHECK_CALLBACK * pfMemCheck);
#endif // FS_SUPPORT_CHECK_MEMORY

/*********************************************************************
*
*       Configuration checking functions
*/
int  FS_CONF_GetMaxPath           (void);
int  FS_CONF_IsFATSupported       (void);
int  FS_CONF_IsEFSSupported       (void);
int  FS_CONF_IsFreeSectorSupported(void);
int  FS_CONF_IsCacheSupported     (void);
int  FS_CONF_IsEncryptionSupported(void);
int  FS_CONF_IsJournalSupported   (void);
char FS_CONF_GetDirectoryDelimiter(void);
int  FS_CONF_IsDeInitSupported    (void);
int  FS_CONF_GetOSLocking         (void);
int  FS_CONF_GetNumVolumes        (void);
int  FS_CONF_IsTrialVersion       (void);
int  FS_CONF_GetDebugLevel        (void);

/*********************************************************************
*
*       SPI bus width handling
*/
#define FS_BUSWIDTH_CMD_SHIFT               8u
#define FS_BUSWIDTH_ADDR_SHIFT              4u
#define FS_BUSWIDTH_DATA_SHIFT              0u
#define FS_BUSWIDTH_MASK                    0x0Fu
#define FS_BUSWIDTH_CMD_1BIT                (1uL << FS_BUSWIDTH_CMD_SHIFT)
#define FS_BUSWIDTH_CMD_2BIT                (2uL << FS_BUSWIDTH_CMD_SHIFT)
#define FS_BUSWIDTH_CMD_4BIT                (4uL << FS_BUSWIDTH_CMD_SHIFT)
#define FS_BUSWIDTH_ADDR_1BIT               (1uL << FS_BUSWIDTH_ADDR_SHIFT)
#define FS_BUSWIDTH_ADDR_2BIT               (2uL << FS_BUSWIDTH_ADDR_SHIFT)
#define FS_BUSWIDTH_ADDR_4BIT               (4uL << FS_BUSWIDTH_ADDR_SHIFT)
#define FS_BUSWIDTH_DATA_1BIT               (1uL << FS_BUSWIDTH_DATA_SHIFT)
#define FS_BUSWIDTH_DATA_2BIT               (2uL << FS_BUSWIDTH_DATA_SHIFT)
#define FS_BUSWIDTH_DATA_4BIT               (4uL << FS_BUSWIDTH_DATA_SHIFT)
#define FS_BUSWIDTH_MAKE(Cmd, Addr, Data)   (((Cmd) << FS_BUSWIDTH_CMD_SHIFT)   | \
                                             ((Addr) << FS_BUSWIDTH_ADDR_SHIFT) | \
                                             ((Data) << FS_BUSWIDTH_DATA_SHIFT))

/*********************************************************************
*
*       SPI bus width decoding
*
*  Description
*    Macros for the handling of SPI bus width.
*
*  Additional information
*    The functions of the FS_NOR_HW_TYPE_SPIFI NOR hardware layer
*    and of the FS_NAND_HW_TYPE_QSPI NAND hardware layer that exchange
*    data with the storage device take a parameter that specifies
*    how many data lines have to be used to send different parts
*    of a request and to receive the answer. The value of this
*    parameter is encoded as a 16-bit value and the macros in
*    this section can help extract the bus width value for a
*    specific part of the request and response.
*
*    The value is encoded as follows:
*    +-----------+----------------------------------------------------------+
*    | Bit range | Description                                              |
*    +-----------+----------------------------------------------------------+
*    | 0-3       | Number of data lines to be used to transfer the data.    |
*    +-----------+----------------------------------------------------------+
*    | 4-7       | Number of data lines to be used to transfer the address. |
*    +-----------+----------------------------------------------------------+
*    | 8-11      | Number of data lines to be used to transfer the command. |
*    +-----------+----------------------------------------------------------+
*/
#define FS_BUSWIDTH_GET_CMD(BusWidth)       (((BusWidth) >> FS_BUSWIDTH_CMD_SHIFT)  & FS_BUSWIDTH_MASK)   // Returns the number of data lines to be used for sending the command code.
#define FS_BUSWIDTH_GET_ADDR(BusWidth)      (((BusWidth) >> FS_BUSWIDTH_ADDR_SHIFT) & FS_BUSWIDTH_MASK)   // Returns the number of data lines to be used for sending the data address.
#define FS_BUSWIDTH_GET_DATA(BusWidth)      (((BusWidth) >> FS_BUSWIDTH_DATA_SHIFT) & FS_BUSWIDTH_MASK)   // Returns the number of data lines to be used for sending and receiving of the user data.

/*********************************************************************
*
*       Device and logical drivers
*/

/*********************************************************************
*
*       Driver types
*/
extern const FS_DEVICE_TYPE FS_RAMDISK_Driver;         // Driver that uses RAM as storage
extern const FS_DEVICE_TYPE FS_WINDRIVE_Driver;        // Driver for Windows drives and file images
extern const FS_DEVICE_TYPE FS_MMC_CM_Driver;          // Driver for SD/MMC using card controller
extern const FS_DEVICE_TYPE FS_MMC_CM_RO_Driver;       // Driver for SD/MMC using card controller (read-only mode)
extern const FS_DEVICE_TYPE FS_MMC_SPI_Driver;         // Driver for SD/MMC using SPI
extern const FS_DEVICE_TYPE FS_IDE_Driver;             // Driver for IDE and Compact Flash
extern const FS_DEVICE_TYPE FS_NOR_Driver;             // Driver for NOR flashes (fast write)
extern const FS_DEVICE_TYPE FS_NAND_Driver;            // Driver for SLC NAND flashes
extern const FS_DEVICE_TYPE FS_NOR_BM_Driver;          // Driver for NOR flashes (reduced RAM usage)
extern const FS_DEVICE_TYPE FS_NAND_UNI_Driver;        // Driver for SLC NAND flashes with ECC engine
extern const FS_DEVICE_TYPE FS_NAND_UNI_RO_Driver;     // Driver for SLC NAND flashes with ECC engine (read-only mode)
extern const FS_DEVICE_TYPE FS_DISKPART_Driver;        // Logical driver for disk partitions
extern const FS_DEVICE_TYPE FS_CRYPT_Driver;           // Logical driver for encryption
extern const FS_DEVICE_TYPE FS_READAHEAD_Driver;       // Logical driver that reads sectors in advance
extern const FS_DEVICE_TYPE FS_RAID1_Driver;           // Logical driver that performs disk mirroring
extern const FS_DEVICE_TYPE FS_RAID5_Driver;           // Logical driver that performs disk stripping with parity checking
extern const FS_DEVICE_TYPE FS_SECSIZE_Driver;         // Logical driver that converts between different sector sizes
extern const FS_DEVICE_TYPE FS_WRBUF_Driver;           // Logical driver that buffers sector write operations
extern const FS_DEVICE_TYPE FS_LOGVOL_Driver;          // Logical driver for combining / splitting physical volumes

/*********************************************************************
*
*       NOR driver
*/

/*********************************************************************
*
*       NOR block types
*
*  Description
*    Type of data stored by the Block map NOR driver to a physical sector.
*/
#define FS_NOR_BLOCK_TYPE_UNKNOWN               0             // Unknown data type.
#define FS_NOR_BLOCK_TYPE_DATA                  1             // Physical sector stores a data block.
#define FS_NOR_BLOCK_TYPE_WORK                  2             // Physical sector stores a work block.
#define FS_NOR_BLOCK_TYPE_EMPTY_ERASED          3             // Physical sector is empty.
#define FS_NOR_BLOCK_TYPE_EMPTY_NOT_ERASED      4             // Physical sector is stores invalid data.

/*********************************************************************
*
*       Device operation flags
*
*  Description
*    Values to be used for the Flags member of FS_NOR_SPI_DEVICE_PARA
*/
#define FS_NOR_SPI_DEVICE_FLAG_ERROR_STATUS     (1u << 0)     // The device reports errors via Flag Status Register.
#define FS_NOR_SPI_DEVICE_FLAG_WEL_ADDR_MODE    (1u << 1)     // The write enable latch has to be set before changing the address mode.

/*********************************************************************
*
*       NOR sector types
*
*  Description
*    Type of data stored by the Sector map NOR driver to a physical sector.
*/
#define FS_NOR_SECTOR_TYPE_UNKNOWN              0             // Unknown data type.
#define FS_NOR_SECTOR_TYPE_DATA                 1             // Physical sector stores file system data.
#define FS_NOR_SECTOR_TYPE_WORK                 2             // Physical sector is used by the internal copy operation.
#define FS_NOR_SECTOR_TYPE_INVALID              3             // Physical sector contains invalid data.

/*********************************************************************
*
*       FS_NOR_SECTOR_INFO
*
*  Description
*    Information about a physical sector maintained by the sector map NOR driver.
*/
typedef struct {
  U32 Off;                    // Position of the physical sector relative to the first byte of NOR flash device
  U32 Size;                   // Size of physical sector in bytes
  U32 EraseCnt;               // Number of times the physical sector has been erased
  U16 NumUsedSectors;         // Number of logical sectors that contain valid data
  U16 NumFreeSectors;         // Number of logical sectors that are empty (i.e. blank)
  U16 NumEraseableSectors;    // Number of logical sectors that contain old (i.e. invalid) data.
  U8  Type;                   // Type of data stored in the physical sector (see FS_NOR_SECTOR_TYPE_...)
} FS_NOR_SECTOR_INFO;

/*********************************************************************
*
*       FS_NOR_DISK_INFO
*
*  Description
*    Management information maintained by the sector map NOR driver.
*/
typedef struct {
  U32 NumPhysSectors;         // Number of physical sectors available for data storage.
  U32 NumLogSectors;          // Number of available logical sectors.
  U32 NumUsedSectors;         // Number of logical sectors that store valid data.
  U16 BytesPerSector;         // Size of the logical sector used by the driver in bytes.
} FS_NOR_DISK_INFO;

/*********************************************************************
*
*       FS_NOR_STAT_COUNTERS
*
*  Description
*    Statistical counters maintained by the sector map NOR driver.
*/
typedef struct {
  U32 EraseCnt;               // Number of sector erase operations.
  U32 ReadSectorCnt;          // Number of logical sectors read by the file system.
  U32 WriteSectorCnt;         // Number of logical sectors written by the file system.
  U32 CopySectorCnt;          // Number of logical sectors copied internally by the driver.
  U32 ReadCnt;                // Number of times the driver read data from the NOR flash device.
  U32 ReadByteCnt;            // Number of bytes read by the driver from the NOR flash device.
  U32 WriteCnt;               // Number of times the driver wrote data to the NOR flash device.
  U32 WriteByteCnt;           // Number of bytes written by the driver to the NOR flash device.
} FS_NOR_STAT_COUNTERS;

/*********************************************************************
*
*       FS_NOR_BM_SECTOR_INFO
*
*  Description
*    Information about a physical sector maintained by the block map NOR driver.
*/
typedef struct {
  U32 Off;                    // Position of the physical sector relative to the first byte of NOR flash device
  U32 Size;                   // Size of physical sector in bytes
  U32 EraseCnt;               // Number of times the physical sector has been erased
  U16 lbi;                    // Index of the logical block stored in the physical sector
  U8  Type;                   // Type of data stored in the physical sector (see FS_NOR_BLOCK_TYPE_...)
} FS_NOR_BM_SECTOR_INFO;

/*********************************************************************
*
*       FS_NOR_BM_DISK_INFO
*
*  Description
*    Management information maintained by the block map NOR driver.
*/
typedef struct {
  U16 NumPhySectors;          // Number of physical sectors available for data storage.
  U16 NumLogBlocks;           // Number blocks available for the storage of file system data.
  U16 NumUsedPhySectors;      // Number of physical sectors that store valid data.
  U16 LSectorsPerPSector;     // Number of logical sectors that are mapped in a physical sector.
  U16 BytesPerSector;         // Size of the logical sector used by the driver in bytes.
  U32 EraseCntMax;            // Maximum value of an erase count.
  U32 EraseCntMin;            // Minimum value of an erase count.
  U32 EraseCntAvg;            // Average value of an erase count.
  U8  HasFatalError;          // Indicates if a fatal error has occurred during the operation (0 - no fatal error, 1 - a fatal error occurred)
  U8  ErrorType;              // Code indicating the type of fatal error that occurred.
  U32 ErrorPSI;               // Index of the physical sector where the fatal error occurred.
  U8  IsWriteProtected;       // Indicates if the NOR flash device has been switched permanently to read-only mode as a result of a fatal error (0 - NOR flash device is writable, 1 - NOR flash device is write protected)
  U8  IsWLSuspended;          // Indicates if the wear leveling process is temporarily suspended (0 - active, 1 - suspended)
  U32 MaxEraseCntDiff;        // Difference between the erase counts of two physical sectors that trigger an active wear leveling operation.
  U16 NumEraseCnt;            // Number of erase counts used for the calculation of the erase count average (EraseCntAvg)
  U16 NumPhySectorsValid;     // Number of physical sectors that have a valid header.
  U16 NumWorkBlocks;          // Number of work blocks used by the Block Map NOR driver.
} FS_NOR_BM_DISK_INFO;

/*********************************************************************
*
*       FS_NOR_BM_STAT_COUNTERS
*
*  Description
*    Statistical counters maintained by the block map NOR driver.
*
*  Additional information
*    The statistical counters can be queried via FS_NOR_BM_GetStatCounters()
*    and can be set to 0 via FS_NOR_BM_ResetStatCounters().
*
*    aBitErrorCnt[0] stores the number of 1 bit error occurrences,
*    aBitErrorCnt[1] stores the number of 2 bit error occurrences, and so on.
*/
typedef struct {
  U32 NumFreeBlocks;          // Number of blocks that are not used for data storage.
  U32 EraseCnt;               // Number of sector erase operations.
  U32 ReadSectorCnt;          // Number of logical sectors read by the file system.
  U32 WriteSectorCnt;         // Number of logical sectors written by the file system.
  U32 ConvertViaCopyCnt;      // Number of times a work block has been converted via a copy operation.
  U32 ConvertInPlaceCnt;      // Number of times a work block has been converted in-place (i.e. by changing the block type from work to data).
  U32 NumValidSectors;        // Number of valid logical sectors stored on the NOR flash device.
  U32 CopySectorCnt;          // Number of logical sectors copied internally by the driver.
  U32 NumReadRetries;         // Number of times the driver retried a read operation due to an error.
  U32 ReadPSHCnt;             // Number of times the driver read from the header of a physical sector.
  U32 WritePSHCnt;            // Number of times the driver wrote to the header of a physical sector.
  U32 ReadLSHCnt;             // Number of times the driver read from the header of a logical sector.
  U32 WriteLSHCnt;            // Number of times the driver wrote to the header of a logical sector.
  U32 ReadCnt;                // Number of times the driver read data from the NOR flash device.
  U32 ReadByteCnt;            // Number of bytes read by the driver from the NOR flash device.
  U32 WriteCnt;               // Number of times the driver wrote data to the NOR flash device.
  U32 WriteByteCnt;           // Number of bytes written by the driver to the NOR flash device.
  U32 BitErrorCnt;            // Total number of bit errors corrected.
  U32 aBitErrorCnt[FS_NOR_STAT_MAX_BIT_ERRORS];   // Number of times a specific number of bit errors occurred.
} FS_NOR_BM_STAT_COUNTERS;

/*********************************************************************
*
*       FS_NOR_BM_MOUNT_INFO
*
*  Description
*    Information about the mounted NOR flash device.
*/
typedef struct {
  U16 NumPhySectors;          // Number of physical sectors available for data storage.
  U16 NumLogBlocks;           // Number blocks available for the storage of file system data.
  U16 LSectorsPerPSector;     // Number of logical sectors that are mapped in a physical sector.
  U16 BytesPerSector;         // Size of the logical sector used by the driver in bytes.
  U8  HasFatalError;          // Indicates if a fatal error has occurred during the operation (0 - no fatal error, 1 - a fatal error occurred)
  U8  ErrorType;              // Code indicating the type of fatal error that occurred.
  U32 ErrorPSI;               // Index of the physical sector where the fatal error occurred.
  U8  IsWriteProtected;       // Indicates if the NOR flash device has been switched permanently to read-only mode as a result of a fatal error (0 - NOR flash device is writable, 1 - NOR flash device is write protected)
  U8  IsWLSuspended;          // Indicates if the wear leveling process is temporarily suspended (0 - active, 1 - suspended)
  U16 NumWorkBlocks;          // Number of work blocks used by the Block Map NOR driver.
} FS_NOR_BM_MOUNT_INFO;

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_WRITE_OFF
*
*  Function description
*    Writes data to the NOR flash device.
*
*  Parameters
*    Unit       Index of the physical layer instance (0-based)
*    Off        Byte offset to write to.
*    pData      [IN] Data to be written to NOR flash device.
*    NumBytes   Number of bytes to be written.
*
*  Return value
*    ==0      OK, data written.
*    !=0      An error occurred.
*
*  Additional information
*    This function is a member of the NOR physical layer API
*    and is mandatory to be implemented by each NOR physical layer.
*    The NOR driver calls this function when it writes data to the
*    NOR flash device. It is guaranteed that the NOR driver tries to
*    modify memory regions of the NOR flash device that are erased.
*    This means that the function does not have to check if the memory
*    region it has to modify are already erased. FS_NOR_PHY_TYPE_WRITE_OFF
*    has to be able to handle write requests that stretch over multiple
*    physical sectors.
*
*    Off specifies a number of bytes relative to the StartAddr value specified
*    in the call to FS_NOR_PHY_TYPE_CONFIGURE.
*/
typedef int FS_NOR_PHY_TYPE_WRITE_OFF(U8 Unit, U32 Off, const void * pData, U32 NumBytes);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_READ_OFF
*
*  Function description
*    Reads data from the NOR flash device.
*
*  Parameters
*    Unit       Index of the physical layer instance (0-based)
*    pData      [OUT] Data read from NOR flash device.
*    Off        Byte offset to read from.
*    NumBytes   Number of bytes to be read.
*
*  Return value
*    ==0      OK, data read.
*    !=0      An error occurred.
*
*  Additional information
*    This function is a member of the NOR physical layer API
*    and is mandatory to be implemented by each NOR physical layer.
*    The NOR driver calls this function when it reads data from the
*    NOR flash device. FS_NOR_PHY_TYPE_READ_OFF has to be able to handle
*    read request that stretch over multiple physical sectors.
*
*    Off specifies a number of bytes relative to the StartAddr value specified
*    in the call to FS_NOR_PHY_TYPE_CONFIGURE.
*/
typedef int FS_NOR_PHY_TYPE_READ_OFF(U8 Unit, void * pData, U32 Off, U32 NumBytes);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_ERASE_SECTOR
*
*  Function description
*    Erases a NOR physical sector.
*
*  Parameters
*    Unit           Index of the physical layer instance (0-based)
*    SectorIndex    Index of the NOR physical sector to be erased.
*
*  Return value
*    ==0    OK, physical sector is not blank.
*    !=0    An error occurred.
*
*  Additional information
*    This function is a member of the NOR physical layer API
*    and is mandatory to be implemented by each NOR physical layer.
*    The NOR driver calls this function when it erases a physical sector
*    NOR flash device. The erase operation must set to 1 all the bits
*    in the specified physical sector.
*
*    SectorIndex is relative to StartAddr value specified in the call to
*    FS_NOR_PHY_TYPE_CONFIGURE. The physical sector located at StartAddr
*    has the index 0.
*/
typedef int FS_NOR_PHY_TYPE_ERASE_SECTOR(U8 Unit, unsigned int SectorIndex);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_GET_SECTOR_INFO
*
*  Function description
*    Returns information about a NOR physical sector.
*
*  Parameters
*    Unit           Index of the physical layer instance (0-based)
*    SectorIndex    Index of the NOR physical sector to be queried.
*    pOff           [OUT] Byte offset of the NOR physical sector. Can be NULL.
*    pNumBytes      [OUT] Number of bytes in the NOR physical sector. Can be NULL.
*
*  Additional information
*    This function is a member of the NOR physical layer API
*    and is mandatory to be implemented by each NOR physical layer.
*    The NOR driver calls this function when it tries to determine
*    the position and the size of a physical sector.
*
*    The value returned via pOff and SectorIndex are relative to StartAddr
*    value specified in the call to FS_NOR_PHY_TYPE_CONFIGURE.
*/
typedef void FS_NOR_PHY_TYPE_GET_SECTOR_INFO(U8 Unit, unsigned int SectorIndex, U32 * pOff, U32 * pNumBytes);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_GET_NUM_SECTORS
*
*  Function description
*    Returns the number of NOR physical sectors.
*
*  Parameters
*    Unit       Index of the physical layer instance (0-based)
*
*  Return value
*    !=0      OK, the number of NOR physical sectors.
*    ==0      An error occurred.
*
*  Additional information
*    This function is a member of the NOR physical layer API
*    and is mandatory to be implemented by each NOR physical layer.
*    The value returned by this function is the number of physical
*    sectors located in the memory region specified by StartAddr
*    and NumBytes values specified in the call to FS_NOR_PHY_TYPE_CONFIGURE.
*/
typedef int FS_NOR_PHY_TYPE_GET_NUM_SECTORS(U8 Unit);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_CONFIGURE
*
*  Function description
*    Configures an instance of the NOR physical layer.
*
*  Parameters
*    Unit       Index of the physical layer instance (0-based)
*    BaseAddr   Address in system memory where the first byte of the
*               NOR flash device is mapped.
*    StartAddr  Address of the first byte of the NOR flash device to
*               be used for data storage.
*    NumBytes   Number of bytes to be used for data storage.
*
*  Additional information
*    This function is a member of the NOR physical layer API
*    and is mandatory to be implemented by each NOR physical layer.
*    FS_NOR_PHY_TYPE_CONFIGURE is called by the NOR driver during the
*    initialization of the file system.
*
*    For more information about the parameters refer to FS_NOR_Configure()
*    and FS_NOR_BM_Configure().
*/
typedef void FS_NOR_PHY_TYPE_CONFIGURE(U8 Unit, U32 BaseAddr, U32 StartAddr, U32 NumBytes);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_ON_SELECT_PHY
*
*  Function description
*    Prepares the access to the NOR flash device.
*
*  Parameters
*    Unit       Index of the physical layer instance (0-based)
*
*  Additional information
*    This function is a member of the NOR physical layer API
*    and is mandatory to be implemented by each NOR physical layer.
*    The NOR driver calls this function during the file system
*    initialization when a NOR physical layer is assigned to
*    a driver instance. Typically, FS_NOR_PHY_TYPE_ON_SELECT_PHY
*    allocates the memory requires for the instance of the
*    NOR physical layer and set up everything requires for
*    the access to NOR flash device.
*/
typedef void FS_NOR_PHY_TYPE_ON_SELECT_PHY(U8 Unit);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_DE_INIT
*
*  Function description
*    Frees the resources allocated by the NOR physical layer instance.
*
*  Parameters
*    Unit       Index of the physical layer instance (0-based)
*
*  Additional information
*    This function is a member of the NOR physical layer API.
*    It is mandatory to be implemented only by the NOR physical layers
*    that allocate dynamic memory. FS_NOR_PHY_TYPE_DE_INIT is called
*    by the NOR driver at the file system deinitialization when the application
*    calls FS_DeInit(). It has to free any dynamic memory  allocated
*    for the instance of specified the NOR physical layer.
*/
typedef void FS_NOR_PHY_TYPE_DE_INIT(U8 Unit);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_IS_SECTOR_BLANK
*
*  Function description
*    Verifies if a NOR physical sector is blank.
*
*  Parameters
*    Unit           Index of the physical layer instance (0-based)
*    SectorIndex    Index of the NOR physical sector to be checked.
*
*  Return value
*    !=0    NOR physical sector is blank.
*    ==0    NOR physical sector is not blank.
*
*  Additional information
*    This function is a member of the NOR physical layer API.
*    The implementation of this function is optional. By default
*    the NOR driver checks if a physical sector is blank by reading
*    the entire contents of the physical sector and by checking that
*    all bits are set to 1. If the hardware provides a faster method
*    to check if a physical sector is blank then this function can
*    implemented to use this method for increasing the write performance.
*    If the operation of the NOR driver has to be fail-safe then
*    FS_NOR_PHY_TYPE_IS_SECTOR_BLANK must be able to detect an
*    interrupted erase operation and in that case return that the
*    physical sector is not blank even when all the bits in the
*    physical sector read as 1.
*/
typedef int FS_NOR_PHY_TYPE_IS_SECTOR_BLANK(U8 Unit, unsigned int SectorIndex);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE_INIT
*
*  Function description
*    Initializes the instance of the NOR physical layer.
*
*  Parameters
*    Unit       Index of the physical layer instance (0-based)
*
*  Return value
*    ==0    OK, physical layer initialized.
*    !=0    An error occurred.
*
*  Additional information
*    This function is a member of the NOR physical layer API.
*    The implementation of this function is optional.
*    If implemented, FS_NOR_PHY_TYPE_INIT is called by the NOR driver
*    before the first access to the NOR flash device to identify the device,
*    read the device parameters, etc.
*/
typedef int FS_NOR_PHY_TYPE_INIT(U8 Unit);

/*********************************************************************
*
*       FS_NOR_PHY_TYPE
*
*  Description
*    NOR physical layer API.
*/
typedef struct {
  FS_NOR_PHY_TYPE_WRITE_OFF       * pfWriteOff;         // Writes data to the NOR flash device.
  FS_NOR_PHY_TYPE_READ_OFF        * pfReadOff;          // Reads data from the NOR flash device.
  FS_NOR_PHY_TYPE_ERASE_SECTOR    * pfEraseSector;      // Erases a NOR physical sector.
  FS_NOR_PHY_TYPE_GET_SECTOR_INFO * pfGetSectorInfo;    // Returns information about a NOR physical sector.
  FS_NOR_PHY_TYPE_GET_NUM_SECTORS * pfGetNumSectors;    // Returns the number of NOR physical sectors.
  FS_NOR_PHY_TYPE_CONFIGURE       * pfConfigure;        // Configures an instance of the NOR physical layer.
  FS_NOR_PHY_TYPE_ON_SELECT_PHY   * pfOnSelectPhy;      // Prepares the access to the NOR flash device.
  FS_NOR_PHY_TYPE_DE_INIT         * pfDeInit;           // Frees the resources allocated by the NOR physical layer instance.
  FS_NOR_PHY_TYPE_IS_SECTOR_BLANK * pfIsSectorBlank;    // Verifies if a NOR physical sector is blank.
  FS_NOR_PHY_TYPE_INIT            * pfInit;             // Initializes the instance of the NOR physical layer.
} FS_NOR_PHY_TYPE;

/*********************************************************************
*
*       FS_NOR_READ_CFI_CALLBACK
*
*  Function description
*    Reads CFI information from a NOR flash device.
*
*  Parameters
*    Unit       Index of the physical layer (0-based).
*    BaseAddr   Address in system memory where the NOR flash device is mapped.
*    Off        Byte offset to read from.
*    pData      [OUT] CFI information read
*    NumItems   Number of 16-bit values to be read.
*
*  Additional information
*    This is the type of the callback function invoked by CFI NOR physical
*    layers to read CFI information from NOR flash device. The CFI NOR physical
*    layers already have an internal function for reading the CFI information.
*    FS_NOR_CFI_SetReadCFICallback() can be used to register a callback function
*    that replaces the functionality of this internal function.
*/
typedef void (FS_NOR_READ_CFI_CALLBACK)(U8 Unit, U32 BaseAddr, U32 Off, U8 * pData, unsigned NumItems);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_INIT
*
*  Function description
*    Initializes the SPI hardware.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Return value
*    > 0      OK, frequency of the SPI clock in kHz.
*    ==0      An error occurred.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    This function is called by the NOR physical layer once and
*    before any other function of the hardware layer each time
*    the file system mounts the serial NOR flash.
*/
typedef int FS_NOR_HW_TYPE_SPI_INIT(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_ENABLE_CS
*
*  Function description
*    Enables the serial NOR flash device.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    The Chip Select (CS) signal is used to address a specific serial NOR
*    flash device connected via SPI. Enabling is equal to setting the CS
*    signal to a logic low level.
*/
typedef void FS_NOR_HW_TYPE_SPI_ENABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_DISABLE_CS
*
*  Function description
*    Disables the serial NOR flash device.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    The Chip Select (CS) signal is used to address a specific serial NOR
*    flash device connected via SPI. Disabling is equal to setting the CS
*    signal to a logic high level.
*/
typedef void FS_NOR_HW_TYPE_SPI_DISABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_READ
*
*  Function description
*    Transfers data from serial NOR flash device to MCU via 1 data line.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    pData      [OUT] Data transfered from the serial NOR flash device.
*    NumBytes   Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*/
typedef void FS_NOR_HW_TYPE_SPI_READ(U8 Unit, U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_WRITE
*
*  Function description
*    Transfers data from MCU to serial NOR flash device via 1 data line.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    pData      [IN] Data transfered to the serial NOR flash device.
*    NumBytes   Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*/
typedef void FS_NOR_HW_TYPE_SPI_WRITE(U8 Unit, const U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_READ_X2
*
*  Function description
*    Transfers data from serial NOR flash device to MCU via 2 data lines.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    pData      [OUT] Data transfered from the serial NOR flash device.
*    NumBytes   Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    FS_NOR_HW_TYPE_SPI_READ_X2 transfers 2 bits of data on each clock period.
*    Typically, the data is transferred via the DataOut (DO) and DataIn (DI)
*    signals of the serial NOR flash where the even numbered bits of a byte
*    are sent via the DI signal while the odd-numbered bits via the DO signal.
*
*    FS_NOR_HW_TYPE_SPI_READ_X2 is used only by the SFDP NOR physical layer.
*/
typedef void FS_NOR_HW_TYPE_SPI_READ_X2(U8 Unit, U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_WRITE_X2
*
*  Function description
*    Transfers data from MCU to serial NOR flash device via 2 data lines.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    pData      [IN] Data transfered to the serial NOR flash device.
*    NumBytes   Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    FS_NOR_HW_TYPE_SPI_WRITE_X2 transfers 2 bits of data on each clock period.
*    Typically, the data is transferred via the DataOut (DO) and DataIn (DI)
*    signals of the serial NOR flash where the even numbered bits of a byte
*    are sent via the DI signal while the odd-numbered bits via the DO signal.
*
*    FS_NOR_HW_TYPE_SPI_WRITE_X2 is used only by the SFDP NOR physical layer.
*/
typedef void FS_NOR_HW_TYPE_SPI_WRITE_X2(U8 Unit, const U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_READ_X4
*
*  Function description
*    Transfers data from serial NOR flash device to MCU via 4 data lines.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    pData      [OUT] Data transfered from the serial NOR flash device.
*    NumBytes   Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    FS_NOR_HW_TYPE_SPI_READ_X4 exchanges 4 bits of data on each clock period.
*    Typically, the data is transferred via the DataOut (DO), DataIn (DO),
*    Write Protect (WP), and Hold (H) signals of the serial NOR flash device.
*    A byte is transferred as follows:
*    * bits 0 and 4 via the DI signal
*    * bits 1 and 5 via the DO signal
*    * bits 2 and 6 via the WP signal
*    * bits 3 and 7 via the H signal.
*
*    FS_NOR_HW_TYPE_SPI_READ_X4 is used only by the SFDP NOR physical layer.
*/
typedef void FS_NOR_HW_TYPE_SPI_READ_X4(U8 Unit, U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_WRITE_X4
*
*  Function description
*    Transfers data from MCU to serial NOR flash device via 4 data lines.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    pData      [IN] Data transfered to the serial NOR flash device.
*    NumBytes   Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    FS_NOR_HW_TYPE_SPI_WRITE_X4 exchanges 4 bits of data on each clock period.
*    Typically, the data is transferred via the DataOut (DO), DataIn (DO),
*    Write Protect (WP), and Hold (H) signals of the serial NOR flash device.
*    A byte is transferred as follows:
*    * bits 0 and 4 via the DI signal
*    * bits 1 and 5 via the DO signal
*    * bits 2 and 6 via the WP signal
*    * bits 3 and 7 via the H signal.
*
*    FS_NOR_HW_TYPE_SPI_WRITE_X4 is used only by the SFDP NOR physical layer.
*/
typedef void FS_NOR_HW_TYPE_SPI_WRITE_X4(U8 Unit, const U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_DELAY
*
*  Function description
*    Blocks the execution for the specified time.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    ms         Number of milliseconds to block the execution.
*
*  Return value
*    ==0    OK, delay executed.
*    < 0    Functionality not supported.
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API.
*    The implementation of this function is optional. FS_NOR_HW_TYPE_SPI_DELAY
*    can block the execution for longer than the number of milliseconds
*    specified but not less than that. Typically, the function is implemented
*    using a delay function of the used RTOS if any. If the hardware layer
*    chooses not to implement FS_NOR_HW_TYPE_SPI_DELAY then the pfDelay
*    member of FS_NOR_HW_TYPE_SPI can be set to NULL or to the address of
*    a function that returns a negative value. If the function is not
*    implemented by the hardware layer then the NOR physical layer blocks
*    the execution by using a software loop. The number of software loops
*    is calculated based in the SPI clock frequency returned by FS_NOR_HW_TYPE_SPI_INIT.
*/
typedef int FS_NOR_HW_TYPE_SPI_DELAY(U8 Unit, U32 ms);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_LOCK
*
*  Function description
*    Requests exclusive access to SPI bus.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    The NOR physical layer calls this function to indicate that it
*    needs exclusive to access the NOR flash device via the SPI bus.
*    It is guaranteed that the NOR physical layer does not attempt to
*    exchange any data with the serial NOR flash device via the SPI bus
*    before calling this function first. It is also guaranteed that
*    FS_NOR_HW_TYPE_SPI_LOCK and FS_NOR_HW_TYPE_SPI_UNLOCK are called in pairs.
*    Typically, this function is used for synchronizing the access to SPI bus
*    when the SPI bus is shared between the serial NOR flash and other SPI devices.
*
*    A possible implementation would make use of an OS semaphore that is
*    acquired in FS_NOR_HW_TYPE_SPI_LOCK and released in FS_NOR_HW_TYPE_SPI_UNLOCK.
*/
typedef void FS_NOR_HW_TYPE_SPI_LOCK(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI_UNLOCK
*
*  Function description
*    Requests exclusive access to SPI bus.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Additional information
*    This function is a member of the SPI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    The NOR physical layer calls this function after it no longer needs
*    to access the serial NOR flash device via the SPI bus. It is guaranteed
*    that the NOR physical layer does not attempt to exchange any data with
*    the serial NOR flash device via the SPI bus before calling FS_NOR_HW_TYPE_SPI_LOCK.
*    It is also guaranteed that FS_NOR_HW_TYPE_SPI_UNLOCK and
*    FS_NOR_HW_TYPE_SPI_LOCK are called in pairs.
*
*    FS_NOR_HW_TYPE_SPI_UNLOCK and FS_NOR_HW_TYPE_SPI_LOCK can be used to
*    synchronize the access to the SPI bus when other devices than the
*    serial NAND flash are connected to it. A possible implementation would
*    make use of an OS semaphore that is acquired FS_NOR_HW_TYPE_SPI_LOCK
*    and released in FS_NOR_HW_TYPE_SPI_UNLOCK.
*/
typedef void FS_NOR_HW_TYPE_SPI_UNLOCK(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPI
*
*  Description
*    Hardware layer for serial NOR flash devices.
*/
typedef struct {
  FS_NOR_HW_TYPE_SPI_INIT       * pfInit;             // Initializes the SPI hardware.
  FS_NOR_HW_TYPE_SPI_ENABLE_CS  * pfEnableCS;         // Enables the serial NOR flash device.
  FS_NOR_HW_TYPE_SPI_DISABLE_CS * pfDisableCS;        // Disables the serial NOR flash device.
  FS_NOR_HW_TYPE_SPI_READ       * pfRead;             // Transfers data from serial NOR flash device to MCU via 1 data line.
  FS_NOR_HW_TYPE_SPI_WRITE      * pfWrite;            // Transfers data from MCU to serial NOR flash device via 1 data line.
  FS_NOR_HW_TYPE_SPI_READ_X2    * pfRead_x2;          // Transfers data from serial NOR flash device to MCU via 2 data lines.
  FS_NOR_HW_TYPE_SPI_WRITE_X2   * pfWrite_x2;         // Transfers data from MCU to serial NOR flash device via 2 data lines.
  FS_NOR_HW_TYPE_SPI_READ_X4    * pfRead_x4;          // Transfers data from serial NOR flash device to MCU via 4 data lines.
  FS_NOR_HW_TYPE_SPI_WRITE_X4   * pfWrite_x4;         // Transfers data from MCU to serial NOR flash device via 4 data lines.
  FS_NOR_HW_TYPE_SPI_DELAY      * pfDelay;            // Blocks the execution for the specified time.
  FS_NOR_HW_TYPE_SPI_LOCK       * pfLock;             // Requests exclusive access to SPI bus.
  FS_NOR_HW_TYPE_SPI_UNLOCK     * pfUnlock;           // Releases the exclusive access to SPI bus.
} FS_NOR_HW_TYPE_SPI;

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_INIT
*
*  Function description
*    Initializes the SPI hardware.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Return value
*    > 0      OK, frequency of the SPI clock in Hz.
*    ==0      An error occurred.
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    This function is called by the NOR physical layer once and
*    before any other function of the hardware layer each time
*    the file system mounts the serial NOR flash. FS_NOR_HW_TYPE_SPIFI_INIT
*    has to perform the initialization of clock signals, GPIO ports,
*    SPI controller, etc.
*/
typedef int FS_NOR_HW_TYPE_SPIFI_INIT(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_SET_CMD_MODE
*
*  Function description
*    Configures the hardware for direct access to serial NOR flash.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    The physical layer calls this function before it starts sending commands
*    directly to the serial NOR flash. Typically, the SPI hardware can operate
*    in two modes: command and memory. In command mode the physical layer can
*    access the serial NOR flash directly by sending commands to it.
*    In the memory mode the SPI hardware translates read accesses to an assigned
*    memory region to serial NOR flash read commands. In this way, the contents
*    of the contents of the serial NOR flash is mapped into this memory region.
*
*    If the command and memory mode are mutually exclusive,
*    FS_NOR_HW_TYPE_SPIFI_SET_CMD_MODE has to disable the memory mode.
*/
typedef void FS_NOR_HW_TYPE_SPIFI_SET_CMD_MODE(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_SET_MEM_MODE
*
*  Function description
*    Configures the hardware for access to serial NOR flash via system memory.
*
*  Parameters
*    Unit           Index of the hardware layer (0-based).
*    ReadCmd        Code of the command to be used for reading the data.
*    NumBytesAddr   Number of address bytes to be used for the read command.
*    NumBytesDummy  Number of dummy bytes to be sent after the read command.
*    BusWidth       Number of data lines to be used for the data transfer.
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API.
*    The implementation of this function is optional. If the hardware layer
*    does not implement this function then the data is read from the serial
*    NOR flash device using FS_NOR_HW_TYPE_SPIFI_READ_DATA.
*
*    This function is called by the physical layer when it no longer wants to send
*    commands directly to the serial NOR flash device. After the call to this
*    function the physical layer expects that the contents of the serial NOR flash
*    device is available in the system memory region assigned to SPI hardware
*    and that the data can be accessed by simple memory read operations.
*
*    If the command and memory mode are mutually exclusive,
*    then FS_NOR_HW_TYPE_SPIFI_SET_MEM_MODE has to disable the command mode.
*
*    BusWidth encodes the number of data lines to be used when transferring the
*    command, address and data. The value can be decoded using FS_BUSWIDTH_GET_CMD(),
*    FS_BUSWIDTH_GET_ADDR() and FS_BUSWIDTH_GET_DATA(). The dummy bytes are sent
*    using the same number of data lines as the address. For additional information
*    refer to \ref{SPI bus width decoding}
*/
typedef void FS_NOR_HW_TYPE_SPIFI_SET_MEM_MODE(U8 Unit, U8 ReadCmd, unsigned NumBytesAddr, unsigned NumBytesDummy, U16 BusWidth);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_EXEC_CMD
*
*  Function description
*    Sends a command to serial NOR flash that does not transfer any data.
*
*  Parameters
*    Unit           Index of the hardware layer (0-based).
*    Cmd            Code of the command to be sent.
*    BusWidth       Number of data lines to be used for sending the command.
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    Typically, the physical layer calls FS_NOR_HW_TYPE_SPIFI_EXEC_CMD
*    to enable or disable the write mode in the serial NOR flash, to set the
*    number of address bytes, etc. FS_NOR_HW_TYPE_SPIFI_EXEC_CMD is called by
*    the physical layer only with the SPI hardware configured in command mode.
*    FS_NOR_HW_TYPE_SPIFI_EXEC_CMD has to wait for the command to complete
*    before it returns.
*
*    BusWidth is not encoded as is the case with the other functions of this
*    physical layer but instead is stores the number of data lines.
*    That is FS_BUSWIDTH_GET_CMD() is not required for decoding the value.
*    Permitted values are 1, 2 and 4.
*/
typedef void FS_NOR_HW_TYPE_SPIFI_EXEC_CMD(U8 Unit, U8 Cmd, U8 BusWidth);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_READ_DATA
*
*  Function description
*    Transfers data from serial NOR flash to MCU.
*
*  Parameters
*    Unit           Index of the hardware layer (0-based).
*    Cmd            Code of the command to be sent.
*    pPara          [IN] Command parameters (address and dummy bytes). Can be NULL.
*    NumBytesPara   Total number of address and dummy bytes to be sent. Can be 0.
*    NumBytesAddr   Number of address bytes to be send. Can be 0.
*    pData          [OUT] Data read from the serial NOR flash device. Can be NULL.
*    NumBytesData   Number of bytes to read from the serial NOR flash device. Can be 0.
*    BusWidth       Number of data lines to be used for the data transfer.
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    This function is called with the SPI hardware in command mode. Typically,
*    the physical layer calls this function when it wants to read parameters
*    or status information from the serial NOR flash device.
*    FS_NOR_HW_TYPE_SPIFI_READ_DATA has to wait for the data transfer to complete
*    before it returns.
*
*    If FS_NOR_HW_TYPE_SPIFI_SET_MEM_MODE is provided then the physical layer
*    reads the contents of the serial NOR flash device via reads accesses to the
*    system memory. FS_NOR_HW_TYPE_SPIFI_READ_DATA is used by the physical
*    layer to read the contents of the serial NOR flash device if
*    FS_NOR_HW_TYPE_SPIFI_SET_MEM_MODE is not provided.
*
*    The first address byte is stored at *pPara. NumBytesAddr is always smaller
*    than or equal to NumBytesPara. If NumBytesAddr is equal to NumBytesPara then
*    no dummy bytes have to be sent. The number of dummy bytes to be sent can
*    be calculated as NumBytesPara - NumBytesAddr. If the hardware is sending
*    dummy cycles instead of bytes then the number of dummy bytes have to converted
*    to clock cycles by taking into account the number of data lines used for the
*    data transfer. The dummy bytes are sent via the same number of data lines
*    as the address.
*
*    BusWidth encodes the number of data lines to be used when transferring the
*    command, address and data. The value can be decoded using FS_BUSWIDTH_GET_CMD(),
*    FS_BUSWIDTH_GET_ADDR() and FS_BUSWIDTH_GET_DATA(). The dummy bytes are sent
*    using the same number of data lines as the address. For additional information
*    refer to \ref{SPI bus width decoding}
*/
typedef void FS_NOR_HW_TYPE_SPIFI_READ_DATA(U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, U8 * pData, unsigned NumBytesData, U16 BusWidth);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_WRITE_DATA
*
*  Function description
*    Transfers data from MCU to serial NOR flash.
*
*  Parameters
*    Unit           Index of the hardware layer (0-based).
*    Cmd            Code of the command to be sent.
*    pPara          [IN] Command parameters (address and dummy bytes). Can be NULL.
*    NumBytesPara   Total number of address and dummy bytes to be sent. Can be 0.
*    NumBytesAddr   Number of address bytes to be send. Can be 0.
*    pData          [IN] Data to be sent to the serial NOR flash device. Can be NULL.
*    NumBytesData   Number of bytes to be written the serial NOR flash device. Can be 0.
*    BusWidth       Number of data lines to be used for the data transfer.
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API
*    and it has to be implemented by any hardware layer.
*
*    This function is called with the SPI hardware in command mode. Typically, the
*    physical layer calls this function when it wants to modify the data in a page
*    of the serial NOR flash device or when to erase a NOR physical sector.
*    FS_NOR_HW_TYPE_SPIFI_WRITE_DATA has to wait for the data transfer to complete
*    before it returns.
*
*    The first address byte is stored at *pPara. NumBytesAddr is always smaller
*    than or equal to NumBytesPara. If NumBytesAddr is equal to NumBytesPara then
*    no dummy bytes have to be sent. The number of dummy bytes to be sent can
*    be calculated as NumBytesPara - NumBytesAddr. If the hardware is sending
*    dummy cycles instead of bytes then the number of dummy bytes have to converted
*    to clock cycles by taking into account the number of data lines used for the
*    data transfer. The dummy bytes are sent via the same number of data lines
*    as the address.
*
*    BusWidth encodes the number of data lines to be used when transferring the
*    command, address and data. The value can be decoded using FS_BUSWIDTH_GET_CMD(),
*    FS_BUSWIDTH_GET_ADDR() and FS_BUSWIDTH_GET_DATA(). The dummy bytes are sent
*    using the same number of data lines as the address. For additional information
*    refer to \ref{SPI bus width decoding}
*/
typedef void FS_NOR_HW_TYPE_SPIFI_WRITE_DATA(U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, const U8 * pData, unsigned NumBytesData, U16 BusWidth);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_POLL
*
*  Function description
*    Checks periodically the value of a status flag.
*
*  Parameters
*    Unit           Index of the hardware layer (0-based).
*    Cmd            Code of the command to be sent.
*    BitPos         Position of the bit to be checked.
*    BitValue       Value of the bit to wait for.
*    Delay          Number of clock cycles to wait between two queries.
*    TimeOut_ms     Maximum number of milliseconds to wait for the bit to be set.
*    BusWidth       Number of data lines to be used for the data transfer.
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    This function is called with the SPI hardware in command mode and has to send
*    periodically a command and to read one byte from the serial NOR flash device.
*    FS_NOR_HW_TYPE_SPIFI_POLL has to wait until the bit at BitPos in the response
*    returned by the serial NOR flash device is set to the value specified by BitValue.
*    A BitPos of 0 specifies the position of the least significant bit in the response.
*
*    BusWidth encodes the number of data lines to be used when transferring the
*    command and data. The value can be decoded using FS_BUSWIDTH_GET_CMD(),
*    and FS_BUSWIDTH_GET_DATA(). For additional information refer to
*    \ref{SPI bus width decoding}
*/
typedef int FS_NOR_HW_TYPE_SPIFI_POLL(U8 Unit, U8 Cmd, U8 BitPos, U8 BitValue, U32 Delay, U32 TimeOut_ms, U16 BusWidth);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_DELAY
*
*  Function description
*    Blocks the execution for the specified time.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*    ms         Number of milliseconds to block the execution.
*
*  Return value
*    ==0    OK, delay executed.
*    < 0    Functionality not supported.
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API.
*    The implementation of this function is optional. FS_NOR_HW_TYPE_SPIFI_DELAY
*    can block the execution for longer than the number of milliseconds
*    specified but not less than that. Typically, the function is implemented
*    using a delay function of the used RTOS if any. If the hardware layer
*    chooses not to implement FS_NOR_HW_TYPE_SPIFI_DELAY then the pfDelay
*    member of FS_NOR_HW_TYPE_SPIFI can be set to NULL or to the address of
*    a function that returns a negative value. If the function is not
*    implemented by the hardware layer then the NOR physical layer blocks
*    the execution by using a software loop. The number of software loops
*    is calculated based in the SPI clock frequency returned by FS_NOR_HW_TYPE_SPIFI_INIT.
*/
typedef int FS_NOR_HW_TYPE_SPIFI_DELAY(U8 Unit, U32 ms);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_LOCK
*
*  Function description
*    Requests exclusive access to SPI bus.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    The NOR physical layer calls this function to indicate that it
*    needs exclusive to access the NOR flash device via the SPI bus.
*    It is guaranteed that the NOR physical layer does not attempt to
*    exchange any data with the serial NOR flash device via the SPI bus
*    before calling this function first. It is also guaranteed that
*    FS_NOR_HW_TYPE_SPIFI_LOCK and FS_NOR_HW_TYPE_SPIFI_UNLOCK are called in pairs.
*    Typically, this function is used for synchronizing the access to SPI bus
*    when the SPI bus is shared between the serial NOR flash and other SPI devices.
*
*    A possible implementation would make use of an OS semaphore that is
*    acquired in FS_NOR_HW_TYPE_SPIFI_LOCK and released in FS_NOR_HW_TYPE_SPIFI_UNLOCK.
*/
typedef void FS_NOR_HW_TYPE_SPIFI_LOCK(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI_UNLOCK
*
*  Function description
*    Requests exclusive access to SPI bus.
*
*  Parameters
*    Unit       Index of the hardware layer (0-based).
*
*  Additional information
*    This function is a member of the SPIFI NOR hardware layer API.
*    The implementation of this function is optional.
*
*    The NOR physical layer calls this function when it no longer needs
*    to access the serial NOR flash device via the SPI bus. It is guaranteed
*    that the NOR physical layer does not attempt to exchange any data with
*    the serial NOR flash device via the SPI bus before calling FS_NOR_HW_TYPE_SPIFI_LOCK.
*    It is also guaranteed that FS_NOR_HW_TYPE_SPIFI_UNLOCK and
*    FS_NOR_HW_TYPE_SPIFI_LOCK are called in pairs.
*
*    FS_NOR_HW_TYPE_SPIFI_UNLOCK and FS_NOR_HW_TYPE_SPIFI_LOCK can be used to
*    synchronize the access to the SPI bus when other devices than the
*    serial NAND flash are connected to it. A possible implementation would
*    make use of an OS semaphore that is acquired FS_NOR_HW_TYPE_SPIFI_LOCK
*    and released in FS_NOR_HW_TYPE_SPIFI_UNLOCK.
*/
typedef void FS_NOR_HW_TYPE_SPIFI_UNLOCK(U8 Unit);

/*********************************************************************
*
*       FS_NOR_HW_TYPE_SPIFI
*
*  Description
*    Hardware layer for serial NOR flash devices that are memory-mapped.
*/
typedef struct {
  FS_NOR_HW_TYPE_SPIFI_INIT         * pfInit;         // Initializes the hardware.
  FS_NOR_HW_TYPE_SPIFI_SET_CMD_MODE * pfSetCmdMode;   // Configures the hardware for direct access to serial NOR flash.
  FS_NOR_HW_TYPE_SPIFI_SET_MEM_MODE * pfSetMemMode;   // Configures the hardware for access to serial NOR flash via system memory.
  FS_NOR_HW_TYPE_SPIFI_EXEC_CMD     * pfExecCmd;      // Sends a command to serial NOR flash that does not transfer any data.
  FS_NOR_HW_TYPE_SPIFI_READ_DATA    * pfReadData;     // Transfers data from serial NOR flash to MCU.
  FS_NOR_HW_TYPE_SPIFI_WRITE_DATA   * pfWriteData;    // Transfers data from MCU to serial NOR flash.
  FS_NOR_HW_TYPE_SPIFI_POLL         * pfPoll;         // Checks periodically the value of a status flag.
  FS_NOR_HW_TYPE_SPIFI_DELAY        * pfDelay;        // Blocks the execution for the specified time.
  FS_NOR_HW_TYPE_SPIFI_LOCK         * pfLock;         // Requests exclusive access to SPI bus.
  FS_NOR_HW_TYPE_SPIFI_UNLOCK       * pfUnlock;       // Releases the exclusive access to SPI bus.
} FS_NOR_HW_TYPE_SPIFI;

/*********************************************************************
*
*       FS_NOR_SPI_TYPE
*
*  Description
*    Operations on serial NOR devices
*/
typedef struct FS_NOR_SPI_TYPE FS_NOR_SPI_TYPE;   //lint -esym(9058, FS_NOR_SPI_TYPE) tag unused outside of typedefs. Rationale: this is an opaque data type.

/*********************************************************************
*
*       FS_NOR_SPI_DEVICE_LIST
*
*  Description
*    Defines a list of supported devices.
*/
typedef struct {
  U8                       NumDevices;
  const FS_NOR_SPI_TYPE ** ppDevice;
} FS_NOR_SPI_DEVICE_LIST;

/*********************************************************************
*
*       FS_NOR_SPI_DEVICE_PARA
*
*  Description
*    Defines the parameters of a serial NOR flash device.
*
*  Additional information
*    Flags is an bitwise-OR combination of \ref{Device operation flags}.
*/
typedef struct {
  U8  Id;                                         // Value to identify the serial NOR flash device. This is the 3rd byte in the response to READ ID (0x9F) command.
  U8  ldBytesPerSector;                           // Number of bytes in a physical sector as power of 2.
  U8  ldBytesPerPage;                             // Number of bytes in a page as a power of 2.
  U8  NumBytesAddr;                               // Number of address bytes. 4 for NOR flash devices with a capacity larger than 128 Mbit (16 Mbyte).
  U16 NumSectors;                                 // Total number of physical sectors in the device.
  U8  Flags;                                      // Additional functionality supported by device that requires special processing.
  U8  MfgId;                                      // Id of device manufacturer. This is the 1st byte in the response to READ ID (0x9F) command.
  U8  CmdWrite112;                                // Code of the command used to write the data to NOR flash via 2 data lines. The command itself and the address are transferred via data 1 line.
  U8  CmdWrite122;                                // Code of the command used to write the data to NOR flash via 2 data lines. The command itself is transferred via data 1 line while the address via 2 data lines.
  U8  CmdWrite114;                                // Code of the command used to write the data to NOR flash via 4 data lines. The command itself and the address are transferred via data 1 line.
  U8  CmdWrite144;                                // Code of the command used to write the data to NOR flash via 4 data lines. The command itself is transferred via data 1 line while the address via 4 data lines.
} FS_NOR_SPI_DEVICE_PARA;

/*********************************************************************
*
*       FS_NOR_SPI_DEVICE_PARA_LIST
*
*  Description
*    Defines a list of serial NOR flash device parameters.
*/
typedef struct {
  U8                             NumParas;        // Number of parameters
  const FS_NOR_SPI_DEVICE_PARA * pPara;           // List of parameters.
} FS_NOR_SPI_DEVICE_PARA_LIST;

/*********************************************************************
*
*       FS_NOR_FATAL_ERROR_INFO
*
*  Description
*    Information passed to callback function when a fatal error occurs.
*/
typedef struct {
  U8  Unit;                                       // Index of the driver that triggered the fatal error.
  U8  ErrorType;                                  // Type of the error that occurred.
  U32 ErrorPSI;                                   // Index of the physical sector in which the error occurred.
} FS_NOR_FATAL_ERROR_INFO;

/*********************************************************************
*
*       FS_NOR_ON_FATAL_ERROR_CALLBACK
*
*  Function description
*    The type of the callback function invoked by the NOR driver
*    when a fatal error occurs.
*
*  Parameters
*    pFatalErrorInfo    Information about the fatal error.
*
*  Return value
*    ==0  The NOR driver has to mark the NOR flash device as read only.
*    !=0  The NOR flash device has to remain writable.
*
*  Additional information
*    If the callback function returns a 0 the NOR driver marks the NOR flash
*    device as read-only and it remains in this state until the NOR flash device
*    is low-level formated. In this state, all further write operations
*    are rejected with an error by the NOR driver.
*
*    The application is responsible to handle the fatal error by for example
*    checking the consistency of the file system via FS_CheckDisk(). The callback
*    function is not allowed to invoke any other FS API functions therefore the
*    handling of the error has to be done after the FS API function that triggered
*    the error returns.
*/
typedef int FS_NOR_ON_FATAL_ERROR_CALLBACK(FS_NOR_FATAL_ERROR_INFO * pFatalErrorInfo);

/*********************************************************************
*
*       FS_NOR_CRC_HOOK_CALC_CRC8
*
*  Function description
*    The type of the callback function invoked by the NOR driver
*    to calculate an 8-bit CRC.
*
*  Parameters
*    pData      [IN] Data to be protected by CRC.
*    NumBytes   Number of bytes to be protected by CRC.
*    crc        Initial CRC value.
*
*  Return value
*    Calculated 8-bit CRC.
*
*  Additional information
*    This function calculates the CRC of the NumBytes of data
*    pointed to by pData. The type of polynomial is not relevant.
*/
typedef U8 FS_NOR_CRC_HOOK_CALC_CRC8(const U8 * pData, unsigned NumBytes, U8 crc);

/*********************************************************************
*
*       FS_NOR_CRC_HOOK_CALC_CRC16
*
*  Function description
*    The type of the callback function invoked by the NOR driver
*    to calculate a 16-bit CRC.
*
*  Parameters
*    pData      [IN] Data to be protected by CRC.
*    NumBytes   Number of bytes to be protected by CRC.
*    crc        Initial CRC value.
*
*  Return value
*    Calculated 16-bit CRC.
*
*  Additional information
*    This function calculates the CRC of the NumBytes of data
*    pointed to by pData. The type of polynomial is not relevant.
*    NumBytes is always an even number. pData is always aligned
*    to a 16-bit boundary.
*/
typedef U16 FS_NOR_CRC_HOOK_CALC_CRC16(const U8 * pData, unsigned NumBytes, U16 crc);

/*********************************************************************
*
*       FS_NOR_CRC_HOOK
*
*  Description
*    Callback functions for the CRC calculation.
*
*  Additional information
*    By default the Block Map NOR driver uses internal functions of
*    the file system for the CRC calculation. This structure can be
*    used to configure external CRC calculation callback functions
*    that for example use hardware CRC calculation units for increased
*    performance. The application can register the calculation callback
*    functions via FS_NOR_BM_SetCRCHook()
*/
typedef struct {
  FS_NOR_CRC_HOOK_CALC_CRC8  * pfCalcCRC8;          // Calculates an 8-bit CRC.
  FS_NOR_CRC_HOOK_CALC_CRC16 * pfCalcCRC16;         // Calculates a 16-bit CRC.
} FS_NOR_CRC_HOOK;

/*********************************************************************
*
*       FS_NOR_ECC_HOOK_CALC
*
*  Description
*    Callback function that calculates the ECC of a data block.
*
*  Parameters
*    pData      [IN] Data to be protected by ECC.
*    pECC       [OUT] Calculated parity check.
*
*  Additional information
*    The number of bytes in pData is specified by FS_NOR_ECC_HOOK::ldBytesPerBlock
*/
typedef void FS_NOR_ECC_HOOK_CALC(const U32 * pData, U8 * pECC);

/*********************************************************************
*
*       FS_NOR_ECC_HOOK_APPLY
*
*  Description
*    Callback function that corrects bit errors in a data block using ECC.
*
*  Parameters
*    pData      [IN] Data to be checked for bit errors using ECC.
*               [OUT] Data without bit errors.
*    pECC       [IN] Parity check read from storage.
*               [OUT] Parity check without bit errors.
*
*  Return value
*    >=0      OK, number of bit errors corrected.
*    < 0      Error, an uncorrectable bit error occurred.
*
*  Additional information
*    The number of bytes in pData is specified by FS_NOR_ECC_HOOK::ldBytesPerBlock
*/
typedef int FS_NOR_ECC_HOOK_APPLY(U32 * pData, U8 * pECC);

/*********************************************************************
*
*       FS_NOR_ECC_HOOK
*
*  Description
*    Callback functions for the ECC calculation.
*
*  Additional information
*    By default the Block Map NOR driver uses internal functions of
*    the file system for the ECC calculation. This structure can be
*    used to configure external ECC calculation callback functions
*    that for example use hardware ECC calculation units for increased
*    performance. The application can register the calculation callback
*    functions via FS_NOR_BM_SetECCHook()
*/
typedef struct {
  FS_NOR_ECC_HOOK_CALC  * pfCalc;                   // Calculates the ECC of the specified data.
  FS_NOR_ECC_HOOK_APPLY * pfApply;                  // Verifies and corrects bit errors using ECC.
  U8                      NumBitsCorrectable;       // Number of bits the ECC algorithm is able to correct in the data block and 4 bytes of spare area.
  U8                      ldBytesPerBlock;          // Number of bytes in the data block given as power of 2 value.
} FS_NOR_ECC_HOOK;

/*********************************************************************
*
*       Available physical layers for FS_NOR_Driver and FS_NOR_BM_Driver
*/
extern const FS_NOR_PHY_TYPE FS_NOR_PHY_CFI_1x16;            // 1 x 16-bit CFI compliant NOR flash
extern const FS_NOR_PHY_TYPE FS_NOR_PHY_CFI_2x16;            // 2 x 16-bit CFI compliant NOR flash
extern const FS_NOR_PHY_TYPE FS_NOR_PHY_ST_M25;              // ST M25P compliant Serial NOR flash
extern const FS_NOR_PHY_TYPE FS_NOR_PHY_SFDP;                // Serial NOR flash that supports Serial Flash Discoverable Parameters JDEDEC standard
extern const FS_NOR_PHY_TYPE FS_NOR_PHY_DSPI;                // Serial NOR flash that uses SPI in direct mode to access the NOR flash device (FS_NOR_PHY_ST_M25 and FS_NOR_PHY_SFDP)
extern const FS_NOR_PHY_TYPE FS_NOR_PHY_SPIFI;               // Serial NOR flash connected via a memory-mapped SPI interface

/*********************************************************************
*
*       Default HW layers for the NOR flash drivers.
*/
extern const FS_NOR_HW_TYPE_SPI FS_NOR_HW_ST_M25_Default;
extern const FS_NOR_HW_TYPE_SPI FS_NOR_HW_SFDP_Default;

/*********************************************************************
*
*       Lists of supported serial NOR devices
*/
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_All;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Default;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Micron;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Micron_x;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Micron_x2;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Spansion;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Microchip;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Winbond;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_ISSI;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Macronix;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_GigaDevice;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Cypress;
extern const FS_NOR_SPI_DEVICE_LIST FS_NOR_SPI_DeviceList_Adesto;

/*********************************************************************
*
*       CRC calculation modules
*/
extern const FS_NOR_CRC_HOOK FS_NOR_CRC_SW;             // Uses the emFile-internal software routines

/*********************************************************************
*
*       ECC calculation modules
*/
extern const FS_NOR_ECC_HOOK FS_NOR_ECC_SW_1bit_Man;    // Internal 1-bit ECC routines for the management data.
extern const FS_NOR_ECC_HOOK FS_NOR_ECC_SW_1bit_Data;   // Internal 1-bit ECC routines for the sector data.

/*********************************************************************
*
*       FS_NOR_Driver API functions
*/
void         FS_NOR_Configure                 (U8 Unit, U32 BaseAddr, U32 StartAddr, U32 NumBytes);
void         FS_NOR_ConfigureReserve          (U8 Unit, U8 pctToReserve);
int          FS_NOR_EraseDevice               (U8 Unit);
int          FS_NOR_FormatLow                 (U8 Unit);
int          FS_NOR_GetDiskInfo               (U8 Unit, FS_NOR_DISK_INFO * pDiskInfo);
void         FS_NOR_GetSectorInfo             (U8 Unit, U32 PhySectorIndex, FS_NOR_SECTOR_INFO * pSectorInfo);
#if FS_NOR_ENABLE_STATS
void         FS_NOR_GetStatCounters           (U8 Unit, FS_NOR_STAT_COUNTERS * pStat);
#endif // FS_NOR_ENABLE_STATS
int          FS_NOR_IsLLFormatted             (U8 Unit);
const void * FS_NOR_LogSector2PhySectorAddr   (U8 Unit, U32 LogSectorIndex);
int          FS_NOR_ReadOff                   (U8 Unit, U32 Off, void * pData, U32 NumBytes);
#if FS_NOR_ENABLE_STATS
void         FS_NOR_ResetStatCounters         (U8 Unit);
#endif // FS_NOR_ENABLE_STATS
#if FS_NOR_SKIP_BLANK_SECTORS
void         FS_NOR_SetBlankSectorSkip        (U8 Unit, U8 OnOff);
#endif // FS_NOR_SKIP_BLANK_SECTORS
#if FS_NOR_SUPPORT_VARIABLE_LINE_SIZE
void         FS_NOR_SetDeviceLineSize         (U8 Unit, U8 ldBytesPerLine);
void         FS_NOR_SetDeviceRewriteSupport   (U8 Unit, U8 OnOff);
#endif // FS_NOR_SUPPORT_VARIABLE_LINE_SIZE
#if FS_NOR_OPTIMIZE_DIRTY_CHECK
void         FS_NOR_SetDirtyCheckOptimization (U8 Unit, U8 OnOff);
#endif // FS_NOR_OPTIMIZE_DIRTY_CHECK
#if FS_NOR_VERIFY_ERASE
void         FS_NOR_SetEraseVerification      (U8 Unit, U8 OnOff);
#endif // FS_NOR_VERIFY_ERASE
void         FS_NOR_SetPhyType                (U8 Unit, const FS_NOR_PHY_TYPE * pPhyType);
void         FS_NOR_SetSectorSize             (U8 Unit, U16 SectorSize);
#if FS_NOR_VERIFY_WRITE
void         FS_NOR_SetWriteVerification      (U8 Unit, U8 OnOff);
#endif // FS_NOR_VERIFY_WRITE

/*********************************************************************
*
*       FS_NOR_BM_Driver API functions
*/
void         FS_NOR_BM_Configure              (U8 Unit, U32 BaseAddr, U32 StartAddr, U32 NumBytes);
#if FS_NOR_SUPPORT_CRC
int          FS_NOR_BM_DisableCRC             (void);
#endif // FS_NOR_SUPPORT_CRC
#if FS_NOR_SUPPORT_ECC
int          FS_NOR_BM_DisableECC             (U8 Unit);
#endif // FS_NOR_SUPPORT_ECC
#if FS_NOR_SUPPORT_CRC
int          FS_NOR_BM_EnableCRC              (void);
#endif // FS_NOR_SUPPORT_CRC
#if FS_NOR_SUPPORT_ECC
int          FS_NOR_BM_EnableECC              (U8 Unit);
#endif // FS_NOR_SUPPORT_ECC
int          FS_NOR_BM_EraseDevice            (U8 Unit);
int          FS_NOR_BM_ErasePhySector         (U8 Unit, U32 PhySectorIndex);
int          FS_NOR_BM_FormatLow              (U8 Unit);
int          FS_NOR_BM_GetDiskInfo            (U8 Unit, FS_NOR_BM_DISK_INFO * pDiskInfo);
int          FS_NOR_BM_GetSectorInfo          (U8 Unit, U32 PhySectorIndex, FS_NOR_BM_SECTOR_INFO * pSectorInfo);
#if FS_NOR_ENABLE_STATS
void         FS_NOR_BM_GetStatCounters        (U8 Unit, FS_NOR_BM_STAT_COUNTERS * pStat);
#endif // FS_NOR_ENABLE_STATS
#if FS_NOR_SUPPORT_CRC
int          FS_NOR_BM_IsCRCEnabled           (void);
#endif // FS_NOR_SUPPORT_CRC
#if FS_NOR_SUPPORT_ECC
int          FS_NOR_BM_IsECCEnabled           (U8 Unit);
#endif // FS_NOR_SUPPORT_ECC
int          FS_NOR_BM_IsLLFormatted          (U8 Unit);
int          FS_NOR_BM_Mount                  (U8 Unit, FS_NOR_BM_MOUNT_INFO * pMountInfo);
int          FS_NOR_BM_ReadOff                (U8 Unit, void * pData, U32 Off, U32 NumBytes);
#if FS_NOR_ENABLE_STATS
void         FS_NOR_BM_ResetStatCounters      (U8 Unit);
#endif // FS_NOR_ENABLE_STATS
#if FS_NOR_SKIP_BLANK_SECTORS
void         FS_NOR_BM_SetBlankSectorSkip     (U8 Unit, U8 OnOff);
#endif // FS_NOR_SKIP_BLANK_SECTORS
#if FS_NOR_SUPPORT_VARIABLE_BYTE_ORDER
int          FS_NOR_BM_SetByteOrderBE         (void);
int          FS_NOR_BM_SetByteOrderLE         (void);
#endif // FS_NOR_SUPPORT_VARIABLE_BYTE_ORDER
#if FS_NOR_SUPPORT_CRC
int          FS_NOR_BM_SetCRCHook             (const FS_NOR_CRC_HOOK * pCRCHook);
#endif // FS_NOR_SUPPORT_CRC
#if FS_NOR_SUPPORT_ECC
int          FS_NOR_BM_SetECCHook             (U8 Unit, const FS_NOR_ECC_HOOK * pECCHookMan, const FS_NOR_ECC_HOOK * pECCHookData);
#endif // FS_NOR_SUPPORT_ECC
#if FS_NOR_SUPPORT_VARIABLE_LINE_SIZE
int          FS_NOR_BM_SetDeviceLineSize      (U8 Unit, U8 ldBytesPerLine);
int          FS_NOR_BM_SetDeviceRewriteSupport(U8 Unit, U8 OnOff);
#endif // FS_NOR_SUPPORT_VARIABLE_LINE_SIZE
#if FS_NOR_VERIFY_ERASE
void         FS_NOR_BM_SetEraseVerification   (U8 Unit, U8 OnOff);
#endif // FS_NOR_VERIFY_ERASE
#if FS_NOR_SUPPORT_FAIL_SAFE_ERASE
int          FS_NOR_BM_SetFailSafeErase       (U8 Unit, U8 OnOff);
#endif // FS_NOR_SUPPORT_FAIL_SAFE_ERASE
void         FS_NOR_BM_SetInvalidSectorError  (U8 Unit, U8 OnOff);
void         FS_NOR_BM_SetMaxEraseCntDiff     (U8 Unit, U32 EraseCntDiff);
void         FS_NOR_BM_SetNumWorkBlocks       (U8 Unit, unsigned NumWorkBlocks);
void         FS_NOR_BM_SetOnFatalErrorCallback(FS_NOR_ON_FATAL_ERROR_CALLBACK * pfOnFatalError);
void         FS_NOR_BM_SetPhyType             (U8 Unit, const FS_NOR_PHY_TYPE * pPhyType);
void         FS_NOR_BM_SetSectorSize          (U8 Unit, unsigned SectorSize);
void         FS_NOR_BM_SetUsedSectorsErase    (U8 Unit, U8 OnOff);
#if FS_NOR_VERIFY_WRITE
void         FS_NOR_BM_SetWriteVerification   (U8 Unit, U8 OnOff);
#endif // FS_NOR_VERIFY_WRITE
void         FS_NOR_BM_SuspendWearLeveling    (U8 Unit);
int          FS_NOR_BM_WriteOff               (U8 Unit, const void * pData, U32 Off, U32 NumBytes);

/*********************************************************************
*
*       CFI physical layer API functions
*/
void         FS_NOR_CFI_SetAddrGap         (U8 Unit, U32 StartAddr, U32 NumBytes);
void         FS_NOR_CFI_SetReadCFICallback (U8 Unit, FS_NOR_READ_CFI_CALLBACK * pReadCFI);

/*********************************************************************
*
*       ST M25 physical layer API functions
*/
void         FS_NOR_SPI_Configure          (U8 Unit, U32 SectorSize, U16 NumSectors);
void         FS_NOR_SPI_SetPageSize        (U8 Unit, U16 BytesPerPage);
void         FS_NOR_SPI_SetHWType          (U8 Unit, const FS_NOR_HW_TYPE_SPI * pHWType);
void         FS_NOR_SPI_ReadDeviceId       (U8 Unit, U8 * pId, unsigned NumBytes);
#if FS_NOR_MAX_NUM_DEVICES
void         FS_NOR_SPI_AddDevice          (const FS_NOR_SPI_DEVICE_PARA * pDevicePara);
#endif // FS_NOR_MAX_NUM_DEVICES

/*********************************************************************
*
*       SFDP physical layer API functions
*/
void         FS_NOR_SFDP_Allow2bitMode     (U8 Unit, U8 OnOff);
void         FS_NOR_SFDP_Allow4bitMode     (U8 Unit, U8 OnOff);
void         FS_NOR_SFDP_Configure         (U8 Unit, U32 StartAddr, U32 NumBytes);
void         FS_NOR_SFDP_SetHWType         (U8 Unit, const FS_NOR_HW_TYPE_SPI     * pHWType);
void         FS_NOR_SFDP_SetDeviceList     (U8 Unit, const FS_NOR_SPI_DEVICE_LIST * pDeviceList);
void         FS_NOR_SFDP_SetSectorSize     (U8 Unit, U32 BytesPerSector);
void         FS_NOR_SFDP_SetDeviceParaList (U8 Unit, const FS_NOR_SPI_DEVICE_PARA_LIST * pDeviceParaList);

/*********************************************************************
*
*       SPIFI physical layer API functions
*/
void         FS_NOR_SPIFI_Allow2bitMode    (U8 Unit, U8 OnOff);
void         FS_NOR_SPIFI_Allow4bitMode    (U8 Unit, U8 OnOff);
void         FS_NOR_SPIFI_SetHWType        (U8 Unit, const FS_NOR_HW_TYPE_SPIFI        * pHWType);
void         FS_NOR_SPIFI_SetDeviceList    (U8 Unit, const FS_NOR_SPI_DEVICE_LIST      * pDeviceList);
void         FS_NOR_SPIFI_SetSectorSize    (U8 Unit, U32 BytesPerSector);
void         FS_NOR_SPIFI_SetDeviceParaList(U8 Unit, const FS_NOR_SPI_DEVICE_PARA_LIST * pDeviceParaList);

/*********************************************************************
*
*       DSPI physical layer API functions
*/
void         FS_NOR_DSPI_SetHWType         (U8 Unit, const FS_NOR_HW_TYPE_SPI * pHWType);

/*********************************************************************
*
*       RAMDISK driver
*/
void FS_RAMDISK_Configure(U8 Unit, void * pData, U16 BytesPerSector, U32 NumSectors);

/*********************************************************************
*
*       MMC/SD driver
*/

/*********************************************************************
*
*       Storage card types
*
*  Description
*    Type of storage devices supported by the MMC/SD driver.
*/
#define FS_MMC_CARD_TYPE_UNKNOWN                  0     // The driver was not able to identify the device.
#define FS_MMC_CARD_TYPE_MMC                      1     // The storage device conforms to MMC specification.
#define FS_MMC_CARD_TYPE_SD                       2     // The storage device conforms to SD specification.

/*********************************************************************
*
*       Data transfer types
*
*  Description
*    Types of exchanging data with an MMC/SD storage device.
*/
#define FS_MMC_ACCESS_MODE_UNKNOWN                0u    // Invalid access mode
#define FS_MMC_ACCESS_MODE_DS                     1u    // Clock frequency up to 25 MHz for SD cards and 26 MHz for MMC devices (Default speed).
#define FS_MMC_ACCESS_MODE_HS                     2u    // Clock frequency up to 50 MHz for SD cards and 52 MHz for MMC devices (High speed).
#define FS_MMC_ACCESS_MODE_SDR50                  3u    // Clock frequency up to 100 MHz with 1.8 V signaling. For SD cards only.
#define FS_MMC_ACCESS_MODE_DDR50                  4u    // Clock frequency up to 50 MHz with 1.8 V signaling and data exchanged on both clock edges. For SD cards only.
#define FS_MMC_ACCESS_MODE_SDR104                 5u    // Clock frequency up to 208 MHz with 1.8 V signaling. For SD cards only.
#define FS_MMC_ACCESS_MODE_HS_DDR                 6u    // Clock frequency up to 52 MHz with 1.8 V and 3.3V signaling and data exchanged on both clock edges. For MMC devices only.
#define FS_MMC_ACCESS_MODE_HS200                  7u    // Clock frequency up to 200 MHz with 1.8 V signaling. For MMC devices only.
#define FS_MMC_ACCESS_MODE_HS400                  8u    // Clock frequency up to 200 MHz with 1.8 V signaling and data exchanged on both clock edges. For MMC devices only.

/*********************************************************************
*
*       Driver strength types
*
*  Description
*    Types of output driver strengths of an MMC/SD storage device.
*/
#define FS_MMC_DRIVER_STRENGTH_TYPE_B             0u    // 50 Ohm impedance and x1 driving capability. This is the default value.
#define FS_MMC_DRIVER_STRENGTH_TYPE_A             1u    // 33 Ohm impedance and x1.5 driving capability.
#define FS_MMC_DRIVER_STRENGTH_TYPE_C             2u    // 66 Ohm impedance and x0.75 driving capability.
#define FS_MMC_DRIVER_STRENGTH_TYPE_D             3u    // 100 Ohm impedance and x0.5 driving capability.
#define FS_MMC_DRIVER_STRENGTH_TYPE_E             4u    // 40 Ohm impedance and x1.2 driving capability. For eMMC devices only.

/*********************************************************************
*
*       Card mode error codes
*
*  Description
*    Values that indicate the result of an operation.
*/
#define FS_MMC_CARD_NO_ERROR                      0     // Success.
#define FS_MMC_CARD_RESPONSE_TIMEOUT              1     // No response received.
#define FS_MMC_CARD_RESPONSE_CRC_ERROR            2     // CRC error in response detected.
#define FS_MMC_CARD_READ_TIMEOUT                  3     // No data received.
#define FS_MMC_CARD_READ_CRC_ERROR                4     // CRC error in received data detected.
#define FS_MMC_CARD_WRITE_CRC_ERROR               5     // Card detected an CRC error in the received data.
#define FS_MMC_CARD_RESPONSE_GENERIC_ERROR        6     // Start bit, end bit or command index error.
#define FS_MMC_CARD_READ_GENERIC_ERROR            7     // An error occurred during while receiving data from card.
#define FS_MMC_CARD_WRITE_GENERIC_ERROR           8     // An error occurred during while sending data to card.
#define FS_MMC_CARD_TUNING_ERROR                  9     // An error occurred during the tuning procedure.
#define FS_MMC_CARD_VOLTAGE_SWITCH_ERROR          10    // An error occurred during the voltage switch procedure.

/*********************************************************************
*
*       Card mode response formats
*
*  Description
*    Response types returned by different commands.
*/
#define FS_MMC_RESPONSE_FORMAT_NONE               0                           // No response expected.
#define FS_MMC_RESPONSE_FORMAT_R1                 1                           // Card status (48-bit large)
#define FS_MMC_RESPONSE_FORMAT_R2                 2                           // CID or CSD register (128-bit large)
#define FS_MMC_RESPONSE_FORMAT_R3                 3                           // OCR register (48-bit large)
#define FS_MMC_RESPONSE_FORMAT_R6                 FS_MMC_RESPONSE_FORMAT_R1   // Published RCA response (48-bit large)
#define FS_MMC_RESPONSE_FORMAT_R7                 FS_MMC_RESPONSE_FORMAT_R1   // Card interface condition (48-bit large)

/*********************************************************************
*
*       Card mode command flags
*
*  Description
*    Additional options for the executed command.
*/
#define FS_MMC_CMD_FLAG_DATATRANSFER              (1uL << 0)      // Command that exchanges data with the card.
#define FS_MMC_CMD_FLAG_WRITETRANSFER             (1uL << 1)      // Command that sends data to card. Implies FS_MMC_CMD_FLAG_DATATRANSFER.
#define FS_MMC_CMD_FLAG_SETBUSY                   (1uL << 2)      // Command that expects an R1b command.
#define FS_MMC_CMD_FLAG_INITIALIZE                (1uL << 3)      // Specifies that the initialization delay has to be performed. According to SD specification this is the maximum of 1 millisecond, 74 clock cycles and supply ramp up time.
#define FS_MMC_CMD_FLAG_USE_SD4MODE               (1uL << 4)      // Command that transfers the data via 4 data lines.
#define FS_MMC_CMD_FLAG_STOP_TRANS                (1uL << 5)      // Command that stops a data transfer (typically CMD12)
#define FS_MMC_CMD_FLAG_WRITE_BURST_REPEAT        (1uL << 6)      // Specifies that the same sector data is written to consecutive sector indexes.
#define FS_MMC_CMD_FLAG_USE_MMC8MODE              (1uL << 7)      // Command that transfers the data via 8 data lines (MMC only).
#define FS_MMC_CMD_FLAG_NO_CRC_CHECK              (1uL << 8)      // CRC verification has to be disabled for the command. Typically used with the MMC bus test commands.
#define FS_MMC_CMD_FLAG_WRITE_BURST_FILL          (1uL << 9)      // Specifies that the 32-bit value is used to fill the contents of consecutive sector indexes.
#define FS_MMC_CMD_FLAG_SWITCH_VOLTAGE            (1uL << 10)     // Specifies that the the signaling of the I/O lines has to be changed to 1.8 V

/*********************************************************************
*
*       Card mode clock flags
*
*  Description
*    Additional options for the clock generation.
*
*  Additional information
*    The FS_MMC_CLK_FLAG_STROBE_MODE and FS_MMC_CLK_FLAG_ENHANCED_STROBE
*    flags are used to specify how to sample the data coming from an eMMC
*    device in HS400 access mode. If FS_MMC_CLK_FLAG_STROBE_MODE is set
*    and FS_MMC_CLK_FLAG_ENHANCED_STROBE and cleared then the received
*    data is sampled using the strobe signal while the command response
*    using the clock supplied to eMMC device. If FS_MMC_CLK_FLAG_STROBE_MODE
*    and FS_MMC_CLK_FLAG_ENHANCED_STROBE are both set then the received
*    data as well as the command response are sampled using the strobe signal.
*/
#define FS_MMC_CLK_FLAG_DDR_MODE                  (1uL << 0)      // Specifies that the data is exchanged on both clock edges.
#define FS_MMC_CLK_FLAG_STROBE_MODE               (1uL << 1)      // Specifies that the received data is sampled using a strobe signal generated by the storage device.
#define FS_MMC_CLK_FLAG_ENHANCED_STROBE           (1uL << 2)      // Specifies that the received response and data are sampled using a strobe signal generated by the storage device.

/*********************************************************************
*
*       FS_MMC_CARD_ID
*
*  Description
*    Information about the storage card.
*
*  Additional information
*    The card identification can be read via FS_MMC_GetCardId()
*    or FS_MMC_CM_GetCardId()
*/
typedef struct {
  U8 aData[16];           // Identification information as returned by the SD card or MMC device as response to CMD10 command.
} FS_MMC_CARD_ID;

/*********************************************************************
*
*       FS_MMC_STAT_COUNTERS
*
*  Description
*    Statistical counters maintained by the MMC/SD driver
*/
typedef struct {
  U32 WriteSectorCnt;     // Number of logical sectors read.
  U32 WriteErrorCnt;      // Number of write errors.
  U32 ReadSectorCnt;      // Number of logical sectors written.
  U32 ReadErrorCnt;       // Number of read errors.
  U32 CmdExecCnt;         // Number of commands executed.
} FS_MMC_STAT_COUNTERS;

/*********************************************************************
*
*       FS_MMC_CARD_INFO
*
*  Description
*    Information about the storage device.
*
*  Additional information
*    For a list of permitted values for CardType refer to \ref{Storage card types}.
*    BusWidth can take one of these values: 1, 4 or 8. BytesPerSector is typically 512 bytes.
*    For a list of permitted values for AccessMode refer to \ref{Data transfer types}.
*    IsHighSpeedMode is deprecated and AccessMode can be used instead.
*    IsHighSpeedMode set to 1 is equivalent to AccessMode set to FS_MMC_ACCESS_MODE_HS.
*/
typedef struct {
  U8  CardType;           // Type of the storage card.
  U8  BusWidth;           // Number of data lines used for the data transfer.
  U8  IsWriteProtected;   // Set to 1 if the card is write protected.
  U8  IsHighSpeedMode;    // Set to 1 if the card operates in the high-speed mode.
  U16 BytesPerSector;     // Number of bytes in a sector.
  U32 NumSectors;         // Total number of sectors on the card.
  U32 ClockFreq;          // Frequency of the clock supplied to storage device in Hz.
  U16 VoltageLevel;       // Voltage level of I/O lines.
  U8  AccessMode;         // Type of data transfer.
  U8  DriverStrength;     // Actual output driving strength of the MMC/SD device.
} FS_MMC_CARD_INFO;

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_ENABLE_CS
*
*  Function description
*    Enables the card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The Chip Select (CS) signal is used to address a specific MMC or SD
*    card connected via SPI. Enabling the card is equal to setting the CS
*    signal to a logic low level.
*/
typedef void FS_MMC_HW_TYPE_SPI_ENABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_DISABLE_CS
*
*  Function description
*    Disables the card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The Chip Select (CS) signal is used to address a specific MMC or SD
*    card connected via SPI. Disabling the card is equal to setting the CS
*    signal to a logic high level.
*/
typedef void FS_MMC_HW_TYPE_SPI_DISABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_IS_PRESENT
*
*  Function description
*    Checks if the card is present.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    ==FS_MEDIA_STATE_UNKNOWN   The presence state of the card is unknown.
*    ==FS_MEDIA_NOT_PRESENT     The card is present.
*    ==FS_MEDIA_IS_PRESENT      The card is not present.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    Typically, the card presence detection is implemented via a dedicated
*    Card Detect (CD) signal. This signal is connected to a switch inside
*    the card slot that changes its state each time the card is removed
*    or inserted. If the hardware does not provide such signal the function
*    has to return FS_MEDIA_STATE_UNKNOWN.
*/
typedef int FS_MMC_HW_TYPE_SPI_IS_PRESENT(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_IS_WRITE_PROTECTED
*
*  Function description
*    Checks if the card is write protected.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    ==0     The card data can be modified.
*    !=0     The card data cannot be modified.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    Typically, the card protection status is implemented via a dedicated
*    Write Protected (WP) signal. This signal is connected to a switch inside
*    the card slot that reflects the status of the write protect switch
*    found on SD cards. If the hardware does not provide such signal the
*    function has to return 0.
*
*    MMC and micro SD cards do not have a write protection switch.
*    Please note that the write protect switch does not really protect the
*    data on an SD card form being modified. It is merely an indication
*    that the data has to be protected. It is the responsibility of the host
*    MCU to respect the status of this switch.
*/
typedef int FS_MMC_HW_TYPE_SPI_IS_WRITE_PROTECTED(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_SET_MAX_SPEED
*
*  Function description
*    Configures the clock frequency supplied to the card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    MaxFreq      Maximum allowed frequency of the SPI clock in kHz.
*
*  Return value
*    !=0     The actual clock frequency in kHz.
*    ==0     An error occurred.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The hardware layer is allowed to set a clock frequency smaller
*    than the value specified via MaxFreq but never greater than that.
*    The SPI MMC/SD driver calls this function at least two times during
*    the initialization of the MMC or SD card:
*    \item Before the initialization starts to set the clock frequency to 400 kHz.
*    \item After the card identification to set the standard clock frequency
*          which is 25 MHz for SD cards or 20 MHz for MMC cards.
*
*    The function has to return the actual configured clock frequency.
*    If the precise frequency is unknown such as for implementation using
*    I/O port "bit-banging", the return value has to be less than the specified
*    frequency. This may lead to longer timeout values but is in general
*    does not cause any problems. The SPI MMC/SD driver uses the returned
*    value to calculate timeout values.
*/
typedef U16 FS_MMC_HW_TYPE_SPI_SET_MAX_SPEED(U8 Unit, U16 MaxFreq);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_SET_VOLTAGE
*
*  Function description
*    Configures the voltage level of the card power supply.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    VMin         Minimum supply voltage level in mV.
*    VMax         Maximum supply voltage level in mV.
*
*  Return value
*    ==1    Card slot supports the voltage range.
*    ==0    Card slot does not support the voltage range.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    By default, all cards work with the initial voltage of 3.3V.
*    If hardware layer has to save power, then the supply voltage can
*    be adjusted within the specified range.
*/
typedef int FS_MMC_HW_TYPE_SPI_SET_VOLTAGE(U8 Unit, U16 VMin, U16 VMax);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_READ
*
*  Function description
*    Transfers data from card to MCU.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [OUT] Data transfered from card.
*    NumBytes     Number of bytes to be transfered.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer.
*    FS_MMC_HW_TYPE_SPI_READ does not have to be implemented if
*    the hardware layer provides an implementation for FS_MMC_HW_TYPE_SPI_READ_EX.
*
*    The data is received via the DIN (MISO) signal of the SPI interface with
*    the data being sampled on the rising edge of the clock signal. According
*    to the SD specification the DOUT (MOSI) signal must be driven to logic high
*    during the data transfer, otherwise the SD card will not work properly.
*
*    The SPI MMC/SD driver calls this function only when the card is
*    enabled that is with the CS signal at logic low level.
*/
typedef void FS_MMC_HW_TYPE_SPI_READ(U8 Unit, U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_WRITE
*
*  Function description
*    Transfers data from MCU to card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [IN] Data to be transfered to card.
*    NumBytes     Number of bytes to be transfered to card.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer.
*    FS_MMC_HW_TYPE_SPI_WRITE does not have to be implemented if
*    the hardware layer provides an implementation for FS_MMC_HW_TYPE_SPI_WRITE_EX.
*
*    The data is sent via the DOUT (MOSI) signal of the SPI interface with
*    the data being changed on the falling edge of the clock signal.
*
*    The SPI MMC/SD driver also calls this function when the card is not
*    enabled that is with the CS signal at logic low in order to generate
*    empty cycles.
*/
typedef void FS_MMC_HW_TYPE_SPI_WRITE(U8 Unit, const U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_READ_EX
*
*  Function description
*    Transfers data from card to MCU.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [OUT] Data transfered from card.
*    NumBytes     Number of bytes to be transfered.
*
*  Return value
*    ==0    OK, data transferred.
*    !=0    An error occurred.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer.
*    FS_MMC_HW_TYPE_SPI_READ_EX does not have to be implemented if
*    the hardware layer provides an implementation for FS_MMC_HW_TYPE_SPI_READ.
*    It provides the same functionality as FS_MMC_HW_TYPE_SPI_READ
*    with the difference that it can report the result of the operation
*    to the SPI MMC/SD driver.
*
*    The SPI MMC/SD driver calls this function only when the card is
*    enabled that is with the CS signal at logic low level.
*/
typedef int FS_MMC_HW_TYPE_SPI_READ_EX(U8 Unit, U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_WRITE_EX
*
*  Function description
*    Transfers data from MCU to card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [IN] Data to be transfered to card.
*    NumBytes     Number of bytes to be transfered to card.
*
*  Return value
*    ==0    OK, data transferred.
*    !=0    An error occurred.
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer.
*    FS_MMC_HW_TYPE_SPI_WRITE_EX does not have to be implemented if
*    the hardware layer provides an implementation for FS_MMC_HW_TYPE_SPI_WRITE.
*    It provides the same functionality as FS_MMC_HW_TYPE_SPI_WRITE
*    with the difference that it can report the result of the operation
*    to the SPI MMC/SD driver.
*
*    The SPI MMC/SD driver also calls this function when the card is not
*    enabled that is with the CS signal at logic low in order to generate
*    empty cycles.
*/
typedef int FS_MMC_HW_TYPE_SPI_WRITE_EX(U8 Unit, const U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_LOCK
*
*  Function description
*    Requires exclusive access to SPI bus.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer.
*    The implementation of this function is optional.
*
*    The SPI MMC/SD driver calls this function to indicate that it
*    needs exclusive to access the card via the SPI bus.
*    It is guaranteed that the SPI MMC/SD driver does not attempt to
*    exchange any data with the card via the SPI bus before calling
*    this function first. It is also guaranteed that FS_MMC_HW_TYPE_SPI_LOCK
*    and FS_MMC_HW_TYPE_SPI_UNLOCK are called in pairs.
*    Typically, this function is used for synchronizing the access to SPI bus
*    when the SPI bus is shared between the card and other SPI devices.
*
*    A possible implementation would make use of an OS semaphore that is
*    acquired in FS_MMC_HW_TYPE_SPI_LOCK and released in FS_MMC_HW_TYPE_SPI_UNLOCK.
*
*    FS_MMC_HW_TYPE_SPI_LOCK is called only when the SPI MMC/SD driver
*    is compiled with FS_MMC_SUPPORT_LOCKINGset to 1.
*/
typedef void FS_MMC_HW_TYPE_SPI_LOCK(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI_UNLOCK
*
*  Function description
*    Releases exclusive access to SPI bus.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the SPI MMC/SD hardware layer.
*    The implementation of this function is optional.
*
*    The SPI MMC/SD driver calls this function when it no longer needs
*    to access the card via the SPI bus. It is guaranteed that the SPI MMC/SD
*    driver does not attempt to exchange any data with the card via the SPI bus
*    before calling FS_MMC_HW_TYPE_SPI_LOCK.
*    It is also guaranteed that FS_MMC_HW_TYPE_SPI_UNLOCK and
*    FS_MMC_HW_TYPE_SPI_LOCK are called in pairs.
*
*    FS_MMC_HW_TYPE_SPI_UNLOCK and FS_MMC_HW_TYPE_SPI_LOCK can be used to
*    synchronize the access to the SPI bus when other devices than the
*    serial NAND flash are connected to it. A possible implementation would
*    make use of an OS semaphore that is acquired FS_MMC_HW_TYPE_SPI_LOCK
*    and released in FS_MMC_HW_TYPE_SPI_UNLOCK.
*
*    FS_MMC_HW_TYPE_SPI_LOCK is called only when the SPI MMC/SD driver
*    is compiled with FS_MMC_SUPPORT_LOCKINGset to 1.
*/
typedef void FS_MMC_HW_TYPE_SPI_UNLOCK(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_SPI
*
*  Description
*    Hardware layer for the SPI MMC/SD driver.
*/
typedef struct {
  FS_MMC_HW_TYPE_SPI_ENABLE_CS          * pfEnableCS;           // Enables the card.
  FS_MMC_HW_TYPE_SPI_DISABLE_CS         * pfDisableCS;          // Disables the card.
  FS_MMC_HW_TYPE_SPI_IS_PRESENT         * pfIsPresent;          // Checks if the card is inserted.
  FS_MMC_HW_TYPE_SPI_IS_WRITE_PROTECTED * pfIsWriteProtected;   // Checks if the card is write protected.
  FS_MMC_HW_TYPE_SPI_SET_MAX_SPEED      * pfSetMaxSpeed;        // Configures the clock frequency supplied to the card.
  FS_MMC_HW_TYPE_SPI_SET_VOLTAGE        * pfSetVoltage;         // Configures the voltage level of the card power supply.
  FS_MMC_HW_TYPE_SPI_READ               * pfRead;               // Transfers data from card to MCU.
  FS_MMC_HW_TYPE_SPI_WRITE              * pfWrite;              // Transfers data from MCU to card.
  FS_MMC_HW_TYPE_SPI_READ_EX            * pfReadEx;             // Transfers data from card to MCU.
  FS_MMC_HW_TYPE_SPI_WRITE_EX           * pfWriteEx;            // Transfers data from MCU to card.
  FS_MMC_HW_TYPE_SPI_LOCK               * pfLock;               // Requires exclusive access to SPI bus.
  FS_MMC_HW_TYPE_SPI_UNLOCK             * pfUnlock;             // Releases exclusive access to SPI bus.
} FS_MMC_HW_TYPE_SPI;

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_INIT
*
*  Function description
*    Initializes the hardware layer.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The Card Mode MMC/SD driver calls FS_MMC_HW_TYPE_CM_INIT before
*    any other function of the hardware layer. FS_MMC_HW_TYPE_CM_INIT
*    must perform any steps required to initialize the target hardware
*    such as enabling the clocks, setting up special function registers,
*    etc.
*/
typedef void FS_MMC_HW_TYPE_CM_INIT(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_DELAY
*
*  Function description
*    Blocks the execution for the specified time.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    ms           Number of milliseconds to block the execution.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The specified time is a minimum delay. The actual delay is permitted
*    to be longer. This can be helpful when using an RTOS. Every RTOS has
*    a delay API function, but the accuracy is typically 1 tick, which is
*    1 ms in most cases. Therefore, a delay of 1 tick is typically between
*    0 and 1 ms. To compensate for this, the equivalent of 1 tick should
*    be added to the delay parameter before passing it to an RTOS delay function.
*/
typedef void FS_MMC_HW_TYPE_CM_DELAY(int ms);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SPI_IS_PRESENT
*
*  Function description
*    Checks if the card is present.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    ==FS_MEDIA_STATE_UNKNOWN   The presence state of the card is unknown.
*    ==FS_MEDIA_NOT_PRESENT     The card is present.
*    ==FS_MEDIA_IS_PRESENT      The card is not present.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    Typically, the card presence detection is implemented via a dedicated
*    Card Detect (CD) signal. This signal is connected to a switch inside
*    the card slot that changes its state each time the card is removed
*    or inserted. If the hardware does not provide such signal the function
*    has to return FS_MEDIA_STATE_UNKNOWN.
*/
typedef int FS_MMC_HW_TYPE_CM_IS_PRESENT(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SPI_IS_WRITE_PROTECTED
*
*  Function description
*    Checks if the card is write protected.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    ==0     The card data can be modified.
*    !=0     The card data cannot be modified.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    Typically, the card protection status is implemented via a dedicated
*    Write Protected (WP) signal. This signal is connected to a switch inside
*    the card slot that reflects the status of the write protect switch
*    found on SD cards. If the hardware does not provide such signal the
*    function has to return 0.
*
*    eMMC devices and micro SD cards do not have a write protection switch.
*    Please note that the write protect switch does not really protect the
*    data on an SD card form being modified. It is merely an indication
*    that the data has to be protected. It is the responsibility of the host
*    MCU to respect the status of this switch.
*/
typedef int FS_MMC_HW_TYPE_CM_IS_WRITE_PROTECTED(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SPI_SET_MAX_SPEED
*
*  Function description
*    Configures the clock frequency supplied to the card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    MaxFreq      Maximum allowed frequency of the bus clock in kHz.
*
*  Return value
*    !=0     The actual clock frequency in kHz.
*    ==0     An error occurred.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The hardware layer is allowed to set a clock frequency smaller
*    than the value specified via MaxFreq but never greater than that.
*    The Card Mode MMC/SD driver calls this function at least two times during
*    the initialization of the MMC or SD card:
*    \item Before the initialization starts to set the clock frequency to 400 kHz.
*    \item After the card identification to set the standard clock frequency
*          which is 25 MHz for SD cards or 26 MHz for MMC cards and eMMC devices.
*
*    FS_MMC_HW_TYPE_CM_SPI_SET_MAX_SPEED is called a third time by the
*    Card Mode MMC/SD driver during the initialization if the high speed mode
*    is enabled in the driver via FS_MMC_CM_AllowHighSpeedMode() and the card
*    supports this mode. In this case the clock frequency is set to 50 MHz for
*    SD cards and to 52 MHz for MMC cards and eMMC devices.
*
*    The function has to return the actual configured clock frequency.
*    If the precise frequency is unknown such as for implementation using
*    I/O port "bit-banging", the return value has to be less than the specified
*    frequency. This may lead to longer timeout values but is in general
*    does not cause any problems. The Card Mode MMC/SD driver uses the returned
*    value to calculate timeout values.
*/
typedef U16 FS_MMC_HW_TYPE_CM_SET_MAX_SPEED(U8 Unit, U16 MaxFreq);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SET_RESPONSE_TIMEOUT
*
*  Function description
*    Configures the maximum time to wait for a response from card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    Value        Number of clock cycles to wait.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The specified value is the maximum number of clock cycles the
*    hardware layer has to wait for a response from the card.
*    If the card does not respond within the specified timeout then
*    FS_MMC_HW_TYPE_CM_GET_RESPONSE has to return an error to the
*    Card Mode MMC/SD driver.
*/
typedef void FS_MMC_HW_TYPE_CM_SET_RESPONSE_TIMEOUT(U8 Unit, U32 Value);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SET_READ_DATA_TIMEOUT
*
*  Function description
*    Configures the maximum time to wait for data from card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    Value        Number of clock cycles to wait.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The specified value is the maximum number of clock cycles the
*    hardware layer has to wait for data to be received from the card.
*    If the card does not starts sending the data within the specified
*    timeout then FS_MMC_HW_TYPE_CM_READ_DATA has to return an error
*    to the Card Mode MMC/SD driver. The timeout value specified via
*    FS_MMC_HW_TYPE_CM_SET_READ_DATA_TIMEOUT has to be used as timeout
*    for a write data operation if the MMC/SD host controller supports
*    this feature.
*/
typedef void FS_MMC_HW_TYPE_CM_SET_READ_DATA_TIMEOUT(U8 Unit, U32 Value);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SEND_CMD
*
*  Function description
*    Sends a command to card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    Cmd          Command code.
*    CmdFlags     Command options.
*    ResponseType Type of response expected.
*    Arg          Command arguments.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    Cmd is a command code defined in the SD or MMC specification.
*
*    CmdFlags specifies additional information about the command execution
*    such as the direction of the data transfer if any, the number of data
*    lines to be used for the data exchange, etc. CmdFlags is a bitwise
*    OR-combination of \ref{Card mode command flags}
*
*    ResponseType specifies the format of the response expected for the sent command.
*    It can be one of \ref{Card mode response formats}. If the command requests
*    a response from the card then the Card Mode MMC/SD driver calls
*    FS_MMC_HW_TYPE_CM_GET_RESPONSE to get it.
*
*    Arg is the value that is stored at the bit positions 8:39 in a command codeword.
*
*    A command codeword is always sent via the CMD signal. The same signal is used
*    by a card to send a response back to MCU.
*/
typedef void FS_MMC_HW_TYPE_CM_SEND_CMD(U8 Unit, unsigned Cmd, unsigned CmdFlags, unsigned ResponseType, U32 Arg);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_GET_RESPONSE
*
*  Function description
*    Receives a response from card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [OUT] Received response.
*    NumBytes     Number of bytes to be received.
*
*  Return value
*    ==0      OK, response received.
*    !=0      Error code indicating the failure reason.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The response is sent by the card via the CMD signal. A response codeword
*    can be either 48- or 136-bit large.
*
*    Refer to \ref{Card mode error codes} for possible return values.
*
*    The MMC and SD specifications describe the structure of a response
*    in terms of bit units with bit 0 being the first transmitted via
*    the CMD signal. The following table shows at which byte offsets
*    the response has to be stored to pData.
*
*    48-bit response:
*    +-------------------+--------------------+
*    | Byte offset pData | Bit range response |
*    +-------------------+--------------------+
*    | 0                 | 47-40              |
*    +-------------------+--------------------+
*    | 1                 | 39-32              |
*    +-------------------+--------------------+
*    | 2                 | 31-24              |
*    +-------------------+--------------------+
*    | 3                 | 23-16              |
*    +-------------------+--------------------+
*    | 4                 | 15-8               |
*    +-------------------+--------------------+
*    | 5                 | 7-0                |
*    +-------------------+--------------------+
*
*    136-bit response:
*    +-------------------+--------------------+
*    | Byte offset pData | Bit range response |
*    +-------------------+--------------------+
*    | 0                 | 135-128            |
*    +-------------------+--------------------+
*    | 1                 | 127-120            |
*    +-------------------+--------------------+
*    | 2                 | 119-112            |
*    +-------------------+--------------------+
*    | 3                 | 111-104            |
*    +-------------------+--------------------+
*    | 4                 | 103-96             |
*    +-------------------+--------------------+
*    | 5                 | 95-88              |
*    +-------------------+--------------------+
*    | 6                 | 87-80              |
*    +-------------------+--------------------+
*    | 7                 | 79-72              |
*    +-------------------+--------------------+
*    | 8                 | 71-64              |
*    +-------------------+--------------------+
*    | 9                 | 63-56              |
*    +-------------------+--------------------+
*    | 10                | 55-48              |
*    +-------------------+--------------------+
*    | 11                | 47-40              |
*    +-------------------+--------------------+
*    | 12                | 39-32              |
*    +-------------------+--------------------+
*    | 13                | 31-24              |
*    +-------------------+--------------------+
*    | 14                | 23-16              |
*    +-------------------+--------------------+
*    | 15                | 15-8               |
*    +-------------------+--------------------+
*    | 16                | 7-0                |
*    +-------------------+--------------------+
*
*    The first and last byte of a response codeword store control and check
*    information that is not used by the Card Mode MMC/SD driver.
*    If this information is not provided by the SD and MMC host controller
*    then the hardware layer does not have to store any data to these locations
*    of pData. That is the response data must be stored at byte offset 1 in pData.
*/
typedef int FS_MMC_HW_TYPE_CM_GET_RESPONSE(U8 Unit, void * pData, U32 NumBytes);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_READ_DATA
*
*  Function description
*    Transfers data from card to MCU.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [OUT] Data read from the card.
*    BlockSize    Number of bytes in a block to be transferred.
*    NumBlocks    Number of blocks be transferred.
*
*  Return value
*    ==0      OK, data read.
*    !=0      Error code indicating the failure reason.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The Card Mode MMC/SD driver initiates a data transfer by sending
*    a read command via FS_MMC_HW_TYPE_CM_SEND_CMD. After it checks
*    the response via FS_MMC_HW_TYPE_CM_GET_RESPONSE the Card Mode
*    MMC/SD driver calls FS_MMC_HW_TYPE_CM_READ_DATA to transfer the
*    data from card.
*
*    Typically, an MMC or SD host controller transfers the data one block
*    at a time. BlockSize specifies the size of one block in bytes with the
*    number of blocks being specified via NumBlocks. BlockSize and NumBlocks
*    can never be 0. Typically the size of block is 512 bytes but during the
*    initialization of the card the Card Mode MMC/SD driver can request blocks
*    of different (smaller) sizes. The total number of bytes to be transferred by
*    FS_MMC_HW_TYPE_CM_READ_DATA is BlockSize * NumBlocks.
*
*    The number of data signals to be used for the data transfer is specified
*    by the Card Mode MMC/SD driver via the command flags when the command that
*    initiates the read operation is sent. FS_MMC_CMD_FLAG_USE_SD4MODE is
*    set in the command flags when the data has to be exchanged via 4 data
*    signals. This flag can be set for SD cards as well as MMC devices.
*    FS_MMC_CMD_FLAG_USE_MMC8MODE is set only for MMC devices to transfer
*    the data via 8 data signals.
*
*    Refer to \ref{Card mode error codes} for possible return values.
*/
typedef int FS_MMC_HW_TYPE_CM_READ_DATA(U8 Unit, void * pData, unsigned BlockSize, unsigned NumBlocks);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_WRITE_DATA
*
*  Function description
*    Transfers data from MCU to card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [IN] Data to be written to card.
*    BlockSize    Number of bytes in a block to be transferred.
*    NumBlocks    Number of blocks be transferred.
*
*  Return value
*    ==0      OK, data written.
*    !=0      Error code indicating the failure reason.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    The Card Mode MMC/SD driver initiates a data transfer by sending
*    a write command via FS_MMC_HW_TYPE_CM_SEND_CMD. After it checks
*    the response via FS_MMC_HW_TYPE_CM_GET_RESPONSE the Card Mode
*    MMC/SD driver calls FS_MMC_HW_TYPE_CM_WRITE_DATA to transfer the
*    data to card.
*
*    Typically, an MMC or SD host controller transfers the data one block
*    at a time. BlockSize specifies the size of one block in bytes with the
*    number of blocks being specified via NumBlocks. BlockSize and NumBlocks
*    can never be 0. Typically the size of block is 512 bytes but during the
*    initialization of the card the Card Mode MMC/SD driver can request blocks
*    of different (smaller) sizes. The total number of bytes to be transferred by
*    FS_MMC_HW_TYPE_CM_WRITE_DATA is BlockSize * NumBlocks.
*
*    The number of data signals to be used for the data transfer is specified
*    by the Card Mode MMC/SD driver via the command flags when the command that
*    initiates the write operation is sent. FS_MMC_CMD_FLAG_USE_SD4MODE is
*    set in the command flags when the data has to be exchanged via 4 data
*    signals. This flag can be set for SD cards as well as MMC devices.
*    FS_MMC_CMD_FLAG_USE_MMC8MODE is set only for MMC devices to transfer
*    the data via 8 data signals.
*
*    Refer to \ref{Card mode error codes} for possible return values.
*/
typedef int FS_MMC_HW_TYPE_CM_WRITE_DATA(U8 Unit, const void * pData, unsigned BlockSize, unsigned NumBlocks);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SET_DATA_POINTER
*
*  Function description
*    Configures the pointer to the data to be exchanged.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        Data exchanged with the card.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    It has to be implemented only by a hardware layer that has to
*    prepare the data transfer such as via DMA before the read or write
*    command that initiates that data transfer is sent to card.
*
*    FS_MMC_HW_TYPE_CM_SET_DATA_POINTER is called by the Card Mode MMC/SD driver
*    before each command that exchanges the data with the card. pData points
*    to the same memory region as pData passed to FS_MMC_HW_TYPE_CM_READ_DATA
*    and FS_MMC_HW_TYPE_CM_WRITE_DATA.
*/
typedef void FS_MMC_HW_TYPE_CM_SET_DATA_POINTER(U8 Unit, const void * pData);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SET_BLOCK_LEN
*
*  Function description
*    Configures the size of the data block to be exchanged.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    BlockSize    Number of bytes in a block.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    It has to be implemented only by a hardware layer that has to
*    prepare the data transfer such as via DMA before the read or write
*    command that initiates that data transfer is sent to card.
*
*    FS_MMC_HW_TYPE_CM_SET_BLOCK_LEN is called by the Card Mode MMC/SD driver
*    before each command that exchanges the data with the card. The same value
*    is passed via BlockSize as the value passed via BlockSize to
*    FS_MMC_HW_TYPE_CM_READ_DATA and FS_MMC_HW_TYPE_CM_WRITE_DATA.
*/
typedef void FS_MMC_HW_TYPE_CM_SET_BLOCK_LEN(U8 Unit, U16 BlockSize);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SET_NUM_BLOCKS
*
*  Function description
*    Configures the number of data blocks to be exchanged.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    NumBlocks    Number of bytes to be exchanged.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    It has to be implemented only by a hardware layer that has to
*    prepare the data transfer such as via DMA before the read or write
*    command that initiates that data transfer is sent to card.
*
*    FS_MMC_HW_TYPE_CM_SET_NUM_BLOCKS is called by the Card Mode MMC/SD driver
*    before each command that exchanges the data with the card. The same value
*    is passed via NumBlocks as the value passed via NumBlocks to
*    FS_MMC_HW_TYPE_CM_READ_DATA and FS_MMC_HW_TYPE_CM_WRITE_DATA.
*/
typedef void FS_MMC_HW_TYPE_CM_SET_NUM_BLOCKS(U8 Unit, U16 NumBlocks);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_GET_MAX_READ_BURST
*
*  Function description
*    Returns the maximum number of data blocks that can be transfered
*    at once from card to MCU.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    Number of blocks that can be transfered at once. It cannot be 0.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    FS_MMC_HW_TYPE_CM_GET_MAX_READ_BURST is called by the Card Mode MMC/SD driver
*    during initialization. It has to return the maximum number of 512 byte blocks
*    the hardware layer is able to transfer from the card via a single read command.
*    The larger the number of blocks the hardware layer can exchange at once the
*    better is the read performance.
*/
typedef U16 FS_MMC_HW_TYPE_CM_GET_MAX_READ_BURST(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_GET_MAX_READ_BURST
*
*  Function description
*    Returns the maximum number of data blocks that can be transfered
*    at once from MCU to card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    Number of blocks that can be transfered at once. It cannot be 0.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer
*    and it has to be implemented by any hardware layer.
*
*    FS_MMC_HW_TYPE_CM_GET_MAX_READ_BURST is called by the Card Mode MMC/SD driver
*    during initialization. It has to return the maximum number of 512 byte blocks
*    the hardware layer is able to transfer to the card via a single write command.
*    The larger the number of blocks the hardware layer can exchange at once the
*    better is the write performance.
*/
typedef U16 FS_MMC_HW_TYPE_CM_GET_MAX_WRITE_BURST(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_GET_MAX_REPEAT_WRITE_BURST
*
*  Function description
*    Returns the maximum number of blocks of identical data that can be
*    transfered from MCU to card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    Number of blocks that can be transfered at once.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer is not required to implement this function.
*
*    FS_MMC_HW_TYPE_CM_GET_MAX_REPEAT_WRITE_BURST is called by the Card Mode
*    MMC/SD driver during initialization. It has to return the maximum number
*    of 512 byte blocks the hardware layer is able to repeatedly transfer
*    to the card via a single write command. The command that starts a repeated
*    write operation has FS_MMC_CMD_FLAG_WRITE_BURST_REPEAT set in the command
*    flags passed to FS_MMC_HW_TYPE_CM_SEND_CMD. The Card Mode MMC/SD driver
*    transfers the data by calling FS_MMC_HW_TYPE_CM_WRITE_DATA with pData
*    always pointing to a single block of BlockSize regardless of the NumBlock
*    value. That FS_MMC_HW_TYPE_CM_WRITE_DATA does not have to increment pData
*    by BlockSize after each block transfer. The larger the number of blocks
*    the hardware layer can exchange at once the better is the write performance.
*
*    A return value of 0 indicates that the feature is not supported.
*/
typedef U16 FS_MMC_HW_TYPE_CM_GET_MAX_REPEAT_WRITE_BURST(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_GET_MAX_FILL_WRITE_BURST
*
*  Function description
*    Returns the maximum number of blocks of identical data that can be
*    transfered from MCU to card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    Number of blocks that can be transfered at once.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer is not required to implement this function.
*
*    FS_MMC_HW_TYPE_CM_GET_MAX_FILL_WRITE_BURST is called by the Card Mode
*    MMC/SD driver during initialization. It has to return the maximum number
*    of 512 byte blocks the hardware layer is able to repeatedly transfer
*    to the card via a single write command. The command that starts a repeated
*    write operation has FS_MMC_CMD_FLAG_WRITE_BURST_FILL set in the command
*    flags passed to FS_MMC_HW_TYPE_CM_SEND_CMD. The Card Mode MMC/SD driver
*    transfers the data by calling FS_MMC_HW_TYPE_CM_WRITE_DATA with pData
*    always pointing to a single block of BlockSize regardless of the NumBlocks
*    value. FS_MMC_HW_TYPE_CM_GET_MAX_FILL_WRITE_BURST has to take the first
*    four bytes from pData and send them repeatedly to card until
*    BlockSize * NumBlocks are sent. Typically this operation is realized by
*    using DMA configured to transfer four bytes at once and without incrementing
*    the destination address. The larger the number of blocks the hardware layer
*    can exchange at once the better is the write performance.
*
*    A return value of 0 indicates that the feature is not supported.
*/
typedef U16 FS_MMC_HW_TYPE_CM_GET_MAX_FILL_WRITE_BURST(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SET_VOLTAGE
*
*  Function description
*    Changes the voltage level of the I/O lines.
*
*  Parameters
*    Unit               Index of the hardware layer instance (0-based)
*    VMin               Minimum voltage of I/O lines to be set.
*    VMax               Maximum voltage of I/O lines to be set.
*    IsSwitchSequence   Specifies if a voltage switch sequence has to
*                       be performed according to SD specification.
*                       * 1   Voltage switch sequence has to be performed.
*                       * 0   No voltage switch sequence is required.
*
*  Return value
*    ==0      OK, voltage switched successfully.
*    !=0      Error code indicating the failure reason.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer has to implement this function if the hardware
*    supports 1.8 V signaling and the application enables any ultra high
*    speed mode in the Card Mode MMC/SD driver.
*
*    The values of VMin and VMax are specified in mV with VMin always
*    smaller than or equal to VMax.
*
*    IsSwitchSequence is set to 1 only if an SD card is connected.
*    For MMC cards or eMMC devices IsSwitchSequence is always set to 0.
*    If IsSwitchSequence is set to 1 then the function has to wait
*    for the SD card to change the voltage level of the I/O lines as follows:
*    \item Wait for the SD card to drive either the CMD or one of the DAT lines to low.
*    \item Stop providing clock to SD card.
*    \item Wait for the SD card to start the switch sequence by checking if
*          either the CMD or one of the DAT lines to low.
*    \item Wait at least 5 ms.
*    \item Start providing the clock to SD card using a 1.8 V voltage level.
*    \item Wait at least 1 ms.
*    \item Check if the SD card drives one of the DAT lines to high.
*    This function has to wait for the power to stabilize before it returns.
*
*    Refer to \ref{Card mode error codes} for possible return values.
*/
typedef int FS_MMC_HW_TYPE_CM_SET_VOLTAGE(U8 Unit, U16 VMin, U16 VMax, int IsSwitchSequence);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_GET_VOLTAGE
*
*  Function description
*    Returns the current voltage level of the I/O lines.
*
*  Parameters
*    Unit               Index of the hardware layer instance (0-based)
*
*  Return value
*    !=0      OK, voltage level of I/O lines in mV.
*    ==0      An error occurred.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer has to implement this function if the hardware
*    supports 1.8 V signaling and the application enables any ultra high
*    speed mode in the Card Mode MMC/SD driver.
*/
typedef U16 FS_MMC_HW_TYPE_CM_GET_VOLTAGE(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_SPI_SET_MAX_SPEED_EX
*
*  Function description
*    Configures the clock frequency supplied to the card.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    MaxFreq      Maximum allowed frequency of the bus clock in kHz.
*    ClkFlags     Additional clock generation options.
*
*  Return value
*    !=0     The actual clock frequency in kHz.
*    ==0     An error occurred.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer has to implement this function if the hardware
*    supports 1.8 V signaling and the application enables any ultra high
*    speed mode in the Card Mode MMC/SD driver.
*
*    The hardware layer is allowed to set a clock frequency smaller
*    than the value specified via MaxFreq but never greater than that.
*
*    FS_MMC_HW_TYPE_CM_SPI_SET_MAX_SPEED_EX is similar to
*    FS_MMC_HW_TYPE_CM_SPI_SET_MAX_SPEED with the exception that it
*    allows the configuration of a clock frequency greater than 65535 kHz
*    which is required by some of the ultra high speed modes.
*    In addition, FS_MMC_HW_TYPE_CM_SPI_SET_MAX_SPEED_EX has the ability
*    to configure if the data received from the card has to be sampled
*    on one clock edge or on both clock edges.
*
*    ClkFlags is a bitwise OR-combination of \ref{Card mode clock flags}
*    that specifies for example how the data has to be sampled.
*/
typedef U32 FS_MMC_HW_TYPE_CM_SET_MAX_CLOCK(U8 Unit, U32 MaxFreq, unsigned Flags);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_ENABLE_TUNING
*
*  Function description
*    Stops the normal operation and enters the tuning procedure.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    ==0     OK, tuning procedure activated.
*    !=0     An error occurred.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer has to implement this function if the hardware
*    supports 1.8 V signaling and the application enables an ultra high
*    speed mode that requires the tuning of the data sampling point.
*
*    FS_MMC_HW_TYPE_CM_ENABLE_TUNING is called by the Card Mode MMC/SD driver
*    one time at the beginning of the tuning procedure. The call to this
*    function is followed by one or more calls to FS_MMC_HW_TYPE_CM_START_TUNING.
*    FS_MMC_HW_TYPE_CM_DISABLE_TUNING is called at the end of the tuning procedure.
*
*    Typically, this function can be used to enabled the delay line loop
*    that is used for generating the sampling clock.
*
*    Refer to \ref{Card mode error codes} for possible return values.
*/
typedef int FS_MMC_HW_TYPE_CM_ENABLE_TUNING(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_DISABLE_TUNING
*
*  Function description
*    Restores the normal operation after the tuning procedure.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    Result       Specifies if the tuning procedure was successful or not.
*                 * 0 - The tuning procedure was successful.
*                 * 1 - The tuning procedure failed.
*
*  Return value
*    ==0     OK, tuning procedure deactivated.
*    !=0     An error occurred.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer has to implement this function if the hardware
*    supports 1.8 V signaling and the application enables an ultra high
*    speed mode that requires the tuning of the data sampling point.
*
*    FS_MMC_HW_TYPE_CM_DISABLE_TUNING is called by the Card Mode MMC/SD driver
*    one time at the end of the tuning procedure.
*
*    Typically, this function can be used to disable the delay line loop
*    that is used for generating the sampling clock when the tuning procedure
*    fails.
*
*    Refer to \ref{Card mode error codes} for possible return values.
*/
typedef int FS_MMC_HW_TYPE_CM_DISABLE_TUNING(U8 Unit, int Result);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_START_TUNING
*
*  Function description
*    Indicates the beginning of a tuning sequence.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    TuningIndex  Index of the current tuning sequence (0-based)
*
*  Return value
*    ==0     OK, tuning sequence failed.
*    !=0     An error occurred.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer has to implement this function if the hardware
*    supports 1.8 V signaling and the application enables an ultra high
*    speed mode that requires the tuning of the data sampling point.
*
*    FS_MMC_HW_TYPE_CM_START_TUNING is called by the Card Mode MMC/SD driver
*    one time at the beginning of each tuning sequence. On the first call
*    TuningIndex is set to 0, on the second to 1 and so on. The maximum
*    value of TuningIndex is the value returned by FS_MMC_HW_TYPE_CM_GET_MAX_TUNINGS
*    minus 1. TuningIndex can be used for example to select a different
*    delay line unit.
*
*    Refer to \ref{Card mode error codes} for possible return values.
*/
typedef int FS_MMC_HW_TYPE_CM_START_TUNING(U8 Unit, unsigned TuningIndex);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM_GET_MAX_TUNINGS
*
*  Function description
*    Queries the maximum number of tuning sequences.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    !=0     OK, maximum number of tuning sequences.
*    ==0     An error occurred.
*
*  Additional information
*    This function is a member of the Card Mode MMC/SD hardware layer.
*    A hardware layer has to implement this function if the hardware
*    supports 1.8 V signaling and the application enables an ultra high
*    speed mode that requires the tuning of the data sampling point.
*
*    FS_MMC_HW_TYPE_CM_GET_MAX_TUNINGS is called by the Card Mode MMC/SD driver
*    one time at the beginning of the tuning procedure after the call to
*    FS_MMC_HW_TYPE_CM_ENABLE_TUNING.
*
*    The return value can be for example the number of delay line units
*    available.
*/
typedef U16 FS_MMC_HW_TYPE_CM_GET_MAX_TUNINGS(U8 Unit);

/*********************************************************************
*
*       FS_MMC_HW_TYPE_CM
*
*  Description
*    Hardware layer API for Card Mode MMC/SD driver
*/
typedef struct {
  FS_MMC_HW_TYPE_CM_INIT                       * pfInitHW;                  // Initializes the hardware layer.
  FS_MMC_HW_TYPE_CM_DELAY                      * pfDelay;                   // Blocks the execution for the specified time.
  FS_MMC_HW_TYPE_CM_IS_PRESENT                 * pfIsPresent;               // Checks if the card is inserted.
  FS_MMC_HW_TYPE_CM_IS_WRITE_PROTECTED         * pfIsWriteProtected;        // Checks if the card is write protected.
  FS_MMC_HW_TYPE_CM_SET_MAX_SPEED              * pfSetMaxSpeed;             // Configures the clock frequency supplied to the card.
  FS_MMC_HW_TYPE_CM_SET_RESPONSE_TIMEOUT       * pfSetResponseTimeOut;      // Configures the maximum time to wait for a response from card.
  FS_MMC_HW_TYPE_CM_SET_READ_DATA_TIMEOUT      * pfSetReadDataTimeOut;      // Configures the maximum time to wait for data from card.
  FS_MMC_HW_TYPE_CM_SEND_CMD                   * pfSendCmd;                 // Sends a command to card.
  FS_MMC_HW_TYPE_CM_GET_RESPONSE               * pfGetResponse;             // Receives a response from card.
  FS_MMC_HW_TYPE_CM_READ_DATA                  * pfReadData;                // Transfers data from card to MCU.
  FS_MMC_HW_TYPE_CM_WRITE_DATA                 * pfWriteData;               // Transfers data from MCU to card.
  FS_MMC_HW_TYPE_CM_SET_DATA_POINTER           * pfSetDataPointer;          // Configures the pointer to the data to be exchanged.
  FS_MMC_HW_TYPE_CM_SET_BLOCK_LEN              * pfSetHWBlockLen;           // Configures the size of the data block to be exchanged.
  FS_MMC_HW_TYPE_CM_SET_NUM_BLOCKS             * pfSetHWNumBlocks;          // Configures the number of data blocks to be exchanged.
  FS_MMC_HW_TYPE_CM_GET_MAX_READ_BURST         * pfGetMaxReadBurst;         // Returns the maximum number of data blocks that can be transfered at once from card to MCU.
  FS_MMC_HW_TYPE_CM_GET_MAX_WRITE_BURST        * pfGetMaxWriteBurst;        // Returns the maximum number of data blocks that can be transfered at once from MCU to card.
  FS_MMC_HW_TYPE_CM_GET_MAX_REPEAT_WRITE_BURST * pfGetMaxWriteBurstRepeat;  // Returns the maximum number of blocks with identical data that can be transfered at once from MCU to card.
  FS_MMC_HW_TYPE_CM_GET_MAX_FILL_WRITE_BURST   * pfGetMaxWriteBurstFill;    // Returns the maximum number of blocks with identical data that can be transfered at once from MCU to card.
  FS_MMC_HW_TYPE_CM_SET_VOLTAGE                * pfSetVoltage;              // Sets the voltage level of I/O lines.
  FS_MMC_HW_TYPE_CM_GET_VOLTAGE                * pfGetVoltage;              // Returns the voltage level of I/O lines.
  FS_MMC_HW_TYPE_CM_SET_MAX_CLOCK              * pfSetMaxClock;             // Configures the clock frequency supplied to the card.
  FS_MMC_HW_TYPE_CM_ENABLE_TUNING              * pfEnableTuning;            // Enables the tuning procedure.
  FS_MMC_HW_TYPE_CM_DISABLE_TUNING             * pfDisableTuning;           // Disables the tuning procedure.
  FS_MMC_HW_TYPE_CM_START_TUNING               * pfStartTuning;             // Indicates the beginning of a tuning sequence.
  FS_MMC_HW_TYPE_CM_GET_MAX_TUNINGS            * pfGetMaxTunings;           // Returns the maximum number of tuning sequences.
} FS_MMC_HW_TYPE_CM;

/*********************************************************************
*
*       Compatibility HW layers
*/
extern const FS_MMC_HW_TYPE_SPI     FS_MMC_HW_SPI_Default;
extern const FS_MMC_HW_TYPE_CM      FS_MMC_HW_CM_Default;

/*********************************************************************
*
*       SPI MMC/SD driver API functions
*/
void FS_MMC_ActivateCRC             (void);
void FS_MMC_DeactivateCRC           (void);
int  FS_MMC_GetCardId               (U8 Unit, FS_MMC_CARD_ID * pCardId);
#if FS_MMC_ENABLE_STATS
void FS_MMC_GetStatCounters         (U8 Unit, FS_MMC_STAT_COUNTERS * pStat);
void FS_MMC_ResetStatCounters       (U8 Unit);
#endif // FS_MMC_ENABLE_STATS
void FS_MMC_SetHWType               (U8 Unit, const FS_MMC_HW_TYPE_SPI * pHWType);

/*********************************************************************
*
*       Card Mode MMC/SD driver API functions
*/
void FS_MMC_CM_Allow4bitMode        (U8 Unit, U8 OnOff);
void FS_MMC_CM_Allow8bitMode        (U8 Unit, U8 OnOff);
void FS_MMC_CM_AllowBufferedWrite   (U8 Unit, U8 OnOff);
void FS_MMC_CM_AllowHighSpeedMode   (U8 Unit, U8 OnOff);
void FS_MMC_CM_AllowReliableWrite   (U8 Unit, U8 OnOff);
#if (FS_MMC_SUPPORT_UHS != 0) && (FS_MMC_SUPPORT_SD != 0)
void FS_MMC_CM_AllowAccessModeDDR50 (U8 Unit, U8 OnOff);
#endif // FS_MMC_SUPPORT_UHS != 0 && FS_MMC_SUPPORT_SD != 0
#if (FS_MMC_SUPPORT_UHS != 0) && (FS_MMC_SUPPORT_MMC != 0)
void FS_MMC_CM_AllowAccessModeHS200 (U8 Unit, U8 OnOff);
void FS_MMC_CM_AllowAccessModeHS400 (U8 Unit, U8 OnOff);
void FS_MMC_CM_AllowAccessModeHS_DDR(U8 Unit, U8 OnOff);
#endif // FS_MMC_SUPPORT_UHS != 0 && FS_MMC_SUPPORT_MMC != 0
#if (FS_MMC_SUPPORT_UHS != 0) && (FS_MMC_SUPPORT_SD != 0)
void FS_MMC_CM_AllowAccessModeSDR104(U8 Unit, U8 OnOff);
void FS_MMC_CM_AllowAccessModeSDR50 (U8 Unit, U8 OnOff);
#endif // FS_MMC_SUPPORT_UHS != 0 && FS_MMC_SUPPORT_SD != 0
#if FS_MMC_SUPPORT_UHS
void FS_MMC_CM_AllowVoltageLevel1V8 (U8 Unit, U8 OnOff);
#endif // FS_MMC_SUPPORT_UHS
#if FS_MMC_SUPPORT_POWER_SAVE
int  FS_MMC_CM_AllowPowerSaveMode   (U8 Unit, U8 OnOff);
int  FS_MMC_CM_EnterPowerSaveMode   (U8 Unit);
#endif // FS_MMC_SUPPORT_POWER_SAVE
int  FS_MMC_CM_Erase                (U8 Unit, U32 StartSector, U32 NumSectors);
int  FS_MMC_CM_GetCardId            (U8 Unit, FS_MMC_CARD_ID * pCardId);
int  FS_MMC_CM_GetCardInfo          (U8 Unit, FS_MMC_CARD_INFO * pCardInfo);
#if FS_MMC_ENABLE_STATS
void FS_MMC_CM_GetStatCounters      (U8 Unit, FS_MMC_STAT_COUNTERS * pStat);
#endif // FS_MMC_ENABLE_STATS
int  FS_MMC_CM_ReadExtCSD           (U8 Unit, U32 * pBuffer);
#if FS_MMC_ENABLE_STATS
void FS_MMC_CM_ResetStatCounters    (U8 Unit);
#endif // FS_MMC_ENABLE_STATS
#if FS_MMC_SUPPORT_UHS
void FS_MMC_CM_SetDriverStrength    (U8 Unit, unsigned DriverStrength);
#endif // FS_MMC_SUPPORT_UHS
void FS_MMC_CM_SetHWType            (U8 Unit, const FS_MMC_HW_TYPE_CM * pHWType);
#if (FS_MMC_SUPPORT_UHS != 0) && (FS_MMC_SUPPORT_MMC != 0)
void FS_MMC_CM_SetHS200Tuning       (U8 Unit, U8 OnOff);
#endif // FS_MMC_SUPPORT_UHS != 0 && FS_MMC_SUPPORT_MMC != 0
#if (FS_MMC_SUPPORT_UHS != 0) && (FS_MMC_SUPPORT_SD != 0)
void FS_MMC_CM_SetSDR50Tuning       (U8 Unit, U8 OnOff);
void FS_MMC_CM_SetSDR104Tuning      (U8 Unit, U8 OnOff);
#endif // FS_MMC_SUPPORT_UHS != 0 && FS_MMC_SUPPORT_SD != 0
void FS_MMC_CM_SetSectorRange       (U8 Unit, U32 StartSector, U32 MaxNumSectors);
int  FS_MMC_CM_UnlockCardForced     (U8 Unit);
int  FS_MMC_CM_WriteExtCSD          (U8 Unit, unsigned Off, const U8 * pData, unsigned NumBytes);

/*********************************************************************
*
*       IDE/CF driver
*/

/*********************************************************************
*
*       FS_IDE_HW_TYPE_RESET
*
*  Function description
*    Resets the bus interface.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the CF/IDE hardware layer.
*    All hardware layers are required to implement this function.
*
*    This function is called when the driver detects a new media.
*    For ATA HD drives, there is no action required and this function
*    can be left empty.
*/
typedef void FS_IDE_HW_TYPE_RESET(U8 Unit);

/*********************************************************************
*
*       FS_IDE_HW_TYPE_IS_PRESENT
*
*  Function description
*    Checks if the device is connected.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Return value
*    !=0      Device is connected.
*    ==0      Device is not connected.
*
*  Additional information
*    This function is a member of the CF/IDE hardware layer.
*    All hardware layers are required to implement this function.
*/
typedef int FS_IDE_HW_TYPE_IS_PRESENT(U8 Unit);

/*********************************************************************
*
*       FS_IDE_HW_TYPE_DELAY_400NS
*
*  Function description
*    Blocks the execution for 400ns.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the CF/IDE hardware layer.
*    All hardware layers are required to implement this function.
*
*    The function is always called when a command is sent or parameters
*    are set in the CF card or IDE device to give the integrated logic
*    time to finish a command. When using slow CF cards or IDE devices
*    with fast processors this function should guarantee that a delay of 400ns
*    is respected. However, this function may be left empty if you fast devices
*    are used (modern CF-Cards and IDE drives are faster than 400ns when
*    executing commands.)
*/
typedef void FS_IDE_HW_TYPE_DELAY_400NS(U8 Unit);

/*********************************************************************
*
*       FS_IDE_HW_TYPE_READ_REG
*
*  Function description
*    Reads the value of a register.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    AddrOff      Offset of the register that has to be read.
*
*  Return value
*    Value read from the register.
*
*  Additional information
*    This function is a member of the CF/IDE hardware layer.
*    All hardware layers are required to implement this function.
*
*    A register is 16-bit large.
*/
typedef U16 FS_IDE_HW_TYPE_READ_REG(U8 Unit, unsigned AddrOff);

/*********************************************************************
*
*       FS_IDE_HW_TYPE_WRITE_REG
*
*  Function description
*    Writes the value of a register.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    AddrOff      Offset of the register that has to be written.
*    Data         Value to be written to the specified register.
*
*  Additional information
*    This function is a member of the CF/IDE hardware layer.
*    All hardware layers are required to implement this function.
*
*    A register is 16-bit large.
*/
typedef void FS_IDE_HW_TYPE_WRITE_REG(U8 Unit, unsigned AddrOff, U16 Data);

/*********************************************************************
*
*       FS_IDE_HW_TYPE_READ_DATA
*
*  Function description
*    Transfers data from device to MCU.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [OUT] Data transferred from device.
*    NumBytes     Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the CF/IDE hardware layer.
*    All hardware layers are required to implement this function.
*/
typedef void FS_IDE_HW_TYPE_READ_DATA(U8 Unit, U8 * pData, unsigned NumBytes);

/*********************************************************************
*
*       FS_IDE_HW_TYPE_WRITE_DATA
*
*  Function description
*    Transfers data from MCU do device.
*
*  Parameters
*    Unit         Index of the hardware layer instance (0-based)
*    pData        [IN] Data to be transferred to device.
*    NumBytes     Number of bytes to be transferred.
*
*  Additional information
*    This function is a member of the CF/IDE hardware layer.
*    All hardware layers are required to implement this function.
*/
typedef void FS_IDE_HW_TYPE_WRITE_DATA(U8 Unit, const U8 * pData, unsigned NumBytes);

/*********************************************************************
*
*       FS_IDE_HW_TYPE
*
*  Description
*    Hardware layer API
*/
typedef struct {
  FS_IDE_HW_TYPE_RESET       * pfReset;           // Resets the bus interface.
  FS_IDE_HW_TYPE_IS_PRESENT  * pfIsPresent;       // Checks if the device is connected.
  FS_IDE_HW_TYPE_DELAY_400NS * pfDelay400ns;      // Blocks the execution for 400ns.
  FS_IDE_HW_TYPE_READ_REG    * pfReadReg;         // Reads the value of a register.
  FS_IDE_HW_TYPE_WRITE_REG   * pfWriteReg;        // Writes the value of a register.
  FS_IDE_HW_TYPE_READ_DATA   * pfReadData;        // Transfers data from device to MCU.
  FS_IDE_HW_TYPE_WRITE_DATA  * pfWriteData;       // Transfers data from MCU do device.
} FS_IDE_HW_TYPE;

/*********************************************************************
*
*       Compatibility HW layer
*/
extern const FS_IDE_HW_TYPE FS_IDE_HW_Default;

/*********************************************************************
*
*       API functions
*/
void FS_IDE_Configure(U8 Unit, U8 IsSlave);
void FS_IDE_SetHWType(U8 Unit, const FS_IDE_HW_TYPE * pHWType);

/*********************************************************************
*
*       NAND driver
*/

/*********************************************************************
*
*       NAND block types
*
*  Description
*    Type of data stored to a NAND block.
*/
#define FS_NAND_BLOCK_TYPE_UNKNOWN              0     // The type of the block cannot be determined.
#define FS_NAND_BLOCK_TYPE_BAD                  1     // The block marked as defective.
#define FS_NAND_BLOCK_TYPE_EMPTY                2     // The block does not store any data.
#define FS_NAND_BLOCK_TYPE_WORK                 3     // The block that stores data temporarily.
#define FS_NAND_BLOCK_TYPE_DATA                 4     // The block that stores data permanently.

/*********************************************************************
*
*       NAND test return values
*
*  Description
*    Return values of the NAND block test functions.
*/
#define FS_NAND_TEST_RETVAL_OK                  0     // The test was successful.
#define FS_NAND_TEST_RETVAL_CORRECTABLE_ERROR   1     // Bit errors occurred that were corrected via ECC.
#define FS_NAND_TEST_RETVAL_FATAL_ERROR         2     // Bit errors occurred that the ECC was not able to correct.
#define FS_NAND_TEST_RETVAL_BAD_BLOCK           3     // The tested block was marked as defective.
#define FS_NAND_TEST_RETVAL_ERASE_FAILURE       4     // An error occurred during the block erase operation.
#define FS_NAND_TEST_RETVAL_WRITE_FAILURE       5     // An error occurred while writing the data to the NAND block.
#define FS_NAND_TEST_RETVAL_READ_FAILURE        6     // An error occurred while reading the data from the NAND block.
#define FS_NAND_TEST_RETVAL_INTERNAL_ERROR      (-1)  // An internal processing error occurred.

/*********************************************************************
*
*       ECC correction status
*
*  Description
*    Result of the bit error correction.
*/
#define FS_NAND_CORR_NOT_APPLIED                0u    // No bit errors detected.
#define FS_NAND_CORR_APPLIED                    1u    // Bit errors were detected and corrected.
#define FS_NAND_CORR_FAILURE                    2u    // Bit errors were detected but not corrected.

/*********************************************************************
*
*       Bad block marking types
*
*  Description
*    Methods to mark a block as defective.
*/
#define FS_NAND_BAD_BLOCK_MARKING_TYPE_UNKNOWN  0     // Not known.
#define FS_NAND_BAD_BLOCK_MARKING_TYPE_ONFI     1     // The block is marked as bad in the first page of the block.
#define FS_NAND_BAD_BLOCK_MARKING_TYPE_LEGACY   2     // The block is marked as bad in the first and second page of the block.

/*********************************************************************
*
*       FS_NAND_BLOCK_INFO
*
*  Description
*    Information about a NAND block.
*
*  Additional information
*    Refer to \ref{NAND block types} for a list of permitted values for Type.
*
*    IsDriverBadBlock is valid only when Type is set to FS_NAND_BLOCK_TYPE_BAD.
*/
typedef struct {
  U32          EraseCnt;                  // Number of times the block has been erased
  U32          lbi;                       // Logical block index assigned to queried physical block.
  U16          NumSectorsBlank;           // Sectors are not used yet.
  U16          NumSectorsValid;           // Sectors contain correct data.
  U16          NumSectorsInvalid;         // Sectors have been invalidated.
  U16          NumSectorsECCError;        // Sectors have incorrect ECC.
  U16          NumSectorsECCCorrectable;  // Sectors have correctable ECC error.
  U16          NumSectorsErrorInECC;      // Sectors have error in ECC.
  U8           IsDriverBadBlock;          // Set to 1 if the block has been marked as bad by the driver.
  U8           BadBlockErrorType;         // Type of the error that caused the block to be marked as defective.
  U16          BadBlockErrorBRSI;         // Block-relative sector index on which the fatal error occurred that caused the block to be marked as defective.
  U8           Type;                      // Type of data stored in the block.
  const char * sType;                     // Zero-terminated string holding the block type.
} FS_NAND_BLOCK_INFO;

/*********************************************************************
*
*       FS_NAND_DISK_INFO
*
*  Description
*    Information about the NAND partition.
*
*  Additional information
*    Refer to \ref{Bad block marking types} for a list of permitted
*    values for BadBlockMarkingType.
*/
typedef struct {
  U32 NumPhyBlocks;                       // Total number of blocks in the NAND partition.
  U32 NumLogBlocks;                       // Total number of NAND blocks that can be used to store data.
  U32 NumUsedPhyBlocks;                   // Number of NAND blocks that store valid data.
  U32 NumBadPhyBlocks;                    // Number of NAND blocks that are marked as defective.
  U32 NumPagesPerBlock;                   // Number of pages in a NAND block.
  U32 NumSectorsPerBlock;                 // Number of logical sectors stored in a NAND block.
  U32 BytesPerPage;                       // Number of bytes in the main area of a NAND page.
  U32 BytesPerSpareArea;                  // Number of bytes in the spare area of a NAND page.
  U32 BytesPerSector;                     // Number of bytes is a logical sector.
  U32 EraseCntMin;                        // Minimum value of the erase counts in the NAND partition.
  U32 EraseCntMax;                        // Maximum value of the erase counts in the NAND partition.
  U32 EraseCntAvg;                        // Average value of the erase counts of all the NAND blocks in the NAND partition.
  U8  BadBlockMarkingType;                // Type of the bad block marking.
  U8  IsWriteProtected;                   // Set to 1 if the NAND flash device cannot be written.
  U8  HasFatalError;                      // Set to 1 if the SLC1 NAND driver reported a fatal error.
  U8  ErrorType;                          // Type of fatal error that has occurred.
  U32 ErrorSectorIndex;                   // Sector index on which a fatal error occurred.
  U16 BlocksPerGroup;                     // Number of NAND blocks in a group.
  U32 NumWorkBlocks;                      // Number of work blocks used by the SLC1 NAND driver.
  U8  IsFormatted;                        // Set to 1 if the NAND partition has been low-level formatted.
} FS_NAND_DISK_INFO;

/*********************************************************************
*
*       FS_NAND_ECC_INFO
*
*  Description
*    Information about the ECC used to protect data stored on the NAND flash.
*/
typedef struct {
  U8 NumBitsCorrectable;                // Number of bits the ECC should be able to correct.
  U8 ldBytesPerBlock;                   // Number of bytes the ECC should protect.
  U8 HasHW_ECC;                         // Set to 1 if the NAND flash has HW internal ECC.
} FS_NAND_ECC_INFO;

/*********************************************************************
*
*       FS_NAND_ECC_RESULT
*
*  Description
*    Information about the ECC number of bits corrected in an ECC block.
*
*  Additional information
*    This structure is filled by the FS_NAND_PHY_TYPE_GET_ECC_RESULT
*    function of the NAND physical layer to return the result of the
*    bit error correction operation.
*
*    Refer to \ref{ECC correction status} for a list of permitted values
*    for CorrectionStatus.
*
*    An ECC block is the number of bytes protected by a single ECC.
*    Typically, the ECC block is 512 bytes large, therefor the number
*    of ECC blocks in a 2 KByte page is 4. Most of the NAND flash devices
*    report the number of bits corrected in each of the ECC blocks.
*    The value stored to MaxNumBitsCorrected must be the maximum of these
*    values.
*/
typedef struct {
  U8 CorrectionStatus;                  // Indicates if the correction succeeded or failed.
  U8 MaxNumBitsCorrected;               // Maximum number of bit errors detected and corrected in any ECC block of a page.
} FS_NAND_ECC_RESULT;

/*********************************************************************
*
*       FS_NAND_DEVICE_INFO
*
*  Description
*    Information about the NAND flash device.
*
*  Additional information
*    The initialization function of the physical layer
*    FS_NAND_PHY_TYPE_INIT_GET_DEVICE_INFO uses this structure to return
*    information about the NAND flash device to the NAND driver.
*
*    Typical values for BPP_Shift are 9 and 11 for NAND flash devices
*    with a page size (without the spare area) of 512 and 2048 bytes
*    respectively.
*
*    BytesPerSpareArea is typically 1/32 of the page size (2^BPP_Shift)
*    but some NAND flash devices have a spare area larger than this.
*    For example Micron MT29F8G08ABABA has a spare area of 224 bytes
*    for a page size of 4096 bytes.
*
*    DataBusWith can take the following values:
*      * 0 - unknown
*      * 1 - SPI
*      * 8 - parallel 8-bit
*      * 16 - parallel 16-bit
*
*     Refer to \ref{Bad block marking types} for a list of permitted values
*     for BadBlockMarkingType.
*
*     PPO_Shift (Planes Per Operation) has to be specified as a power of two value.
*     Most of the NAND physical layers set this value to 0 to indicate that
*     they do not support parallel operations. FS_NAND_PHY_2048x8_TwoPlane sets
*     this value to 1 because it can read and write two NAND pages at once and
*     it can erase two NAND blocks at once. This information is used by the
*     Universal NAND driver to correctly identify and mark defective blocks.
*/
typedef struct {
  U8               BPP_Shift;           // Bytes per page as a power of two value.
  U8               PPB_Shift;           // Pages per block as a power of two value.
  U16              NumBlocks;           // Total number of blocks in the NAND flash device.
  U16              BytesPerSpareArea;   // Number of bytes in the spare area.
  FS_NAND_ECC_INFO ECC_Info;            // Information about the ECC capability required by the NAND flash device.
  U8               DataBusWidth;        // Number of lines used for exchanging the data with the NAND flash device.
  U8               BadBlockMarkingType; // Specifies how the blocks are marked as defective.
  U8               PPO_Shift;           // Number of operations performed in parallel by the NAND physical layer.
} FS_NAND_DEVICE_INFO;

/*********************************************************************
*
*       FS_NAND_STAT_COUNTERS
*
*  Description
*    Statistical counters of NAND flash driver.
*
*  Additional information
*    This structure can be used to get statistical information about
*    the operation of the Universal as well as SLC1 NAND driver via
*    the function FS_NAND_UNI_GetStatCounters() and FS_NAND_GetStatCounters()
*    respectively.
*
*    aBitErrorCnt[0] stores the number of 1 bit error occurrences,
*    aBitErrorCnt[1] stores the number of 2 bit error occurrences, and so on.
*/
typedef struct {
  U32 NumFreeBlocks;                    // Number of NAND blocks not used for data.
  U32 NumBadBlocks;                     // Number of NAND blocks marked as defective.
  U32 EraseCnt;                         // Number of block erase operation performed.
  U32 ReadDataCnt;                      // Number of times the NAND driver read from the main area of a page.
  U32 ReadSpareCnt;                     // Number of times the NAND driver read from the spare area of a page.
  U32 ReadSectorCnt;                    // Number of logical sectors read from the NAND flash.
  U32 NumReadRetries;                   // Number of times a read operation has been retried because of an error.
  U32 WriteDataCnt;                     // Number of times the NAND driver wrote to the main area of a page.
  U32 WriteSpareCnt;                    // Number of times the NAND driver wrote to the spare area of a page.
  U32 WriteSectorCnt;                   // Number of logical sectors wrote to the NAND flash.
  U32 NumWriteRetries;                  // Number of times a write operation has been retried because of an error.
  U32 ConvertViaCopyCnt;                // Number of block conversions via copy.
  U32 ConvertInPlaceCnt;                // Number of block conversions in place.
  U32 NumValidSectors;                  // Number of logical sectors that contain valid data.
  U32 CopySectorCnt;                    // Number of times the NAND driver copied a logical sector to another location.
  U32 BlockRelocationCnt;               // Number of times the NAND driver relocated a NAND block due to errors.
  U32 ReadByteCnt;                      // Number of bytes read from NAND flash.
  U32 WriteByteCnt;                     // Number of bytes written to NAND flash.
  U32 BitErrorCnt;                      // Number of bit errors detected and corrected.
  U32 aBitErrorCnt[FS_NAND_STAT_MAX_BIT_ERRORS];    // Number of times a specific number of bit errors occurred.
} FS_NAND_STAT_COUNTERS;

/*********************************************************************
*
*       FS_NAND_FATAL_ERROR_INFO
*
*  Description
*    Information passed to callback function when a fatal error occurs.
*/
typedef struct {
  U8  Unit;                     // Instance of the NAND driver that generated the fatal error.
  U8  ErrorType;                // Type of fatal error.
  U32 ErrorSectorIndex;         // Index of the physical sector where the error occurred. Not applicable for all type of errors.
} FS_NAND_FATAL_ERROR_INFO;

/*********************************************************************
*
*       FS_NAND_MOUNT_INFO
*
*  Description
*    Information about the mounted NAND flash device.
*/
typedef struct {
  U32 NumPhyBlocks;                       // Total number of blocks in the NAND partition.
  U32 NumLogBlocks;                       // Total number of NAND blocks that can be used to store data.
  U32 NumPagesPerBlock;                   // Number of pages in a NAND block.
  U32 NumSectorsPerBlock;                 // Number of logical sectors stored in a NAND block.
  U32 BytesPerPage;                       // Number of bytes in the main area of a NAND page.
  U32 BytesPerSpareArea;                  // Number of bytes in the spare area of a NAND page.
  U32 BytesPerSector;                     // Number of bytes is a logical sector.
  U8  BadBlockMarkingType;                // Type of the bad block marking.
  U8  IsWriteProtected;                   // Set to 1 if the NAND flash device cannot be written.
  U8  HasFatalError;                      // Set to 1 if the SLC1 NAND driver reported a fatal error.
  U8  ErrorType;                          // Type of fatal error that has occurred.
  U32 ErrorSectorIndex;                   // Sector index on which a fatal error occurred.
  U16 BlocksPerGroup;                     // Number of NAND blocks in a group.
  U32 NumWorkBlocks;                      // Number of work blocks used by the SLC1 NAND driver.
} FS_NAND_MOUNT_INFO;

/*********************************************************************
*
*       FS_NAND_ON_FATAL_ERROR_CALLBACK
*
*  Function description
*    The type of the callback function invoked by the NAND driver
*    when a fatal error occurs.
*
*  Parameters
*    pFatalErrorInfo    Information about the fatal error.
*
*  Return value
*    ==0  The NAND driver has to mark the NAND flash device as read only.
*    !=0  The NAND flash device has to remain writable.
*
*  Additional information
*    If the callback function returns a 0 the NAND driver marks the NAND flash
*    device as read-only and it remains in this state until the NAND flash device
*    is low-level formated. In this state all further write operations are rejected
*    with an error by the NAND driver.
*
*    The application is responsible to handle the fatal error by for example
*    checking the consistency of the file system via FS_CheckDisk(). The callback
*    function is not allowed to invoke any other FS API functions therefore the
*    handling of the error has to be done after the FS API function that triggered
*    the error returns.
*/
typedef int FS_NAND_ON_FATAL_ERROR_CALLBACK(FS_NAND_FATAL_ERROR_INFO * pFatalErrorInfo);

/*********************************************************************
*
*       FS_NAND_TEST_INFO
*
*  Description
*    Additional information passed to test routine.
*
*  Additional information
*    The test routine returns information about what went wrong during
*    a test via FS_NAND_UNI_TestBlock() and FS_NAND_TestBlock().
*/
typedef struct {
  //
  // Input (required only for FS_NAND_UNI_TestBlock)
  //
  U8  NumBitsCorrectable;     // Number of bits the ECC can correct in the data and spare area (typ. 4)
  U8  OffSpareECCProt;        // Offset in the spare area of the first byte protected by ECC (typ. 4).
  U8  NumBytesSpareECCProt;   // Number of bytes in the spare area protected by ECC (typ. 4 bytes)
  U16 BytesPerSpare;          // Total number of bytes in the spare area. When set to 0 the default value of 1/32 of page size is used.
  //
  // Output
  //
  U32 BitErrorCnt;            // Number of bit errors detected and corrected.
  U32 PageIndex;              // Index of the physical page where the error happened.
} FS_NAND_TEST_INFO;

/*********************************************************************
*
*       FS_NAND_ECC_HOOK_CALC
*
*  Function description
*    Calculates the parity bits of the specified data using ECC.
*
*  Parameters
*    pData      [IN] Data to be written to the main area of the page.
*    pSpare     [IN] Data to be written to the spare area of the page.
*               [OUT] The calculated parity bits.
*
*  Additional information
*    FS_NAND_ECC_HOOK_CALC is called by the Universal NAND driver before it writes
*    the data to the NAND flash device to calculate the parity check bits. The parity
*    check bits are stored together with the data to NAND flash device. They are
*    used by FS_NAND_ECC_HOOK_CALC to correct eventual bit errors.
*
*    This function has to calculate the parity bits of pData and of
*    four bytes of pSpare. That is the calculated parity bits are used
*    to protect the data stored in pData as well as pSpare. The number
*    of bytes in pData to be protected by ECC is specified via \tt{ldBytesPerBlock}
*    in FS_NAND_ECC_HOOK. The data in pSpare to be protected by ECC is located
*    at byte offset four. The calculated parity bits must be stored to pSpare
*    at byte offset eight. The byte order of the stored parity bits is
*    not relevant for the Universal NAND driver. pSpare is organized as follows:
*
*    +------------+----------------------------------------------------+
*    | Byte range | Description                                        |
*    +------------+----------------------------------------------------+
*    | 0-3        | Not protected by ECC                               |
*    +------------+----------------------------------------------------+
*    | 4-7        | Data to be protected by ECC                        |
*    +------------+----------------------------------------------------+
*    | 8-N        | Parity check bits calculated via the ECC algorithm |
*    +------------+----------------------------------------------------+
*
*    FS_NAND_ECC_HOOK_CALC is not allowed to store more than N - 8 + 1
*    parity check bytes at byte offset eight to pSpare. N depends on the
*    size of the main and spare are of the NAND flash and on ldBytesPerBlock
*    and can be calculate using this formula:
*
*    \code
*    N = ((BytesPerSpareArea / (BytesPerPage >> ldBytesPerBlock)) - 8) - 1
*    \endcode
*
*    +------------------------+------------------------------------------------------------+
*    | Parameter              | Description                                                |
*    +------------------------+------------------------------------------------------------+
*    | \tt{BytesPerSpareArea} | Number of bytes in the spare area of the NAND flash device |
*    +------------------------+------------------------------------------------------------+
*    | \tt{BytesPerPage}      | Number of bytes in the spare area of the NAND flash device |
*    +------------------------+------------------------------------------------------------+
*    | \tt{ldBytesPerBlock}   | Value specified in FS_NAND_ECC_HOOK                        |
*    +------------------------+------------------------------------------------------------+
*/
typedef void FS_NAND_ECC_HOOK_CALC(const U32 * pData, U8 * pSpare);

/*********************************************************************
*
*       FS_NAND_ECC_HOOK_APPLY
*
*  Function description
*    Checks and corrects bit errors in the specified data using ECC.
*
*  Parameters
*    pData      [IN] Data read from the main area of the page.
*               [OUT] Corrected main area data.
*    pSpare     [IN] Data read from the spare area of the page.
*               [OUT] Corrected spare area data.
*
*  Return value
*    < 0      Uncorrectable bit errors detected.
*    >=0      Number of bit errors detected and corrected.
*
*  Additional information
*    FS_NAND_ECC_HOOK_APPLY is called by the Universal NAND driver after it reads
*    data with from the NAND flash device to verify that the data is not corrupted.
*    If the function detects bit errors then it uses the parity check bits to correct
*    them. The correction has to be performed in place that is directly in pData and pSpare.
*    The parity check bits are located at byte offset eight in pSpare.
*    They protect all the bytes in pData and four bytes of pSpare. Refer to
*    FS_NAND_ECC_HOOK_CALC for a description of pSpare data layout.
*    FS_NAND_ECC_HOOK_APPLY function has to also correct the bit errors that occurred
*    in the parity check.
*
*    For backwards compatibility the return value is interpreted
*    differently by the Universal NAND driver if \tt{ldBytesPerBlock}
*    is set to 0 as follows:
*    +-------+---------------------------------------------+
*    | Value | Description                                 |
*    +-------+---------------------------------------------+
*    | 0     | No error detected.                          |
*    +-------+---------------------------------------------+
*    | 1     | Bit errors corrected. Data is OK.           |
*    +-------+---------------------------------------------+
*    | 2     | Error in ECC detected. Data is OK.          |
*    +-------+---------------------------------------------+
*    | 3     | Uncorrectable bit error. Data is corrupted. |
*    +-------+---------------------------------------------+
*/
typedef int FS_NAND_ECC_HOOK_APPLY(U32 * pData, U8 * pSpare);

/*********************************************************************
*
*       FS_NAND_ECC_HOOK
*
*  Description
*    ECC calculation algorithm.
*
*  Additional information
*    This structure contains pointers to API functions and attributes
*    related to an ECC calculation algorithm.
*
*    ldBytesPerBlock is typically set to 9 which indicates a block
*    size of 512 bytes. The size of the ECC block is imposed by the
*    specifications of the NAND flash device. If the value is set to
*    0 the Universal NAND driver assumes that the ECC block is 512 bytes large.
*
*    NumBitsCorrectable is used by the Universal NAND driver to check
*    if the ECC algorithm is capable of correcting the number of bit
*    errors required by the used NAND flash device.
*/
typedef struct {
  FS_NAND_ECC_HOOK_CALC  * pfCalc;              // Calculates the ECC of specified data.
  FS_NAND_ECC_HOOK_APPLY * pfApply;             // Verifies and corrects bit errors using ECC.
  U8                       NumBitsCorrectable;  // Number of bits the ECC algorithm is able to correct in the data block and 4 bytes of spare area.
  U8                       ldBytesPerBlock;     // Number of bytes in the data block given as power of 2 value.
} FS_NAND_ECC_HOOK;

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_ERASE_BLOCK
*
*  Function description
*    Erases a NAND block.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    PageIndex    Index of the first page in the NAND block to be erased (0-based)
*
*  Return value
*    ==0  OK, the NAND block has been erased.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API
*    and is mandatory to be implemented by each NAND physical layer.
*    It is called by the NAND driver to set to 1 all the bits of a NAND block.
*    A NAND block is the smallest erasable unit of a NAND flash device.
*
*    The index of the actual NAND block to be erased depends on the
*    number of pages stored in a NAND block. For example if the NAND
*    block contains 64 pages, then the PageIndex parameter passed by
*    the NAND driver to the function has to be be interpreted as follows:
*    * 0   - NAND block 0
*    * 64  - NAND block 1
*    * 128 - NAND block 2
*    * etc.
*/
typedef int FS_NAND_PHY_TYPE_ERASE_BLOCK(U8 Unit, U32 PageIndex);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_INIT_GET_DEVICE_INFO
*
*  Function description
*    Initializes the physical layer.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    pDevInfo     [OUT] Information about the NAND flash device.
*
*  Return value
*    ==0  OK, physical layer has been initialized.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API and it
*    is mandatory to be implemented by each NAND physical layer.
*    It is the first function of the physical layer API that is called
*    by the NAND driver when the NAND flash device is mounted.
*
*    This function initializes hardware layer, resets and tries to identify
*    the NAND flash device. If the NAND flash device can be handled, the
*    pDevInfo is filled with information about the organization and the ECC
*    requirements of the NAND flash device.
*/
typedef int FS_NAND_PHY_TYPE_INIT_GET_DEVICE_INFO(U8 Unit, FS_NAND_DEVICE_INFO * pDevInfo);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_IS_WP
*
*  Function description
*    Checks if the NAND flash device is write protected.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*
*  Return value
*    ==0  Data stored on the NAND flash device can be modified.
*    !=0  Data stored on the NAND flash device cannot be modified.
*
*  Additional information
*    This function is a member of the NAND physical layer API and it
*    is mandatory to be implemented by each NAND physical layer.
*
*    The write protection status is checked by evaluating the bit 7 of
*    the NAND status register. Typical reason for write protection is
*    that either the supply voltage is too low or the /WP-pin is connected
*    to ground.
*/
typedef int FS_NAND_PHY_TYPE_IS_WP(U8 Unit);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_READ
*
*  Function description
*    Reads data from a NAND page.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    PageIndex    Index of the NAND page to read from (0-based).
*    pData        [OUT] Data read from NAND page.
*    Off          Byte offset to read from.
*    NumBytes     Number of bytes to be read.
*
*  Return value
*    ==0  OK, data read.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API and it
*    is mandatory to be implemented by each NAND physical layer.
*    The NAND driver uses FS_NAND_PHY_TYPE_READ to read data from
*    the main as well as from the spare area of a page.
*/
typedef int FS_NAND_PHY_TYPE_READ(U8 Unit, U32 PageIndex, void * pData, unsigned Off, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_READ_EX
*
*  Function description
*    Reads data from a NAND page.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    PageIndex    Index of the NAND page to read from (0-based).
*    pData0       [OUT] Data read from Off0 of the NAND page.
*    Off0         Byte offset to read from for pData0.
*    NumBytes0    Number of bytes to be read.
*    pData1       [OUT] Data read from Off1 of a NAND page.
*    Off1         Byte offset to read from for pData1.
*    NumBytes1    Number of bytes to be read from Off1.
*
*  Return value
*    ==0  OK, data read.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API and it
*    is mandatory to be implemented by each NAND physical layer.
*    FS_NAND_PHY_TYPE_READ_EX is typically used by the NAND driver
*    to read the data from main and spare area of a page at the same time.
*/
typedef int FS_NAND_PHY_TYPE_READ_EX(U8 Unit, U32 PageIndex, void * pData0, unsigned Off0, unsigned NumBytes0, void * pData1, unsigned Off1, unsigned NumBytes1);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_WRITE
*
*  Function description
*    Writes data to a NAND page.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    PageIndex    Index of the NAND page to write to (0-based).
*    pData        [IN] Data to be written to the NAND page.
*    Off          Byte offset to write to.
*    NumBytes     Number of bytes to be written.
*
*  Return value
*    ==0  OK, data written.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API and it
*    is mandatory to be implemented only by NAND physical layer that
*    are working with the SLC1 NAND driver. The Universal NAND driver
*    does not call this function. FS_NAND_PHY_TYPE_WRITE is used by
*    the SLC1 NAND driver to write data to the main as well as to the
*    spare area of a page.
*/
typedef int FS_NAND_PHY_TYPE_WRITE(U8 Unit, U32 PageIndex, const void * pData, unsigned Off, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_WRITE_EX
*
*  Function description
*    Writes data to a NAND page.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    PageIndex    Index of the NAND page to write to (0-based).
*    pData0       [IN] Data to be written to the NAND page at Off0.
*    Off0         Byte offset to write to for pData0.
*    NumBytes0    Number of bytes to be written at Off0.
*    pData1       [IN] Data to be written to the NAND page at Off1.
*    Off1         Byte offset to write to for pData1.
*    NumBytes1    Number of bytes to be written at Off1.
*
*  Return value
*    ==0  OK, data written.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API and it
*    is mandatory to be implemented by each NAND physical layer.
*    The NAND driver uses FS_NAND_PHY_TYPE_WRITE_EX to write data to
*    the main and spare area of a page at the same time.
*/
typedef int FS_NAND_PHY_TYPE_WRITE_EX(U8 Unit, U32 PageIndex, const void * pData, unsigned Off, unsigned NumBytes, const void * pSpare, unsigned OffSpare, unsigned NumBytesSpare);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_ENABLE_ECC
*
*  Function description
*    Enables the hardware ECC.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*
*  Return value
*    ==0  OK, hardware ECC activated.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API. It has
*    to be implemented by the NAND physical layers that provide
*    hardware support for bit error correction either on MCU via
*    a dedicated NAND flash controller or via on-die ECC of the NAND flash device.
*    FS_NAND_PHY_TYPE_ENABLE_ECC is called only by the Universal NAND driver.
*
*    After the call to this function the Universal NAND driver expects that
*    FS_NAND_PHY_TYPE_READ and FS_NAND_PHY_TYPE_READ_EX return corrected data
*    that is without bit errors. In addition, the Universal NAND driver expects
*    that FS_NAND_PHY_TYPE_WRITE and FS_NAND_PHY_TYPE_WRITE_EX calculate and store
*    the ECC to NAND flash device.
*/
typedef int FS_NAND_PHY_TYPE_ENABLE_ECC(U8 Unit);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_DISABLE_ECC
*
*  Function description
*    Deactivates the hardware ECC.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*
*  Return value
*    ==0  OK, hardware ECC deactivated.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API. It has
*    to be implemented by the NAND physical layers that provide
*    hardware support for bit error correction. FS_NAND_PHY_TYPE_DISABLE_ECC
*    is called only by the Universal NAND driver.
*
*    After the call to this function the Universal NAND driver expects that
*    FS_NAND_PHY_TYPE_READ and FS_NAND_PHY_TYPE_READ_EX return data
*    that might contain bit errors. In addition, the Universal NADN driver expects
*    that FS_NAND_PHY_TYPE_WRITE and FS_NAND_PHY_TYPE_WRITE_EX store the data
*    without calculating the ECC.
*/
typedef int FS_NAND_PHY_TYPE_DISABLE_ECC(U8 Unit);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_CONFIGURE_ECC
*
*  Function description
*    Configures the hardware ECC.
*
*  Parameters
*    Unit                 Index of the NAND physical layer instance (0-based)
*    NumBitsCorrectable   Maximum number of bit errors the hardware ECC has to be able to correct.
*    BytesPerECCBlock     Number of consecutive data bytes protected by a single ECC.
*
*  Return value
*    ==0  OK, hardware ECC configured.
*    !=0  An error occurred.
*
*  Additional information
*    This function is a member of the NAND physical layer API. It has
*    to be implemented by the NAND physical layers that provide
*    hardware support for bit error correction with configurable ECC
*    strength. FS_NAND_PHY_TYPE_CONFIGURE_ECC is called only by the
*    Universal NAND driver.
*/
typedef int FS_NAND_PHY_TYPE_CONFIGURE_ECC(U8 Unit, U8 NumBitsCorrectable, U16 BytesPerECCBlock);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_COPY_PAGE
*
*  Function description
*    Copies the contents of an entire page to another page.
*
*  Parameters
*    Unit             Index of the NAND physical layer instance (0-based)
*    PageIndexSrc     Index of the page to copy from.
*    PageIndexDest    Index of the page to copy to.
*
*  Return value
*    ==0  OK, page copied.
*    !=0  An error occurred or operation not supported.
*
*  Additional information
*    This function is a member of the NAND physical layer API. It may be
*    be implemented by the NAND physical layers that can provide
*    a faster method of copying the contents of a page than by first reading
*    the source page contents to MCU and then by writing the contents to the
*    destination page. One such method is the internal page copy operation
*    supported by some NAND flash device. FS_NAND_PHY_TYPE_COPY_PAGE is called
*    only by the Universal NAND driver.
*/
typedef int FS_NAND_PHY_TYPE_COPY_PAGE(U8 Unit, U32 PageIndexSrc, U32 PageIndexDest);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_GET_ECC_RESULT
*
*  Function description
*    Returns the error correction status of the last read page.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    pResult      [OUT] Information about the correction status.
*
*  Return value
*    ==0  OK, information returned.
*    !=0  An error occurred or operation not supported.
*
*  Additional information
*    This function is a member of the NAND physical layer API. It may be
*    implemented by the NAND physical layers that can provide information
*    about the status of the bit correction operation and about the number
*    of bit errors corrected.
*    FS_NAND_PHY_TYPE_GET_ECC_RESULT is called only by the Universal NAND driver.
*
*    The information returned by FS_NAND_PHY_TYPE_GET_ECC_RESULT is used
*    by the Universal NAND driver to decide when a NAND block has to be relocated
*    in order to prevent bit correction errors. A bit correction error occurs when
*    the number of bit errors is greater than the number of bit errors the ECC is
*    able to correct. Typically, a bit correction error causes a data loss.
*/
typedef int FS_NAND_PHY_TYPE_GET_ECC_RESULT(U8 Unit, FS_NAND_ECC_RESULT * pResult);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_DEINIT
*
*  Function description
*    Releases the allocated resources.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*
*  Additional information
*    This function is a member of the NAND physical layer API. It has
*    to be implemented by the NAND physical layers that allocate
*    resources dynamically during the initialization such as memory
*    for the instance.
*
*    The NAND driver calls this function when the file system is unmounted.
*/
typedef void FS_NAND_PHY_TYPE_DEINIT(U8 Unit);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE_SET_RAW_MODE
*
*  Function description
*    Enables or disables the data translation.
*
*  Parameters
*    Unit         Index of the NAND physical layer instance (0-based)
*    OnOff        Activation status of the feature.
*
*  Return value
*    ==0  OK, status changed.
*    !=0  An error occurred or operation not supported.
*
*  Additional information
*    This function is a member of the NAND physical layer API. It may
*    be implemented by the NAND physical layers that store the data to
*    NAND flash device using a different layout than the NAND driver.
*    FS_NAND_PHY_TYPE_SET_RAW_MODE is not called by the NAND driver
*    during the normal operation. It is called only by specific
*    functions of the Universal NAND driver such as FS_NAND_UNI_WritePageRaw()
*    and FS_NAND_UNI_ReadPageRaw().
*/
typedef int FS_NAND_PHY_TYPE_SET_RAW_MODE(U8 Unit, U8 OnOff);

/*********************************************************************
*
*       FS_NAND_PHY_TYPE
*
*  Description
*    NAND physical layer API.
*/
typedef struct {
  FS_NAND_PHY_TYPE_ERASE_BLOCK          * pfEraseBlock;         // Erases a NAND block.
  FS_NAND_PHY_TYPE_INIT_GET_DEVICE_INFO * pfInitGetDeviceInfo;  // Initializes the physical layer.
  FS_NAND_PHY_TYPE_IS_WP                * pfIsWP;               // Checks the write protection status of NAND flash device.
  FS_NAND_PHY_TYPE_READ                 * pfRead;               // Reads data from a NAND page.
  FS_NAND_PHY_TYPE_READ_EX              * pfReadEx;             // Reads data from a NAND page.
  FS_NAND_PHY_TYPE_WRITE                * pfWrite;              // Writes data to a NAND page.
  FS_NAND_PHY_TYPE_WRITE_EX             * pfWriteEx;            // Writes data to a NAND page.
  FS_NAND_PHY_TYPE_ENABLE_ECC           * pfEnableECC;          // Enables the hardware ECC.
  FS_NAND_PHY_TYPE_DISABLE_ECC          * pfDisableECC;         // Disables the hardware ECC.
  FS_NAND_PHY_TYPE_CONFIGURE_ECC        * pfConfigureECC;       // Configures the hardware ECC.
  FS_NAND_PHY_TYPE_COPY_PAGE            * pfCopyPage;           // Copies a NAND page.
  FS_NAND_PHY_TYPE_GET_ECC_RESULT       * pfGetECCResult;       // Returns the result of bit correction via ECC.
  FS_NAND_PHY_TYPE_DEINIT               * pfDeInit;             // Frees allocated resources.
  FS_NAND_PHY_TYPE_SET_RAW_MODE         * pfSetRawMode;         // Enables or disables the raw operation mode.
} FS_NAND_PHY_TYPE;

/*********************************************************************
*
*       FS_NAND_HW_TYPE_INIT_X8
*
*  Function description
*    Initializes the hardware for 8-bit mode access.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API and it has to be implemented by any NAND hardware layer
*    that accesses a NAND flash device via an 8-bit data bus.
*    FS_NAND_HW_TYPE_INIT_X8 is the first function of the hardware
*    layer API that is called by a NAND physical layer during the
*    mounting of the file system.
*
*    This function has to perform any initialization of the MCU hardware
*    required to access the NAND flash device such as clocks, port pins,
*    memory controllers, etc.
*/
typedef void FS_NAND_HW_TYPE_INIT_X8(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_INIT_X16
*
*  Function description
*    Initializes the hardware for 16-bit mode access.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API. The implementation of this function is optional for
*    systems that interface to the NAND flash device via an 8-bit data bus.
*    FS_NAND_HW_TYPE_INIT_X16 must be implemented for systems that
*    have the NAND flash device connected to MCU via an 16-bit data bus.
*
*    FS_NAND_HW_TYPE_INIT_X16 is the first function of the hardware
*    layer API that is called by the NAND physical layer when the NAND
*    flash device is mounted.
*/
typedef void FS_NAND_HW_TYPE_INIT_X16(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_DISABLE_CE
*
*  Function description
*    Disables the NAND flash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API and it is mandatory to be implemented. The implementation
*    of this function can be left empty if the hardware is driving
*    the Chip Enable (CE) signal. Typically, the NAND flash device
*    is disabled by driving the CE signal to a logic low state level.
*/
typedef void FS_NAND_HW_TYPE_DISABLE_CE(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_ENABLE_CE
*
*  Function description
*    Enables the NAND flash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer. The implementation of this function can be left empty if the
*    hardware is driving the Chip Enable (CE) signal. Typically,
*    the NAND flash device is enabled by driving the CE signal to
*    a logic-high level.
*/
typedef void FS_NAND_HW_TYPE_ENABLE_CE(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SET_ADDR_MODE
*
*  Function description
*    Changes the data access to address mode.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware layer.
*
*    After the call to this function the NAND hardware layer has to
*    make sure that any data sent to NAND flash device is interpreted
*    as address information. This can be achieved by setting the
*    Address Latch Enable signal (ALE) signal to logic-high
*    and the Command Latch Enable (CLE) signal to logic low state.
*/
typedef void FS_NAND_HW_TYPE_SET_ADDR_MODE(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SET_CMD_MODE
*
*  Function description
*    Changes the data access to command mode.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware layer.
*
*    After the call to this function the NAND hardware layer has to
*    make sure that any data sent to NAND flash device is interpreted
*    as command information. This can be achieved by setting the
*    Command Latch Enable (CLE) signal to logic-high and the Address
*    Latch Enable signal (ALE) signal to logic low state.
*/
typedef void FS_NAND_HW_TYPE_SET_CMD_MODE(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SET_DATA_MODE
*
*  Function description
*    Changes the data access to data mode.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware layer.
*
*    After the call to this function the NAND hardware layer has to
*    make sure that any data sent to NAND flash device is interpreted
*    as data information. This can be achieved by setting the
*    Command Latch Enable (CLE) and Address Latch Enable signals to logic low state.
*/
typedef void FS_NAND_HW_TYPE_SET_DATA_MODE(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_WAIT_WHILE_BUSY
*
*  Function description
*    Waits for the NAND flash device to become ready.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    us           Maximum time to wait in microseconds.
*
*  Return value
*    ==0    The NAND flash device is ready.
*    !=0    The NAND flash device is busy or the operation is not supported.
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware layer.
*
*    A NAND hardware layer calls this function every time it checks
*    the status of the NAND flash device. A typical implementation uses
*    the status of the Ready/Busy (R/B) signal that is set to logic low state
*    by the NAND flash device as long as it is busy and it cannot accept
*    and other commands from MCU.
*
*    FS_NAND_HW_TYPE_WAIT_WHILE_BUSY must return 1 if the status of
*    the NAND flash device cannot be queried via R/B signal. In this case,
*    the NAND hardware layer checks the status via a read status command.
*
*    Typically, a NAND flash device does not set R/B signal to logic low state
*    immediately after it accepts a command from MCU but only after a time interval
*    labeled as tWB in the data sheet of the NAND flash device. This means that
*    FS_NAND_HW_TYPE_WAIT_WHILE_BUSY has to wait for tWB time interval
*    to elapse before it samples the R/B signal for the first time in
*    order to make sure that it returns correct status information to
*    the NAND physical layer.
*/
typedef int FS_NAND_HW_TYPE_WAIT_WHILE_BUSY(U8 Unit, unsigned us);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_READ_X8
*
*  Function description
*    Reads data from NAND flash device via 8-bit data bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        [OUT] Data read from NAND flash. It cannot be NULL.
*    NumBytes     Number of bytes to be read. It cannot be 0.
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API. It has to be implemented by a NAND hardware layer
*    that exchanges the data with the NAND flash device via an 8-bit
*    data bus.
*
*    FS_NAND_HW_TYPE_READ_X8 is called by a NAND physical layer
*    in data mode to transfer data from NAND flash device to MCU.
*    It is not called in address and command data access modes.
*
*    The transfer of the data is controlled by the MCU using the
*    Read Enable (RE) signal. If the NAND hardware layer exchanges
*    the data via GPIO it has to make sure that the timing of the
*    RE signal meets the specifications of the NAND flash device.
*/
typedef void FS_NAND_HW_TYPE_READ_X8(U8 Unit, void * pData, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_WRITE_X8
*
*  Function description
*    Writes data to NAND flash device via 8-bit data bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        [IN] Data to be written to NAND flash. It cannot be NULL.
*    NumBytes     Number of bytes to be read. It cannot be 0.
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API. It has to be implemented by a NAND hardware layer
*    that exchanges the data with the NAND flash device via an 8-bit
*    data bus.
*
*    FS_NAND_HW_TYPE_WRITE_X8 is called by a NAND physical layer
*    in all data access modes to transfer data from MCU to NAND flash device.
*
*    The transfer of the data is controlled by the MCU using the
*    Write Enable (WE) signal. If the NAND hardware layer exchanges
*    the data via GPIO it has to make sure that the timing of the
*    WE signal meets the specifications of the NAND flash device.
*/
typedef void FS_NAND_HW_TYPE_WRITE_X8(U8 Unit, const void * pData, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_READ_X16
*
*  Function description
*    Reads data from NAND flash device via 16-bit data bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        [OUT] Data read from NAND flash. It cannot be NULL.
*    NumBytes     Number of bytes to be read. It cannot be 0.
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API. It has to be implemented by a NAND hardware layer
*    that exchanges the data with the NAND flash device via an 16-bit
*    data bus.
*
*    FS_NAND_HW_TYPE_READ_X16 is called by a NAND physical layer
*    in data mode to transfer data from NAND flash device to MCU.
*    It is not called in address and command data access modes.
*
*    The transfer of the data is controlled by the MCU using the
*    Read Enable (RE) signal. If the NAND hardware layer exchanges
*    the data via GPIO it has to make sure that the timing of the
*    RE signal meets the specifications of the NAND flash device.
*
*    pData is aligned to a half-word (2-byte) boundary.
*/
typedef void FS_NAND_HW_TYPE_READ_X16(U8 Unit, void * pData, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_WRITE_X16
*
*  Function description
*    Writes data to NAND flash device via 16-bit data bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        [IN] Data to be written to NAND flash. It cannot be NULL.
*    NumBytes     Number of bytes to be read. It cannot be 0.
*
*  Additional information
*    This function is a member of the FS_NAND_PHY_TYPE NAND hardware
*    layer API. It has to be implemented by a NAND hardware layer
*    that exchanges the data with the NAND flash device via an 16-bit
*    data bus.
*
*    FS_NAND_HW_TYPE_WRITE_X16 is called by a NAND physical layer
*    in all data access modes to transfer data from MCU to NAND flash device.
*
*    The transfer of the data is controlled by the MCU using the
*    Write Enable (WE) signal. If the NAND hardware layer exchanges
*    the data via GPIO it has to make sure that the timing of the
*    WE signal meets the specifications of the NAND flash device.
*
*    pData is aligned to a half-word (2-byte) boundary.
*/
typedef void FS_NAND_HW_TYPE_WRITE_X16(U8 Unit, const void * pdata, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_HW_TYPE
*
*  Description
*    NAND hardware layer API for NAND flash devices connected via parallel I/O.
*/
typedef struct {
  FS_NAND_HW_TYPE_INIT_X8         * pfInit_x8;                  // Initializes a NAND flash device with an 8-bit data interface.
  FS_NAND_HW_TYPE_INIT_X16        * pfInit_x16;                 // Initializes a NAND flash device with a 16-bit data interface.
  FS_NAND_HW_TYPE_DISABLE_CE      * pfDisableCE;                // Disables the NAND flash device.
  FS_NAND_HW_TYPE_ENABLE_CE       * pfEnableCE;                 // Enables the NAND flash device.
  FS_NAND_HW_TYPE_SET_ADDR_MODE   * pfSetAddrMode;              // Initiates the transfer of an address.
  FS_NAND_HW_TYPE_SET_CMD_MODE    * pfSetCmdMode;               // Initiates the transfer of a command.
  FS_NAND_HW_TYPE_SET_DATA_MODE   * pfSetDataMode;              // Initiates the transfer of data.
  FS_NAND_HW_TYPE_WAIT_WHILE_BUSY * pfWaitWhileBusy;            // Waits for the NAND flash device to become ready.
  FS_NAND_HW_TYPE_READ_X8         * pfRead_x8;                  // Reads a specified number of bytes from NAND flash device via the 8-bit data interface.
  FS_NAND_HW_TYPE_WRITE_X8        * pfWrite_x8;                 // Writes a specified number of bytes to NAND flash device via the 8-bit data interface.
  FS_NAND_HW_TYPE_READ_X16        * pfRead_x16;                 // Reads a specified number of bytes from NAND flash device via the 16-bit data interface.
  FS_NAND_HW_TYPE_WRITE_X16       * pfWrite_x16;                // Writes a specified number of bytes to NAND flash device via the 16-bit data interface.
} FS_NAND_HW_TYPE;

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_INIT
*
*  Function description
*    Initializes the hardware.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Return value
*    !=0    OK, frequency of the SPI clock in kHz.
*    ==0    An error occurred.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type. FS_NAND_HW_TYPE_SPI_INIT is the first function
*    of the hardware layer API that is called by a NAND physical layer
*    during the mounting of the file system.
*
*    This function has to perform any initialization of the MCU hardware
*    required to access the NAND flash device such as clocks, port pins,
*    SPI controllers, etc.
*
*    A serial NAND flash requires that the SPI communication protocol meets the
*    following requirements:
*    \item 8-bit data length.\n
*    \item The most significant bit hast to be sent out first.\n
*    \item Chip Select (CS) signal should be initially high to disable the serial NAND flash device.\n
*    \item The SPI clock frequency does not have to exceed the maximum
*          clock frequency that is specified by the serial NAND flash device.
*
*    Typically, two SPI modes are supported:
*    \item Mode 0 - CPOL = 0, CPHA = 0, SPI clock remains low in idle state.
*    \item Mode 3 - CPOL = 1, CPHA = 1, SPI clock remains high in idle state.
*/
typedef int FS_NAND_HW_TYPE_SPI_INIT(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_DISABLE_CS
*
*  Function description
*    Disables the NAND flash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The NAND flash device is disabled by driving the Chip Select (CS)
*    signal to logic-high. The NAND flash device ignores any command
*    or data sent with the CS signal set to logic-high.
*/
typedef void FS_NAND_HW_TYPE_SPI_DISABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_ENABLE_CS
*
*  Function description
*    Enables the NAND flash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The NAND flash device is enabled by driving the Chip Select (CS)
*    signal to logic low state. Typically, the NAND flash device is enabled
*    at the beginning of command and data sequence and disabled at the
*    end of it.
*/
typedef void FS_NAND_HW_TYPE_SPI_ENABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_DELAY
*
*  Function description
*    Blocks the execution for the specified number of milliseconds.
*
*  Parameters
*    Unit           Index of the NAND hardware layer instance (0-based)
*    ms             Number of milliseconds to wait.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The time FS_NAND_HW_TYPE_SPI_DELAY blocks does not have to be
*    accurate. That is the function can block the execution longer
*    that the number of specified milliseconds but no less than that.
*/
typedef void FS_NAND_HW_TYPE_SPI_DELAY(U8 Unit, int ms);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_READ
*
*  Function description
*    Transfers a specified number of bytes from NAND flash device to MCU.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        Data read from NAND flash device.
*    NumBytes     Number of bytes to be read.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The function has to sample the data on the falling edge of the SPI clock.
*/
typedef int FS_NAND_HW_TYPE_SPI_READ(U8 Unit, void * pData, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_WRITE
*
*  Function description
*    Transfers a specified number of bytes from MCU to NAND flash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        [IN] Data to be written to NAND flash device.
*    NumBytes     Number of bytes to be written.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The NAND flash device samples the data on the rising edge of the SPI clock.
*/
typedef int FS_NAND_HW_TYPE_SPI_WRITE(U8 Unit, const void * pData, unsigned NumBytes);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_LOCK
*
*  Function description
*    Requests exclusive access to SPI bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API. The implementation of this function is optional.
*
*    The FS_NAND_PHY_SPI physical layer calls this function to indicate
*    that it needs exclusive to access the serial NAND flash device via the SPI bus.
*    It is guaranteed that the FS_NAND_PHY_SPI physical layer does not attempt
*    to exchange any data with the serial NAND flash device via the SPI bus before
*    calling this function first. It is also guaranteed that FS_NAND_HW_TYPE_SPI_LOCK
*    and FS_NAND_HW_TYPE_SPI_UNLOCK are called in pairs. Typically, this function
*    is used for synchronizing the access to SPI bus when the SPI bus is shared
*    between the serial NAND flash and other SPI devices.
*
*    A possible implementation would make use of an OS semaphore that is
*    acquired in FS_NAND_HW_TYPE_SPI_LOCK and released in FS_NAND_HW_TYPE_SPI_UNLOCK.
*/
typedef void FS_NAND_HW_TYPE_SPI_LOCK(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI_UNLOCK
*
*  Function description
*    Releases the exclusive access of SPI bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_SPI NAND hardware
*    layer API. The implementation of this function is optional.
*
*    The FS_NAND_PHY_SPI physical layer calls this function after it no
*    longer needs to access the serial NAND flash device via the SPI bus.
*    It is guaranteed that the FS_NAND_PHY_SPI physical layer does not
*    attempt to exchange any data with the serial NAND flash device via
*    the SPI bus before calling FS_NAND_HW_TYPE_SPI_LOCK.
*    It is also guaranteed that FS_NAND_HW_TYPE_SPI_UNLOCK and
*    FS_NAND_HW_TYPE_SPI_LOCK are called in pairs.
*
*    FS_NAND_HW_TYPE_SPI_UNLOCK and FS_NAND_HW_TYPE_SPI_LOCK can be used to
*    synchronize the access to the SPI bus when other devices than the
*    serial NAND flash are connected to it. A possible implementation would
*    make use of an OS semaphore that is acquired FS_NAND_HW_TYPE_SPI_LOCK
*    and released in FS_NAND_HW_TYPE_SPI_UNLOCK.
*/
typedef void FS_NAND_HW_TYPE_SPI_UNLOCK(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_SPI
*
*  Description
*    NAND hardware layer API for NAND flash devices connected via SPI.
*/
typedef struct {
  FS_NAND_HW_TYPE_SPI_INIT       * pfInit;                      // Initializes the hardware.
  FS_NAND_HW_TYPE_SPI_DISABLE_CS * pfDisableCS;                 // Disables the NAND flash device.
  FS_NAND_HW_TYPE_SPI_ENABLE_CS  * pfEnableCS;                  // Enables the NAND flash device.
  FS_NAND_HW_TYPE_SPI_DELAY      * pfDelay;                     // Block the execution for a specified number of milliseconds.
  FS_NAND_HW_TYPE_SPI_READ       * pfRead;                      // Transfers a specified number of bytes from NAND flash device to MCU.
  FS_NAND_HW_TYPE_SPI_WRITE      * pfWrite;                     // Transfers a specified number of bytes from MCU to NAND flash device.
  FS_NAND_HW_TYPE_SPI_LOCK       * pfLock;                      // Request exclusive access to SPI bus.
  FS_NAND_HW_TYPE_SPI_UNLOCK     * pfUnlock;                    // Releases the exclusive access to SPI bus.
} FS_NAND_HW_TYPE_SPI;

/*********************************************************************
*
*       FS_NAND_HW_TYPE_DF_INIT
*
*  Function description
*    Initializes the hardware.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Return value
*    ==0    OK, initialization was successful.
*    !=0    An error occurred.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_DF NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type. FS_NAND_HW_TYPE_DF_INIT is the first function
*    of the hardware layer API that is called by a NAND physical layer
*    during the mounting of the file system.
*
*    This function has to perform any initialization of the MCU hardware
*    required to access the DataFlash device such as clocks, port pins,
*    memory controllers, etc.
*
*    A DataFlash requires that the SPI communication protocol meets the
*    following requirements:
*    \item 8-bit data length.\n
*    \item The most significant bit hast to be sent out first.\n
*    \item Chip Select (CS) signal should be initially high to disable the DataFlash device.\n
*    \item The SPI clock frequency does not have to exceed the maximum
*          clock frequency that is specified by the DataFlash device (Usually: 20MHz).
*/
typedef int FS_NAND_HW_TYPE_DF_INIT(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_DF_ENABLE_CS
*
*  Function description
*    Enables the DataFlash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_DF NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The DataFlash device is enabled by driving the Chip Select (CS)
*    signal to logic low state.
*/
typedef void FS_NAND_HW_TYPE_DF_ENABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_DF_DISABLE_CS
*
*  Function description
*    Disables the DataFlash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_DF NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The DataFlash device is disabled by driving the Chip Select (CS)
*    signal to logic-high.
*/
typedef void FS_NAND_HW_TYPE_DF_DISABLE_CS(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_DF_READ
*
*  Function description
*    Transfers a specified number of bytes from DataFlash device to MCU.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        [OUT] Data read from DataFlash device.
*    NumBytes     Number of bytes to be read.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_DF NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*/
typedef void FS_NAND_HW_TYPE_DF_READ(U8 Unit, U8 * pData, int NumBytes);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_DF_WRITE
*
*  Function description
*    Transfers a specified number of bytes from MCU to DataFlash device.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    pData        [IN] Data to be written to DataFlash device.
*    NumBytes     Number of bytes to be written.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_DF NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*/
typedef void FS_NAND_HW_TYPE_DF_WRITE(U8 Unit, const U8 * pData, int NumBytes);

  /*********************************************************************
*
*       FS_NAND_HW_TYPE_DF
*
*  Description
*    NAND hardware layer API for DataFlash devices.
*/
typedef struct {
  FS_NAND_HW_TYPE_DF_INIT       * pfInit;                       // Initializes the hardware.
  FS_NAND_HW_TYPE_DF_ENABLE_CS  * pfEnableCS;                   // Enables the DataFlash device.
  FS_NAND_HW_TYPE_DF_DISABLE_CS * pfDisableCS;                  // Disables the DataFlash device.
  FS_NAND_HW_TYPE_DF_READ       * pfRead;                       // Transfers a specified number of bytes from DataFlash device to MCU.
  FS_NAND_HW_TYPE_DF_WRITE      * pfWrite;                      // Transfers a specified number of bytes from MCU to DataFlash device.
} FS_NAND_HW_TYPE_DF;

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_INIT
*
*  Function description
*    Initializes the hardware.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Return value
*    Frequency of the QSPI clock in kHz.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    This function has to configure anything necessary for the data transfer
*    via QSPI such as clocks, port pins, QSPI peripheral, DMA, etc.
*
*    The frequency value returned by the function has to be greater
*    than or equal to the actual clock frequency used to transfer
*    the data via QSPI. The FS_NAND_PHY_QSPI physical layer uses this value
*    to calculate the number of software cycles it has to wait for NAND flash
*    device to finish an operation before a timeout error is reported.
*    FS_NAND_HW_TYPE_QSPI_INIT must return 0 if the clock frequency is unknown.
*    In this case, the FS_NAND_PHY_QSPI physical layer waits indefinitely for
*    the end of a NAND flash operation and it never reports a timeout error.
*/
typedef int FS_NAND_HW_TYPE_QSPI_INIT(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_EXEC_CMD
*
*  Function description
*    Executes a command without data transfer.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*    Cmd          Code of the command to be sent.
*    BusWidth     Number of data lines to be used for sending
*                 the command code.
*
*  Return value
*    ==0    OK, command sent.
*    !=0    An error occurred.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    FS_NAND_HW_TYPE_QSPI_EXEC_CMD must to send a 8 bits to NAND flash
*    device containing the value specified via Cmd.
*
*    BusWidth specifies the number of data lines to be used when sending
*    the command code. Permitted values are:
*    * 1 for standard SPI
*    * 2 for quad SPI (2 data lines)
*    * 4 for quad SPI (4 data lines)
*    Please note that this is not an encoded value such as the value
*    passed via BusWidth in a call to FS_NAND_HW_TYPE_QSPI_READ_DATA,
*    FS_NAND_HW_TYPE_QSPI_WRITE_DATA or FS_NAND_HW_TYPE_QSPI_POLL.
*/
typedef int FS_NAND_HW_TYPE_QSPI_EXEC_CMD(U8 Unit, U8 Cmd, U8 BusWidth);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_READ_DATA
*
*  Function description
*    Transfers data from NAND flash device to MCU.
*
*  Parameters
*    Unit           Index of the NAND hardware layer instance (0-based)
*    Cmd            Code of the command to be sent.
*    pPara          Additional command parameters (can be NULL).
*    NumBytesPara   Number of additional bytes to be sent after command.
*                   Can be 0 if pPara is NULL.
*    NumBytesAddr   Number of address bytes to be sent.
*                   Can be 0 if pPara is NULL.
*    pData          Data received from the NAND flash device.
*    NumBytesData   Number of bytes to be received from the NAND flash device.
*    BusWidth       Specifies the number of data lines to be used
*                   during the data transfer.
*
*  Return value
*    ==0    OK, data transferred.
*    !=0    An error occurred.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The data has to be sent in this order: Cmd (1 byte) and pPara (NumBytesPara bytes).
*    The first NumBytesAddr bytes in pPara represent the address bytes while the
*    rest until NumBytesPara bytes are dummy bytes. These values are useful for
*    QSPI hardware that can differentiate between address and dummy bytes.
*    If NumBytesPara and NumBytesAddr are equal, no dummy bytes have to be sent.
*    NumBytesPara is never be smaller than NumBytesAddr. The data received from
*    NAND flash has to be stored to pData (NumBytesData bytes).
*
*    The number of data lines to be used during the data transfer
*    is specified via BusWidth. The value is encoded with each nibble (4 bits)
*    specifying the number of data lines for one part of the data transfer.
*    The macros \ref{SPI bus width decoding} can be used to decode the number
*    of data lines of each part of the request (command and address)
*    and of the response (data). The dummy bytes have to be sent using
*    the number of data lines returned by FS_BUSWIDTH_GET_ADDR().
*/
typedef int FS_NAND_HW_TYPE_QSPI_READ_DATA(U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, U8 * pData, unsigned NumBytesData, U16 BusWidth);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_WRITE_DATA
*
*  Function description
*    Transfers data from MCU to NAND flash device.
*
*  Parameters
*    Unit          Index of the NAND hardware layer instance (0-based)
*    Cmd            Code of the command to be sent.
*    pPara          Additional command parameters (can be NULL).
*    NumBytesPara   Number of additional bytes to be sent after command.
*                   Can be 0 if pPara is NULL.
*    NumBytesAddr   Number of address bytes to be sent.
*                   Can be 0 if pPara is NULL.
*    pData          Data to be sent to NAND flash device.
*    NumBytesData   Number of data bytes to be sent to NAND flash device.
*    BusWidth       Specifies the number of data lines to be used
*                   during the data transfer.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The data has to be sent in this order: Cmd (1 byte), pPara (NumBytesPara bytes),
*    pData (NumBytesData bytes). The first NumBytesAddr bytes in pPara represent
*    the address bytes while the rest until NumBytesPara bytes are dummy bytes.
*    These values are useful for QSPI hardware that can differentiate between
*    address and dummy bytes. If NumBytesPara and NumBytesAddr are equal,
*    no dummy bytes have to be sent. NumBytesPara will never be smaller than NumBytesAddr.
*
*    The number of data lines to be used during the data transfer
*    is specified via BusWidth. The value is encoded with each nibble
*    specifying the number of data lines for one part of the data transfer.
*    The macros \ref{SPI bus width decoding} can be used to decode the number
*    of data lines of each part of the request (command, address and data).
*    The dummy bytes have to be sent using the number of data lines returned
*    by FS_BUSWIDTH_GET_ADDR().
*/
typedef int FS_NAND_HW_TYPE_QSPI_WRITE_DATA(U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, unsigned NumBytesAddr, const U8 * pData, unsigned NumBytesData, U16 BusWidth);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_POLL
*
*  Function description
*    Sends a command repeatedly and checks the response data for a specified condition.
*
*  Parameters
*    Unit           Index of the NAND hardware layer instance (0-based)
*    Cmd            Code of the command to be sent.
*    pPara          Additional command parameters (can be NULL).
*    NumBytesPara   Number of additional bytes to be sent after command.
*                   Can be 0 if pPara is NULL.
*    BitPos         Position of the bit inside response data that has
*                   to be checked (0-based, with 0 being LSB)
*    BitValue       Bit value to wait for.
*    Delay_ms       Time between two command executions.
*    TimeOut_ms     Maximum time to wait for the bit at BitPos to be set to BitValue.
*    BusWidth       Specifies how many data lines have to be used for sending
*                   the command and parameters and for reading the data.
*
*  Return value
*    > 0    Error, timeout occurred.
*    ==0    OK, bit set to specified value.
*    < 0    Error, feature not supported.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API. The implementation of this function is optional.
*    FS_NAND_HW_TYPE_QSPI_POLL is called by the QSPI NAND physical layer
*    when it has to wait for the NAND flash device to reach a certain status.
*
*    FS_NAND_HW_TYPE_QSPI_POLL has to read periodically a byte of data from
*    the NAND flash device until the the value of the bit at BitPos in the
*    in the returned data byte is set to a value specified by BitValue.
*    The data is read by sending a command with a code specified
*    by Cmd followed by an optional number of additional bytes
*    specified by pPara and NumBytesPara. Then 8 bits of data are
*    transferred from NAND flash device to MCU and this the value
*    stores the bit to be checked.
*
*    The number of data lines to be used during the data transfer
*    is specified via BusWidth. The value is encoded with each nibble
*    specifying the number of data lines for one part of the data transfer.
*    The macros \ref{SPI bus width decoding} can be used to decode the number
*    of data lines of each part of the request (command, parameters and data).
*    The pPara data has to be sent using the number of data lines returned
*    by FS_BUSWIDTH_GET_ADDR().
*
*    The time FS_NAND_HW_TYPE_QSPI_POLL waits for the condition to be true
*    shall not exceed the number of milliseconds specified by TimeOut_ms.
*    Delay_ms specifies the number of milliseconds that can elapse
*    between the execution of two consecutive commands.
*/
typedef int FS_NAND_HW_TYPE_QSPI_POLL(U8 Unit, U8 Cmd, const U8 * pPara, unsigned NumBytesPara, U8 BitPos, U8 BitValue, U32 Delay, U32 TimeOut_ms, U16 BusWidth);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_DELAY
*
*  Function description
*    Blocks the execution for the specified number of milliseconds.
*
*  Parameters
*    Unit           Index of the NAND hardware layer instance (0-based)
*    ms             Number of milliseconds to wait.
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API and it is mandatory to be implemented by any NAND hardware
*    layer of this type.
*
*    The time FS_NAND_HW_TYPE_QSPI_DELAY blocks does not have to be
*    accurate. That is the function can block the execution longer
*    that the number of specified milliseconds but no less than that.
*/
typedef void FS_NAND_HW_TYPE_QSPI_DELAY(U8 Unit, int ms);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_LOCK
*
*  Function description
*    Requests exclusive access to SPI bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API. The implementation of this function is optional.
*
*    The FS_NAND_PHY_QSPI physical layer calls this function to indicate
*    that it needs exclusive to access the NAND flash device via the SPI bus.
*    It is guaranteed that the FS_NAND_PHY_QSPI physical layer does not attempt
*    to communicate via the SPI bus before calling this function first. It is also
*    guaranteed that FS_NAND_HW_TYPE_QSPI_LOCK and FS_NAND_HW_TYPE_QSPI_UNLOCK
*    are called in pairs.
*
*    FS_NAND_HW_TYPE_QSPI_UNLOCK and FS_NAND_HW_TYPE_QSPI_LOCK can be used to
*    synchronize the access to the SPI bus when other devices than the NAND flash
*    are connected to it. A possible implementation would make use of an
*    OS semaphore that is acquired in this function and released in
*    FS_NAND_HW_TYPE_QSPI_UNLOCK.
*/
typedef void FS_NAND_HW_TYPE_QSPI_LOCK(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI_UNLOCK
*
*  Function description
*    Releases the exclusive access of SPI bus.
*
*  Parameters
*    Unit         Index of the NAND hardware layer instance (0-based)
*
*  Additional information
*    This function is a member of the FS_NAND_HW_TYPE_QSPI NAND hardware
*    layer API. The implementation of this function is optional.
*
*    The FS_NAND_PHY_QSPI physical layer calls this function when it no
*    longer needs to access the NAND flash device via the SPI bus.
*    It is guaranteed that the FS_NAND_PHY_QSPI physical layer does not
*    attempt to communicate via the SPI bus before calling FS_NAND_HW_TYPE_QSPI_LOCK.
*    It is also guaranteed that FS_NAND_HW_TYPE_QSPI_UNLOCK and
*    FS_NAND_HW_TYPE_QSPI_LOCK are called in pairs.
*
*    FS_NAND_HW_TYPE_QSPI_UNLOCK and FS_NAND_HW_TYPE_QSPI_LOCK can be used to
*    synchronize the access to the SPI bus when other devices than the NAND flash
*    are connected to it. A possible implementation would make use of an
*    OS semaphore that is acquired in this function and released in
*    FS_NAND_HW_TYPE_QSPI_UNLOCK.
*/
typedef void FS_NAND_HW_TYPE_QSPI_UNLOCK(U8 Unit);

/*********************************************************************
*
*       FS_NAND_HW_TYPE_QSPI
*
*  Description
*    NAND hardware layer API for NAND flash devices connected via quad and dual SPI.
*/
typedef struct {
  FS_NAND_HW_TYPE_QSPI_INIT       * pfInit;                     // Initializes the hardware.
  FS_NAND_HW_TYPE_QSPI_EXEC_CMD   * pfExecCmd;                  // Executes a command without data transfer.
  FS_NAND_HW_TYPE_QSPI_READ_DATA  * pfReadData;                 // Transfers data from NAND flash device to MCU.
  FS_NAND_HW_TYPE_QSPI_WRITE_DATA * pfWriteData;                // Transfers data from MCU to NAND flash device.
  FS_NAND_HW_TYPE_QSPI_POLL       * pfPoll;                     // Sends a command repeatedly and checks the response data for a specified condition.
  FS_NAND_HW_TYPE_QSPI_DELAY      * pfDelay;                    // Blocks the execution for the specified number of milliseconds.
  FS_NAND_HW_TYPE_QSPI_LOCK       * pfLock;                     // Requests exclusive access to SPI bus.
  FS_NAND_HW_TYPE_QSPI_UNLOCK     * pfUnlock;                   // Releases the exclusive access of SPI bus.
} FS_NAND_HW_TYPE_QSPI;

/*********************************************************************
*
*       FS_NAND_SPI_DEVICE_TYPE
*
*  Description
*    Operations on serial NAND devices
*/
typedef struct FS_NAND_SPI_DEVICE_TYPE FS_NAND_SPI_DEVICE_TYPE;       //lint -esym(9058, FS_NAND_SPI_DEVICE_TYPE) tag unused outside of typedefs. Rationale: this is an opaque data type.

/*********************************************************************
*
*       FS_NAND_SPI_DEVICE_LIST
*
*  Description
*    Defines a list of supported devices.
*/
typedef struct {
  U8                               NumDevices;
  const FS_NAND_SPI_DEVICE_TYPE ** ppDevice;
} FS_NAND_SPI_DEVICE_LIST;

/*********************************************************************
*
*       Available physical layers
*/
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_x;                  // 512 or 2048 byte pages,  8-bit or 16-bit interface
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_x8;                 // 512 or 2048 byte pages,  8-bit interface
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_512x8;              // 512 byte pages, 8-bit interface
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_2048x8;             // 2048 byte pages, 8-bit interface
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_2048x8_TwoPlane;    // 2048 byte pages, 8-bit interface, two-plane operation
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_2048x8_Small;       // 2048 byte pages, 8-bit interface (reduced ROM usage)
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_2048x16;            // 2048 byte pages, 16-bit interface
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_4096x8;             // 4096 byte pages, 8-bit interface
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_DataFlash;          // Physical layer for ATMEL serial DataFlash
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_ONFI;               // Physical layer for NAND flashes which support ONFI
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_ONFI_RO;            // Physical layer for NAND flashes which support ONFI (read-only mode)
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_ONFI_Small;         // Physical layer for NAND flashes which support ONFI (reduced ROM usage)
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_SPI;                // Physical layer for NAND flashes with SPI interface
extern const FS_NAND_PHY_TYPE FS_NAND_PHY_QSPI;               // Physical layer for NAND flashes with quad and dual SPI interface

/*********************************************************************
*
*       Default HW layers for the NAND flash drivers.
*/
extern const FS_NAND_HW_TYPE      FS_NAND_HW_Default;
extern const FS_NAND_HW_TYPE_SPI  FS_NAND_HW_SPI_Default;
extern const FS_NAND_HW_TYPE_DF   FS_NAND_HW_DF_Default;

/*********************************************************************
*
*       ECC calculation modules
*/
extern const FS_NAND_ECC_HOOK FS_NAND_ECC_HW_NULL;    // Uses the HW ECC of the NAND flash device
extern const FS_NAND_ECC_HOOK FS_NAND_ECC_HW_4BIT;    // Uses the HW ECC of the NAND flash controller
extern const FS_NAND_ECC_HOOK FS_NAND_ECC_HW_8BIT;    // Uses the HW ECC of the NAND flash controller
extern const FS_NAND_ECC_HOOK FS_NAND_ECC_SW_1BIT;    // 1-bit SW ECC over 512 bytes of page and 4 bytes of spare area

/*********************************************************************
*
*       Lists of supported serial NAND devices
*/
extern const FS_NAND_SPI_DEVICE_LIST FS_NAND_SPI_DeviceList_All;
extern const FS_NAND_SPI_DEVICE_LIST FS_NAND_SPI_DeviceList_Default;
extern const FS_NAND_SPI_DEVICE_LIST FS_NAND_SPI_DeviceList_ISSI;
extern const FS_NAND_SPI_DEVICE_LIST FS_NAND_SPI_DeviceList_Macronix;
extern const FS_NAND_SPI_DEVICE_LIST FS_NAND_SPI_DeviceList_Micron;
extern const FS_NAND_SPI_DEVICE_LIST FS_NAND_SPI_DeviceList_Toshiba;
extern const FS_NAND_SPI_DEVICE_LIST FS_NAND_SPI_DeviceList_Winbond;

/*********************************************************************
*
*       SLC1 NAND driver functions
*/
#if FS_NAND_SUPPORT_FAST_WRITE
  int  FS_NAND_Clean                (U8 Unit, unsigned NumBlocksFree, unsigned NumSectorsFree);
#endif // FS_NAND_SUPPORT_FAST_WRITE
int  FS_NAND_EraseBlock             (U8 Unit, unsigned BlockIndex);
int  FS_NAND_EraseFlash             (U8 Unit);
int  FS_NAND_FormatLow              (U8 Unit);
int  FS_NAND_GetBlockInfo           (U8 Unit, U32 PhyBlockIndex, FS_NAND_BLOCK_INFO * pBlockInfo);
int  FS_NAND_GetDiskInfo            (U8 Unit, FS_NAND_DISK_INFO * pDiskInfo);
#if FS_NAND_ENABLE_STATS
  void FS_NAND_GetStatCounters      (U8 Unit, FS_NAND_STAT_COUNTERS * pStat);
#endif // FS_NAND_ENABLE_STATS
int  FS_NAND_IsBlockBad             (U8 Unit, unsigned BlockIndex);
int  FS_NAND_IsLLFormatted          (U8 Unit);
int  FS_NAND_ReadPageRaw            (U8 Unit, U32 PageIndex, void * pData, unsigned NumBytes);
int  FS_NAND_ReadPhySector          (U8 Unit, U32 PhySectorIndex, void * pData, unsigned * pNumBytesData, void * pSpare, unsigned * pNumBytesSpare);
#if FS_NAND_ENABLE_STATS
  void FS_NAND_ResetStatCounters    (U8 Unit);
#endif // FS_NAND_ENABLE_STATS
void FS_NAND_SetBlockRange          (U8 Unit, U16 FirstBlock, U16 MaxNumBlocks);
#if FS_NAND_SUPPORT_FAST_WRITE
  int  FS_NAND_SetCleanThreshold    (U8 Unit, unsigned NumBlocksFree, unsigned NumSectorsFree);
#endif // FS_NAND_SUPPORT_FAST_WRITE
#if FS_NAND_VERIFY_ERASE
  void FS_NAND_SetEraseVerification (U8 Unit, U8 OnOff);
#endif // FS_NAND_VERIFY_ERASE
void FS_NAND_SetMaxEraseCntDiff     (U8 Unit, U32 EraseCntDiff);
void FS_NAND_SetNumWorkBlocks       (U8 Unit, U32 NumWorkBlocks);
void FS_NAND_SetOnFatalErrorCallback(FS_NAND_ON_FATAL_ERROR_CALLBACK * pfOnFatalError);
void FS_NAND_SetPhyType             (U8 Unit, const FS_NAND_PHY_TYPE * pPhyType);
#if FS_NAND_VERIFY_WRITE
  void FS_NAND_SetWriteVerification (U8 Unit, U8 OnOff);
#endif // FS_NAND_VERIFY_WRITE
int  FS_NAND_TestBlock              (U8 Unit, unsigned BlockIndex, U32 Pattern, FS_NAND_TEST_INFO * pInfo);
int  FS_NAND_WritePage              (U8 Unit, U32 PageIndex, const void * pData, unsigned NumBytes);
int  FS_NAND_WritePageRaw           (U8 Unit, U32 PageIndex, const void * pData, unsigned NumBytes);

/*********************************************************************
*
*       Universal NAND driver functions
*/
void FS_NAND_UNI_AllowBlankUnusedSectors       (U8 Unit, U8 OnOff);
void FS_NAND_UNI_AllowReadErrorBadBlocks       (U8 Unit, U8 OnOff);
int  FS_NAND_UNI_Clean                         (U8 Unit, unsigned NumBlocksFree, unsigned NumSectorsFree);
int  FS_NAND_UNI_EraseBlock                    (U8 Unit, unsigned BlockIndex);
int  FS_NAND_UNI_EraseFlash                    (U8 Unit);
int  FS_NAND_UNI_GetBlockInfo                  (U8 Unit, U32 BlockIndex, FS_NAND_BLOCK_INFO * pBlockInfo);
int  FS_NAND_UNI_GetDiskInfo                   (U8 Unit, FS_NAND_DISK_INFO * pDiskInfo);
#if FS_NAND_ENABLE_STATS
  void FS_NAND_UNI_GetStatCounters             (U8 Unit, FS_NAND_STAT_COUNTERS * pStat);
#endif // FS_NAND_ENABLE_STATS
int  FS_NAND_UNI_IsBlockBad                    (U8 Unit, unsigned BlockIndex);
int  FS_NAND_UNI_Mount                         (U8 Unit, FS_NAND_MOUNT_INFO * pMountInfo);
int  FS_NAND_UNI_ReadLogSectorPartial          (U8 Unit, U32 LogSectorIndex, void * pData, unsigned Off, unsigned NumBytes);
int  FS_NAND_UNI_ReadPageRaw                   (U8 Unit, U32 PageIndex, void * pData, unsigned NumBytes);
int  FS_NAND_UNI_ReadPhySector                 (U8 Unit, U32 PhySectorIndex, void * pData, unsigned * pNumBytesData, void * pSpare, unsigned * pNumBytesSpare);
#if FS_NAND_ENABLE_STATS
  void FS_NAND_UNI_ResetStatCounters           (U8 Unit);
#endif // FS_NAND_ENABLE_STATS
void FS_NAND_UNI_SetBlockRange                 (U8 Unit, U16 FirstBlock, U16 MaxNumBlocks);
void FS_NAND_UNI_SetBlockReserve               (U8 Unit, unsigned pctOfBlocks);
int  FS_NAND_UNI_SetCleanThreshold             (U8 Unit, unsigned NumBlocksFree, unsigned NumSectorsFree);
#if FS_NAND_RECLAIM_DRIVER_BAD_BLOCKS
  void FS_NAND_UNI_SetDriverBadBlockReclamation(U8 Unit, U8 OnOff);
#endif // FS_NAND_RECLAIM_DRIVER_BAD_BLOCKS
void FS_NAND_UNI_SetECCHook                    (U8 Unit, const FS_NAND_ECC_HOOK * pECCHook);
#if FS_NAND_VERIFY_ERASE
  void FS_NAND_UNI_SetEraseVerification        (U8 Unit, U8 OnOff);
#endif // FS_NAND_VERIFY_ERASE
#if FS_NAND_MAX_BIT_ERROR_CNT
  void FS_NAND_UNI_SetMaxBitErrorCnt           (U8 Unit, unsigned BitErrorCnt);
#endif // FS_NAND_MAX_BIT_ERROR_CNT
  void FS_NAND_UNI_SetMaxEraseCntDiff          (U8 Unit, U32 EraseCntDiff);
#if FS_NAND_SUPPORT_BLOCK_GROUPING
  int  FS_NAND_UNI_SetNumBlocksPerGroup        (U8 Unit, unsigned BlocksPerGroup);
#endif // FS_NAND_SUPPORT_BLOCK_GROUPING
void FS_NAND_UNI_SetNumWorkBlocks              (U8 Unit, U32 NumWorkBlocks);
void FS_NAND_UNI_SetOnFatalErrorCallback       (FS_NAND_ON_FATAL_ERROR_CALLBACK * pfOnFatalError);
void FS_NAND_UNI_SetPhyType                    (U8 Unit, const FS_NAND_PHY_TYPE * pPhyType);
#if FS_NAND_MAX_BIT_ERROR_CNT
  void FS_NAND_UNI_SetWriteDisturbHandling     (U8 Unit, U8 OnOff);
#endif // FS_NAND_MAX_BIT_ERROR_CNT
#if FS_NAND_VERIFY_WRITE
  void FS_NAND_UNI_SetWriteVerification        (U8 Unit, U8 OnOff);
#endif // FS_NAND_VERIFY_WRITE
int  FS_NAND_UNI_TestBlock                     (U8 Unit, unsigned BlockIndex, U32 Pattern, FS_NAND_TEST_INFO * pInfo);
int  FS_NAND_UNI_WritePage                     (U8 Unit, U32 PageIndex, const void * pData, unsigned NumBytes);
int  FS_NAND_UNI_WritePageRaw                  (U8 Unit, U32 PageIndex, const void * pData, unsigned NumBytes);

/*********************************************************************
*
*       NAND physical layer specific functions
*/
int  FS_NAND_PHY_ReadDeviceId           (U8 Unit, U8 * pId, U32 NumBytes);
int  FS_NAND_PHY_ReadONFIPara           (U8 Unit, void * pPara);
void FS_NAND_PHY_SetHWType              (U8 Unit, const FS_NAND_HW_TYPE * pHWType);

/*********************************************************************
*
*       2048x16 physical layer
*/
void FS_NAND_2048x16_SetHWType          (U8 Unit, const FS_NAND_HW_TYPE * pHWType);

/*********************************************************************
*
*       2048x8 physical layer
*/
#if FS_NAND_SUPPORT_READ_CACHE
  void FS_NAND_2048x8_EnableReadCache   (U8 Unit);
  void FS_NAND_2048x8_DisableReadCache  (U8 Unit);
#endif // FS_NAND_SUPPORT_READ_CACHE
void   FS_NAND_2048x8_SetHWType         (U8 Unit, const FS_NAND_HW_TYPE * pHWType);

/*********************************************************************
*
*       4096x8 physical layer
*/
void FS_NAND_4096x8_SetHWType           (U8 Unit, const FS_NAND_HW_TYPE * pHWType);

/*********************************************************************
*
*       512x8 physical layer
*/
void FS_NAND_512x8_SetHWType            (U8 Unit, const FS_NAND_HW_TYPE * pHWType);

/*********************************************************************
*
*       x8 physical layer
*/
void FS_NAND_x8_SetHWType               (U8 Unit, const FS_NAND_HW_TYPE * pHWType);
#if (FS_NAND_SUPPORT_AUTO_DETECTION == 0)
  void FS_NAND_x8_Configure             (U8 Unit, unsigned NumBlocks, unsigned PagesPerBlock, unsigned BytesPerPage, unsigned BytesPerSpareArea);
#endif // FS_NAND_SUPPORT_AUTO_DETECTION == 0

/*********************************************************************
*
*       x physical layer
*/
void FS_NAND_x_SetHWType                (U8 Unit, const FS_NAND_HW_TYPE * pHWType);
#if (FS_NAND_SUPPORT_AUTO_DETECTION == 0)
  void FS_NAND_x_Configure              (U8 Unit, unsigned NumBlocks, unsigned PagesPerBlock, unsigned BytesPerPage, unsigned BytesPerSpareArea, unsigned DataBusWidth);
#endif // FS_NAND_SUPPORT_AUTO_DETECTION == 0

/*********************************************************************
*
*       ONFI physical layer
*/
void FS_NAND_ONFI_SetHWType             (U8 Unit, const FS_NAND_HW_TYPE * pHWType);

/*********************************************************************
*
*       SPI physical layer
*/
#if FS_NAND_SUPPORT_READ_CACHE
  void FS_NAND_SPI_EnableReadCache      (U8 Unit);
  void FS_NAND_SPI_DisableReadCache     (U8 Unit);
#endif // FS_NAND_SUPPORT_READ_CACHE
void FS_NAND_SPI_SetHWType              (U8 Unit, const FS_NAND_HW_TYPE_SPI * pHWType);
void FS_NAND_SPI_SetDeviceList          (U8 Unit, const FS_NAND_SPI_DEVICE_LIST * pDeviceList);

/*********************************************************************
*
*       DataFlash physical layer
*/
void FS_NAND_DF_EraseChip               (U8 Unit);
void FS_NAND_DF_SetMinPageSize          (U8 Unit, U32 NumBytes);
void FS_NAND_DF_SetHWType               (U8 Unit, const FS_NAND_HW_TYPE_DF * pHWType);

/*********************************************************************
*
*       QSPI physical layer
*/
#if FS_NAND_SUPPORT_READ_CACHE
  void FS_NAND_QSPI_EnableReadCache     (U8 Unit);
  void FS_NAND_QSPI_DisableReadCache    (U8 Unit);
#endif // FS_NAND_SUPPORT_READ_CACHE
void FS_NAND_QSPI_SetHWType             (U8 Unit, const FS_NAND_HW_TYPE_QSPI * pHWType);
void FS_NAND_QSPI_Allow2bitMode         (U8 Unit, U8 OnOff);
void FS_NAND_QSPI_Allow4bitMode         (U8 Unit, U8 OnOff);

/*********************************************************************
*
*       WinDrive driver
*/
#ifdef _WIN32
  void FS_WINDRIVE_Configure  (U8 Unit, const char * sWindowsDriveName);
  void FS_WINDRIVE_ConfigureEx(U8 Unit, LPCWSTR      sWindowsDriveName);
  int  FS_WINDRIVE_SetGeometry(U8 Unit, U32 BytesPerSector, U32 NumSectors);
#endif // _WIN32

/*********************************************************************
*
*       Logical drivers
*/

/*********************************************************************
*
*       Logical volume
*/
#if (FS_LOGVOL_SUPPORT_DRIVER_MODE == 0)
int  FS_LOGVOL_Create     (const char * sVolumeName);
int  FS_LOGVOL_AddDevice  (const char * sVolumeName, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, U32 StartSector, U32 NumSectors);
#else
int  FS_LOGVOL_AddDeviceEx(U8 Unit, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, U32 StartSector, U32 NumSectors);
#endif // FS_LOGVOL_SUPPORT_DRIVER_MODE == 0

/*********************************************************************
*
*       MBR disk partition
*/
void FS_DISKPART_Configure(U8 Unit, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, U8 PartIndex);

/*********************************************************************
*
*       Cryptography
*/

/*********************************************************************
*
*       FS_CRYPT_ALGO_TYPE
*
*  Description
*    Encryption algorithm API.
*/
typedef struct {
  void (*pfPrepare)(void * pContext, const U8 * pKey);
  void (*pfEncrypt)(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex);
  void (*pfDecrypt)(void * pContext, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 BlockIndex);
  U16    BitsPerBlock;     // Number of bits encrypted at once by the algorithm: AES -> 128, DES -> 64.
} FS_CRYPT_ALGO_TYPE;

/*********************************************************************
*
*       FS_CRYPT_OBJ
*
*  Description
*    Stores information about the encryption operation.
*/
typedef struct {
  void                     * pContext;
  const FS_CRYPT_ALGO_TYPE * pAlgoType;
  U16                        ldBytesPerBlock;
} FS_CRYPT_OBJ;

/*********************************************************************
*
*       FS_CRYPT_ALGO_CONTEXT_DES
*
*  Description
*    Data used by the DES encryption algorithm.
*/
typedef struct {
  U32 aRoundKey[32];
} FS_CRYPT_ALGO_CONTEXT_DES;

/*********************************************************************
*
*       FS_CRYPT_ALGO_CONTEXT_AES
*
*  Description
*    Data used by the AES128 and AES256 encryption algorithms.
*/
typedef struct {
  U32 aRoundKeyEnc[60];
  U32 aRoundKeyDec[60];
} FS_CRYPT_ALGO_CONTEXT_AES;

extern const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_DES;      // DES encryption algorithm (56-bit key)
extern const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_AES128;   // AES encryption algorithm (128-bit key)
extern const FS_CRYPT_ALGO_TYPE FS_CRYPT_ALGO_AES256;   // AES encryption algorithm (256-bit key)

void FS_CRYPT_Configure(U8 Unit, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, const FS_CRYPT_ALGO_TYPE * pAlgoType, void * pContext, const U8 * pKey);
void FS_CRYPT_Prepare  (FS_CRYPT_OBJ * pCryptObj, const FS_CRYPT_ALGO_TYPE * pAlgoType, void * pContext, U32 BytesPerBlock, const U8 * pKey);
void FS_CRYPT_Encrypt  (const FS_CRYPT_OBJ * pCryptObj, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 * pBlockIndex);
void FS_CRYPT_Decrypt  (const FS_CRYPT_OBJ * pCryptObj, U8 * pDest, const U8 * pSrc, U32 NumBytes, U32 * pBlockIndex);

#if FS_SUPPORT_ENCRYPTION
  int FS_SetEncryptionObject(FS_FILE * pFile, FS_CRYPT_OBJ * pCryptObj);
#endif

/*********************************************************************
*
*       Encryption performance test routines
*/
void FS_CRYPT_DES_Encrypt1MB   (void);
void FS_CRYPT_DES_Decrypt1MB   (void);
void FS_CRYPT_AES256_Encrypt1MB(void);
void FS_CRYPT_AES256_Decrypt1MB(void);
void FS_CRYPT_AES128_Encrypt1MB(void);
void FS_CRYPT_AES128_Decrypt1MB(void);

/*********************************************************************
*
*       Sector read ahead driver
*/
typedef struct {
  U32 ReadSectorCnt;
  U32 ReadSectorCachedCnt;
} FS_READAHEAD_STAT_COUNTERS;

void FS_READAHEAD_Configure          (U8 Unit, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, U32 * pData, U32 NumBytes);
#if FS_READAHEAD_ENABLE_STATS
  void FS_READAHEAD_GetStatCounters  (U8 Unit, FS_READAHEAD_STAT_COUNTERS * pStat);
  void FS_READAHEAD_ResetStatCounters(U8 Unit);
#endif // FS_READAHEAD_ENABLE_STATS

/*********************************************************************
*
*       RAID operating modes
*
*  Description
*    Operating modes of a RAID logical driver.
*
*  Additional information
*    The operating mode of RAID5 logical driver can be queried via
*    the FS_RAID5_GetOperatingMode() API function.
*/
#define FS_RAID_OPERATING_MODE_NORMAL        0     // All storage devices present and operating normally.
#define FS_RAID_OPERATING_MODE_DEGRADED      1     // One storage device is not operating properly.
#define FS_RAID_OPERATING_MODE_FAILURE       2     // Two or more storage devices are not operating properly.

/*********************************************************************
*
*       Disk mirroring driver
*/
void FS_RAID1_Configure      (U8 Unit, const FS_DEVICE_TYPE * pDeviceType0, U8 DeviceUnit0, const FS_DEVICE_TYPE * pDeviceType1, U8 DeviceUnit1);
void FS_RAID1_SetSectorRanges(U8 Unit, U32 NumSectors, U32 StartSector0, U32 StartSector1);
void FS_RAID1_SetSyncBuffer  (U8 Unit, void * pBuffer, U32 NumBytes);
void FS_RAID1_SetSyncSource  (U8 Unit, unsigned StorageIndex);

/*********************************************************************
*
*       Driver for disk stripping with parity checking
*/
void FS_RAID5_AddDevice       (U8 Unit, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, U32 StartSector);
void FS_RAID5_SetNumSectors   (U8 Unit, U32 NumSectors);
int  FS_RAID5_GetOperatingMode(U8 Unit);

/*********************************************************************
*
*       Sector size conversion driver
*/
void FS_SECSIZE_Configure(U8 Unit, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, U16 BytesPerSector);

/*********************************************************************
*
*       Sector write buffer
*/

#define FS_SIZEOF_WRBUF_SECTOR_INFO               sizeof(FS_WRBUF_SECTOR_INFO)

/*********************************************************************
*
*       Write buffer size
*
*  Description
*    Calculates the write buffer size.
*
*  Additional information
*    This define can be used in an application to calculate the number
*    of bytes that have to be allocated in order to store the specified
*    number of logical sectors. \tt{NumSectors} specifies the number
*    of logical sectors while \tt{BytesPerSector} the size of a logical
*    sector in bytes.
*/
#define FS_SIZEOF_WRBUF(NumSectors, BytesPerSector)     ((FS_SIZEOF_WRBUF_SECTOR_INFO + (BytesPerSector)) * NumSectors)    // Calculates the write buffer size.

/*********************************************************************
*
*       Write buffer API functions
*/
void FS_WRBUF_Configure(U8 Unit, const FS_DEVICE_TYPE * pDeviceType, U8 DeviceUnit, void * pBuffer, U32 NumBytes);

/*********************************************************************
*
*       Sector cache
*/
#if FS_SUPPORT_CACHE

/*********************************************************************
*
*       General Cache mode defines, do not use in application
*/
#define FS_CACHE_MODE_W         0x02u
#define FS_CACHE_MODE_D         0x04u

/*********************************************************************
*
*       Sector cache modes
*
*  Description
*    Operating modes of sector caches.
*
*  Additional information
*    The operating mode of a cache module can be configured via
*    the FS_CACHE_SetMode() function separately for each sector type.
*/
#define FS_CACHE_MODE_R         0x01u                                                    // Pure read cache.
#define FS_CACHE_MODE_WT        (FS_CACHE_MODE_R | FS_CACHE_MODE_W)                     // Write-through cache.
#define FS_CACHE_MODE_WB        (FS_CACHE_MODE_R | FS_CACHE_MODE_W | FS_CACHE_MODE_D)   // Write-back cache.

/*********************************************************************
*
*       Sector cache types
*
*  Description
*    Types of sector caches.
*
*  Additional information
*    The type of a cache module can be configured via FS_AssignCache()
*    function when the sector cache is enabled for a specified volume.
*/
#define FS_CACHE_NONE           NULL                    // Pseudo-type that can be used to disable the sector cache.
#define FS_CACHE_ALL            FS_CacheAll_Init        // A pure read cache.
#define FS_CACHE_MAN            FS_CacheMan_Init        // A pure read cache that caches only the management sectors.
#define FS_CACHE_RW             FS_CacheRW_Init         // A read / write cache module.
#define FS_CACHE_RW_QUOTA       FS_CacheRWQuota_Init    // A read / write cache module with configurable capacity per sector type.
#define FS_CACHE_MULTI_WAY      FS_CacheMultiWay_Init   // A read / write cache module with configurable associativity level.

/*********************************************************************
*
*       Sizes of internal data structures used by the cache modules.
*/
#define FS_SIZEOF_CACHE_ALL_DATA                sizeof(CACHE_ALL_DATA)
#define FS_SIZEOF_CACHE_ALL_BLOCK_INFO          sizeof(CACHE_ALL_BLOCK_INFO)
#define FS_SIZEOF_CACHE_MAN_DATA                sizeof(CACHE_MAN_DATA)
#define FS_SIZEOF_CACHE_MAN_BLOCK_INFO          sizeof(CACHE_MAN_BLOCK_INFO)
#define FS_SIZEOF_CACHE_RW_DATA                 sizeof(CACHE_RW_DATA)
#define FS_SIZEOF_CACHE_RW_BLOCK_INFO           sizeof(CACHE_RW_BLOCK_INFO)
#define FS_SIZEOF_CACHE_RW_QUOTA_DATA           sizeof(CACHE_RW_QUOTA_DATA)
#define FS_SIZEOF_CACHE_RW_QUOTA_BLOCK_INFO     sizeof(CACHE_RW_QUOTA_BLOCK_INFO)
#define FS_SIZEOF_CACHE_MULTI_WAY_DATA          sizeof(CACHE_MULTI_WAY_DATA)
#define FS_SIZEOF_CACHE_MULTI_WAY_BLOCK_INFO    sizeof(CACHE_MULTI_WAY_BLOCK_INFO)

/*********************************************************************
*
*       Sector cache size
*
*  Description
*    Calculates the cache size.
*
*  Additional information
*    These defines can be used to calculate the size of the memory
*    area to be assigned to a cache module based on the size of a
*    sector (\tt SectorSize) and the number of sectors to be cached
*    (\tt NumSectors). The sector size of a specific volume can be
*    queried via FS_GetVolumeInfo().
*/
#define FS_SIZEOF_CACHE_ALL(NumSectors, SectorSize)         (FS_SIZEOF_CACHE_ALL_DATA +               \
                                                              (FS_SIZEOF_CACHE_ALL_BLOCK_INFO +       \
                                                              (SectorSize)) * (NumSectors))                     // Calculates the cache size of a FS_CACHE_ALL cache module.
#define FS_SIZEOF_CACHE_MAN(NumSectors, SectorSize)         (FS_SIZEOF_CACHE_MAN_DATA +               \
                                                              (FS_SIZEOF_CACHE_MAN_BLOCK_INFO +       \
                                                              (SectorSize)) * (NumSectors))                     // Calculates the cache size of a FS_CACHE_MAN cache module.
#define FS_SIZEOF_CACHE_RW(NumSectors, SectorSize)          (FS_SIZEOF_CACHE_RW_DATA +                \
                                                              (FS_SIZEOF_CACHE_RW_BLOCK_INFO +        \
                                                              (SectorSize)) * (NumSectors))                     // Calculates the cache size of a FS_CACHE_RW cache module.
#define FS_SIZEOF_CACHE_RW_QUOTA(NumSectors, SectorSize)    (FS_SIZEOF_CACHE_RW_QUOTA_DATA +          \
                                                              (FS_SIZEOF_CACHE_RW_QUOTA_BLOCK_INFO +  \
                                                              (SectorSize)) * (NumSectors))                     // Calculates the cache size of a FS_CACHE_RW_QUOTA cache module.
#define FS_SIZEOF_CACHE_MULTI_WAY(NumSectors, SectorSize)   (FS_SIZEOF_CACHE_MULTI_WAY_DATA +         \
                                                              (FS_SIZEOF_CACHE_MULTI_WAY_BLOCK_INFO + \
                                                              (SectorSize)) * (NumSectors))                     // Calculates the cache size of a FS_CACHE_MULTI_WAY cache module.
#define FS_SIZEOF_CACHE_ANY(NumSectors, SectorSize)         FS_SIZEOF_CACHE_RW_QUOTA(NumSectors, SectorSize)    // Calculates the size of cache that works with any cache module.

/*********************************************************************
*
*       Cache specific types
*/
typedef U32    FS_INIT_CACHE (FS_DEVICE * pDevice, void * pData, I32 NumBytes);
typedef U32 (* FS_CACHE_TYPE)(FS_DEVICE * pDevice, void * pData, I32 NumBytes);

/*********************************************************************
*
*       Cache specific prototypes
*/
U32    FS_AssignCache        (const char * sVolumeName, void * pData, I32 NumBytes, FS_INIT_CACHE * pfInit);
void   FS_CACHE_Clean        (const char * sVolumeName);
int    FS_CACHE_Command      (const char * sVolumeName, int   Cmd,      void * pData);
int    FS_CACHE_SetMode      (const char * sVolumeName, int   TypeMask, int    ModeMask);
int    FS_CACHE_SetQuota     (const char * sVolumeName, int   TypeMask, U32    NumSectors);
int    FS_CACHE_SetAssocLevel(const char * sVolumeName, int   AssocLevel);
int    FS_CACHE_GetNumSectors(const char * sVolumeName, U32 * pNumSectors);
int    FS_CACHE_Invalidate   (const char * sVolumeName);

/*********************************************************************
*
*       Cache initialization prototypes
*/
U32 FS_CacheAll_Init     (FS_DEVICE * pDevice, void * pData, I32 NumBytes);
U32 FS_CacheMan_Init     (FS_DEVICE * pDevice, void * pData, I32 NumBytes);
U32 FS_CacheRW_Init      (FS_DEVICE * pDevice, void * pData, I32 NumBytes);
U32 FS_CacheRWQuota_Init (FS_DEVICE * pDevice, void * pData, I32 NumBytes);
U32 FS_CacheMultiWay_Init(FS_DEVICE * pDevice, void * pData, I32 NumBytes);

#endif // FS_SUPPORT_CACHE

/*********************************************************************
*
*       File system selection prototype
*/
#if FS_SUPPORT_MULTIPLE_FS
  int FS_SetFSType(const char * sVolumeName, int FSType);
  int FS_GetFSType(const char * sVolumeName);
#endif // FS_SUPPORT_MULTIPLE_FS

/*********************************************************************
*
*       Memory allocation functions
*/
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
  #define FS_ALLOC_ZEROED(NumBytes, sDesc)              FS_AllocZeroedEx(NumBytes, sDesc)
#else
  #define FS_ALLOC_ZEROED(NumBytes, sDesc)              FS_AllocZeroed(NumBytes)
#endif

#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
  #define FS_ALLOC_ZEROED_PTR(ppMem, NumBytes, sDesc)   FS_AllocZeroedPtrEx(ppMem, NumBytes, sDesc)
#else
  #define FS_ALLOC_ZEROED_PTR(ppMem, NumBytes, sDesc)   FS_AllocZeroedPtr(ppMem, NumBytes)
#endif

#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
  #define FS_TRY_ALLOC(NumBytes, sDesc)                 FS_TryAllocEx(NumBytes, sDesc)
#else
  #define FS_TRY_ALLOC(NumBytes, sDesc)                 FS_TryAlloc(NumBytes)
#endif

#if FS_SUPPORT_DEINIT
  #define  FS_FREE(pMem)                                FS_Free(pMem)
#else
  #define  FS_FREE(pMem)
#endif

void * FS_Alloc         (I32     NumBytes);
void * FS_AllocZeroed   (I32     NumBytes);
void   FS_AllocZeroedPtr(void ** pp, I32 NumBytes);
void * FS_TryAlloc      (I32     NumBytes);
void * FS_GetFreeMem    (I32  *  pNumBytes);
#if FS_SUPPORT_DEINIT
  void FS_Free          (void *  pData);
#endif // FS_SUPPORT_DEINIT

#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
  void * FS_AllocEx         (I32     NumBytes,         const char * sDesc);
  void * FS_AllocZeroedEx   (I32     NumBytes,         const char * sDesc);
  void   FS_AllocZeroedPtrEx(void ** pp, I32 NumBytes, const char * sDesc);
  void * FS_TryAllocEx      (I32     NumBytes,         const char * sDesc);
#endif // FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL

/*********************************************************************
*
*       File system locking
*/
#if (FS_OS_LOCKING == FS_OS_LOCKING_API)
  void FS_Lock        (void);
  void FS_Unlock      (void);
#endif // (FS_OS_LOCKING == FS_OS_LOCKING_API)
#if (FS_OS_LOCKING == FS_OS_LOCKING_DRIVER)
  void FS_LockVolume  (const char * sVolumeName);
  void FS_UnlockVolume(const char * sVolumeName);
#endif // (FS_OS_LOCKING == FS_OS_LOCKING_DRIVER)

/*********************************************************************
*
*       Instrumentation
*/

/*********************************************************************
*
*       FS_PROFILE_API
*/
typedef struct {
  void (*pfRecordEndCall)   (unsigned EventId);
  void (*pfRecordEndCallU32)(unsigned EventId, U32 Para0);
  void (*pfRecordVoid)      (unsigned EventId);
  void (*pfRecordU32)       (unsigned EventId, U32 Para0);
  void (*pfRecordU32x2)     (unsigned EventId, U32 Para0, U32 Para1);
  void (*pfRecordU32x3)     (unsigned EventId, U32 Para0, U32 Para1, U32 Para2);
  void (*pfRecordU32x4)     (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3);
  void (*pfRecordU32x5)     (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4);
  void (*pfRecordU32x6)     (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5);
  void (*pfRecordU32x7)     (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6);
  void (*pfRecordString)    (unsigned EventId, const char * pPara0);
  void (*pfRecordStringx2)  (unsigned EventId, const char * pPara0, const char * pPara1);
} FS_PROFILE_API;

U32  FS_PROFILE_GetAPIDesc(const char ** psDesc);
void FS_PROFILE_SetAPI    (const FS_PROFILE_API * pAPI, U32 IdOffset);

#if FS_SUPPORT_PROFILE
  void FS_SYSVIEW_Init(void);
#endif

/*********************************************************************
*
*       Debug message management
*/
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)
  void FS_AddErrorFilter(U32 FilterMask);
  void FS_SetErrorFilter(U32 FilterMask);
  U32  FS_GetErrorFilter(void);
#endif // FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS)
  void FS_AddWarnFilter (U32 FilterMask);
  void FS_SetWarnFilter (U32 FilterMask);
  U32  FS_GetWarnFilter (void);
#endif // FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
  void FS_AddLogFilter  (U32 FilterMask);
  void FS_SetLogFilter  (U32 FilterMask);
  U32  FS_GetLogFilter  (void);
#endif // FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL

/*********************************************************************
*
*       Application supplied functions FS_X_...
*/

/*********************************************************************
*
*       Debug support
*/
void FS_X_Log     (const char * s);
void FS_X_Warn    (const char * s);
void FS_X_ErrorOut(const char * s);

/*********************************************************************
*
*       Misc. functions
*/
void FS_X_Panic      (int ErrorCode);
void FS_X_AddDevices (void);
U32  FS_X_GetTimeDate(void);

/*********************************************************************
*
*       Compatibility defines and functions
*/

/*********************************************************************
*
*       Deprecated error codes
*/
#define FS_ERR_OK                       FS_ERRCODE_OK
#define FS_ERR_EOF                      FS_ERRCODE_EOF
#define FS_ERR_CMDNOTSUPPORTED          (-4)
#define FS_ERR_DISKFULL                 FS_ERRCODE_VOLUME_FULL
#define FS_ERR_INVALIDPAR               FS_ERRCODE_INVALID_PARA
#define FS_ERR_WRITEONLY                FS_ERRCODE_WRITE_ONLY_FILE
#define FS_ERR_READONLY                 FS_ERRCODE_READ_ONLY_FILE
#define FS_ERR_READERROR                FS_ERRCODE_READ_FAILURE
#define FS_ERR_WRITEERROR               FS_ERRCODE_WRITE_FAILURE
#define FS_ERROR_ALLOC                  FS_ERRCODE_OUT_OF_MEMORY
#define FS_ERROR_INVALID_PARA           FS_ERRCODE_INVALID_PARA
#define FS_ERROR_UNKNOWN_DEVICE         FS_ERRCODE_UNKNOWN_DEVICE

/*********************************************************************
*
*       Cache API
*/
#define FS_CACHEALL_API             FS_CacheAll_Init
#define FS_CACHEMAN_API             FS_CacheMan_Init
#define FS_CACHE_MODE_FULL          (FS_CACHE_MODE_R | FS_CACHE_MODE_W | FS_CACHE_MODE_D)

/*********************************************************************
*
*       Configuration API
*/
#define FS_WriteUpdateDir(OnOff)                    FS_ConfigUpdateDirOnWrite(OnOff)
#define FS_ConfigFileEncryption(pFile, pCryptObj)   FS_SetEncryptionObject(pFile, pCryptObj)
#define FS_ConfigFileBufferFlags(pFile, Flags)      FS_SetFileBufferFlags(pFile, Flags)
#define WINDRIVE_Configure(Unit, sWindowsDriveName) FS_WINDRIVE_Configure(Unit, sWindowsDriveName)
#define FS_ConfigUpdateDirOnWrite(OnOff)            FS_ConfigOnWriteDirUpdate(OnOff)

/*********************************************************************
*
*       File API
*/
#define FS_FFlush(pFile)            FS_SyncFile(pFile)
#define FS_FILE_CURRENT             FS_SEEK_CUR
#define FS_FILE_END                 FS_SEEK_END
#define FS_FILE_BEGIN               FS_SEEK_SET

/*********************************************************************
*
*       NAND driver
*/
#define FS_NAND_ON_FATAL_ERROR_CB                       FS_NAND_ON_FATAL_ERROR_CALLBACK
#define FS_NAND_SetOnFatalErrorCB(pfOnFatalError)       FS_NAND_SetOnFatalErrorCallback(pfOnFatalError)
#define FS_NAND_ReadDeviceId(Unit, pId, NumBytes)       FS_NAND_PHY_ReadDeviceId(Unit, pId, NumBytes)
#define FS_NAND_ECC_NULL                                FS_NAND_ECC_HW_NULL
#define FS_NAND_ECC_1BIT                                FS_NAND_ECC_SW_1BIT
#define FS_NAND_UNI_SetOnFatalErrorCB(pfOnFatalError)   FS_NAND_UNI_SetOnFatalErrorCallback(pfOnFatalError)
#define NAND_BLOCK_TYPE_UNKNOWN                         FS_NAND_BLOCK_TYPE_UNKNOWN
#define NAND_BLOCK_TYPE_BAD                             FS_NAND_BLOCK_TYPE_BAD
#define NAND_BLOCK_TYPE_EMPTY                           FS_NAND_BLOCK_TYPE_EMPTY
#define NAND_BLOCK_TYPE_WORK                            FS_NAND_BLOCK_TYPE_WORK
#define NAND_BLOCK_TYPE_DATA                            FS_NAND_BLOCK_TYPE_DATA
#define FS_DF_ChipErase(Unit)                           FS_NAND_DF_EraseChip(Unit)

int  FS_DF_HW_X_Init     (U8 Unit);
void FS_DF_HW_X_EnableCS (U8 Unit);
void FS_DF_HW_X_DisableCS(U8 Unit);
void FS_DF_HW_X_Read     (U8 Unit,       U8 * pData, int NumBytes);
void FS_DF_HW_X_Write    (U8 Unit, const U8 * pData, int NumBytes);

int  FS_NAND_HW_SPI_X_Init     (U8 Unit);
void FS_NAND_HW_SPI_X_DisableCS(U8 Unit);
void FS_NAND_HW_SPI_X_EnableCS (U8 Unit);
void FS_NAND_HW_SPI_X_Delay    (U8 Unit, int ms);
int  FS_NAND_HW_SPI_X_Read     (U8 Unit,       void * pData, unsigned NumBytes);
int  FS_NAND_HW_SPI_X_Write    (U8 Unit, const void * pData, unsigned NumBytes);

void FS_NAND_HW_X_Init_x8         (U8 Unit);
void FS_NAND_HW_X_Init_x16        (U8 Unit);
void FS_NAND_HW_X_DisableCE       (U8 Unit);
void FS_NAND_HW_X_EnableCE        (U8 Unit);
void FS_NAND_HW_X_SetAddrMode     (U8 Unit);
void FS_NAND_HW_X_SetCmdMode      (U8 Unit);
void FS_NAND_HW_X_SetDataMode     (U8 Unit);
int  FS_NAND_HW_X_WaitWhileBusy   (U8 Unit, unsigned us);
void FS_NAND_HW_X_Read_x8         (U8 Unit,       void * pBuffer, unsigned NumBytes);
void FS_NAND_HW_X_Write_x8        (U8 Unit, const void * pBuffer, unsigned NumBytes);
void FS_NAND_HW_X_Read_x16        (U8 Unit,       void * pBuffer, unsigned NumBytes);
void FS_NAND_HW_X_Write_x16       (U8 Unit, const void * pBuffer, unsigned NumBytes);

/*********************************************************************
*
*       NOR driver
*/
#define FS_NOR_READ_CFI_FUNC                            FS_NOR_READ_CFI_CALLBACK
#define FS_NOR_CFI_SetReadCFIFunc(Unit, pfReadCFI)      FS_NOR_CFI_SetReadCFICallback(Unit, pfReadCFI)
#define FS_NOR_BM_SetUsedSectorsErasure(Unit, OnOff)    FS_NOR_BM_SetUsedSectorsErase(Unit, OnOff)

int  FS_NOR_SPI_HW_X_Init      (U8 Unit);
void FS_NOR_SPI_HW_X_EnableCS  (U8 Unit);
void FS_NOR_SPI_HW_X_DisableCS (U8 Unit);
void FS_NOR_SPI_HW_X_Read      (U8 Unit,       U8 * pData, int NumBytes);
void FS_NOR_SPI_HW_X_Write     (U8 Unit, const U8 * pData, int NumBytes);
void FS_NOR_SPI_HW_X_Read_x2   (U8 Unit,       U8 * pData, int NumBytes);
void FS_NOR_SPI_HW_X_Write_x2  (U8 Unit, const U8 * pData, int NumBytes);
void FS_NOR_SPI_HW_X_Read_x4   (U8 Unit,       U8 * pData, int NumBytes);
void FS_NOR_SPI_HW_X_Write_x4  (U8 Unit, const U8 * pData, int NumBytes);

/*********************************************************************
*
*       MMC/SD driver
*/
#define FS_MMC_CardMode_Driver                    FS_MMC_CM_Driver
#define MMC_CARD_ID                               FS_MMC_CARD_ID
void FS_MMC_HW_X_EnableCS        (U8 Unit);
void FS_MMC_HW_X_DisableCS       (U8 Unit);
int  FS_MMC_HW_X_IsPresent       (U8 Unit);
int  FS_MMC_HW_X_IsWriteProtected(U8 Unit);
U16  FS_MMC_HW_X_SetMaxSpeed     (U8 Unit, U16 MaxFreq);
int  FS_MMC_HW_X_SetVoltage      (U8 Unit, U16 Vmin,   U16 Vmax);
#if FS_MMC_REPORT_HW_ERRORS
  int  FS_MMC_HW_X_Read          (U8 Unit,       U8 * pData, int NumBytes);
  int  FS_MMC_HW_X_Write         (U8 Unit, const U8 * pData, int NumBytes);
#else
  void FS_MMC_HW_X_Read          (U8 Unit,       U8 * pData, int NumBytes);
  void FS_MMC_HW_X_Write         (U8 Unit, const U8 * pData, int NumBytes);
#endif
#if FS_MMC_SUPPORT_LOCKING
  void FS_MMC_HW_X_Lock          (U8 Unit);
  void FS_MMC_HW_X_Unlock        (U8 Unit);
#endif

void   FS_MMC_HW_X_InitHW             (U8 Unit);
void   FS_MMC_HW_X_Delay              (int ms);
void   FS_MMC_HW_X_SetResponseTimeOut (U8 Unit, U32 Value);
void   FS_MMC_HW_X_SetReadDataTimeOut (U8 Unit, U32 Value);
void   FS_MMC_HW_X_SendCmd            (U8 Unit, unsigned Cmd, unsigned CmdFlags, unsigned ResponseType, U32 Arg);
int    FS_MMC_HW_X_GetResponse        (U8 Unit, void *pBuffer, U32 Size);
int    FS_MMC_HW_X_ReadData           (U8 Unit,       void * pBuffer, unsigned NumBytes, unsigned NumBlocks);
int    FS_MMC_HW_X_WriteData          (U8 Unit, const void * pBuffer, unsigned NumBytes, unsigned NumBlocks);
void   FS_MMC_HW_X_SetDataPointer     (U8 Unit, const void * p);
void   FS_MMC_HW_X_SetHWBlockLen      (U8 Unit, U16 BlockSize);
void   FS_MMC_HW_X_SetHWNumBlocks     (U8 Unit, U16 NumBlocks);
U16    FS_MMC_HW_X_GetMaxReadBurst    (U8 Unit);
U16    FS_MMC_HW_X_GetMaxWriteBurst   (U8 Unit);

/*********************************************************************
*
*       CF card / IDE driver
*/
void FS_IDE_HW_Reset     (U8 Unit);
int  FS_IDE_HW_IsPresent (U8 Unit);
void FS_IDE_HW_Delay400ns(U8 Unit);
U16  FS_IDE_HW_ReadReg   (U8 Unit, unsigned AddrOff);
void FS_IDE_HW_WriteReg  (U8 Unit, unsigned AddrOff, U16 Data);
void FS_IDE_HW_ReadData  (U8 Unit,       U8 * pData, unsigned NumBytes);
void FS_IDE_HW_WriteData (U8 Unit, const U8 * pData, unsigned NumBytes);

/*********************************************************************
*
*       Journaling add-on
*/
#define FS_CreateJournal(sVolumeName, NumBytes)         FS_JOURNAL_Create(sVolumeName, NumBytes)
#define FS_BeginTransaction(sVolumeName)                FS_JOURNAL_Begin(sVolumeName)
#define FS_EndTransaction(sVolumeName)                  FS_JOURNAL_End(sVolumeName)

/*********************************************************************
*
*       Disk checking
*/
#define FS_ERRCODE_0FILE                      FS_CHECKDISK_ERRCODE_0FILE
#define FS_ERRCODE_SHORTEN_CLUSTER            FS_CHECKDISK_ERRCODE_SHORTEN_CLUSTER
#define FS_ERRCODE_CROSSLINKED_CLUSTER        FS_CHECKDISK_ERRCODE_CROSSLINKED_CLUSTER
#define FS_ERRCODE_FEW_CLUSTER                FS_CHECKDISK_ERRCODE_FEW_CLUSTER
#define FS_ERRCODE_CLUSTER_UNUSED             FS_CHECKDISK_ERRCODE_CLUSTER_UNUSED
#define FS_ERRCODE_CLUSTER_NOT_EOC            FS_CHECKDISK_ERRCODE_CLUSTER_NOT_EOC
#define FS_ERRCODE_INVALID_CLUSTER            FS_CHECKDISK_ERRCODE_INVALID_CLUSTER
#define FS_ERRCODE_INVALID_DIRECTORY_ENTRY    FS_CHECKDISK_ERRCODE_INVALID_DIRECTORY_ENTRY
#define FS_CHECKDISK_RETVAL_ERROR             (-1)      // Error while accessing the file system

#define FS_QUERY_F_TYPE                  FS_ON_CHECK_DISK_ERROR_CALLBACK
#define FS_ON_CHECK_DISK_ERROR_CALLBACK  FS_CHECKDISK_ON_ERROR_CALLBACK
#define FS_EFS_CheckDisk(sVolumeName, pBuffer, BufferSize, MaxRecursionLevel, pfOnError)  FS_CheckDisk(sVolumeName, pBuffer, BufferSize, MaxRecursionLevel, pfOnError)
#define FS_FAT_CheckDisk(sVolumeName, pBuffer, BufferSize, MaxRecursionLevel, pfOnError)  FS_CheckDisk(sVolumeName, pBuffer, BufferSize, MaxRecursionLevel, pfOnError)
#define FS_EFS_CheckDisk_ErrCode2Text(ErrCode)                                            FS_CheckDisk_ErrCode2Text(ErrCode)
#define FS_FAT_CheckDisk_ErrCode2Text(ErrCode)                                            FS_CheckDisk_ErrCode2Text(ErrCode)

/*********************************************************************
*
*       Space information functions
*/
#define FS_GetFreeSpace(sVolume)  FS_GetVolumeFreeSpace(sVolume)
#define FS_GetTotalSpace(sVolume) FS_GetVolumeSize(sVolume)

/*********************************************************************
*
*       FAT
*/
#define FS_FormatSD(sVolumeName)              FS_FAT_FormatSD(sVolumeName)
#define FS_FAT_ConfigMaintainFATCopy(OnOff)   FS_FAT_ConfigFATCopyMaintenance(OnOff)
#define FS_FAT_ConfigUseFSInfoSector(OnOff)   FS_FAT_ConfigFSInfoSectorUse(OnOff)

/*********************************************************************
*
*       SPI bus width handling
*/
#define FS_NOR_BUSWIDTH_CMD_SHIFT                 FS_BUSWIDTH_CMD_SHIFT
#define FS_NOR_BUSWIDTH_ADDR_SHIFT                FS_BUSWIDTH_ADDR_SHIFT
#define FS_NOR_BUSWIDTH_DATA_SHIFT                FS_BUSWIDTH_DATA_SHIFT
#define FS_NOR_BUSWIDTH_MASK                      FS_BUSWIDTH_MASK
#define FS_NOR_BUSWIDTH_CMD_1BIT                  FS_BUSWIDTH_CMD_1BIT
#define FS_NOR_BUSWIDTH_CMD_2BIT                  FS_BUSWIDTH_CMD_2BIT
#define FS_NOR_BUSWIDTH_CMD_4BIT                  FS_BUSWIDTH_CMD_4BIT
#define FS_NOR_BUSWIDTH_ADDR_1BIT                 FS_BUSWIDTH_ADDR_1BIT
#define FS_NOR_BUSWIDTH_ADDR_2BIT                 FS_BUSWIDTH_ADDR_2BIT
#define FS_NOR_BUSWIDTH_ADDR_4BIT                 FS_BUSWIDTH_ADDR_4BIT
#define FS_NOR_BUSWIDTH_DATA_1BIT                 FS_BUSWIDTH_DATA_1BIT
#define FS_NOR_BUSWIDTH_DATA_2BIT                 FS_BUSWIDTH_DATA_2BIT
#define FS_NOR_BUSWIDTH_DATA_4BIT                 FS_BUSWIDTH_DATA_4BIT
#define FS_NOR_MAKE_BUSWIDTH(Cmd, Addr, Data)     FS_BUSWIDTH_MAKE(Cmd, Addr, Data)
#define FS_NOR_GET_BUSWIDTH_CMD(BusWidth)         FS_BUSWIDTH_GET_CMD(BusWidth)
#define FS_NOR_GET_BUSWIDTH_ADDR(BusWidth)        FS_BUSWIDTH_GET_ADDR(BusWidth)
#define FS_NOR_GET_BUSWIDTH_DATA(BusWidth)        FS_BUSWIDTH_GET_DATA(BusWidth)

/*********************************************************************
*
*       Encryption add-on
*/
#define FS_DES_CONTEXT                            FS_CRYPT_ALGO_CONTEXT_DES
#define FS_AES_CONTEXT                            FS_CRYPT_ALGO_CONTEXT_AES

/*********************************************************************
*
*       Memory allocation
*/
#define FS_PF_ALLOC(NumBytes)                     FS_MEM_ALLOC_CALLBACK(NumBytes)
#define FS_PF_FREE(pData)                         FS_MEM_FREE_CALLBACK(pData)

/*********************************************************************
*
*       Misc. functionality
*/
#define FS_MEMORY_IS_ACCESSIBLE_CALLBACK(pMem, NumBytes)      FS_MEMORY_CHECK_CALLBACK(pMem, NumBytes)
#define FS_SetMemAccessCallback(sVolumeName, pfMemoryCheck)   FS_SetMemCheckCallback(sVolumeName, pfMemoryCheck);

#if defined(__cplusplus)
}              // Make sure we have C-declarations in C++ programs
#endif

#endif // FS_H

/*************************** End of file ****************************/
