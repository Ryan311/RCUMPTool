#ifndef __ATT_API_H
#define __ATT_API_H

/**

Copyright (c) Realtek Corporation. All rights reserved.

\file    att_api.h

\brief   LE attribute protocol (ATT) functions.

*/


#include "le_basic.h"
#include "transmission.h"
#include "rx_queue.h"


// Definition
#define ATT_MTU_DEFAULT         23
#define ATT_TIME_OUT_30_SEC     30000
#define ATT_TIME_OUT_INFINITE   0


// OP code
#define ATT_OPCODE_ERROR_RESPONSE                0x1
#define ATT_OPCODE_EXCHANGE_MTU_REQUEST          0x2
#define ATT_OPCODE_EXCHANGE_MTU_RESPONSE         0x3
#define ATT_OPCODE_FIND_INFORMATION_REQUEST      0x4
#define ATT_OPCODE_FIND_INFORMATION_RESPONSE     0x5
#define ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST    0x6
#define ATT_OPCODE_FIND_BY_TYPE_VALUE_RESPONSE   0x7
#define ATT_OPCODE_READ_BY_TYPE_REQUEST          0x8
#define ATT_OPCODE_READ_BY_TYPE_RESPONSE         0x9
#define ATT_OPCODE_READ_REQUEST                  0xa
#define ATT_OPCODE_READ_RESPONSE                 0xb
#define ATT_OPCODE_READ_BLOB_REQUEST             0xc
#define ATT_OPCODE_READ_BLOB_RESPONSE            0xd
#define ATT_OPCODE_READ_MULTIPLE_REQUEST         0xe
#define ATT_OPCODE_READ_MULTIPLE_RESPONSE        0xf
#define ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST    0x10
#define ATT_OPCODE_READ_BY_GROUP_TYPE_RESPONSE   0x11
#define ATT_OPCODE_WRITE_REQUEST                 0x12
#define ATT_OPCODE_WRITE_RESPONSE                0x13
#define ATT_OPCODE_WRITE_COMMAND                 0x52
#define ATT_OPCODE_SIGNED_WRITE_COMMAND          0xd2
#define ATT_OPCODE_PREPARE_WRITE_REQUEST         0x16
#define ATT_OPCODE_PREPARE_WRITE_RESPONSE        0x17
#define ATT_OPCODE_EXECUTE_WRITE_REQUEST         0x18
#define ATT_OPCODE_EXECUTE_WRITE_RESPONSE        0x19
#define ATT_OPCODE_HANDLE_VALUE_NOTIFICATION     0x1b
#define ATT_OPCODE_HANDLE_VALUE_INDICATION       0x1d
#define ATT_OPCODE_HANDLE_VALUE_CONFIRMATION     0x1e

// PDU length
#define ATT_PDU_LEN_MAX                                256//23
#define ATT_PDU_LEN_ERROR_RESPONSE                     5
#define ATT_PDU_LEN_EXCHANGE_MTU_REQUEST               3
#define ATT_PDU_LEN_EXCHANGE_MTU_RESPONSE              3
#define ATT_PDU_LEN_FIND_INFORMATION_REQUEST           5
#define ATT_PDU_LEN_FIND_INFORMATION_RESPONSE_BASE     2
#define ATT_PDU_LEN_FIND_BY_TYPE_VALUE_REQUEST_BASE    7
#define ATT_PDU_LEN_FIND_BY_TYPE_VALUE_RESPONSE_BASE   1
#define ATT_PDU_LEN_READ_BY_TYPE_REQUEST_BASE          5
#define ATT_PDU_LEN_READ_BY_TYPE_RESPONSE_BASE         2
#define ATT_PDU_LEN_READ_REQUEST                       3
#define ATT_PDU_LEN_READ_RESPONSE_BASE                 1
#define ATT_PDU_LEN_READ_BLOB_REQUEST                  5
#define ATT_PDU_LEN_READ_BLOB_RESPONSE_BASE            1
#define ATT_PDU_LEN_READ_MULTIPLE_REQUEST_BASE         1
#define ATT_PDU_LEN_READ_MULTIPLE_RESPONSE_BASE        1
#define ATT_PDU_LEN_READ_BY_GROUP_TYPE_REQUEST_BASE    5
#define ATT_PDU_LEN_READ_BY_GROUP_TYPE_RESPONSE_BASE   2
#define ATT_PDU_LEN_WRITE_REQUEST_BASE                 3
#define ATT_PDU_LEN_WRITE_RESPONSE                     1
#define ATT_PDU_LEN_WRITE_COMMAND_BASE                 3
#define ATT_PDU_LEN_SIGNED_WRITE_COMMAND_BASE          3
#define ATT_PDU_LEN_PREPARE_WRITE_REQUEST_BASE         5
#define ATT_PDU_LEN_PREPARE_WRITE_RESPONSE_BASE        5
#define ATT_PDU_LEN_EXECUTE_WRITE_REQUEST              2
#define ATT_PDU_LEN_EXECUTE_WRITE_RESPONSE             1
#define ATT_PDU_LEN_HANDLE_VALUE_NOTIFICATION_BASE     3
#define ATT_PDU_LEN_HANDLE_VALUE_INDICATION_BASE       3
#define ATT_PDU_LEN_HANDLE_VALUE_CONFIRMATION          1

