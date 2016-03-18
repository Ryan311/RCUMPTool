/**

Copyright @ Realtek Semiconductor Corp. All Rights Reserved.

THIS CODE AND INFORMATION IS PROVIDED [AS] [IS] WITHOUT WARRANTY OF ANY
KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.


\file :

DFU_Client.cpp

\brief  :   

Implementation of DFU client api.

Rui Huang

Environment:   

Win7 user mode only

Comment:



Revision History:
When            Who                         What                   
-------------   ------------------------    -----------------------
2014/10/27      Rui_huang
*/

#include "stdafx.h"

#include "gatt_api.h"
#include "assigned_number.h"
#include "GattEntity.h"
#include "GattInstance.h"
#include "reg_le.h"
#include "LEHIDClient.h"
#include "GattPublic.h"

#include "DFU_Def.h"

#include <tchar.h>
#include "trace.h"
#include "aes.h"

#ifdef WPP_TRACING_ENABLE
#include "DFU_Client.tmh"
#endif

int _CPU_FREQ = 0; //定义一个全局变量保存 CPU 频率 (MHz)
extern aes_context ctx;
extern unsigned char AES_Key[32]; 
extern UINT8 RandData[16];

CDFUClient::CDFUClient()
{
	m_pGattInstance = NULL;
	m_pGattClient = NULL;
	m_pUpperUI = NULL;
	ZeroMemory(&m_DfuDataInfo,sizeof(m_DfuDataInfo));
	m_pPrivate = NULL;                       // call back context.

	m_DFUServerHandleStart = 0;    // DFU service start handle.
	m_DFUServerHandleEnd = 0;      // DFU service end handle.

	m_ullAddr = 0;
	ZeroMemory(&m_DFUHandles,sizeof(m_DFUHandles));
	ZeroMemory(&m_DfuDataInfo,sizeof(m_DfuDataInfo));
	ZeroMemory(&ImgHeader,sizeof(ImgHeader));

	m_bIsSetOnNotify = FALSE;
	m_bIsDisconnect = FALSE; 
	m_bIsUIClosing = FALSE;
}

CDFUClient::~CDFUClient()
{
	UnInit();
}


BOOL CDFUClient::Init(unsigned long long ServerAddr)
{
	BOOL bRet = FALSE;

	if(m_pGattInstance == NULL)
	{
		m_pGattInstance = CGattInstance::GetGattInstance();
	}

	if(m_pGattInstance == NULL)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("GetGattInstance Fail")));
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}

	m_ullAddr = ServerAddr;
	return bRet;
}

VOID CDFUClient::UnInit()
{
	if (0 != m_ullAddr)
	{
		Disconnect(m_ullAddr);
		m_ullAddr = 0;
	}
	if(m_pGattInstance)
	{
		m_pGattInstance->Release();
		m_pGattInstance = NULL;
	}
}

VOID CDFUClient::SetSecLevel(UINT8 SecLevel)
{
	m_SecLevel = SecLevel;
}

RT_STATUS CDFUClient::ConnectTo(
	IN unsigned long long ServerAddress
)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	ASSERT(0 != ServerAddress);
	
	m_pGattClient = m_pGattInstance->GATTConnect(ServerAddress);
	if (NULL == m_pGattClient)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Connect to server fail")));
		return RT_STATUS_FAILURE;
	}

	m_ullAddr = ServerAddress;
	//if(m_bIsSetOnNotify == FALSE)
	{
		//Discover Services
		Status = DiscoverDFUServices(m_pGattClient);
		if (RT_STATUS_SUCCESS != Status)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("DiscoverDFUServices error")));
			goto exit;
		}
		Status = DiscoverDFUDataChar(m_pGattClient);
		if (RT_STATUS_SUCCESS != Status)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("DiscoverDFU error")));
			goto exit;
		}
		Status = DiscoverControlPointChar(m_pGattClient);
		if (RT_STATUS_SUCCESS != Status)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("DiscoverControlPointChar error")));
			goto exit;
		}
	}

	// Enable Notification
	EnableNotifyIndicate();
	Status = RT_STATUS_SUCCESS; 
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--")));
	return Status;
}

void CDFUClient::Disconnect(IN unsigned long long ServerAddress)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	ASSERT(ServerAddress != 0);

	CString DbgString;
	
	if (m_pGattInstance == NULL)
	{
		return;
	}

	CGattEntity * pGattClient = m_pGattInstance->GetGattEntityByAddr(ServerAddress);
	if(NULL == pGattClient)
	{
		return;
	}

	m_pGattInstance->GATTClose(pGattClient->m_BtAddrRemote);
	m_pGattClient = NULL;
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--")));
}


