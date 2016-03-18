#pragma once

#include "gatt_api_export.h"
#include <Windows.h>
#include <vector>
using namespace std;


#define ATTR_TABLE_LEN 64

//#define LINK_LOST_CALLBACK_TABLE_LEN_MAX 32
typedef UINT (WINAPI * LINK_LOST_CALLBACK)(void * pPrivate);

class CGattInstance;
class CGattEntity;
typedef struct _PARA_NEW_SOCKET{
	HANDLE hEventSockReady;
	CGattEntity* pGattEntity;
	CGattInstance *pGattInstance;
}PARA_NEW_SOCKET;

class CGattEntity
{
public:
	CGattEntity(void);
	virtual ~CGattEntity(void);
public:

	ULONGLONG m_BtAddrRemote;
	GATT m_LeGatt;

private:

//	static unsigned long m_RefCount;

//public:
//	void AddRef();
//	void ReleaseRef();
//	unsigned long GetRef();
public:
	HANDLE m_hThreadRecv;	
	static UINT WINAPI StartToRecv(LPVOID lpvoid);

	//Link lost indication
	HANDLE m_hThreadMonitorLink;
//	vector<LINK_LOST_CALLBACK>	m_pfnLinkLostCallBackTable;
	LINK_LOST_CALLBACK m_pfnLinkLostCallBack;
	void * m_LinkLossCBParam;
	
//Export functions here
public:

	void RtGattInitLinkLostMonitor();

	void RtGattRegisterLinkLostCallback(LINK_LOST_CALLBACK Callback,void *pPrivate);
	void RtGattUnRegisterLinkLostCallback(LINK_LOST_CALLBACK Callback);
	//
	LE_STATUS
		GetRemoteAttrTable(GATT_ATTRIBUTE *pAttrTable, unsigned long AttrTableLength);

	//Role Server Operation
	// Characteristic value notification
	LE_STATUS
		RtGattServerNotifications(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long ValueLen
		);
	LE_STATUS RtGattServerWaitForConfirm();
	// Characteristic value indication
	LE_STATUS
		RtGattServerIndications(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long ValueLen
		);

	void
		RtGattAddCallback(
		GATT_CALLBACK Callback,
		void *pPrivate
		);

	void
		RtGattRemoveCallback(
		GATT_CALLBACK Callback
		);

	void GATTGetRxData(
		OUT unsigned char ** ppRxBuf,
		OUT unsigned long * pRxDataLen
		);

	void GATTGetRxIndicationData(
		unsigned short *pHandle,
		unsigned char *pValue,
		unsigned long *pValueLen
		);


	void GATTParseIndication(
		IN unsigned char *  pAttRawBuf,
		IN unsigned long    AttRawBufLen,
		OUT unsigned short* pHandle,
		OUT unsigned char** ppGattValue,
		OUT unsigned long * pGattValueLen
		);
	LE_STATUS RtGattWaitForIndication();
	LE_STATUS RtGattClientNotifyIndicationFetchedOver();
	LE_STATUS RtGattWaitForNotification();
    LE_STATUS RtGattServerWaitRequests();
	// Sever configuration
	LE_STATUS RtGattClientExchangeMtu();
	LE_STATUS RtGattClientExchangeMtu(unsigned short AttMtu);

	// Primary service discovery
	LE_STATUS
		RtGattClientDiscoverAllPrimaryServices(
		unsigned short StartingHandle,
		unsigned short EndingHandle,
		GATT_GROUP_HANDLE_VALUE *pGroupHandleValueList,
		unsigned long *pGroupHandleValueListLen
		);

	LE_STATUS
		RtGattClientDiscoverPrimaryServiceByServiceUuid(
		unsigned short StartingHandle,
		unsigned short EndingHandle,
		LE_UUID ServiceUuid,
		GATT_GROUP_HANDLE *pGroupHandleList,
		unsigned long *pGroupHandleListLen
		);

	// Relationship discovery
	LE_STATUS
		RtGattClientFindIncludedServices(
		unsigned short StartingHandle,
		unsigned short EndingHandle,
		GATT_HANDLE_INCLUDE *pHandleIncludeList,
		unsigned long *pHandleIncludeListLen
		);

