/*++

Copyright (c) Realtek Corporation. All rights reserved.

Module Name:
	os_abstrator.h

+*/

/** 
\file     os_abstractor.h
\brief   In order for further OS imgration, os provided API should be wrappered. 
           This module is to do the wrapper work.
*/
#ifndef _INC_ABSTRACTOR_H
#define _INC_ABSTRACTOR_H

#include "rt_type.h"

/**
    memory type enumeration specifies the the type of system memory to allocate.
    RT_HIGH_PRIORITY    : Map to windows' non_pageable system memory, and can be accessed from ANY IRQL. 
                                       But it's a limited resource, allocate it only when necessary.
    RT_MEDIUM_PRIORITY        : Reserved for further usage
    RT_LOW_PRIORITY    : Map to windows' pageable system memory, only can be accessed at IRQL<DISPATCH
*/    
typedef enum _RT_MEMORY_TYPE {
    RT_HIGH_PRIORITY = 0,   
    RT_MEDIUM_PRIORITY,
    RT_LOW_PRIORITY
}RT_MEMORY_TYPE, *PRT_MEMORY_TYPE;

/*----------------------------------------------------------------------------------
    MEMORY FUNCTION
----------------------------------------------------------------------------------*/
/**
    Allocate memory buffer with sizeof bufferlen . 
    \param [OUT]    ppBuffer            <RT_VOID**>           : pointer to the address of buffer
    \param [IN] BufferLen                <RT_U32>                 : size of buffer 
    \param [IN] Type                <RT_MEMORY_TYPE>        : Memory type
*/
EXTERN RT_STATUS
OsAllocateMemory(
    OUT RT_VOID** ppBuffer,
    IN RT_U32 BufferLen,
    IN RT_MEMORY_TYPE Type
    );

/**
    Free specified memory buffer and set it to NULL. 
    \param [IN OUT]    ppBuffer            <RT_VOID**>           : pointer to the address of buffer
*/
EXTERN RT_VOID
OsFreeMemory(
    IN OUT RT_VOID** ppBuffer
    );

/**
    Allocate memory buffer with sizeof bufferlen and fill it with zero. 
    \param [OUT]    ppBuffer            <RT_VOID**>           : pointer to the address of buffer
    \param [IN] BufferLen                <RT_U32>                 : size of buffer 
    \param [IN] Type                <RT_MEMORY_TYPE>        : Memory type
*/
EXTERN RT_STATUS
OsCallocateMemory(
    OUT RT_VOID** ppBuffer,
    IN RT_U32 BufferLen,
    IN RT_MEMORY_TYPE Type
    );

/**
    Fill specified buffer with zero with specified length. 
    \param [OUT]    pBuffer            <RT_VOID*>               : the address of buffer
    \param [IN] BufferLen                <RT_U32>                 : size of buffer to fill
*/
EXTERN RT_VOID
OsZeroMemory(
    OUT RT_VOID* pBuffer,
    IN RT_U32 BufferLen
    );

/**
    Fill specified buffer with specifed character with specified length. 
    \param [OUT]   pBuffer            <RT_VOID*>               : the address of buffer
    \param [IN]     BufferLen           <RT_U32>                 : size of buffer to fill
    \param [IN]     FillChar              <RT_U8>                   : character to fill with
*/
EXTERN RT_VOID
OsFillMemory(
    OUT RT_VOID* pBuffer,
    IN RT_U32 BufferLen,
    IN RT_U8 FillChar
    );

/**
    Copy specified length data from pSrcBuffer to the pDestBuffer. 
    Note: CopyMemory will fail if source buffer and destination buffer are overlapped    
    \param [OUT]    pDestBuffer            <RT_VOID*>           : address of Destination buffer
    \param [IN]       pSrcBuffer             <RT_VOID*>           : Address of Source buffer    
    \param [IN]       BufferLen               <RT_U32>               : size of data to copy
*/
EXTERN RT_VOID
OsCopyMemory(
    OUT RT_VOID* pDestBuffer,
    IN RT_VOID* pSrcBuffer,
    IN RT_U32 BufferLen
    );

/**
    Move specified length data from pSrcBuffer to the pDestBuffer. 
    Note: MoveMemory will succeed no matter source buffer and destination buffer are overlapped
    \param [OUT]    pDestBuffer            <RT_VOID*>           : address of Destination buffer
    \param [IN]       pSrcBuffer             <RT_VOID*>           : Address of Source buffer    
    \param [IN]       BufferLen               <RT_U32>               : size of data to copy
*/
EXTERN RT_VOID
OsMoveMemory(
    OUT RT_VOID* pDestBuffer,
    IN RT_VOID* pSrcBuffer,
    IN RT_U32 BufferLen
    );