//RT_STATUS CDFUClient::DiscoverDFUService(
//		IN CGattEntity * pGattClient
//)
//{
//	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
//	RT_STATUS Status = RT_STATUS_SUCCESS;
//
//	if(pGattClient == NULL)
//	{
//		Status = RT_STATUS_NOT_CONNECT;
//		goto exit; 
//	}
//
//	GATT_GROUP_HANDLE HandleList[GATT_GROUP_HANDLE_LIST_LEN_MAX];
//	unsigned long  GroupHandleListLen = 0;
//
//	LE_UUID DfuServiceUuid;				//0x12a24d2efe14488e93d2173cffd00000
//	unsigned char UuidDfuService[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0x87, 0x62, 0x00, 0x00};
//	LE_16_BYTE DfuSercie;
//	memcpy(DfuSercie.Value, UuidDfuService, 16);
//	LeUuidSetBit128(&DfuServiceUuid, DfuSercie);	//discover DFU Service
//
//	LE_STATUS LeStatus = pGattClient->RtGattClientDiscoverPrimaryServiceByServiceUuid(
//		LE_TABLE_START_HANDLE,
//		LE_TABLE_END_HANDLE,
//		DfuServiceUuid,
//		HandleList,
//		&GroupHandleListLen);
//
//	if (LE_SUCCESS != LeStatus)
//	{
//		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("RtGattClientDiscoverPrimaryServiceByServiceUuid with error(%d)"),LeStatus));
//		Status = RT_STATUS_PROCEDURE_FAIL;
//		goto exit; 
//	}
//	else
//	{
//		//
//		//  *NOTICE*: only record first Group Handle.
//		//
//		m_DFUServerHandleStart = HandleList[0].HandleStart;
//		m_DFUServerHandleEnd = HandleList[0].HandleEnd;
//		Status = RT_STATUS_SUCCESS;
//		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("HandleStart %d, HandleEnd %d"),m_DFUServerHandleStart, m_DFUServerHandleEnd));
//	}
//	
//
//	{// Handle Shake Service
//		GATT_GROUP_HANDLE HandleList2[GATT_GROUP_HANDLE_LIST_LEN_MAX];
//		GroupHandleListLen = 0;
//		unsigned char UuidHandShake[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0xff, 0xd1, 0x00, 0x00};
//		LE_16_BYTE HandShakeService;		//0x12a24d2efe14488e93d2173cFFd10000
//		memcpy(HandShakeService.Value, UuidHandShake, 16);
//		LE_UUID HandShakeServiceUuid;				//0x12a24d2efe14488e93d2173c87620000
//		LeUuidSetBit128(&HandShakeServiceUuid, HandShakeService);	//discover HandShake Service
//
//		LE_STATUS LeStatus = pGattClient->RtGattClientDiscoverPrimaryServiceByServiceUuid(
//			LE_TABLE_START_HANDLE,
//			LE_TABLE_END_HANDLE,
//			HandShakeServiceUuid,
//			HandleList2,
//			&GroupHandleListLen);
//
//		if (LE_SUCCESS != LeStatus)
//		{
//			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("RtGattClientDiscoverPrimaryServiceByServiceUuid with error(%d)"),LeStatus));
//			Status = RT_STATUS_PROCEDURE_FAIL;
//			goto exit; 
//		}
//		else
//		{
//			//
//			//  *NOTICE*: only record first Group Handle.
//			//
//			m_HandShakeService.HandleStart = HandleList[0].HandleStart;
//			m_HandShakeService.HandleEnd = HandleList[0].HandleEnd;
//			Status = RT_STATUS_SUCCESS;
//			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("HandShake HandleStart %d, HandleEnd %d"),m_HandShakeService.HandleStart, m_HandShakeService.HandleEnd));
//		}
//	}
//
//
//exit:
//	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("-- Status(%x)"),Status));
//	return Status;
//}