// Long attribute value length
#define ATT_LONG_ATTR_VALUE_LEN_MAX   512

// Error code
#define ATT_ERROR_INVALID_HANDLE                     0x1
#define ATT_ERROR_READ_NOT_PERMITTED                 0x2
#define ATT_ERROR_WRITE_NOT_PERMITTED                0x3
#define ATT_ERROR_INVALID_PDU                        0x4
#define ATT_ERROR_INSUFFICIENT_AUTHENTICATION        0x5
#define ATT_ERROR_REQUEST_NOT_SUPPORTED              0x6
#define ATT_ERROR_INVALID_OFFSET                     0x7
#define ATT_ERROR_INSUFFICIENT_AUTHORIZATION         0x8
#define ATT_ERROR_PREPARE_QUEUE_FULL                 0x9
#define ATT_ERROR_ATTRIBUTE_NOT_FOUND                0xa
#define ATT_ERROR_ATTRIBUTE_NOT_LONG                 0xb
#define ATT_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE   0xc
#define ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH     0xd
#define ATT_ERROR_UNLIKELY_ERROR                     0xe
#define ATT_ERROR_INSUFFICIENT_ENCRYPTION            0xf
#define ATT_ERROR_UNSUPPORTED_GROUP_TYPE             0x10
#define ATT_ERROR_INSUFFICIENT_RESOURCES             0x11
#define ATT_ERROR_UNDEFINED                          0xff

// Execute write request flag
#define ATT_EXECUTE_WRITE_REQUEST_FLAG_CANCEL   0x0
#define ATT_EXECUTE_WRITE_REQUEST_FLAG_WRITE    0x1



// Command RX structure
typedef struct
{
	LE_STATUS Status;
	unsigned char Data[ATT_PDU_LEN_MAX];
	unsigned long DataLen;
}
ATT_PDU_RX;

typedef struct
{
	unsigned short Handle;
	unsigned char Data[ATT_PDU_LEN_MAX];
	unsigned long DataLen;
}
ATT_PDU_RX_INDICATION;


// ATT structure
typedef struct ATT_ ATT;
struct ATT_
{
	// Member
	void *pTransmission;
	ATT_PDU_RX PduRx;
	ATT_PDU_RX_INDICATION PduRxIndication;
	RTK_ATT_RX_QUEUE_HEAD RxNotificationPduHead;
	RTK_ATT_RX_QUEUE_HEAD RxResponseOrCfmPduHead;
	unsigned short MtuDefault;
	unsigned short Mtu;
};


#ifdef __cplusplus
extern "C"{
#endif
// Constructer and destructor
LE_STATUS
__cdecl
ConstructAtt(
	ATT *pAtt,
	unsigned short MtuDefault,
	void *pTransmission
	);

void
 __cdecl
DestructAtt(
	ATT *pAtt
	);

#ifdef __cplusplus
}
#endif