/**
    Compare specified length of data in two buffers. 
    \param [IN]       pBuffer1           <RT_VOID*>           : address of Destination buffer
    \param [IN]       pBuffer2           <RT_VOID*>           : Address of Source buffer    
    \param [IN]       BufferLen          <RT_U32>               : size of data to copy
    \return  return negtive value if character in pBuffer1 is smaller
                return zero if all cahracters in pbuffer1 and pBuffer2 are equal
                return positive value if character in pBuffer1 is larger
*/
EXTERN RT_S32
OsCompareMemory(
    IN RT_VOID* pBuffer1,
    IN RT_VOID* pBuffer2,
    IN RT_U32 BufferLen
    );

/*----------------------------------------------------------------------------------
    SPINLOCK FUNCTION
----------------------------------------------------------------------------------*/
/**
    OS_SPINLOCK is a kind of synchronization mechanisms which can be used to protect shared data or resources
    Storage for OS_SPINLOCK should be RESIDENT, and no routine should hold it for longer than 25 micorseconds.
    Never implement routines that do any of the following while holding a spinlock
    @Cause exceptions
    @Attempt to access pageable memory
    @Make recursive call cause spin lock be held for longer than 25 ms
    @Attempt to acquire another spin lock
    To synchoronize access to data structure which not required in DPC_LEVEL, use mutex instead
    To synchoronize access to simple data structures, OS_ATOMIC will be used instead.
    
*/
typedef struct _OS_SPINLOCK  OS_SPINLOCK,  *POS_SPINLOCK;

/**
    Allocate a spin lock and do initialization. 
    \param [OUT]       ppSpinLock           <POS_SPINLOCK*>      : pointer of address of spinlock
    \param [IN]          IdStr                    <RT_S8*>                   : caller's ID string    
*/
EXTERN RT_STATUS
OsAllocateSpinLock(
    OUT POS_SPINLOCK* ppSpinLock,
    IN RT_S8* IdStr
    );

/**
    Free a spin lock storage. 
    \param [IN OUT]       ppSpinLock           <POS_SPINLOCK*>      : pointer of address of spinlock
*/
EXTERN RT_VOID
OsFreeSpinLock(
    IN OUT POS_SPINLOCK* ppSpinLock
    );

/**
    Acquires a spin lock so caller can synchronize access to shared data. 
    Remember to release spinlock ASAP
    \param [IN]       pSpinLock           <POS_SPINLOCK>      : pointer of spinlock
*/
EXTERN RT_VOID
OsAcquireSpinLock(
    IN POS_SPINLOCK pSpinLock
    );

/**
    Release the spin lock. 
    \param [IN]       pSpinLock           <POS_SPINLOCK>      : address of spinlock
*/
EXTERN RT_VOID
OsReleaseSpinLock(
    IN POS_SPINLOCK pSpinLock
    );

/*----------------------------------------------------------------------------------
    MUTEX FUNCTION
----------------------------------------------------------------------------------*/
/**
    OS_MUTEX is another kind of synchronization mechanisms which can be used to protect shared data or resources
    It requires low-overhead which runs at IRQL <= APC_LEVEL.
    If another thread has already acquired the mutex, execution of the current thread is suspended until the
    mutex is released instead of spinlock's busy waiting.
    But DO NOT USE IT at IQRL == DISPATCH_LEVEL
*/
typedef struct _OS_MUTEX OS_MUTEX, *POS_MUTEX;

/**
    Allocate a mutex and do initialization. 
    \param [OUT]       ppMutex           <POS_MUTEX*>      : pointer of address of mutex
    \param [IN]          IdStr                    <RT_S8*>          : caller's ID string    
*/
EXTERN RT_STATUS
OsAllocateMutex(
    OUT POS_MUTEX* ppMutex,
    IN RT_S8* IdStr
    );

/**
    Free a mutex storage. 
    \param [IN OUT]       ppMutex           <POS_MUTEX*>      : pointer of address of mutex
*/
EXTERN RT_VOID
OsFreeMutex(
    IN OUT POS_MUTEX* ppMutex
    );

/**
    Acquires a mutex so caller can synchronize access to shared data. 
    If mutex has been acquired by another, it will suspended until the mutex becomes avaiable
    \param [IN OUT]       pMutex           <POS_MUTEX>      : pointer of mutex
*/
EXTERN RT_VOID
OsAcquireMutex(
    IN OUT POS_MUTEX pMutex
    );