RT_STATUS CDFUClient::DiscoverDFUServices(IN CGattEntity * pGattClient)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;

	if(pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit; 
	}

	GATT_GROUP_HANDLE_VALUE AllServiceList[GATT_GROUP_HANDLE_LIST_LEN_MAX];
	unsigned long  AllServiceListLen = 0;

	LE_UUID DfuServiceUuid;				//0x12a24d2efe14488e93d2173cffd00000
	unsigned char UuidDfuService[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0x87, 0x62, 0x00, 0x00};
	LE_16_BYTE DfuSercie;
	memcpy(DfuSercie.Value, UuidDfuService, 16);
	LeUuidSetBit128(&DfuServiceUuid, DfuSercie);	//discover DFU Service

	unsigned char UuidHandShake[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0xff, 0xd1, 0x00, 0x00};
	LE_16_BYTE HandShakeService;		//0x12a24d2efe14488e93d2173cFFd10000
	memcpy(HandShakeService.Value, UuidHandShake, 16);
	LE_UUID HandShakeServiceUuid;				//0x12a24d2efe14488e93d2173c87620000
	LeUuidSetBit128(&HandShakeServiceUuid, HandShakeService);	//discover HandShake Service

	LE_STATUS LeStatus = m_pGattClient->RtGattClientDiscoverAllPrimaryServices(
		LE_TABLE_START_HANDLE,
		LE_TABLE_END_HANDLE,
		AllServiceList,
		&AllServiceListLen);

	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("RtGattClientDiscoverAllPrimaryServices with error(%d)"),LeStatus));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit; 
	}

	bool hasDFUSer = FALSE;
	unsigned int ServiceListIndex = 0;

	for(ServiceListIndex=0;ServiceListIndex<AllServiceListLen;ServiceListIndex++)
	{
		if (LeUuidIsEqualToBit128(DfuServiceUuid,AllServiceList[ServiceListIndex].Value.Value.Bit128))
		{
			m_DFUServerHandleStart = AllServiceList[ServiceListIndex].HandleStart;
			m_DFUServerHandleEnd = AllServiceList[ServiceListIndex].HandleEnd;
			hasDFUSer = TRUE;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has OTA Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
		}
		else if (LeUuidIsEqualToBit128(HandShakeServiceUuid,AllServiceList[ServiceListIndex].Value.Value.Bit128))
		{
			m_HandShakeService.HandleStart = AllServiceList[ServiceListIndex].HandleStart;
			m_HandShakeService.HandleEnd = AllServiceList[ServiceListIndex].HandleEnd;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has HandShake Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
		}
	}

	if(!hasDFUSer)
		Status = RT_STATUS_NOT_SUPPORT;


exit:
	return Status;
}

RT_STATUS CDFUClient::DiscoverDFUDataChar(IN CGattEntity * pGattClient)
{

	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;

	if(pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit; 
	}

	GATT_HANDLE_CHAR_DECLARATION HandleCharDecList[GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX];
	ULONG HandleCharDecListLen = 0;

	LE_UUID DFUCharUuidShort;
	LeUuidSetBit16(&DFUCharUuidShort, AN_DEVICE_DFU_DATA_SHORT);

	unsigned char UuidDFUDataChar[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0x87, 0x63, 0x00, 0x00};
	LE_16_BYTE DFUDataChar;		//0x12a24d2efe14488e93d2173c87630000
	memcpy(DFUDataChar.Value, UuidDFUDataChar, 16);
	LE_UUID DFUCharUuid;				//0x12a24d2efe14488e93d2173c87620000
	LeUuidSetBit128(&DFUCharUuid, DFUDataChar);	//discover HandShake Service

	LE_STATUS LeStatus = pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
		m_DFUServerHandleStart,
		m_DFUServerHandleEnd,
		HandleCharDecList,
		&HandleCharDecListLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Find All Chars Error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	unsigned long HandleCharDecListLenIndex = 0;
	for (HandleCharDecListLenIndex = 0; HandleCharDecListLenIndex < HandleCharDecListLen; ++HandleCharDecListLenIndex)
	{
		if (LeUuidIsEqualTo(HandleCharDecList[HandleCharDecListLenIndex].CharDec.Uuid, DFUCharUuidShort)
			|| 	LeUuidIsEqualToBit128(DFUCharUuid, HandleCharDecList[HandleCharDecListLenIndex].CharDec.Uuid.Value.Bit128))
		{
			m_DFUHandles.DFUDataHandle = HandleCharDecList[HandleCharDecListLenIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("DFU Data Value (0x%04x)"),m_DFUHandles.DFUDataHandle));
			Status = RT_STATUS_SUCCESS;
			break;
		}
	}
	if (HandleCharDecListLenIndex>= HandleCharDecListLen)
	{
		Status = RT_STATUS_NOT_SUPPORT;
		goto exit;
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("-- Status(%x)"),Status));
	return Status;
}

