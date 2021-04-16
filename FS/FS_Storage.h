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
File        : FS_Storage.h
Purpose     : Define global functions and types to be used by an
              application using the storage API.

              This file needs to be included by any module using the
              storage API.
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef FS_STORAGE_H               // Avoid recursive and multiple inclusion
#define FS_STORAGE_H

/*********************************************************************
*
*             #include Section
*
**********************************************************************
*/

#include "FS_ConfDefaults.h"        /* FS Configuration */
#include "FS_Types.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

/*********************************************************************
*
*       Presence status
*
*  Description
*    Indicates if a removable storage device is present or not.
*/
#define FS_MEDIA_NOT_PRESENT        0     // Storage device is not present.
#define FS_MEDIA_IS_PRESENT         1     // Storage device is present.
#define FS_MEDIA_STATE_UNKNOWN      2     // Presence status is unknown.

/*********************************************************************
*
*       Transfer direction
*
*  Description
*    Direction of the transfered data.
*
*  Additional information
*    One of these values is passed by the file system via Operation
*    parameter to the callback function registered via
*    FS_STORAGE_SetOnDeviceActivityCallback()
*/
#define FS_OPERATION_READ           0     // Data is transferred from storage device to MCU.
#define FS_OPERATION_WRITE          1     // Data is transferred from MCU to storage device.

/*********************************************************************
*
*       Sector data type
*
*  Description
*    Type of data stored in a logical sector
*/
#define FS_SECTOR_TYPE_DATA         0u    // Sector that stores file data.
#define FS_SECTOR_TYPE_DIR          1u    // Sector that stores directory entries.
#define FS_SECTOR_TYPE_MAN          2u    // Sector that stores entries of the allocation table.

/*********************************************************************
*
*       Public types
*
**********************************************************************
*/

/*********************************************************************
*
*       FS_DEV_INFO
*
*  Description
*    Information about the storage device.
*
*  Additional information
*    NumHeads and SectorsPerTrack are relevant only for mechanical
*    drives. The application can access the information about
*    the storage device by calling FS_STORAGE_GetDeviceInfo().
*/
typedef struct {
  U16 NumHeads;          // Number of read / write heads.
  U16 SectorsPerTrack;   // Number of sectors stored on a track.
  U32 NumSectors;        // Total number of sectors on the storage device.
  U16 BytesPerSector;    // Size of a logical sector in bytes.
} FS_DEV_INFO;

/*********************************************************************
*
*       FS_STORAGE_COUNTERS
*
*  Description
*    Statistical counters.
*
*  Additional information
*    ReadSectorCnt can be (and typically is) higher than
*    ReadOperationCnt, since one read operation can request multiple
*    sectors (in a burst). The same applies to write operations:
*    WriteSectorCnt can be (and typically is) higher than
*    WriteOperationCnt, since one read operation can request multiple
*    sectors (in a burst).
*
*    The statistical counters can be read via FS_STORAGE_GetCounters().
*    They are set to 0 when the file system is initialized.
*    Additionally, the application can explicitly set them to 0
*    via FS_STORAGE_ResetCounters().
*/
typedef struct {
  U32 ReadOperationCnt;       // Number of "Read sector operation" calls.
  U32 ReadSectorCnt;          // Number of sectors read (before cache).
  U32 ReadSectorCachedCnt;    // Number of sectors read from cache
  U32 WriteOperationCnt;      // Number of "Write sector operation" calls
  U32 WriteSectorCnt;         // Number of sectors written (before cache).
  U32 WriteSectorCntCleaned;  // Number of sectors written by the cache to storage in order to make room for other data.
  U32 ReadSectorCntMan;       // Number of management sectors read (before cache).
  U32 ReadSectorCntDir;       // Number of directory sectors (which store directory entries) read (before cache).
  U32 WriteSectorCntMan;      // Number of management sectors written (before cache).
  U32 WriteSectorCntDir;      // Number of directory sectors (which store directory entries) written (before cache).
} FS_STORAGE_COUNTERS;

