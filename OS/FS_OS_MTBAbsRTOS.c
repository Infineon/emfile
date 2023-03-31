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
-------------------------- END-OF-HEADER -----------------------------

File    : FS_OS_MTBAbsRTOS.c
Purpose : OS Layer for the file system. RTOS_AWARE component must be defined
          additionally to enable OS functionality since the same file is
          used in both RTOS and non-RTOS scenarios.
*/

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/

#include "FS_OS.h"
#include "cyhal_system.h"

#if defined(COMPONENT_RTOS_AWARE)
#include "cyabs_rtos.h"
#endif

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#if defined(COMPONENT_RTOS_AWARE)
#define MAX_LOCKS           (6)
#define SEMA_MAX_COUNT      (1Lu)
#define SEMA_INIT_COUNT     (0Lu)
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#if defined(COMPONENT_RTOS_AWARE)
static cy_mutex_t mutex[MAX_LOCKS];

#if FS_SUPPORT_DEINIT
    static uint32_t NumberLocks;
#endif
#endif /* #if defined(COMPONENT_RTOS_AWARE) */

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Directive 4.6', 7,\
'The third-party defines the function interface with basic numeral type')

/*********************************************************************
*
*       FS_X_OS_Lock
*
*  Function description
*    Acquires the specified OS synchronization object.
*
*  Parameters
*    LockIndex    Index of the OS synchronization object (0-based).
*
*  Additional information
*    This function has to be implemented by any OS layer.
*    The file system calls FS_X_OS_Lock() when it tries to enter a critical
*    section that is protected by the OS synchronization object specified via
*    LockIndex. FS_X_OS_Lock() has to block the execution of the calling task
*    until the OS synchronization object can be acquired. The OS synchronization
*    object is later released via a call to FS_X_OS_Unlock(). All OS synchronization
*    objects are created in FS_X_OS_Init().
*
*    It is guaranteed that the file system does not perform a recursive locking
*    of the OS synchronization object. That is FS_X_OS_Lock() is not called
*    two times in a row from the same task on the same OS synchronization object
*    without a call to FS_X_OS_Unlock() in between.
*/
void FS_X_OS_Lock(unsigned LockIndex) {
#if defined(COMPONENT_RTOS_AWARE)
    cy_rslt_t result = cy_rtos_get_mutex(&mutex[LockIndex], CY_RTOS_NEVER_TIMEOUT);

    CY_ASSERT(CY_RSLT_SUCCESS == result);
    FS_USE_PARA(result); /* To avoid compiler warning in Release mode */

    FS_DEBUG_LOG((FS_MTYPE_OS, "OS: LOCK Index: %d\n", LockIndex));
#else
    FS_USE_PARA(LockIndex);
    //lint -esym(522, FS_X_OS_Lock) Highest operation lacks side-effects
#endif /* #if defined(COMPONENT_RTOS_AWARE) */
}

/*********************************************************************
*
*       FS_X_OS_Unlock
*
*  Function description
*    Releases the specified OS synchronization object.
*
*  Parameters
*    LockIndex    Index of the OS synchronization object (0-based).
*
*  Additional information
*    This function has to be implemented by any OS layer.
*    The OS synchronization object to be released was acquired via
*    a call to FS_X_OS_Lock(). All OS synchronization objects are
*    created in FS_X_OS_Init().
*/
void FS_X_OS_Unlock(unsigned LockIndex) {
#if defined(COMPONENT_RTOS_AWARE)
    cy_rslt_t result = cy_rtos_set_mutex(&mutex[LockIndex]);

    CY_ASSERT(CY_RSLT_SUCCESS == result);
    FS_USE_PARA(result); /* To avoid compiler warning in Release mode */

    FS_DEBUG_LOG((FS_MTYPE_OS, "OS: UNLOCK Index: %d\n", LockIndex));
#else
    FS_USE_PARA(LockIndex);
    //lint -esym(522, FS_X_OS_Unlock) Highest operation lacks side-effects
#endif /* #if defined(COMPONENT_RTOS_AWARE) */
}

/*********************************************************************
*
*       FS_X_OS_Init
*
*  Function description
*    Allocates the OS layer resources.
*
*  Parameters
*    NumLocks   Number of OS synchronization objects required.
*
*  Additional information
*    This function has to be implemented by any OS layer.
*    FS_X_OS_Init() is called during the file system initialization.
*    It has to create the number of specified OS synchronization objects.
*    The type of the OS synchronization object is not relevant as long
*    as it can be used to protect a critical section. The file system
*    calls FS_X_OS_Lock() before it enters a critical section and
*    FS_X_OS_Unlock() when the critical sector is leaved.
*
*    In addition, FS_X_OS_Init() has to create the OS synchronization
*    object used by the optional functions FS_X_OS_Signal() and
*    FS_X_OS_Wait().
*/
void FS_X_OS_Init(unsigned NumLocks) {
#if defined(COMPONENT_RTOS_AWARE)
    uint32_t i;
    cy_rslt_t result;

    for (i = 0; i < NumLocks; i++) {
        result = cy_rtos_init_mutex(&mutex[i]);
        CY_ASSERT(CY_RSLT_SUCCESS == result);
    }

#if FS_SUPPORT_DEINIT
    NumberLocks = NumLocks;
#endif

    FS_USE_PARA(result); /* To avoid compiler warning in Release mode */
#else
    FS_USE_PARA(NumLocks);
    //lint -esym(522, FS_X_OS_Init) Highest operation lacks side-effects
#endif /* #if defined(COMPONENT_RTOS_AWARE) */
}