RT_STATUS CDFUClient::DiscoverHandShakeSrvChar()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	GATT_HANDLE_CHAR_DECLARATION HandShakeChar;	// Only one
	unsigned long HandShakeCharDeclListLen = 0;

	LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
		m_HandShakeService.HandleStart,
		m_HandShakeService.HandleEnd,
		&HandShakeChar,
		&HandShakeCharDeclListLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Get RCU Hand Shake Service Characteristic Error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	LE_UUID uuidHandShake;
	LeUuidSetBit16(&uuidHandShake,HAND_SHAKE_CHAR);

	if (LeUuidIsEqualTo(uuidHandShake, HandShakeChar.CharDec.Uuid))
	{
		m_HandShakeCharValueHandle =  HandShakeChar.CharDec.ValueHandle;
		m_HandShakeNotificationHandle = m_HandShakeCharValueHandle + 1;
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("RCU Hand Shake Char(0x%04x),HandShakeNotificationHandle(0x%04x)"),
			m_HandShakeCharValueHandle,m_HandShakeCharValueHandle));
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status %x"),Status));
	return Status;
}

RT_STATUS CDFUClient::DiscoverControlPointChar(IN CGattEntity * pGattClient)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if(pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit; 
	}

	GATT_HANDLE_CHAR_DECLARATION DFUHandleCharDecList[GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX];
	ULONG HandleCharDecListLen = 0;

	LE_UUID SCControlPointCharUuidShort;
	LeUuidSetBit16(&SCControlPointCharUuidShort, AN_DEVICE_FIRMWARE_UPDATE_CHAR_SHORT);

	unsigned char UuidDFUCPChar[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0x87, 0x64, 0x00, 0x00};
	LE_16_BYTE DFUCPChar;		//0x12a24d2efe14488e93d2173c87640000
	memcpy(DFUCPChar.Value, UuidDFUCPChar, 16);
	LE_UUID DFUCPCharUuid;
	LeUuidSetBit128(&DFUCPCharUuid, DFUCPChar);	//discover HandShake Service

	LE_STATUS LeStatus = pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
		m_DFUServerHandleStart,
		m_DFUServerHandleEnd,
		DFUHandleCharDecList,
		&HandleCharDecListLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Find All Chars Error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	unsigned long HandleCharDecListLenIndex = 0;
	for (HandleCharDecListLenIndex = 0; HandleCharDecListLenIndex < HandleCharDecListLen; ++HandleCharDecListLenIndex)
	{
		if (LeUuidIsEqualTo(DFUHandleCharDecList[HandleCharDecListLenIndex].CharDec.Uuid, SCControlPointCharUuidShort)
			|| 	LeUuidIsEqualToBit128(DFUCPCharUuid, DFUHandleCharDecList[HandleCharDecListLenIndex].CharDec.Uuid.Value.Bit128))
		{
			m_DFUHandles.ControlPointValueHandle = DFUHandleCharDecList[HandleCharDecListLenIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("DFU ControlPointValueHandle (0x%04x)"),m_DFUHandles.ControlPointValueHandle));
			Status = RT_STATUS_SUCCESS;
			break;
		}           
	}

	//Find Client char config handle of Control Point
	// Find Descriptors of DFU Char
	GATT_HANDLE_TYPE DescriptorsHandleTypeList[GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX];
	unsigned long DescriptorsHandleTypeListLen;
	LE_UUID ClientConfigUuid;
	LeUuidSetBit16(&ClientConfigUuid, AN_CLIENT_CHARACTERISTIC_CONFIGURATION);

	USHORT FindHandleStart = 0,FindHandleEnd = 0;
	FindHandleStart = DFUHandleCharDecList[HandleCharDecListLenIndex].Handle + 1;
	if (HandleCharDecListLenIndex == (HandleCharDecListLen - 1))
	{
		FindHandleEnd = m_DFUServerHandleEnd;
	}
	else
	{
		FindHandleEnd = DFUHandleCharDecList[HandleCharDecListLenIndex+1].Handle + 1;
	}
	LeStatus = pGattClient->RtGattClientDiscoverAllCharacteristicDescriptors(
		FindHandleStart,FindHandleEnd,
		DescriptorsHandleTypeList,
		&DescriptorsHandleTypeListLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Find All Descriptors error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	unsigned long DescriptorsHandleTypeListLenIndex = 0;
	for (;DescriptorsHandleTypeListLenIndex<DescriptorsHandleTypeListLen;DescriptorsHandleTypeListLenIndex++)
	{
		if (LeUuidIsEqualTo(DescriptorsHandleTypeList[DescriptorsHandleTypeListLenIndex].Type, ClientConfigUuid))
		{
			m_DFUHandles.ControlPointClientConfigHandle = DescriptorsHandleTypeList[DescriptorsHandleTypeListLenIndex].Handle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T(" DFU ControlPointClientConfigHandle (0x%04x)"),
				m_DFUHandles.ControlPointClientConfigHandle));
			Status = RT_STATUS_SUCCESS;
			goto exit;
		}
	}
	Status = RT_STATUS_PROCEDURE_FAIL;

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("-- Status(%x)"),Status));
	return Status;
}