/*********************************************************************
*
*       FS_ON_DEVICE_ACTIVITY_CALLBACK
*
*  Function description
*    The type of the callback function invoked by the file system
*    on a logical sector read or write operation.
*
*  Parameters
*    pDevice      Storage device that performed the operation.
*    Operation    Type of the operation performed. Can be one
*                 of the values listed in \ref{Transfer direction}
*    StartSector  Index of the first logical sector transfered (0-based)
*    NumSectors   Number of logical sectors transfered in the current
*                 operation.
*    SectorType   Type of the data stored in the logical sector.
*                 It can be one of the values listed in
*                 \ref{Sector data type}
*
*  Additional information
*    This is the type of function that can be registered
*    via FS_STORAGE_SetOnDeviceActivityCallback()
*/
typedef void (FS_ON_DEVICE_ACTIVITY_CALLBACK)(FS_DEVICE * pDevice, unsigned Operation, U32 StartSector, U32 NumSectors, int Sectortype);

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
int      FS_STORAGE_Clean                      (const char * sVolumeName);
int      FS_STORAGE_CleanOne                   (const char * sVolumeName, int * pMoreToClean);
#if FS_SUPPORT_DEINIT
void     FS_STORAGE_DeInit                     (void);
#endif // FS_SUPPORT_DEINIT
int      FS_STORAGE_FreeSectors                (const char * sVolumeName, U32 FirstSector, U32 NumSectors);
int      FS_STORAGE_GetCleanCnt                (const char * sVolumeName, U32 * pCleanCnt);
#if FS_STORAGE_ENABLE_STAT_COUNTERS
void     FS_STORAGE_GetCounters                (FS_STORAGE_COUNTERS * pStat);
#endif // FS_STORAGE_ENABLE_STAT_COUNTERS
int      FS_STORAGE_GetDeviceInfo              (const char * sVolumeName, FS_DEV_INFO * pDeviceInfo);
int      FS_STORAGE_GetSectorUsage             (const char * sVolumeName, U32 SectorIndex);
unsigned FS_STORAGE_Init                       (void);
int      FS_STORAGE_ReadSector                 (const char * sVolumeName,       void * pData, U32 SectorIndex);
int      FS_STORAGE_ReadSectors                (const char * sVolumeName,       void * pData, U32 FirstSector, U32 NumSectors);
int      FS_STORAGE_RefreshSectors             (const char * sVolumeName, U32 FirstSector, U32 NumSectors, void * pBuffer, U32 NumBytes);
#if FS_STORAGE_ENABLE_STAT_COUNTERS
void     FS_STORAGE_ResetCounters              (void);
#endif // FS_STORAGE_ENABLE_STAT_COUNTERS
#if FS_STORAGE_SUPPORT_DEVICE_ACTIVITY
void     FS_STORAGE_SetOnDeviceActivityCallback(const char * sVolumeName, FS_ON_DEVICE_ACTIVITY_CALLBACK * pfOnDeviceActivity);
#endif // FS_STORAGE_SUPPORT_DEVICE_ACTIVITY
void     FS_STORAGE_Sync                       (const char * sVolumeName);
int      FS_STORAGE_SyncSectors                (const char * sVolumeName, U32 FirstSector, U32 NumSectors);
void     FS_STORAGE_Unmount                    (const char * sVolumeName);
void     FS_STORAGE_UnmountForced              (const char * sVolumeName);
int      FS_STORAGE_WriteSector                (const char * sVolumeName, const void * pData, U32 SectorIndex);
int      FS_STORAGE_WriteSectors               (const char * sVolumeName, const void * pData, U32 FirstSector, U32 NumSectors);

/*********************************************************************
*
*       Compatibility defines
*/
#define FS_InitStorage()                                                                  FS_STORAGE_Init()
#define FS_ReadSector(sVolumeName, pData, SectorIndex)                                    FS_STORAGE_ReadSector(sVolumeName,  pData, SectorIndex)
#define FS_WriteSector(sVolumeName, pData, SectorIndex)                                   FS_STORAGE_WriteSector(sVolumeName, pData, SectorIndex)
#define FS_UnmountLL(sVolumeName)                                                         FS_STORAGE_Unmount(sVolumeName)
#define FS_CleanVolume(sVolumeName)                                                       FS_STORAGE_Sync(sVolumeName)
#define FS_GetDeviceInfo(sVolumeName, pDevInfo)                                           FS_STORAGE_GetDeviceInfo(sVolumeName, pDevInfo)
#define FS_ONDEVICEACTIVITYHOOK(pDevice, Operation, StartSector, NumSectors, Sectortype)  FS_ON_DEVICE_ACTIVITY_CALLBACK(pDevice, Operation, StartSector, NumSectors, Sectortype)
#define FS_SetOnDeviceActivityHook(sVolumeName, pfOnDeviceActivity)                       FS_STORAGE_SetOnDeviceActivityCallback(sVolumeName, pfOnDeviceActivity)

#if defined(__cplusplus)
  }              /* Make sure we have C-declarations in C++ programs */
#endif

#endif // FS_STORAGE_H

/*************************** End of file ****************************/
