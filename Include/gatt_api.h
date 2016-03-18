#ifndef __GATT_API_H
#define __GATT_API_H

/**

Copyright (c) Realtek Corporation. All rights reserved.

\file    att_api.h

\brief   LE generic attribute profile (GATT) functions.

*/


#include "le_basic.h"
#include "assigned_number.h"
#include "att_api.h"
#include "windows.h"
// Definition



// Definition

// Attribute characteristic properties
#define GATT_CHAR_PROPERTY_BROADCAST                     (0x1 << 0)		// Bit 0
#define GATT_CHAR_PROPERTY_READ                          (0x1 << 1)		// Bit 1
#define GATT_CHAR_PROPERTY_WRITE_WITHOUT_RESPONSE        (0x1 << 2)		// Bit 2
#define GATT_CHAR_PROPERTY_WRITE                         (0x1 << 3)		// Bit 3
#define GATT_CHAR_PROPERTY_NOTIFY                        (0x1 << 4)		// Bit 4
#define GATT_CHAR_PROPERTY_INDICATE                      (0x1 << 5)		// Bit 5
#define GATT_CHAR_PROPERTY_AUTHENTICATED_SIGNED_WRITES   (0x1 << 6)		// Bit 6
#define GATT_CHAR_PROPERTY_EXTENDED_PROPERTIES           (0x1 << 7)		// Bit 7

// Attribute characteristic extended properties
#define GATT_CHAR_EXTENDED_PROPERTY_RELIABLE_WRITE         (0x1 << 0)		// Bit 0
#define GATT_CHAR_EXTENDED_PROPERTY_WRITABLE_AUXILIARIES   (0x1 << 1)		// Bit 1

// Attribute client characteristic configuration
#define GATT_CLIENT_CHAR_CONFIG_NONE                       0x0				// 0x0
#define GATT_CLIENT_CHAR_CONFIG_NOTIFICATION               (0x1 << 0)		// Bit 0
#define GATT_CLIENT_CHAR_CONFIG_INDICATION                 (0x1 << 1)		// Bit 1

// Attribute server characteristic configuration
#define GATT_SERVER_CHAR_CONFIG_NONE                       0x0				// 0x0
#define GATT_SERVER_CHAR_CONFIG_BROADCAST                  (0x1 << 0)		// Bit 0


// Attribute permission
#define GATT_ATTRIBUTE_PERMISSION_READ             (0x1 << 0)		// Bit 0
#define GATT_ATTRIBUTE_PERMISSION_WRITE            (0x1 << 1)		// Bit 1
#define GATT_ATTRIBUTE_PERMISSION_AUTHORIZATION    (0x1 << 2)		// Bit 2
#define GATT_ATTRIBUTE_PERMISSION_SECURITY         (0x1 << 3)		// Bit 3

#define GATT_ATTRIBUTE_ENCRYPTION_KEY_SIZE_MASK    (0xf << 4)		// Bit 4 ~ 7
#define GATT_ATTRIBUTE_ENCRYPTION_KEY_SIZE_SHIFT   4

// Attribute handle
#define GATT_ATTRIBUTE_HANDLE_MIN   0x1
#define GATT_ATTRIBUTE_HANDLE_MAX   0xffff

// Information data format
#define GATT_INFO_DATA_FORMAT_UUID_BIT_16    0x1
#define GATT_INFO_DATA_FORMAT_UUID_BIT_128   0x2


// GATT functions
#define GATT_GROUP_HANDLE_VALUE_LIST_LEN_MAX        32
#define GATT_GROUP_HANDLE_LIST_LEN_MAX              32
#define GATT_HANDLE_INCLUDE_LIST_LEN_MAX            32
#define GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX   32
#define GATT_HANDLE_TYPE_LIST_LEN_MAX               32
#define GATT_HANDLE_CHAR_VALUE_LIST_LEN_MAX         32
#define GATT_CHAR_VALUE_LIST_LEN_MAX				32