RT_STATUS CDFUClient::EnableNotifyIndicate()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	LE_STATUS LeStatus = LE_SUCCESS;
	if(m_DFUHandles.ControlPointClientConfigHandle)
	{
		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(m_DFUHandles.ControlPointClientConfigHandle,(unsigned char *)&CurrentNotifyValue,sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("write DFU Measure client Configuration Value Handle (0x%04x) error "),m_DFUHandles.ControlPointClientConfigHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--Status(%x)"), Status));
	return Status;

}


VOID CDFUClient::SetLeControlUIClosed(bool Value)
{
	m_bIsUIClosing = Value;
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("SetLeControlUIClosed closed")));
}

// For DFU Procedure
/************************************************************************/
/* Load FW Patch file                                                  */
/************************************************************************/
RT_STATUS CDFUClient::OTALoadFWImageFromFile(CString &pszFileName)
{
	RT_STATUS Status = RT_STATUS_SUCCESS;
	CFileException e;
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("\nFW Patch File name is %ws\n"), pszFileName));
	if(!ImageFile.Open(pszFileName, CFile::modeRead, &e))
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Open file error, %d"), e.m_cause));
		return RT_STATUS_NO_SUCH_FILE_DIRECTORY;
	}

	ImageFile.Read(&ImgHeader, sizeof(ImgHeader));
	ImageFile.Read(DataBuf, ImgHeader.length*4);
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("Image Info: \n\toffset(0x%x), \n\tsignature(0x%x), \n\tversion(0x%x), \n\tchecksum(0x%x), \n\tlength(0x%x), \n\tota_flag(0x%x), \n\treserved_8(0x%x)\n"), 
		ImgHeader.offset, ImgHeader.signature, ImgHeader.version, ImgHeader.checksum, ImgHeader.length, ImgHeader.ota_flag, ImgHeader.reserved_8));

	ImageFile.Close();
	return Status;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
RT_STATUS CDFUClient::OTAGetTargetImageInfo()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if (m_pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit;
	}
	LE_STATUS LeStatus = LE_SUCCESS;
	REPORT_RECEIVED_IMAGE_INFO targetImageInfo;
	targetImageInfo.Opcode = OPCODE_DFU_REPORT_RECEIVED_IMAGE_INFO;
	targetImageInfo.nSignature = ImgHeader.signature;

	LeStatus = m_pGattClient->RtGattClientWriteCharacteristicValue(
		m_DFUHandles.ControlPointValueHandle,(UINT8*)&targetImageInfo,sizeof(targetImageInfo));
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Write Value error%x"),LeStatus));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	// Handle response
	REPORT_RECEIVED_IMAGE_INFO_RESPONSE* ImgInfo = NULL;
	Status =  OTAGetControlPointResponseCode(targetImageInfo.Opcode, (UINT8**)&ImgInfo);
	if( RT_STATUS_SUCCESS == Status )
	{
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("Remote Target Response Value is 0x%x"), ImgInfo->RspValue));
		if(ImgInfo->RspValue == ERROR_STATE_SUCCESS)
		{
			m_DfuDataInfo.CurInfo.OriginalVersion = ImgInfo->OriginalFWVersion;
			m_DfuDataInfo.CurInfo.ImageUpdateOffset = ImgInfo->nImageUpdateOffset;
			DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("Remote Image Info: \n\tOriginalFWVersion(0x%x), \n\tnImageUpdateOffset(0x%x)"), 
				ImgInfo->OriginalFWVersion, ImgInfo->nImageUpdateOffset));
			Status = RT_STATUS_SUCCESS;
		}
		else
		{
			Status = RT_STATUS_RESPONSE_ERROR;
		}
	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("OTAGetControlPointResponseCode Fail")));
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--Status(%x)"), Status));
	return Status;
}

extern aes_context ctx;
extern BOOL g_bAesEnable;