/**
    Try to acquire mutex without suspending the current thread.
    It returns immediately, whether or not the mutex has been acquired.
    \param [IN OUT]       pMutex           <POS_MUTEX>      : pointer of mutex
    \return True if mutex has been acquired, and false otherwise
*/
EXTERN RT_BOOL
OsTryAcquireMutex(
    IN OUT POS_MUTEX pMutex
    );

/**
    Release the mutex. 
    \param [IN OUT]       pMutex           <POS_MUTEX>      : address of mutex
*/
EXTERN RT_VOID
OsReleaseMutex(
    IN OUT POS_MUTEX pMutex
    );

/*----------------------------------------------------------------------------------
    ATOMIC FUNCTION
----------------------------------------------------------------------------------*/
/**
    Atomic functions are to protect simple shared data. 
    Acturally it mainly use for memory barrier.
    Compared to spin lock, it is faster and would not waste CPU busing waiting.
    Both Windows and Linux provide such functions, only some useful functions are wrapped here.
    Bit  and 64 bit value manipulation will later be added if required.
*/
typedef struct _OS_ATOMIC OS_ATOMIC, *POS_ATOMIC;

/**
    Allocate a atomic and do initialization. 
    \param [OUT]       ppAtomic           <POS_ATOMIC*>      : pointer of address of atomic
    \param [IN]         InitialValue                    <RT_S32>     : Initialize value    
*/
EXTERN RT_STATUS
OsAllocateAtomic(
    OUT POS_ATOMIC* ppAtomic,
    IN RT_S32   InitialValue
    );

/**
    Free a atomic storage. 
    \param [IN OUT]       ppAtomic           <POS_ATOMIC*>      : pointer of address of atomic
*/
EXTERN RT_VOID
OsFreeAtomic(
    IN OUT POS_ATOMIC* ppAtomic
    );

/**
    Read atomic value. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, result will be stored
    \return return atomic value
*/
EXTERN RT_S32
OsReadAtomic(
    IN OUT POS_ATOMIC pAtomic
    );

 /**
    Set atomic value to specified value.
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, result will be stored
    \param [IN]       Value           <RT_S32>      : value to be set  
*/
EXTERN RT_VOID
OsWriteAtomic(
    IN OUT POS_ATOMIC pAtomic,
    IN RT_S32 Value
    );
/**
    Increases the value of the specified atomic by one as an atomic operation. 
    \param [IN]       pAtomic           <POS_ATOMIC>      : pointer of atomic
    \return return resulting incremented value
*/
EXTERN RT_S32
OsIncrementAtomic(
    IN POS_ATOMIC pAtomic
    );

/**
    decreases the value of the specified atomic by one as an atomic operation. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic
    \return return resulting decremented value
*/
EXTERN RT_S32
OsDecrementAtomic(
    IN OUT POS_ATOMIC pAtomic
    );

/**
    Perform an atomic addition operation on the specified value. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, result will be stored
    \param [IN]              Value              <RT_U32>               : Value to add
    \return return resulting value
*/
EXTERN RT_S32
OsAddAtomic(
    IN OUT POS_ATOMIC pAtomic,
    IN RT_U32 Value
    );

/**
    Performs an atomic AND operation on the specified Atomic and value. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, and AND result will stored
    \param [IN]       Value                     <RT_S32>               : another AND operand
    \return return pAtomic original value
*/
EXTERN RT_S32
OsAndAtomic(
    IN OUT POS_ATOMIC pAtomic,
    IN RT_S32 Value
    );

/**
    Performs an atomic OR operation on the specified Atomic and value. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, and OR result will stored
    \param [IN]       Value                     <RT_S32>               : another OR operand
    \return return pAtomic original value
*/
EXTERN RT_S32
OsOrAtomic(
    IN OUT POS_ATOMIC pAtomic,
    IN RT_S32 Value
    );

/**
    Performs an atomic XOR operation on the specified Atomic and value. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, and XOR result will stored
    \param [IN]       Value                     <RT_S32>               : another XOR operand
    \return return pAtomic original value
*/
EXTERN RT_S32
OsXorAtomic(
    IN OUT POS_ATOMIC pAtomic,
    IN RT_S32 Value
    );

/**
    Test the specified bit of the specifed atomic and set it to 0. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, and result will stored
    \param [IN]             Offset               <RT_S32>               : bit offset to be checked
    \return return pAtomic original value
*/
EXTERN RT_BOOL
OsBitTestAndResetAtomic(
    IN OUT POS_ATOMIC pAtomic,
    IN RT_S32 Offset
    );