// Error message structure
typedef struct
{
	unsigned char RequestOpCodeInError;
	unsigned short AttributeHandleInError;
	unsigned char ErrorCode;
}
GATT_ERROR_MESSAGE;


// Attribute type structure
typedef struct
{
	LE_UUID Uuid;
}
GATT_ATTRIBUTE_TYPE;


// Attribute value service structure
typedef struct
{
	LE_UUID Uuid;
}
GATT_SERVICE;


// Attribute value include structure
typedef struct
{
	unsigned short HandleStart;
	unsigned short HandleEnd;
	unsigned char IsUuidVaild;
	LE_UUID Uuid;
}
GATT_INCLUDE;


// Attribute value characteristic structure

// Characteristic declaration
typedef struct
{
	unsigned char Property;
	unsigned short ValueHandle;
	LE_UUID Uuid;
}
GATT_CHAR_DECLARATION;

// Characteristic value declaration
typedef struct
{
	unsigned char Bytes[ATT_LONG_ATTR_VALUE_LEN_MAX];
	unsigned long ByteNum;
}
GATT_CHAR_VALUE;

typedef union
{
	unsigned char onebit:1;
	unsigned char twobits:2;
	unsigned char nibble:4;
	unsigned char uint8;
	//define more here

}GATT_DESCRIPTOR_FORMAT_VALUE;



// Characteristic descriptor declaration
#define GATT_AGGREGATE_FORMAT_HANDLE_LIST_LEN_MAX   16
typedef union
{
	// Characteristic extended properties
	unsigned short ExtendedProperties;

	// Characteristic user description
	struct
	{
		unsigned char Text[ATT_LONG_ATTR_VALUE_LEN_MAX];
		unsigned long ByteNum;
	}
	String;

	// Client characteristic configuration
	unsigned short ClientConfigurationBits;

	// Server characteristic configuration
	unsigned short ServerConfigurationBits;

	// Characteristic presentation format
	struct
	{
		unsigned char Format;
		unsigned char Exponent;
		unsigned short Unit;
		unsigned char NameSpace;
		unsigned short Description;
	}
	PresentationFormat;

	// Characteristic aggregate format
	struct
	{
		unsigned short Hanlde[GATT_AGGREGATE_FORMAT_HANDLE_LIST_LEN_MAX];
		unsigned long Len;
	}
	AttributeHandleList;
	
	//valid range
	struct 
	{
		GATT_DESCRIPTOR_FORMAT_VALUE	LowerInclusive;
		GATT_DESCRIPTOR_FORMAT_VALUE	UpperInclusive;
	}ValidRange;

	//
	struct  
	{
        unsigned char ReportId;
		unsigned char ReportType;
    //    unsigned short Handle;
	}HidRpIdMap;

	unsigned short UUID;
}
GATT_CHAR_DESCRIPTOR;

typedef union
{
	GATT_CHAR_DECLARATION Declaration;
	GATT_CHAR_VALUE Value;
	GATT_CHAR_DESCRIPTOR Descriptor;
}
GATT_CHAR;


// Attribute value structure
typedef union
{
	GATT_SERVICE Service;
	GATT_INCLUDE Include;
	GATT_CHAR Char;
}
GATT_ATTRIBUTE_VALUE;


// Attribute structure
typedef struct
{
	unsigned short Handle;
	GATT_ATTRIBUTE_TYPE Type;
	GATT_ATTRIBUTE_VALUE Value;
	unsigned long Permissions;
}
GATT_ATTRIBUTE;


// Group handle-value entry
typedef struct
{
	unsigned short HandleStart;
	unsigned short HandleEnd;
	LE_UUID Value;
}
GATT_GROUP_HANDLE_VALUE;

// Group handle entry
typedef struct
{
	unsigned short HandleStart;
	unsigned short HandleEnd;
}
GATT_GROUP_HANDLE;

// Handle-include entry
typedef struct
{
	unsigned short Handle;
	GATT_INCLUDE Include;
}
GATT_HANDLE_INCLUDE;