RT_STATUS CDFUClient::OTAStartDFU()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if (m_pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit;
	}
	LE_STATUS LeStatus = LE_SUCCESS;

	if(m_DfuDataInfo.CurInfo.ImageUpdateOffset == 0)
	{// offset is zero, need to send 0x01
		DFU_START_DFU startDfu;
		startDfu.Opcode = OPCODE_DFU_START_DFU;
		startDfu.checksum = ImgHeader.checksum;
		startDfu.length = ImgHeader.length;
		startDfu.offset = ImgHeader.offset;
		startDfu.ota_flag = ImgHeader.ota_flag;
		startDfu.signature = ImgHeader.signature;
		startDfu.version =  ImgHeader.version;
		startDfu.reserved_8 = ImgHeader.reserved_8; 

		if(1)
		{
			OTA_AES_Init(3);
			startDfu.reservedForAes = 0;
			UINT8 * pAesData = (UINT8*)&startDfu + 1;
			aes_encrypt(&ctx, pAesData, pAesData);
		}

		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicValue(
			m_DFUHandles.ControlPointValueHandle,(UINT8*)&startDfu,sizeof(startDfu));
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Write Value error%x"),LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}

		// Handle response
		DFU_START_DFU_RESPONSE* ImgInfo = NULL;
		Status = OTAGetControlPointResponseCode(OPCODE_DFU_START_DFU, (UINT8**)&ImgInfo);
		if( RT_STATUS_SUCCESS == Status )
		{
			DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("Remote Target Response Value is 0x%x"), ImgInfo->RspValue));
			if(ImgInfo->RspValue == ERROR_STATE_SUCCESS)
			{
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("Start to send data from beginning")));
				DFUPushImageToTarget();
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("Send data end")));
			}
			else
			{
				Status = RT_STATUS_RESPONSE_ERROR;
			}
		}
		else
		{
			DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("OTAGetControlPointResponseCode Fail")));
		}
	}
	else
	{// offset is not zero, no need to send 0x01, start from offset
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("Start to send data from offset")));
		DFUPushImageToTarget();
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("Send data end")));
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--Status(%x)"), Status));
	return Status;
}


RT_STATUS CDFUClient::OTAValidFW()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if (m_pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit;
	}
	LE_STATUS LeStatus = LE_SUCCESS;

	VALIDATE_FW_IMAGE validImage;
	validImage.Opcode = OPCODE_DFU_VALIDATE_FW_IMAGE;
	validImage.nSignature = ImgHeader.signature;

	LeStatus = m_pGattClient->RtGattClientWriteCharacteristicValue(
		m_DFUHandles.ControlPointValueHandle,(UINT8*)&validImage,sizeof(validImage));
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Write Value error%x"),LeStatus));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	// Handle response
	VALIDATE_FW_IMAGE_RESPONSE* ImgInfo = NULL;
	Status = OTAGetControlPointResponseCode(OPCODE_DFU_VALIDATE_FW_IMAGE, (UINT8**)&ImgInfo);
	if( LE_SUCCESS == Status )
	{
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("Remote Target Response Value is 0x%x"), ImgInfo->RspValue));
		if(ImgInfo->RspValue == ERROR_STATE_SUCCESS)
		{
			Status = RT_STATUS_SUCCESS;
			printf("Image is valid\n");
		}
		else
		{
			Status = RT_STATUS_RESPONSE_ERROR;
			printf("Image is invalid\n");
		}
	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("OTAGetControlPointResponseCode Fail")));
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--Status(%x)"), Status));
	return Status;
}

RT_STATUS CDFUClient::OTAActiveAndReset()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if (m_pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit;
	}
	LE_STATUS LeStatus = LE_SUCCESS;

	ACTIVE_IMAGE_RESET activeReset;
	activeReset.Opcode = OPCODE_DFU_ACTIVE_IMAGE_RESET;

	LeStatus = m_pGattClient->RtGattClientWriteCharacteristicValue(
		m_DFUHandles.ControlPointValueHandle,(UINT8*)&activeReset,sizeof(activeReset));
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Write Value error%x"),LeStatus));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	// No Notification
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--Status(%x)"), Status));
	return Status;
}


int CPU_Frequency(void) //MHz
{
	LARGE_INTEGER CurrTicks, TicksCount;
	__int64 iStartCounter, iStopCounter;

	DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
	DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	QueryPerformanceFrequency(&TicksCount);
	QueryPerformanceCounter(&CurrTicks);

	TicksCount.QuadPart /= 16;
	TicksCount.QuadPart += CurrTicks.QuadPart;

	__asm rdtsc
		__asm mov DWORD PTR iStartCounter, EAX
		__asm mov DWORD PTR (iStartCounter+4), EDX

		while(CurrTicks.QuadPart<TicksCount.QuadPart)
			QueryPerformanceCounter(&CurrTicks);

	__asm rdtsc
		__asm mov DWORD PTR iStopCounter, EAX
		__asm mov DWORD PTR (iStopCounter + 4), EDX

		SetThreadPriority(GetCurrentThread(), dwOldThreadP);
	SetPriorityClass(GetCurrentProcess(), dwOldProcessP);

	return (int)((iStopCounter-iStartCounter)/62500);
}