/**
    Test the specified bit of the specifed atomic and set it to 1. 
    \param [IN OUT]       pAtomic           <POS_ATOMIC>      : pointer of atomic, and result will stored
    \param [IN]             Offset               <RT_S32>               : bit offset to be checked
    \return return pAtomic original value
*/
EXTERN RT_BOOL
OsBitTestAndSetAtomic(
    IN OUT POS_ATOMIC pAtomic,
    IN RT_S32 Offset
    );

/*----------------------------------------------------------------------------------
    SIGNAL FUNCTION
----------------------------------------------------------------------------------*/
/**
    Signal is used for notification other than synchronization.
    When a signal is set to signaled state, all threads that were waiting for the event to be signaled state
    become eligible for execution. 
    Signal will remains in the signaled state until manully calls ClearSignal
*/
typedef struct _OS_SIGNAL OS_SIGNAL, *POS_SIGNAL;

/**
    Allocate a signal object and initialize the signal state to non-signaled state.
    If you want a signaled signal, call setsignal function immediately.
    \param [OUT]       ppSignal           <POS_SIGNAL*>      : pointer of address of signal object
    \param [IN]          IdStr                    <RT_S8*>           : caller's ID string    
*/
EXTERN RT_STATUS
OsAllocateSignal(
    OUT POS_SIGNAL* ppSignal,
    IN RT_S8* IdStr
    );

/**
    Free a signal storage. 
    \param [IN OUT]       ppSignal           <POS_SIGNAL*>      : pointer of address of signal object
*/
EXTERN RT_VOID
OsFreeSignal(
    IN OUT POS_SIGNAL* ppSignal
    );

/**
    Cause signal to be in signaled state which attempts to satisfy as many waits as possible on the signal.
    It remains in signaled state until manully call OsClearSignal. 
    \param [IN OUT]       pSignal           <POS_SIGNAL>      : signal object to be set
*/
EXTERN RT_VOID
OsSetSignal(
    IN OUT POS_SIGNAL pSignal
    );

/**
    Cause signal to be in non-signaled state 
    \param [IN OUT]       pSignal           <POS_SIGNAL>      : signal object to be clear
*/
EXTERN RT_VOID
OsClearSignal(
    IN OUT POS_SIGNAL pSignal
    );

/**
    put current thread into a wait state until given OS_SIGNAL is set to signaled state or time out
    \param [IN]    pSignal    <POS_SIGNAL>      : signal object to be waiting
    \param [IN]     Timeout   <RT_S32>             : timerout value in milliseconds. 0 means indefinitely
    \return Timeout error if signal is not signaled in specified time;
               FAILURE error if other error condition
               success if objects signaled
*/
EXTERN RT_STATUS
OsWaitForSignal(
    IN POS_SIGNAL pSignal,
    IN RT_S32 msTimeout
    );

#if TODO
/**
    put current thread into a wait state until one of given OS_SIGNALs is set to signaled state or time out
    \param [IN]     pSignal          <POS_SIGNAL*>     : signal objects to be waiting
    \param [IN]     SignalCount   <RT_U32>                : number of signal objects
    \param [IN]     Timeout         <RT_S32>             : timerout value in milliseconds. 0 means indefinitely
    \param [OUT]  pSinalIndex    <RT_U32*>               : if it returns since one of objects' signaled, return index
    \return Timeout error if no signaled signal in specified time;
               FAILURE error if other error condition
               success if one of objects signaled, and its index could be get in singalindex parameter
*/
EXTERN RT_STATUS
OsWaitMultiSignal(
    IN POS_SIGNAL pSignal[],
    IN RT_U32    SignalCount,
    IN RT_S32 msTimeout,
    OUT RT_U32* pSinalIndex
    );
#endif //TODO
/*----------------------------------------------------------------------------------
    TIMER FUNCTION
----------------------------------------------------------------------------------*/
/**
    Timer functions are to provide time out operations and perform a periodic operation. 
*/
typedef struct _OS_TIMER OS_TIMER, *POS_TIMER;

///timer callback function definition
typedef RT_VOID (*TIMER_CALLBACK_FN)(RT_VOID* Context);

/**
    Allocate a timer. Need to call OsInitializeTimer. 
    \param [OUT]       ppTimer           <POS_TIMER*>      : pointer of address of timer to be allocated
    \param [IN]         IdStr                    <RT_S8*>          : caller's ID string for further debug
*/
EXTERN RT_STATUS
OsAllocateTimer(
    OUT POS_TIMER* ppTimer,
    IN RT_S8* IdStr
    );