#if FS_SUPPORT_DEINIT

/*********************************************************************
*
*       FS_X_OS_DeInit
*
*  Function description
*    Releases the OS layer resources.
*
*  Additional information
*    This function has to be implemented only for file system configurations
*    that set FS_SUPPORT_DEINIT to 1. FS_X_OS_DeInit() has to release all
*    the OS synchronization objects that were allocated in FS_X_OS_Init().
*/
void FS_X_OS_DeInit(void) {
#if defined(COMPONENT_RTOS_AWARE)
    unsigned i;
    cy_rslt_t result;
    unsigned NumLocks = NumberLocks;

    for (i = 0; i < NumLocks; i++) {
        result = cy_rtos_deinit_mutex(&mutex[i]);
        CY_ASSERT(CY_RSLT_SUCCESS == result);
    }

    NumberLocks = 0u;

    FS_USE_PARA(result); /* To avoid compiler warning in Release mode */
#else
    //lint -esym(522, FS_X_OS_DeInit) Highest operation lacks side-effects
#endif /* #if defined(COMPONENT_RTOS_AWARE) */
}

#endif // FS_SUPPORT_DEINIT

/*********************************************************************
*
*       FS_X_OS_GetTime
*
*  Function description
*    Returns the number milliseconds elapsed since the application start.
*
*  Return value
*    Number of milliseconds since the start of the application.
*
*  Additional information
*    The implementation of this function is optional. FS_X_OS_GetTime()
*    is not called by the file system. It is typically used by some
*    test applications for performance measurements.
*/
U32 FS_X_OS_GetTime(void) {
#if defined(COMPONENT_RTOS_AWARE)
    cy_time_t tval = 0;

    cy_rslt_t result = cy_rtos_get_time(&tval);
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    FS_USE_PARA(result); /* To avoid compiler warning in Release mode */

  return (U32)tval;
#else
  return 0;
#endif /* #if defined(COMPONENT_RTOS_AWARE) */
}

/*********************************************************************
*
*       FS_X_OS_Wait
*
*  Function description
*    Waits for an OS synchronization object to be signaled.
*
*  Parameters
*    TimeOut    Maximum time in milliseconds to wait for the
*               OS synchronization object to be signaled.
*
*  Return value
*    ==0      OK, the OS synchronization object was signaled within the timeout.
*    !=0      An error or a timeout occurred.
*
*  Additional information
*    The implementation of this function is optional. FS_X_OS_Wait()
*    is called by implementations of the hardware layers that work
*    in event-driven mode. That is a condition is not check periodically
*    by the CPU until is met but the hardware layer calls FS_X_OS_Wait()
*    to block the execution while waiting for the condition to be met.
*    The blocking is realized via an OS synchronization object that is
*    signaled via FS_X_OS_Signal() in an interrupt that is triggered
*    when the condition is met.
*/
int FS_X_OS_Wait(int TimeOut) {
    /* This function is meant to be used by the HW layer implementations to
     * block the execution until an event/interrupt occurs. The occurrence of an
     * event is signaled using the FS_X_OS_Signal() function. Since multiple
     * storage drivers can be operating simultaneously and this function does
     * not differentiate between the different storage devices/drivers, one
     * driver could get unblocked when the other driver calls FS_X_OS_Signal().
     * In order to avoid this, this function is not implemented and the HW layer
     * implementations will directly implement such functionality when required.
     */
     FS_USE_PARA(TimeOut);
     return 0;
}

/*********************************************************************
*
*       FS_X_OS_Signal
*
*  Function description
*    Signals an OS synchronization object.
*
*  Additional information
*    The implementation of this function is optional. FS_X_OS_Signal()
*    is called by implementations of the hardware layers that work in
*    event-driven mode. Refer to FS_X_OS_Wait() for more details about
*    how this works.
*/
void FS_X_OS_Signal(void) {
    /* This function is meant to be used by the HW layer implementations to
     * signal the occurrence of an event/interrupt to unblock a task that has
     * been put into waiting by calling FS_X_OS_Wait(). Since multiple storage
     * drivers can be operating simultaneously and this function does not
     * differentiate between the different storage devices/drivers, one driver
     * could get unblocked when the other driver calls FS_X_OS_Signal().
     * In order to avoid this, this function is not implemented and the HW layer
     * implementations will directly implement such functionality when required.
     */
  ;
}

/*********************************************************************
*
*       FS_X_OS_Delay
*
*  Function description
*    Block the execution for the specified time.
*
*  Parameters
*    ms     Number of milliseconds to block the execution.
*
*  Additional information
*    The implementation of this function is optional. FS_X_OS_Delay()
*    is called by implementations of the hardware layers to block
*    efficiently the execution of a task.
*/
void FS_X_OS_Delay(int ms) {
   cy_rslt_t result;

   if(ms > 0)
   {
       /* cyhal_system_delay_ms() uses RTOS API to implement the delay when
        * RTOS_AWARE component is defined.
        */
        result = cyhal_system_delay_ms((uint32_t)ms);
        CY_ASSERT(CY_RSLT_SUCCESS == result);
   }

   FS_USE_PARA(result); /* To avoid compiler warning in Release mode */
}
CY_MISRA_BLOCK_END('MISRA C-2012 Directive 4.6')
/*************************** End of file ****************************/