void us_delay(__int64 Us) //利用循环和 CPU 的频率延时, 参数: 微秒
{
	__int64 iCounter, iStopCounter;

	__asm rdtsc
		__asm mov DWORD PTR iCounter, EAX
		__asm mov DWORD PTR (iCounter+4), EDX

		iStopCounter = iCounter + Us*_CPU_FREQ;

	while(iStopCounter-iCounter>0)
	{
		__asm rdtsc
			__asm mov DWORD PTR iCounter, EAX
			__asm mov DWORD PTR (iCounter+4), EDX
	}
}


/************************************************************************/
/* Push Image to Target                                                 */
/************************************************************************/
RT_STATUS CDFUClient::DFUPushImageToTarget()
{// start put file to target
	int k = 0;
	LE_STATUS LeStatus = LE_SUCCESS;
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if (m_pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit;
	}

	RECEIVE_FW_IMAGE ReceiveImg;
	ReceiveImg.Opcode = OPCODE_DFU_RECEIVE_FW_IMAGE;
	ReceiveImg.nSignature = ImgHeader.signature;
	if(m_DfuDataInfo.CurInfo.ImageUpdateOffset != 0)
	{
		ReceiveImg.nImageUpdateOffset = m_DfuDataInfo.CurInfo.ImageUpdateOffset;
	}
	else
	{
		ReceiveImg.nImageUpdateOffset = 12;
	}
	
	LeStatus = m_pGattClient->RtGattClientWriteCharacteristicValue(
		m_DFUHandles.ControlPointValueHandle,(UINT8*)&ReceiveImg,sizeof(ReceiveImg));
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Write Value error%x"),LeStatus));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	//	Sleep(20);
	// check with remote image info
	UINT8 *buf = DataBuf;	// Image Buffer
	UINT32 DataLen = ImgHeader.length*4;	// Image Length
    //printf("(%d)DataLen = %d\n", __LINE__,DataLen);
	if(m_DfuDataInfo.CurInfo.ImageUpdateOffset != 0)
	{
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("ImageUpdateOffset(%d) is not zero, start to push from the offset"),m_DfuDataInfo.CurInfo.ImageUpdateOffset));
		buf = DataBuf + (m_DfuDataInfo.CurInfo.ImageUpdateOffset - 12);
		DataLen = ImgHeader.length*4 - (m_DfuDataInfo.CurInfo.ImageUpdateOffset -12);
	}
    //printf("(%d)DataLen = %d\n", __LINE__,DataLen);


    // fix issue
	UINT8 *bufend = DataBuf + ImgHeader.length*4;
	//UINT8 *bufend = DataBuf + DataLen;

	HWND Confighwnd = ::FindWindow(NULL, L"RCU");
	int i = 0;
	int bufCount = DataLen/20 + 1;
	UINT32 sendUnit = 0;
	while(TRUE)
	{
		///TODO: send entire buf one by one
		sendUnit = (buf+20 < bufend) ? 20 : (bufend-buf);

		if(1)
		{
			if(sendUnit >= 16)
			{
				aes_encrypt(&ctx, buf, buf);
			}
		}

		LeStatus = m_pGattClient->RtGattClientWriteWithoutResponse(
			m_DFUHandles.DFUDataHandle,buf, sendUnit);
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Write Value error%x"),LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
		us_delay(3000);//ok

		buf += sendUnit;
		if (buf >= bufend)
			break;
		
		::SendMessage(Confighwnd, WM_OTA_DOWNLOAD_PROCESS, (WPARAM)i, (LPARAM)bufCount);
		i++;
	}

	::SendMessage(Confighwnd, WM_OTA_DOWNLOAD_PROCESS, (WPARAM)bufCount, (LPARAM)bufCount);

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_DFU, (_T("--Status(%x)"), Status));
	return Status;
}

