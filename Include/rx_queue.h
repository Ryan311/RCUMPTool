#ifndef _RTK_RTK_ATT_RX_QUEUE_H_
#define _RTK_RTK_ATT_RX_QUEUE_H_
#include "windows.h"
#include "rt_type.h"
#include "bt_list.h"


typedef struct _RTK_ATT_RX_QUEUE_HEAD{
	RT_LIST_ENTRY ListEntry;


	RT_U32 	DataNum;
	HANDLE    hNewDataArrialSemaphore;    ///semaphore when new data arrives
	PCRITICAL_SECTION Lock;

}RTK_ATT_RX_QUEUE_HEAD, *PRTK_ATT_RX_QUEUE_HEAD;


typedef struct _RTK_ATT_RX_DATA{
	RT_LIST_ENTRY ConnListItem;
	RT_U8	RxDataBuffer[256];
	RT_U8	RxDatalength;
}RTK_ATT_RX_DATA, *PRTK_ATT_RX_DATA;



EXTERN
INLINE
RT_BOOL
RxQueue_Init(
	IN PRTK_ATT_RX_QUEUE_HEAD QueueHead
	);

/**
	DeInitialize a list with its header
	\param[IN]	QueueHead	<PRTK_RECV_HEAD>:	pointer to the head of RecvQueue
	\return if init success return _TRUE, else return _FALSE
*/
EXTERN
INLINE
RT_BOOL
RecvQueue_DeInit(
	IN PRTK_ATT_RX_QUEUE_HEAD pRecvQueueHead
	)
;
/**
    Push a new entry to the queue. 
    Insert a new entry after the specified head. This is good for implementing stacks.
    \param [IN]       PendingConn     <PRTK_PENDING_RECVDATA>: new entry to be added
    \param [IN]	QueueHead    <PRTK_RECV_HEAD>                 : List header after which to add new entry 
*/
EXTERN
INLINE
RT_VOID 
RecvQueue_Push(
	IN PRTK_ATT_RX_DATA  RxData,
	IN PRTK_ATT_RX_QUEUE_HEAD  QueueHead 
	)
;

/**
    Push a new entry to the queue. 
    Insert a new entry after the specified head. This is good for implementing stacks.
    \param [IN]       PendingConn     <PRTK_PENDING_RECVDATA>: new entry to be added
    \param [IN]	QueueHead    <PRTK_RECV_HEAD>                 : List header after which to add new entry 
*/
EXTERN
INLINE
RT_VOID 
RecvQueue_PushFront(
	IN PRTK_ATT_RX_DATA  RxData,
	IN PRTK_ATT_RX_QUEUE_HEAD  QueueHead 
	)
;
/**
    Delete entry from the list
    Note: ListIsEmpty() on this list entry would not return true, since its state is undefined
    \param [IN] PopConn    	<PRTK_PENDING_RECVDATA*>  : list entry to be deleted
    \param [IN] QueueHead    <PRTK_RECV_HEAD>                : List header after which to add new entry 
*/
EXTERN
INLINE
RT_VOID 
RecvQueue_Pop(
	OUT PRTK_ATT_RX_DATA  *PopRxData,
	IN PRTK_ATT_RX_QUEUE_HEAD QueueHead
	)
;
/**
	Tell whether the queue is empty
	\param [IN] QueueHead          <PRTK_RECV_HEAD> : List header of which to be test
	\return if RecvQueue is empty return _TRUE, else return _FALSE
*/
EXTERN
INLINE 
RT_BOOL 
RecvQueue_IsEmpty(
	IN PRTK_ATT_RX_QUEUE_HEAD QueueHead
	)
;

/**
	empty RecvQueue and free memory allocated for Queue Item.
	\param[IN]	QueueHead	<PRTK_RECV_HEAD>:	a pointer to the head of RecvQueue.
*/
EXTERN
RT_VOID
RecvQueue_Cleanup(
	IN PRTK_ATT_RX_QUEUE_HEAD QueueHead
	);



EXTERN
RT_U32
RecvQueue_GetQueueLen(
	IN PRTK_ATT_RX_QUEUE_HEAD QueueHead
);



#endif