// TX function
LE_STATUS
AttTxErrorResponse(
	ATT *pAtt,
	unsigned char RequestOpCodeInError,
	unsigned short AttributeHandleInError,
	unsigned char ErrorCode
	);

LE_STATUS
AttTxExchangeMtuRequest(
	ATT *pAtt,
	unsigned short ClientRxMtu
	);

LE_STATUS
AttTxExchangeMtuResponse(
	ATT *pAtt,
	unsigned short ServerRxMtu
	);

LE_STATUS
AttTxFindInformationRequest(
	ATT *pAtt,
	unsigned short StartingHandle,
	unsigned short EndingHandle
	);

LE_STATUS
AttTxFindInformationResponse(
	ATT *pAtt,
	unsigned char Format,
	unsigned char *pInformationData,
	unsigned long InformationDataSize
	);

LE_STATUS
AttTxFindByTypeValueRequest(
	ATT *pAtt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	unsigned short AttributeType,
	unsigned char *pAttributeValue,
	unsigned long AttributeValueSize
	);

LE_STATUS
AttTxFindByTypeValueResponse(
	ATT *pAtt,
	unsigned char *pHandlesInformationList,
	unsigned long HandlesInformationListSize
	);

LE_STATUS
AttTxReadByTypeRequest(
	ATT *pAtt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	unsigned char *pAttributeType,
	unsigned long AttributeTypeSize
	);

LE_STATUS
AttTxReadByTypeResponse(
	ATT *pAtt,
	unsigned char Length,
	unsigned char *pAttributeDataList,
	unsigned long AttributeDataListSize
	);

LE_STATUS
AttTxReadRequest(
	ATT *pAtt,
	unsigned short AttributeHandle
	);

LE_STATUS
AttTxReadResponse(
	ATT *pAtt,
	unsigned char *pAttributeValue,
	unsigned long AttributeValueSize
	);

LE_STATUS
AttTxReadBlobRequest(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned short ValueOffset
	);

LE_STATUS
AttTxReadBlobResponse(
	ATT *pAtt,
	unsigned char *pPartAttributeValue,
	unsigned long PartAttributeValueSize
	);

LE_STATUS
AttTxReadMultipleRequest(
	ATT *pAtt,
	unsigned char *pSetOfHandles,
	unsigned long SetOfHandlesSize
	);

LE_STATUS
AttTxReadMultipleResponse(
	ATT *pAtt,
	unsigned char *pSetOfValues,
	unsigned long SetOfValuesSize
	);

LE_STATUS
AttTxReadByGroupTypeRequest(
	ATT *pAtt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	unsigned char *pAttributeGroupType,
	unsigned long AttributeGroupTypeSize
	);

LE_STATUS
AttTxReadByGroupTypeResponse(
	ATT *pAtt,
	unsigned char Length,
	unsigned char *pAttributeDataList,
	unsigned long AttributeDataListSize
	);

LE_STATUS
AttTxWriteRequest(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long AttributeValueSize
	);

LE_STATUS
AttTxWriteResponse(
	ATT *pAtt
	);

LE_STATUS
AttTxWriteCommand(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long AttributeValueSize
	);

LE_STATUS
AttTxSignedWriteCommand(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long AttributeValueSize,
	unsigned char *pAuthenticationSignature
	);

LE_STATUS
AttTxPrepareWriteRequest(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned short ValueOffset,
	unsigned char *pPartAttributeValue,
	unsigned long PartAttributeValueSize
	);

LE_STATUS
AttTxPrepareWriteResponse(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned short ValueOffset,
	unsigned char *pPartAttributeValue,
	unsigned long PartAttributeValueSize
	);

LE_STATUS
AttTxExecuteWriteRequest(
	ATT *pAtt,
	unsigned char Flags
	);

LE_STATUS
AttTxExecuteWriteResponse(
	ATT *pAtt
	);

LE_STATUS
AttTxHandleValueNotification(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long AttributeValueSize
	);