// Handle-CharDeclaration entry
typedef struct
{
	unsigned short Handle;
	GATT_CHAR_DECLARATION CharDec;
}
GATT_HANDLE_CHAR_DECLARATION;

// Handle-Type entry
typedef struct
{
	unsigned short Handle;
	LE_UUID Type;
}
GATT_HANDLE_TYPE;

// Handle-CharValue entry
typedef struct
{
	unsigned short Handle;
	GATT_CHAR_VALUE CharValue;
}
GATT_HANDLE_CHAR_VALUE;

// Write queue entry
typedef struct
{
	unsigned short Handle;
	unsigned short Offset;
	unsigned char Value[ATT_PDU_LEN_MAX];
	unsigned long ValueSize;
}
GATT_WRITE_QUEUE;


// GATT structure
typedef struct GATT_ GATT;

typedef void
(*GATT_CALLBACK)(
	GATT *pGatt,
	GATT_ATTRIBUTE *pAttr,
	void *pPrivate
	);

typedef struct
{
	GATT_CALLBACK Callback;
	void *pPrivate;
}
GATT_CALLBACK_ENTRY;

#define GATT_WRITE_QUEUE_LEN_MAX      32
#define GATT_CALLBACK_TABLE_LEN_MAX   32
struct GATT_
{
	// Member
	ATT *pAtt;
	GATT_ATTRIBUTE *pAttrTable;
	GATT_WRITE_QUEUE WriteQueue[GATT_WRITE_QUEUE_LEN_MAX];
	unsigned long WriteQueueLen;
	GATT_CALLBACK_ENTRY CallbackTable[GATT_CALLBACK_TABLE_LEN_MAX];
	unsigned long CallbackTableLen;
    HANDLE SignaResponseArrivalEvent;//used by client
    HANDLE SignalIndicationArrivalEvent;//used by to notify upper indication data arrives
	HANDLE SignalIndicationDataFetchedByUpper;//used by upper to notify gatt, indiaction data has been taken,and should send confimrtion
	HANDLE SignalNotificationArrivalEvent;//used by client
    HANDLE SignalConfirmationArrivalEvent;//used buy server
	HANDLE SignalL2capDisconnetedEvent;
};