	// Characteristic discovery
	LE_STATUS
		RtGattClientDiscoverAllCharacteristicsOfAService(
		unsigned short StartingHandle,
		unsigned short EndingHandle,
		GATT_HANDLE_CHAR_DECLARATION *pHandleCharDecList,
		unsigned long *pHandleCharDecListLen
		);

	LE_STATUS
		RtGattClientDiscoverCharacteristicsByUuid(
		unsigned short StartingHandle,
		unsigned short EndingHandle,
		LE_UUID CharUuid,
		GATT_HANDLE_CHAR_DECLARATION *pHandleCharDecList,
		unsigned long *pHandleCharDecListLen
		);

	// Characteristic descriptor discovery
	LE_STATUS
		RtGattClientDiscoverAllCharacteristicDescriptors(
		unsigned short StartingHandle,
		unsigned short EndingHandle,
		GATT_HANDLE_TYPE *pHandleTypeList,
		unsigned long *pHandleTypeListLen
		);

	// Characteristic value read
	LE_STATUS
		RtGattClientReadCharacteristicValue(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long *pValueLen
		);

	LE_STATUS
		RtGattClientReadUsingCharacteristicUuid(
		unsigned short StartingHandle,
		unsigned short EndingHandle,
		LE_UUID AttributeTypeUuid,
		GATT_HANDLE_CHAR_VALUE *pHandleCharValueList,
		unsigned long *pHandleCharValueListLen
		);

	LE_STATUS
		RtGattClientReadLongCharacteristicValues(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long *pValueLen
		);

	LE_STATUS
		RtGattClientReadMultipleCharacteristicValues(
		unsigned short *pHandleList,
		unsigned long HandleListLen,
		unsigned char *pSetOfValues,
		unsigned long *pSetOfValuesSize
		);

	// Characteristic value write
	LE_STATUS
		RtGattClientWriteWithoutResponse(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long ValueLen
		);

	LE_STATUS
		RtGattClientSignedWriteWithoutResponse(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long ValueLen,
		unsigned char *pAuthenticationSignature
		);

	LE_STATUS
		RtGattClientWriteCharacteristicValue(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long ValueLen
		);

	LE_STATUS
		RtGattClientWriteLongCharacteristicValues(
		unsigned short Handle,
		unsigned char *pValue,
		unsigned long ValueLen
		);

	LE_STATUS
		RtGattClientReliableWrites(
		GATT_HANDLE_CHAR_VALUE *pHandleCharValueList,
		unsigned long HandleCharValueListLen
		);

	// Characteristic value notification
	LE_STATUS
		RtGattClientNotifications(
		unsigned short *pHandle,
		unsigned char *pValue,
		unsigned long *pValueLen
		);

	// Characteristic value indication
	LE_STATUS
		RtGattClientIndications(
		unsigned short *pHandle,
		unsigned char *pValue,
		unsigned long *pValueLen
		);
	// Characteristic descriptor value read
	LE_STATUS
		RtGattClientReadCharacteristicDescriptors(
		unsigned short Handle,
		unsigned char *pDescriptor,
		unsigned long *pDescriptorLen
		);

	LE_STATUS
		RtGattClientReadLongCharacteristicDescriptors(
		unsigned short Handle,
		unsigned char *pDescriptor,
		unsigned long *pDescriptorLen
		);

	// Characteristic descriptor value write
	LE_STATUS
		RtGattClientWriteCharacteristicDescriptors(
		unsigned short Handle,
		unsigned char *pDescriptor,
		unsigned long DescriptorLen
		);

	LE_STATUS
		RtGattClientWriteLongCharacteristicDescriptors(
		unsigned short Handle,
		unsigned char *pDescriptor,
		unsigned long DescriptorLen
		);

	void RtGattGetNotificationData(
		OUT unsigned char * ppRxBuf,
		OUT unsigned long * pRxDataLen
		);

	void RtGattGetIndicationData(
		OUT unsigned char * ppRxBuf,
		OUT unsigned long * pRxDataLen
		);
	void CGattEntity::RtGattServerGetRequestData(
		OUT unsigned char * ppRxBuf,
		OUT unsigned long * pRxDataLen
		);

};