/************************************************************************/
/* Parse Notification Response                                          */
/************************************************************************/
RT_STATUS CDFUClient:: OTAGetControlPointResponseCode(UINT8 Opcode, UINT8** RepData)
{
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	// Wait for notification
	UINT8 *attRxBuffer;
	unsigned long attRxBufLen;
	DEF_RESPONSE_HEADER *ResponseHeader;
	UINT16 NotifyHandle = 0;

	LeStatus = m_pGattClient->RtGattWaitForNotification();
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("DFU ClientNotifyRxProc-- with L2cap Disconnected")));
		return RT_STATUS_PROCEDURE_FAIL;
	}
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("DFU Recv Notify Data")));
	m_pGattClient->GATTGetRxData(&attRxBuffer, &attRxBufLen);

	NotifyHandle = (unsigned short)(attRxBuffer[1] | (attRxBuffer[2] << 8));
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("DFU DfuDataHandle (%04x)"),m_DFUHandles.ControlPointValueHandle));
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("NotifyHandle (%04x)"),NotifyHandle));

	Status = RT_STATUS_FAILURE;
	if (m_DFUHandles.ControlPointValueHandle == NotifyHandle)
	{
		ResponseHeader = (DEF_RESPONSE_HEADER *)(attRxBuffer + 3);
		if( OPCODE_DFU_RESPONSE_CODE == ResponseHeader->Opcode )
		{
			if(Opcode == ResponseHeader->ReqOpcode)
			{
				Status = RT_STATUS_SUCCESS;
				*RepData = attRxBuffer + 3;
			}
			else
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Opcode (0x%x), ResponseHeader->ReqOpcode(0x%x)"),Opcode, ResponseHeader->ReqOpcode));
			}
		}
		else
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("Opcode (0x10), ResponseHeader->Opcode(0x%x)"),ResponseHeader->Opcode));
		}
	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_DFU, (_T("ControlPointValueHandle (0x%x), NotifyHandle(0x%x)"),m_DFUHandles.ControlPointValueHandle, NotifyHandle));
	}
	return Status;
}

RT_STATUS CDFUClient::HandShakeWithRCU()
{
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	//UINT8 *attRxBuffer;
	unsigned char attRxBuffer[128] = {0};
	UINT8 *attData;
	unsigned long attRxBufLen;
	UINT16 notifyHandle;
	UINT8 opCode;
	UINT16 dataLen = 0;
	UINT8 RandData_Enc[16] = {0};
	UINT8 RandData_Des[16] = {0};

	Status = DiscoverHandShakeSrvChar();
	if(Status == RT_STATUS_SUCCESS)
	{
		// Produce a random data
		Produce_RandData();
		aes_encrypt(&ctx, RandData, RandData_Enc);
		// Send the random data to RCU
		WriteCmdToDevice(m_HandShakeCharValueHandle, RandData_Enc, 16);

		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(m_HandShakeNotificationHandle, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("RtGattClientWriteCharacteristicDescriptors Fail")));
			return RT_STATUS_PROCEDURE_FAIL;
		}

		LeStatus = m_pGattClient->RtGattWaitForNotification();
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("ClientNotifyRxProc-- with L2cap Disconnected")));
			return RT_STATUS_PROCEDURE_FAIL;
		}

		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Recv Notify Data")));
		//m_pGattClient->GATTGetRxData(&attRxBuffer, &attRxBufLen);
		m_pGattClient->RtGattGetNotificationData(attRxBuffer, &attRxBufLen);

		DEBUG_DATA(TRACE_LEVEL_VERBOSE,attRxBuffer,attRxBufLen);

		opCode = attRxBuffer[0];
		notifyHandle = (unsigned short)(attRxBuffer[1] | (attRxBuffer[2] << 8));
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("NotifyHandle (%04x)"),notifyHandle));

		dataLen = attRxBufLen - 3;
		if (m_HandShakeCharValueHandle == notifyHandle && dataLen == 16)
		{// ignore header
			attData = attRxBuffer+3;
			aes_decrypt(&ctx, attData, RandData_Des);	// RandData ?= RandData_Des+1
			Status = RT_STATUS_SUCCESS;
			for(int i=0; i<16; i++)
			{
				if((RandData[i]+1) != RandData_Des[i])
				{
					Status = RT_STATUS_FAILURE;
					DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("Rand Data Compare Fail")));
					break;
				}
			}
		}
		else
		{
			Status = RT_STATUS_RESPONSE_ERROR;
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("m_HandShakeCharValueHandle (0x%x), NotifyHandle(0x%x), dataLen(%d)"),m_HandShakeCharValueHandle, notifyHandle, dataLen));
		}
	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("m_HandShakeCharValueHandle (0x%x) is NULL")));
		Status = RT_STATUS_RESOURCE;
	}

	return Status;
}

RT_STATUS CDFUClient::WriteCmdToDevice(unsigned short targetHandle, unsigned char* Buf, int BufLen)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if(targetHandle)
	{
		LeStatus = m_pGattClient->RtGattClientWriteWithoutResponse(targetHandle, Buf, BufLen);
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write Handle (0x%04x) error "), targetHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("--Status(%x)"), Status));
	return Status;
}