/**
    Free a timer storage, if timer is scheduled, need wait for completion before free. 
    \param [IN OUT]       ppTimer           <POS_TIMER*>      : pointer of address of timer to be freed
*/
EXTERN RT_VOID
OsFreeTimer(
    IN OUT POS_TIMER* ppTimer
    );

/**
    Initialize a timer and associate with a callback function. 
    \param [IN]       pTimer           <POS_TIMER>              : Timer to be initialized
    \param [IN]      Callback    <TIMER_CALLBACK_FN>     : Timer's associated callback function
    \param [IN]      Context           <RT_VOID*>               : callback function's parameters
*/
EXTERN RT_STATUS
OsInitializeTimer(
    IN POS_TIMER pTimer,
    IN TIMER_CALLBACK_FN Callback,
    IN RT_VOID* Context
    );

/**
    Set the absolute time interval at which the timer's registered callback is to be executed. 
    If specified timer has been set, it will modify time's interval and reset timer    
    \param [IN]       pTimer           <POS_TIMER>              : Timer to be set
    \param [IN]      msDelay           <RT_S32>                  : time interval in milliseconds
*/
EXTERN RT_VOID
OsSetTimer(
    IN POS_TIMER pTimer,
    IN RT_S32  msDelay
    );

/**
    Cancel the timer before timer interval expires if it was set. 
    \param [IN]       pTimer           <POS_TIMER>              : Timer to be canceled
*/
EXTERN RT_VOID
OsCancelTimer(
    IN POS_TIMER pTimer
    );

/**
    Get current system timer.
    It is used only for timestamp, since its not GMT local time.
    \param [OUT]       pmsCurrentTime           <RT_U32*>              : Pointer to current time in milliseconds.
*/
EXTERN RT_VOID
OsGetCurrentTime(
    OUT RT_U32* pmsCurrentTime
    );

/*----------------------------------------------------------------------------------
    WORKITEM FUNCTION
----------------------------------------------------------------------------------*/
/**
    workitem functions are to provide delayed processing, in which callback routine performs actural processing.
    wokitem resource is limited, and you must not contain indefinite loop or waits for too long in workitem.
    Workitem compared with timer:
    1) Workitem always process lengthy task, while timer always do quickly.
    2) Timer's callback runs at high level, and must not make blocking calls, while workitem could.
*/
typedef struct _OS_WORKITEM OS_WORKITEM, *POS_WORKITEM;

///workitem callback function definition
typedef RT_VOID (*WORKITEM_CALLBACK_FN)(RT_VOID* Context);

/**
    Allocate a workitem and do initialize with its callback function. 
    \param [OUT]  ppWorkItem      <POS_WORKITEM*>      : pointer of address of workitem to be allocated
    \param [IN]     Callback   <WORKITEM_CALLBACK_FN> : workitem's callback function
    \param [IN]     Context               <RT_VOID*>              : callback function's context
    \param [IN]     DeviceObject       <RT_VOID*>              : Windows workitem relates to DeviceObject
    \param [IN]     IdStr                    <RT_S8*>                 : caller's ID string for further debug
*/
EXTERN RT_STATUS
OsAllocateWorkItem(
    OUT POS_WORKITEM* ppWorkItem,
    IN WORKITEM_CALLBACK_FN Callback,
    IN RT_VOID* Context,
//    IN RT_VOID* DeviceObject, //caller would never be response for this member
    IN RT_S8* IdStr
    );

/**
    Free a workitem storage, if workitem is scheduled, need wait for completion before free. 
    \param [IN OUT]       ppWorkItem           <POS_WORKITEM*>      : pointer of address of workitem
*/
EXTERN RT_VOID
OsFreeWorkItem(
    IN OUT POS_WORKITEM* ppWorkItem
    );

/**
    Queue specified workitem and kick it to play. 
    If specified workitem has been queued, it will crash. So call OsIsScheduledWorkitem() before.
    \param [IN]       pWorkItem           <POS_WORKITEM>              : workitem to be scheduled
*/
EXTERN RT_VOID
OsScheduleWorkItem(
    IN POS_WORKITEM pWorkItem
    );

/**
    Check whether specified workitem is scheduled. 
    \param [IN]       pWorkItem           <POS_WORKITEM>              : workitem to be checked
    \return             TRUE if workitem has already scheduled, FALSE otherwise.    
*/
EXTERN RT_BOOL
OsIsScheduledWorkItem(
    IN POS_WORKITEM pWorkItem
    );

#endif //_INC_ABSTRACTOR_H