LE_STATUS
AttTxHandleValueIndication(
	ATT *pAtt,
	unsigned short AttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long AttributeValueSize
	);

LE_STATUS
AttTxHandleValueConfirmation(
	ATT *pAtt
	);



// RX extracting function
void
AttExtractErrorResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pRequestOpCodeInError,
	unsigned short *pAttributeHandleInError,
	unsigned char *pErrorCode
	);

void
AttExtractExchangeMtuRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pClientRxMtu
	);

void
AttExtractExchangeMtuResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pServerRxMtu
	);

void
AttExtractFindInformationRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pStartingHandle,
	unsigned short *pEndingHandle
	);

void
AttExtractFindInformationResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pFormat,
	unsigned char *pInformationData,
	unsigned long *pInformationDataSize
	);

void
AttExtractFindByTypeValueRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pStartingHandle,
	unsigned short *pEndingHandle,
	unsigned short *pAttributeType,
	unsigned char *pAttributeValue,
	unsigned long *pAttributeValueSize
	);

void
AttExtractFindByTypeValueResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pHandlesInformationList,
	unsigned long *pHandlesInformationListSize
	);

void
AttExtractReadByTypeRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pStartingHandle,
	unsigned short *pEndingHandle,
	unsigned char *pAttributeType,
	unsigned long *pAttributeTypeSize
	);

void
AttExtractReadByTypeResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pLength,
	unsigned char *pAttributeDataList,
	unsigned long *pAttributeDataListSize
	);

void
AttExtractReadRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle
	);

void
AttExtractReadResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pAttributeValue,
	unsigned long *pAttributeValueSize
	);

void
AttExtractReadBlobRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned short *pValueOffset
	);

void
AttExtractReadBlobResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pPartAttributeValue,
	unsigned long *pPartAttributeValueSize
	);

void
AttExtractReadMultipleRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pSetOfHandles,
	unsigned long *pSetOfHandlesSize
	);

void
AttExtractReadMultipleResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pSetOfValues,
	unsigned long *pSetOfValuesSize
	);

void
AttExtractReadByGroupTypeRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pStartingHandle,
	unsigned short *pEndingHandle,
	unsigned char *pAttributeGroupType,
	unsigned long *pAttributeGroupTypeSize
	);

void
AttExtractReadByGroupTypeResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pLength,
	unsigned char *pAttributeDataList,
	unsigned long *pAttributeDataListSize
	);

void
AttExtractWriteRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long *pAttributeValueSize
	);

void
AttExtractWriteResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen
	);

void
AttExtractWriteCommand(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long *pAttributeValueSize
	);

void
AttExtractSignedWriteCommand(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long *pAttributeValueSize,
	unsigned char *pAuthenticationSignature
	);

void
AttExtractPrepareWriteRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned short *pValueOffset,
	unsigned char *pPartAttributeValue,
	unsigned long *pPartAttributeValueSize
	);

void
AttExtractPrepareWriteResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned short *pValueOffset,
	unsigned char *pPartAttributeValue,
	unsigned long *pPartAttributeValueSize
	);

void
AttExtractExecuteWriteRequest(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned char *pFlags
	);

void
AttExtractExecuteWriteResponse(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen
	);

void
AttExtractHandleValueNotification(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long *pAttributeValueSize
	);

void
AttExtractHandleValueIndication(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen,
	unsigned short *pAttributeHandle,
	unsigned char *pAttributeValue,
	unsigned long *pAttributeValueSize
	);

void
AttExtractHandleValueConfirmation(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen
	);



// Lower layer access function
LE_STATUS
AttTxPdu(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long PduLen
	);

LE_STATUS
AttRxPdu(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long *pPduLen,
	unsigned long TimeOutMs
	);

void
AttGetPdu(
	ATT *pAtt,
	unsigned char *pPdu,
	unsigned long *pPduLen
	);

void
AttGetResponseOrCfmPdu(
					   ATT *pAtt,
					   unsigned char *pPdu,
					   unsigned long *pPduLen
					   );



#endif