#ifdef __cplusplus
extern "C" {
#endif


// Constructer and destructor
LE_STATUS
__cdecl
ConstructGatt(
	GATT *pGatt,
	ATT *pAtt,
	GATT_ATTRIBUTE *pAttrTable
	);

void
 __cdecl
DestructGatt(
	GATT *pGatt
	);



// Callback
void
__cdecl
GattAddCallback(
	GATT *pGatt,
	GATT_CALLBACK Callback,
	void *pPrivate
	);

void
__cdecl
GattRemoveCallback(
	GATT *pGatt,
	GATT_CALLBACK Callback
	);

void
GattExecuteAllCallback(
	GATT *pGatt,
	GATT_ATTRIBUTE *pAttr
	);

// Receive PDU.
LE_STATUS
__cdecl
GattRxPdu(
       GATT *pGatt,
	unsigned short *pHandle,
	unsigned char *pValue,
	unsigned long *pValueLen
	);

// Server procedure

// Receive PDU.
LE_STATUS
__cdecl
GattServerRxPdu(
	GATT *pGatt
	);

// Request and command
LE_STATUS
GattServerExchangeMtuRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerFindInformationRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerFindByTypeValueRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerReadByTypeRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerReadRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerReadBlobRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerReadMultipleRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerReadByGroupTypeRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerWriteRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerWriteCommand(
	GATT *pGatt
	);

LE_STATUS
GattServerSignedWriteCommand(
	GATT *pGatt
	);

LE_STATUS
GattServerPrepareWriteRequest(
	GATT *pGatt
	);

LE_STATUS
GattServerExecuteWriteRequest(
	GATT *pGatt
	);

// Characteristic value notification
LE_STATUS
__cdecl
GattServerNotifications(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long ValueLen
	);

// Characteristic value indication
LE_STATUS
__cdecl
GattServerIndications(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long ValueLen
	);

LE_STATUS
__cdecl
GattServerWaitForConfirm(GATT *pGatt);


// Client procedure

// Receive PDU.
LE_STATUS
__cdecl
GattClientRxPdu(
	GATT *pGatt,
	unsigned short *pHandle,
	unsigned char *pValue,
	unsigned long *pValueLen
	);

// Sever configuration
LE_STATUS
__cdecl
GattClientExchangeMtu(
	GATT *pGatt
	);

LE_STATUS
__cdecl
GattClientExchangeMtuWithMtu(
							 GATT *pGatt,
							 unsigned short AttMtu
							 );

// Primary service discovery
LE_STATUS
__cdecl
GattClientDiscoverAllPrimaryServices(
	GATT *pGatt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	GATT_GROUP_HANDLE_VALUE *pGroupHandleValueList,
	unsigned long *pGroupHandleValueListLen
	);

LE_STATUS
    __cdecl
GattClientDiscoverPrimaryServiceByServiceUuid(
	GATT *pGatt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	LE_UUID ServiceUuid,
	GATT_GROUP_HANDLE *pGroupHandleList,
	unsigned long *pGroupHandleListLen
	);

// Relationship discovery
LE_STATUS
__cdecl
GattClientFindIncludedServices(
	GATT *pGatt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	GATT_HANDLE_INCLUDE *pHandleIncludeList,
	unsigned long *pHandleIncludeListLen
	);

// Characteristic discovery
LE_STATUS
__cdecl
GattClientDiscoverAllCharacteristicsOfAService(
	GATT *pGatt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	GATT_HANDLE_CHAR_DECLARATION *pHandleCharDecList,
	unsigned long *pHandleCharDecListLen
	);

LE_STATUS
    __cdecl
GattClientDiscoverCharacteristicsByUuid(
	GATT *pGatt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	LE_UUID CharUuid,
	GATT_HANDLE_CHAR_DECLARATION *pHandleCharDecList,
	unsigned long *pHandleCharDecListLen
	);

// Characteristic descriptor discovery
LE_STATUS
__cdecl
GattClientDiscoverAllCharacteristicDescriptors(
	GATT *pGatt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	GATT_HANDLE_TYPE *pHandleTypeList,
	unsigned long *pHandleTypeListLen
	);

// Characteristic value read
LE_STATUS
__cdecl
GattClientReadCharacteristicValue(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long *pValueLen
	);

LE_STATUS
__cdecl
GattClientReadUsingCharacteristicUuid(
	GATT *pGatt,
	unsigned short StartingHandle,
	unsigned short EndingHandle,
	LE_UUID AttributeTypeUuid,
	GATT_HANDLE_CHAR_VALUE *pHandleCharValueList,
	unsigned long *pHandleCharValueListLen
	);

LE_STATUS
__cdecl
GattClientReadLongCharacteristicValues(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long *pValueLen
	);

LE_STATUS
__cdecl
GattClientReadMultipleCharacteristicValues(
	GATT *pGatt,
	unsigned short *pHandleList,
	unsigned long HandleListLen,
	unsigned char *pSetOfValues,
	unsigned long *pSetOfValuesSize
	);

// Characteristic value write
LE_STATUS
__cdecl
GattClientWriteWithoutResponse(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long ValueLen
	);

LE_STATUS
    __cdecl
GattClientSignedWriteWithoutResponse(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long ValueLen,
	unsigned char *pAuthenticationSignature
	);

LE_STATUS
    __cdecl
GattClientWriteCharacteristicValue(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long ValueLen
	);

LE_STATUS
    __cdecl
GattClientWriteLongCharacteristicValues(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pValue,
	unsigned long ValueLen
	);

LE_STATUS
    __cdecl
GattClientReliableWrites(
	GATT *pGatt,
	GATT_HANDLE_CHAR_VALUE *pHandleCharValueList,
	unsigned long HandleCharValueListLen
	);

// Characteristic value notification
LE_STATUS
__cdecl
GattClientNotifications(
GATT *pGatt,
	unsigned short *pHandle,
	unsigned char *pValue,
	unsigned long *pValueLen
	);

// Characteristic value indication
LE_STATUS
__cdecl
GattClientIndications(
	GATT *pGatt,
	unsigned short *pHandle,
	unsigned char *pValue,
	unsigned long *pValueLen
	);

// Characteristic descriptor value read
LE_STATUS
__cdecl
GattClientReadCharacteristicDescriptors(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pDescriptor,
	unsigned long *pDescriptorLen
	);

LE_STATUS
    __cdecl
GattClientReadLongCharacteristicDescriptors(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pDescriptor,
	unsigned long *pDescriptorLen
	);

// Characteristic descriptor value write
LE_STATUS
__cdecl
GattClientWriteCharacteristicDescriptors(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pDescriptor,
	unsigned long DescriptorLen
	);

LE_STATUS
    __cdecl
GattClientWriteLongCharacteristicDescriptors(
	GATT *pGatt,
	unsigned short Handle,
	unsigned char *pDescriptor,
	unsigned long DescriptorLen
	);



// GATT tools
int
UuidIsGattDefined(
	LE_UUID Uuid
	);

void
 __cdecl
AttrGetHandle(
	GATT_ATTRIBUTE *pAttr,
	unsigned short *pHandle
	);

void
 __cdecl
AttrGetTypeUuid(
	GATT_ATTRIBUTE *pAttr,
	LE_UUID *pTypeUuid
	);

void
AttrGetPermissions(
	GATT_ATTRIBUTE *pAttr,
	unsigned long *pPermissions
	);

void
AttrGetValueLen(
	GATT_ATTRIBUTE *pAttr,
	unsigned long *pValueLen
	);

void
AttrGetValueLenMax(
	GATT_ATTRIBUTE *pAttr,
	unsigned long *pValueLenMax
	);

void
__cdecl
AttrGetValue(
	GATT_ATTRIBUTE *pAttr,
	unsigned char *pBuffer
	);

void
AttrSetValue(
	GATT_ATTRIBUTE *pAttr,
	unsigned char *pValue,
	unsigned long ValueLen,
	GATT *pGatt
	);

LE_STATUS
GetAttrRangeByHandle(
	GATT_ATTRIBUTE *pAttrTable,
	unsigned short HandleStart,
	unsigned short HandleEnd,
	GATT_ATTRIBUTE **ppAttrStart,
	GATT_ATTRIBUTE **ppAttrEnd
	);

LE_STATUS
FindAttrInfoByGroupType(
	GATT_ATTRIBUTE *pTableStart,
	GATT_ATTRIBUTE *pTableEnd,
	unsigned short GroupType,
	GATT_ATTRIBUTE **ppGroupStart,
	GATT_ATTRIBUTE **ppGroupEnd
	);

LE_STATUS
FindAttrInfoByType(
	GATT_ATTRIBUTE *pAttrStart,
	GATT_ATTRIBUTE *pAttrEnd,
	unsigned short Type,
	GATT_ATTRIBUTE **ppAttrTarget
	);

LE_STATUS
FindAttrInfoByHandle(
	GATT_ATTRIBUTE *pAttrStart,
	GATT_ATTRIBUTE *pAttrEnd,
	unsigned short Handle,
	GATT_ATTRIBUTE **ppAttrTarget
	);


LE_STATUS
__cdecl
GattClientWaitIndications(GATT *pGatt);

LE_STATUS
__cdecl
GattClientNotifyIndicationFetchedOver(GATT *pGatt);

LE_STATUS
__cdecl
GattClientWaitNotifications(GATT *pGatt);



LE_STATUS
__cdecl
GattClientGetNotificationData(IN GATT *pGatt, 	  
							  OUT unsigned char * ppRxBuf,
							  OUT unsigned long * pRxDataLen);

LE_STATUS
__cdecl
GattClientGetIndicationData(IN GATT *pGatt, 	  
							OUT unsigned char * ppRxBuf,
							OUT unsigned long * pRxDataLen);



#ifdef __cplusplus
}
#endif



#endif