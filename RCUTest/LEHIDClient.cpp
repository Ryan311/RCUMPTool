#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HandShake.h"

#include "GattPublic.h"

//extern "C"{
#include "gatt_api.h"
#include "assigned_number.h"
#include "GattEntity.h"
#include "GattInstance.h"
#include "aes.h"
#include "reg_le.h"
//}
#include "LEHIDClient.h"

#include <tchar.h>
#include "trace.h"

#ifdef WPP_TRACING_ENABLE
	#include "LEHIDCLient.tmh"
#endif

UINT8 HandShakeData[16];
UINT8 HandShakeConfirm[16];
#pragma comment(lib, "HandShake.lib")

enum 
{
	REPORT_INPUT = 1,
	REPORT_OUTPUT = 2,
	REPORT_FEATURE = 3
}HID_REPORT_TYPE;

CLEHIDClient::CLEHIDClient()
{
	m_pGattInstance = NULL;
	m_pGattClient = NULL;
}

CLEHIDClient::~CLEHIDClient()
{
	UnInit();
}


BOOL CLEHIDClient::Init(unsigned long long ServerAddr)
{
	BOOL bRet = FALSE;

	if(m_pGattInstance == NULL)
	{
		m_pGattInstance = CGattInstance::GetGattInstance();
	}

	if(m_pGattInstance == NULL)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("GetGattInstance Fail")));
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}

	m_ullAddr = ServerAddr;
	m_Mainhwnd = ::FindWindow(NULL, L"RCU");
	return bRet;
}

VOID CLEHIDClient::UnInit()
{
	if (0 != m_ullAddr)
	{
		Disconnect();
		m_ullAddr = 0;
	}
	if(m_pGattInstance)
	{
		m_pGattInstance->Release();
		m_pGattInstance = NULL;
	}

}

RT_STATUS CLEHIDClient::ConnectTo()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++, Remote Address is %i64x"), m_ullAddr));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	ASSERT(0 != m_ullAddr);

	for (int i=0; i<3; i++)
	{// Retry 3 times
		m_pGattClient = m_pGattInstance->GATTConnect(m_ullAddr);
		if (NULL != m_pGattClient)
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Connect to server OK")));
			// Clear all this values
			m_HIDReportIDMapValueListLen = 0;
			m_InputClientCharConfigValueHandleListLen = 0;
			m_HIDServiceListLen = 0;
			m_ReportDescriptorValueLen = 0;
			m_BatteryServiceListLen = 0;
			m_HandShakeCharValueHandle = 0;
			break;
		}
		Sleep(100); // Sleep 100 ms for next connect
	}

	if (NULL == m_pGattClient)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("Connect to server fail")));
		Status = RT_STATUS_FAILURE;
	}

	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("-- Status is %x"), Status));
	return Status;
}

void CLEHIDClient::SetConnectMode(int Mode, MP_CONFIG &MPConfig)
{
	m_Mode = Mode;
	if(m_Mode)
	{
		swscanf_s(MPConfig.AlertHandleStr.GetBuffer(0),L"%x",&m_AlertValueHanle);
		swscanf_s(MPConfig.SWHandleStr.GetBuffer(0),L"%x",&m_SWVersionValueHandle);
		swscanf_s(MPConfig.FWHandleStr.GetBuffer(0),L"%x",&m_FWVersionValueHandle);
		swscanf_s(MPConfig.HWHandleStr.GetBuffer(0),L"%x",&m_HWVersionValueHandle);
	}
	swscanf_s(MPConfig.KeyNHandleStr.GetBuffer(0),L"%x",&m_KeyNotifyHandle);
	swscanf_s(MPConfig.VoiceNHandleStr.GetBuffer(0),L"%x",&m_VoiceNotifyHandle);
}

void CLEHIDClient::Disconnect()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++, Remote Address is %i64x"), m_ullAddr));
	ASSERT(m_ullAddr != 0);

	if (m_pGattInstance == NULL)
	{
		return;
	}
	
	m_bNotify = False;
	CGattEntity * pGattClient = m_pGattInstance->GetGattEntityByAddr(m_ullAddr);
	if(NULL == pGattClient)
	{
		return;
	}

	m_pGattInstance->GATTClose(pGattClient->m_BtAddrRemote);
	m_pGattClient = NULL;

	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("Start to wait m_hThreadNotify")));
	DWORD dw = WaitForSingleObject(m_hThreadNotify, 5000);
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("Wait over, dw(%x)"), dw));
	m_hThreadNotify = NULL;
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("--")));
}


RT_STATUS CLEHIDClient::DiscoverService()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;

	if(!m_Mode)
	{
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));

		GATT_GROUP_HANDLE_VALUE AllServiceList[GATT_GROUP_HANDLE_LIST_LEN_MAX] = {0};
		unsigned long AllServiceListLen = 0;
		unsigned long Len = 0;
		unsigned int ServiceListIndex = 0;

		unsigned short ServerUUIDList[GATT_GROUP_HANDLE_LIST_LEN_MAX] = {0};// write down to KernelMode
		unsigned char ServerUUIDListLen = 0;
		m_HIDServiceListLen = 0;		// should set to 0 each time when discover service
		m_BatteryServiceListLen = 0;

		if(m_pGattClient == NULL)
		{
			Status = RT_STATUS_NOT_CONNECT;
			goto exit; 
		}

		unsigned char UuidOTAService[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0xff, 0xd0, 0x00, 0x00};
		unsigned char UuidHandShake[16] = {0x12, 0xa2, 0x4d, 0x2e, 0xfe, 0x14, 0x48, 0x8e, 0x93, 0xd2, 0x17, 0x3c, 0xff, 0xd1, 0x00, 0x00};
		LE_16_BYTE VenderSercie;			//0x12a24d2efe14488e93d2173cFFd00000
		memcpy(VenderSercie.Value, UuidOTAService, 16);
		LE_16_BYTE HandShakeService;		//0x12a24d2efe14488e93d2173cFFd10000
		memcpy(HandShakeService.Value, UuidHandShake, 16);

		LE_UUID GAPServiceUuid, HIDServiceUuid, BaterryServiceUuid, DevinfoServiceUuid, ScanParaServiceUuid;
		LE_UUID OTAServiceUuid, HandShakeServiceUuid;
		LE_UUID OTAServiceShortUuid, HandShakeServiceShortUuid;
		LE_UUID AlertUuid;

		LeUuidSetBit16(&GAPServiceUuid, AN_GENERIC_ACCESS);				//discover GAP Service
		LeUuidSetBit16(&HIDServiceUuid, AN_HID_DEVICE);			//discover LE HID Service
		LeUuidSetBit16(&BaterryServiceUuid, AN_BATTERY_SERVICE);		//discover Battery Service
		LeUuidSetBit16(&DevinfoServiceUuid, AN_DEVICE_INFORMATION);		//discover Device Info Service
		LeUuidSetBit16(&ScanParaServiceUuid, AN_SCAN_PARAMETER);	//discover Scan Parameter Service
		LeUuidSetBit16(&OTAServiceShortUuid, AN_OTA_SERVICE_16);	//discover Scan Parameter Service
		LeUuidSetBit16(&HandShakeServiceShortUuid, AN_HANDSHAKE_SERVICE_16);		//discover Scan Parameter Service
		LeUuidSetBit16(&AlertUuid, AN_IMMEDIATE_ALERT);				//discover Alert
		LeUuidSetBit128(&OTAServiceUuid, VenderSercie);				//discover Vendor Service
		LeUuidSetBit128(&HandShakeServiceUuid, HandShakeService);	//HANDSHAKE_SERVICE);	//discover Vendor Service

		LE_STATUS LeStatus = m_pGattClient->RtGattClientDiscoverAllPrimaryServices(
			LE_TABLE_START_HANDLE,
			LE_TABLE_END_HANDLE,
			AllServiceList,
			&AllServiceListLen);

		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("RtGattClientDiscoverPrimaryServiceByServiceUuid with error(%d)"),LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit; 
		}

		for(ServiceListIndex=0;ServiceListIndex<AllServiceListLen;ServiceListIndex++)
		{
			if (LeUuidIsEqualTo(GAPServiceUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_GapService = AllServiceList[ServiceListIndex];
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has GAP Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
			}
			else if (LeUuidIsEqualTo(HIDServiceUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_HIDServiceList[m_HIDServiceListLen].m_HIDService = AllServiceList[ServiceListIndex];
				m_HIDServiceListLen++;
				m_HIDSupported = TRUE;
				if (m_HIDServiceListLen == 1)
				{
					ServerUUIDListLen ++;
					ServerUUIDList[ServerUUIDListLen - 1] = AN_HID_DEVICE;
				}

				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("LE HID Service(0x%04x) ListLen(%d)"),AllServiceList[ServiceListIndex].Value.Value.Bit16,m_HIDServiceListLen));
			}
			else if (LeUuidIsEqualTo(BaterryServiceUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_BatteryServiceList[m_BatteryServiceListLen].m_BatteryService = AllServiceList[ServiceListIndex];
				m_BatteryServiceListLen ++;
				if (m_BatteryServiceListLen == 1)
				{
					ServerUUIDListLen ++;
					ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
				}
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has Battery Service(0x%04x),ListLen(%d)"),AllServiceList[ServiceListIndex].Value.Value.Bit16,m_BatteryServiceListLen));
			}

			else if (LeUuidIsEqualTo(ScanParaServiceUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_ScanParaService =  AllServiceList[ServiceListIndex];
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has ScanParameter Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
			}
			else if (LeUuidIsEqualTo(DevinfoServiceUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_DeviceInfoService = AllServiceList[ServiceListIndex];
				//m_pDeviceInfoObj = new CDeviceInformationService(m_pGattInstance,m_ullAddr,m_DeviceInfoService.HandleStart,m_DeviceInfoService.HandleEnd);
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has DeviceInformation Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("m_DeviceInfoService HandleStart(0x%04x) HandleEnd(0x%04x)"),m_DeviceInfoService.HandleStart,m_DeviceInfoService.HandleEnd));
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
			}
			else if (LeUuidIsEqualTo(AlertUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_AlertService = AllServiceList[ServiceListIndex];
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has Alert Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("m_AlertService HandleStart(0x%04x) HandleEnd(0x%04x)"),m_AlertService.HandleStart,m_AlertService.HandleEnd));
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
			}
			else if (LeUuidIsEqualTo(OTAServiceShortUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_OTAService = AllServiceList[ServiceListIndex];
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
				m_OTASupported = TRUE;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has OTA Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
			}
			else if (LeUuidIsEqualTo(HandShakeServiceShortUuid,AllServiceList[ServiceListIndex].Value))
			{
				m_HandShakeService = AllServiceList[ServiceListIndex];
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
				m_HandShakeSupported = TRUE;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has HandShake Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
			}
			else if (LeUuidIsEqualToBit128(OTAServiceUuid,AllServiceList[ServiceListIndex].Value.Value.Bit128))
			{
				m_OTAService = AllServiceList[ServiceListIndex];
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
				m_OTASupported = TRUE;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has OTA Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
			}
			else if (LeUuidIsEqualToBit128(HandShakeServiceUuid,AllServiceList[ServiceListIndex].Value.Value.Bit128))
			{
				m_HandShakeService = AllServiceList[ServiceListIndex];
				ServerUUIDListLen ++;
				ServerUUIDList[ServerUUIDListLen - 1] = AllServiceList[ServiceListIndex].Value.Value.Bit16;
				m_HandShakeSupported = TRUE;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PrimaryService has HandShake Service(0x%04x)"),AllServiceList[ServiceListIndex].Value.Value.Bit16));
			}
		}
	}
	else
	{
		// Do Nothing when in MP Mode
	}
	
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("-- Status(%x)"),Status));
	return Status;
}

BOOLEAN CLEHIDClient::bHIDSupported()
{
	return m_HIDSupported;
}

BOOLEAN CLEHIDClient::bOTASupported()
{
	return m_OTASupported;
}

BOOLEAN CLEHIDClient::bHandShakeSupported()
{
	return m_HandShakeSupported;
}


RT_STATUS CLEHIDClient::DiscoverHIDChar(LE_HID_SERVICE_PARAS& HIDServiceParas)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	LE_STATUS LeStatus = LE_SUCCESS;

	if(m_pGattClient == NULL)
	{
		Status = RT_STATUS_NOT_CONNECT;
		goto exit; 
	}

	//get all characteristic declarations in HID Service
	LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
		HIDServiceParas.m_HIDService.HandleStart,
		HIDServiceParas.m_HIDService.HandleEnd,
		HIDServiceParas.m_CharDeclarOfHIDList,
		&HIDServiceParas.m_CharDeclarOfHIDListLen);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read ALL Characteristis of HID Service error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	LE_UUID HidInfoCharUuid, ReportMapUuid, ControlPointUuid, HidReportUuid;
	LE_UUID ProModeUuid, BootKeyboardInReportUuid, BootKeyboardOutReportUuid, BootMouseInpurtReportUuid;
	LeUuidSetBit16(&HidInfoCharUuid, AN_HID_INFO);
	LeUuidSetBit16(&ReportMapUuid, AN_HID_REPORT_DISCRIPTOR);
	LeUuidSetBit16(&ControlPointUuid, AN_HID_CONTROL_POINT);
	LeUuidSetBit16(&HidReportUuid, AN_HID_REPORT);
	LeUuidSetBit16(&ProModeUuid, AN_PROTOCAL_MODE);
	LeUuidSetBit16(&BootKeyboardInReportUuid, AN_HID_BOOT_KEYBOARD_INPUT_REPORT);
	LeUuidSetBit16(&BootKeyboardOutReportUuid, AN_HID_BOOT_KEYBOARD_OUTPUT_REPORT);
	LeUuidSetBit16(&BootMouseInpurtReportUuid, AN_HID_BOOT_MOUSE_INPUT_REPORT);
	
	unsigned long CharDeclarIndex = 0;
	for (CharDeclarIndex=0;CharDeclarIndex<HIDServiceParas.m_CharDeclarOfHIDListLen;CharDeclarIndex++)
	{
		if (LeUuidIsEqualTo(ReportMapUuid,HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.Uuid))
		{
			HIDServiceParas.HIDReportDiscriptorCharDec = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex];
			HIDServiceParas.HIDReportDiscriptorCharDecIndex = (unsigned char)CharDeclarIndex;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get HID Report Map Char(0x%04x), Handle(0x%04x),ValueHandle(0x%04x)"),
				AN_HID_REPORT_DISCRIPTOR,HIDServiceParas.HIDReportDiscriptorCharDec.Handle,
				HIDServiceParas.HIDReportDiscriptorCharDec.CharDec.ValueHandle));
			unsigned short ReportMapStart = 0,ReportMapEnd = 0;
			ReportMapStart = HIDServiceParas.HIDReportDiscriptorCharDec.Handle + 1;  
			if (HIDServiceParas.HIDReportDiscriptorCharDecIndex < HIDServiceParas.m_CharDeclarOfHIDListLen - 1)
			{
				ReportMapEnd = HIDServiceParas.m_CharDeclarOfHIDList[HIDServiceParas.HIDReportDiscriptorCharDecIndex+1].Handle - 1;
			}
			else
			{
				ReportMapEnd = HIDServiceParas.m_HIDService.HandleEnd;
			}
			DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("ReportMapStart(0x%04x) ReportMapEnd(0x%04x) to find Descriptors in Report Map char\n"),
				ReportMapStart,ReportMapEnd));
			if (ReportMapEnd > ReportMapStart)
			{
				Status = GetDescriptorsInfoOfACharacteristic(ReportMapStart,ReportMapEnd,CharDeclarIndex,HIDServiceParas);
				if (Status!=RT_STATUS_SUCCESS)
				{
					DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("GetReportReferenceValue Return Value error")));
					goto exit;
				}
			}
		}
		else if (LeUuidIsEqualTo(HidReportUuid,HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.Uuid))
		{
			HIDServiceParas.HIDReportCharDecList[HIDServiceParas.HIDReportCharDecListLen] = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex];
			HIDServiceParas.HIDReportCharDecListLen ++;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HIDReportCharDecListLen %d"),HIDServiceParas.HIDReportCharDecListLen));
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get HID Report Char(0x%04x), Handle(0x%04x),ValueHandle(0x%04x)"),
				AN_HID_REPORT,HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].Handle,
				HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.ValueHandle));
			//Get HID Report ID mapping Descriptor In HID Report 
			unsigned short StartHand = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].Handle +1;
			unsigned short EndHand = 0;
			if (CharDeclarIndex < HIDServiceParas.m_CharDeclarOfHIDListLen - 1)
			{
				EndHand = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex+1].Handle - 1;
			}
			else
			{
				EndHand = HIDServiceParas.m_HIDService.HandleEnd;
			}
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("StartHand(0x%04x), EndHandle(0x%04x) to Find Descriptors in HID Report Char"),StartHand,EndHand));

			if (StartHand < EndHand)
			{
				Status = GetDescriptorsInfoOfACharacteristic(StartHand,EndHand,CharDeclarIndex,HIDServiceParas);
				if (Status!=RT_STATUS_SUCCESS)
				{
					DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("GetReportReferenceValue Return Value error")));
					goto exit;
				}		

			}
		}
		else if (LeUuidIsEqualTo(BootKeyboardInReportUuid,HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.Uuid))
		{
			HIDServiceParas.HIDBootKeyboardInputReportCharDec = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex];
			HIDServiceParas.HIDBootKeyboardInputReportCharDecIndex = (unsigned char)CharDeclarIndex;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get HID BOOT Keyboard Input Report Char(0x%04x),Handle(0x%04x),ValueHandle(0x%04x)"),
				AN_HID_BOOT_KEYBOARD_INPUT_REPORT,HIDServiceParas.HIDBootKeyboardInputReportCharDec.Handle,
				HIDServiceParas.HIDBootKeyboardInputReportCharDec.CharDec.ValueHandle));
		}
		else if (LeUuidIsEqualTo(BootMouseInpurtReportUuid,HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.Uuid))
		{
			HIDServiceParas.HIDBootMouseInputReportCharDec = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex];
			HIDServiceParas.HIDBootMouseInputReportCharDecIndex = (unsigned char)CharDeclarIndex;;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get HID BOOT Mouse Input Report Char(0x%04x), Handle(0x%04x), ValueHandle(0x%04x)"),
				AN_HID_BOOT_MOUSE_INPUT_REPORT,HIDServiceParas.HIDBootMouseInputReportCharDec.Handle,
				HIDServiceParas.HIDBootMouseInputReportCharDec.CharDec.ValueHandle));
		}
		else if (LeUuidIsEqualTo(ControlPointUuid,HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.Uuid))
		{
			HIDServiceParas.HIDControlPointCharDec = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex];
			HIDServiceParas.HIDControlPointCharDecIndex = (unsigned char)CharDeclarIndex;
			m_HIDControlPointCharValueHandle = HIDServiceParas.HIDControlPointCharDec.CharDec.ValueHandle;

			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get HID Control point Char(0x%04x),Handle(0x%04x),ValueHandle(0x%04x)"),
				AN_HID_CONTROL_POINT,HIDServiceParas.HIDControlPointCharDec.Handle,
				HIDServiceParas.HIDControlPointCharDec.CharDec.ValueHandle));
			// get Value of HID control Point
			unsigned char HIDContolPointValue[10] = {0};
			unsigned long HIDContolPointValueLen = 0;
			LeStatus =  m_pGattClient->RtGattClientReadCharacteristicValue(
				HIDServiceParas.HIDControlPointCharDec.CharDec.ValueHandle,
				HIDContolPointValue,
				&HIDContolPointValueLen);
			if (LeStatus != LE_SUCCESS)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read Value of HID Control point Char Fail, Status(%d)"), LeStatus));
			}
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Read Value(0x%02x) of HID Control point Char"),HIDContolPointValue[0]));

		}
		else if (LeUuidIsEqualTo(HidInfoCharUuid,HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.Uuid))
		{
			HIDServiceParas.HIDInfoCharDec = HIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex];
			HIDServiceParas.HIDInfoCharDecIndex = (unsigned char)CharDeclarIndex;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get HID Information Char(0x%04x) ,Handle(0x%04x),ValueHandle(0x%04x)"),
				AN_HID_INFO,HIDServiceParas.HIDInfoCharDec.Handle,
				HIDServiceParas.HIDInfoCharDec.CharDec.ValueHandle));
		}
	}
	
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("-- Status(%x)"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::WriteDataToChannel(unsigned char *Buf, int BufLen)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	LE_STATUS LeStatus = LE_SUCCESS;
	/*if(m_TxHandle)
	{
		LeStatus = m_pGattClient->RtGattClientWriteWithoutResponse(m_TxHandle, Buf, BufLen);
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write Handle (0x%04x) error "), m_TxHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}*/

//exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("--")));
	return Status;
}

RT_STATUS CLEHIDClient::WriteCmdToDevice(unsigned short targetHandle, unsigned char* Buf, int BufLen)
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


RT_STATUS CLEHIDClient::WriteRequestToDevice(unsigned short targetHandle, unsigned char* Buf, int BufLen)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	if(targetHandle)
	{
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicValue(targetHandle, Buf, BufLen);
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



RT_STATUS CLEHIDClient::EnableHidNotification()
{
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	//if(m_NotificationHandle)
	{
		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(0x0016, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			//DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write DFU Measure client Configuration Value Handle (0x%04x) error "),m_NotificationHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

	{
		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(0x001f, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			//DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write DFU Measure client Configuration Value Handle (0x%04x) error "),m_NotificationHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

	{
		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(0x0026, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			//DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write DFU Measure client Configuration Value Handle (0x%04x) error "),m_NotificationHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

	{
		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(0x002a, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			//DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write DFU Measure client Configuration Value Handle (0x%04x) error "),m_NotificationHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

	{
		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(0x001a, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			//DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write DFU Measure client Configuration Value Handle (0x%04x) error "),m_NotificationHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

	{
		USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(0x0013, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
		if (LE_SUCCESS != LeStatus)
		{
			//DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("write DFU Measure client Configuration Value Handle (0x%04x) error "),m_NotificationHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("--Status(%x)"), Status));
	return Status;
}

RT_STATUS  CLEHIDClient::GetDescriptorsInfoOfACharacteristic(unsigned short StartHandle,unsigned short EndHandle,unsigned long CharDeclarIndex,LE_HID_SERVICE_PARAS ThisHIDServiceParas)
{
	//find all descriptors of HID Report descriptors declaration
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	GATT_HANDLE_TYPE AllDecriptorsOfHIDReport[GATT_HANDLE_TYPE_LIST_LEN_MAX] = {0};
	unsigned long HIDReportAllDecritorsLen = 0;
	LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicDescriptors(
		StartHandle,
		EndHandle,
		(AllDecriptorsOfHIDReport),
		&HIDReportAllDecritorsLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read All discriptors of Char Handle(%04x)error"),ThisHIDServiceParas.HIDReportDiscriptorCharDec.Handle));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	//Get information of HID Report ID mapping
	LE_UUID uuidReportRef;
	LE_UUID uuidReportExtRef;
	LE_UUID uuidCltConfig;

	LeUuidSetBit16(&uuidReportRef,AN_HID_REPORT_REFERENCE);
	LeUuidSetBit16(&uuidCltConfig,AN_CLIENT_CHARACTERISTIC_CONFIGURATION);
	LeUuidSetBit16(&uuidReportExtRef,AN_HID_EXTERNAL_REPORT_REFERENCE);

	for (unsigned int i = 0;i<HIDReportAllDecritorsLen;i++)
	{
		if (LeUuidIsEqualTo(uuidReportRef,AllDecriptorsOfHIDReport[i].Type))
		{
			unsigned short thisHandle = AllDecriptorsOfHIDReport[i].Handle;
			HID_REPORT_ID_MAP_VALUE Value = {0};
			unsigned long valueLen = 0;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Read value of Report Reference Descriptor Handle(0x%04x)"),thisHandle));
			LeStatus = m_pGattClient->RtGattClientReadCharacteristicDescriptors(thisHandle,(unsigned char*)&Value,&valueLen);
			if (LE_SUCCESS != LeStatus)
			{
				//pGattInstance->GATTClose(m_pGattClient->m_BtAddrRemote);
				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("error,Read Report Reference Descriptor value ,Handle(0x%04x)"),thisHandle));
				Status = RT_STATUS_PROCEDURE_FAIL;
				goto exit;
			}
			if ((REPORT_INPUT == Value.ReportType)||(REPORT_FEATURE == Value.ReportType)||(REPORT_OUTPUT == Value.ReportType))
			{
				m_HIDReportIDMapValueList[m_HIDReportIDMapValueListLen] = Value;
				m_HIDReportIDMapValueList[m_HIDReportIDMapValueListLen].Handle = ThisHIDServiceParas.m_CharDeclarOfHIDList[CharDeclarIndex].CharDec.ValueHandle;

				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get HID ReportId MapValue Value. ReportID(0x%02x) ReportType(0x%02x) Handle(0x%04x)"),
					m_HIDReportIDMapValueList[m_HIDReportIDMapValueListLen].ReportId,
					m_HIDReportIDMapValueList[m_HIDReportIDMapValueListLen].ReportType,
					m_HIDReportIDMapValueList[m_HIDReportIDMapValueListLen].Handle));

				m_HIDReportIDMapValueListLen++;
				if (m_HIDReportIDMapValueListLen>=REPORT_ID_MAP_LIST_LEN_MAX)
				{
					break;
				}
			}
		}
		else if (LeUuidIsEqualTo(uuidCltConfig,AllDecriptorsOfHIDReport[i].Type))
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Get ClientConfigDescriptorHandle(0x%04x)"),AllDecriptorsOfHIDReport[i].Handle));
			m_InputClientCharConfigValueHandleList[m_InputClientCharConfigValueHandleListLen] = AllDecriptorsOfHIDReport[i].Handle;
			m_InputClientCharConfigValueHandleListLen++;
		}
		else if (LeUuidIsEqualTo(uuidReportExtRef,AllDecriptorsOfHIDReport[i].Type))
		{
			unsigned short ExtRefHandle = AllDecriptorsOfHIDReport[i].Handle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("External Report Reference Descriptors(0x%04x),Handle(0x%04x)\n"),
				AN_HID_EXTERNAL_REPORT_REFERENCE,ExtRefHandle));
			unsigned char UUIDValue[16] = {0};
			unsigned long UUIDValueLen = 0;
			LeStatus = m_pGattClient->RtGattClientReadCharacteristicDescriptors(ExtRefHandle,UUIDValue,&UUIDValueLen);
			if (LeStatus != LE_SUCCESS)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read UUID Value of External Report Reference Descriptors\n")));
				Status = RT_STATUS_PROCEDURE_FAIL;
				goto exit;
			}
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Read UUID Value (0x%02x%02x) of External Report Reference Descriptors\n"),UUIDValue[1],UUIDValue[0]));

		}

	}	
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("--")));
	return Status;
}

RT_STATUS CLEHIDClient::DiscoverBatteryServiceInfo(LE_BATTERY_SERVICE_PARAS& BatteryServiceInstance)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	GATT_HANDLE_CHAR_DECLARATION BatteryCharDeclList[10] = {0};
	unsigned long BatteryCharDeclListLen = 0;
	unsigned char BatteryCharDeclListIndex = 0;
	GATT_HANDLE_CHAR_DECLARATION BatteryLevelCharDecl = {0};
	unsigned short BatteryLevelStart = 0;
	unsigned short BatteryLevelEnd = 0;

	LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
		BatteryServiceInstance.m_BatteryService.HandleStart,
		BatteryServiceInstance.m_BatteryService.HandleEnd,
		BatteryCharDeclList,
		&BatteryCharDeclListLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID Read All Characteristis of Battery Service error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID:m_BatteryService HandleStart  0x%04x HandleEnd 0x%04x"),BatteryServiceInstance.m_BatteryService.HandleStart,BatteryServiceInstance.m_BatteryService.HandleEnd));
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID:BatteryCharDeclListLen%04x"),BatteryCharDeclListLen));
	LE_UUID uuidBatterylevel;
	LeUuidSetBit16(&uuidBatterylevel, AN_BATTERY_LEVEL);

	for (;BatteryCharDeclListIndex<BatteryCharDeclListLen;BatteryCharDeclListIndex++)
	{
		if (LeUuidIsEqualTo(uuidBatterylevel,BatteryCharDeclList[BatteryCharDeclListIndex].CharDec.Uuid))
		{
			BatteryLevelCharDecl = BatteryCharDeclList[BatteryCharDeclListIndex];
			BatteryLevelStart = BatteryLevelCharDecl.Handle + 1;
			if (BatteryCharDeclListIndex == BatteryCharDeclListLen -1)
			{
				BatteryLevelEnd = BatteryServiceInstance.m_BatteryService.HandleEnd;
			}
			else
			{
				BatteryLevelEnd = BatteryCharDeclList[BatteryCharDeclListIndex+1].Handle - 1;
			}
			BatteryServiceInstance.m_BatteryLevelValueHandle = BatteryLevelCharDecl.CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID:Battery Level Characteristic Declaration Handle 0x%04x"),BatteryLevelCharDecl.Handle));
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID:BatteryLevelStart 0x%04x,EndHandle 0x%04x"),BatteryLevelStart,BatteryLevelEnd));
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID:m_BatteryLevelValueHandle 0x%04x"),BatteryServiceInstance.m_BatteryLevelValueHandle));
			break;
		}

	}
	if (BatteryLevelEnd < BatteryLevelStart)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("BatteryLevelEnd(0x%04x) BatteryLevelStart(0x%04x)"),BatteryLevelEnd,BatteryLevelStart));
		Status = RT_STATUS_NO_SUCH_FILE_DIRECTORY;
		goto exit;
	}

	// Read Value
	GetBatteryLevelValue(BatteryServiceInstance);

	//
	//If GATT_CHAR_PROPERTY_NOTIFY, Get Report ID map, and Client Characteristic configuration Handle
	//
	if ((BatteryLevelCharDecl.CharDec.Property&GATT_CHAR_PROPERTY_NOTIFY) == GATT_CHAR_PROPERTY_NOTIFY)
	{
		GATT_HANDLE_TYPE BatteryReportReference[10] = {0};
		unsigned long BatteryReportReferenceLen = 0;
		LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicDescriptors
			(BatteryLevelStart,
			BatteryLevelEnd,
			BatteryReportReference,
			&BatteryReportReferenceLen
			);
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read Battery Level Characteristis Descriptor of HID Service error")));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}

		DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("BatteryReportReferenceLen 0x%x"),BatteryReportReferenceLen));

		LE_UUID uuidReportIdMap;
		LE_UUID uuidClientConfig;

		LeUuidSetBit16(&uuidReportIdMap,AN_HID_REPORT_ID_MAP);
		LeUuidSetBit16(&uuidClientConfig,AN_CLIENT_CHARACTERISTIC_CONFIGURATION);

		unsigned char BatteryReportReferenceIndex = 0;
		for (;BatteryReportReferenceIndex<BatteryReportReferenceLen;BatteryReportReferenceIndex++)
		{

			if (LeUuidIsEqualTo(uuidReportIdMap,BatteryReportReference[BatteryReportReferenceIndex].Type))
			{
				HID_REPORT_ID_MAP_VALUE ReportIdMapValue = {0};
				unsigned long ReportIdMapValueLen = 0;
				LeStatus = m_pGattClient->RtGattClientReadCharacteristicDescriptors(
					BatteryReportReference[BatteryReportReferenceIndex].Handle,
					(unsigned char*)&ReportIdMapValue,
					&ReportIdMapValueLen
					);
				if (LE_SUCCESS != LeStatus)
				{
					DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read Battery Level Characteristics Descriptor Value of HID Service error")));
					Status = RT_STATUS_PROCEDURE_FAIL;
					goto exit;
				}
				ReportIdMapValue.Handle = BatteryLevelCharDecl.CharDec.ValueHandle;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID Battery Level Map,ReportID(0x%02x),ReportType(0x%02x),Handle(0x%04x)"),
					ReportIdMapValue.ReportId,ReportIdMapValue.ReportType,ReportIdMapValue.Handle));
				memcpy(&BatteryServiceInstance.m_BatteryLevelValueReportIdMap,&ReportIdMapValue,sizeof(HID_REPORT_ID_MAP_VALUE));

			}
			else if (LeUuidIsEqualTo(uuidClientConfig,BatteryReportReference[BatteryReportReferenceIndex].Type))
			{
				BatteryServiceInstance.m_BatteryLevelClientCharConfigHandle = BatteryReportReference[BatteryReportReferenceIndex].Handle;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("m_BatteryLevelClientCharConfigHandle %04x"),BatteryServiceInstance.m_BatteryLevelClientCharConfigHandle));
			}
		}     
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status(%x)"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::GetBatteryLevelValue(LE_BATTERY_SERVICE_PARAS BatteryServiceInstance)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	LE_STATUS LeStatus =  LE_SUCCESS;
	if (BatteryServiceInstance.m_BatteryLevelValueHandle)
	{
		unsigned long BatteryLevelValueLen = 0;
		LeStatus = m_pGattClient->RtGattClientReadCharacteristicValue(
			BatteryServiceInstance.m_BatteryLevelValueHandle,
			&BatteryServiceInstance.m_BatteryLevelValue,
			&BatteryLevelValueLen);
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read Battery Level Value,m_BatteryLevelValueHandle(%04x)"),BatteryServiceInstance.m_BatteryLevelValueHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("m_BatteryLevelValue %d"),BatteryServiceInstance.m_BatteryLevelValue));
	}
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status(%x)"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::DiscoverScanParaServiceInfo()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	GATT_HANDLE_CHAR_DECLARATION ScanParaCharDeclList[10] = {0};
	unsigned long ScanParaCharDeclListLen = 0;
	unsigned char ScanParaCharDeclListIndex = 0;
	unsigned short ScanRefreshStart = 0,ScanRefreshEnd = 0;

	LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
		m_ScanParaService.HandleStart,
		m_ScanParaService.HandleEnd,
		ScanParaCharDeclList,
		&ScanParaCharDeclListLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID Read All Characteristis of Scan Parameters Service error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	LE_UUID uuidScanIntvlWdw;
	LE_UUID uuidScanRefresh;

	LeUuidSetBit16(&uuidScanIntvlWdw,AN_SCAN_INTERVAL_WINDOW);
	LeUuidSetBit16(&uuidScanRefresh,AN_SCAN_REFRESH);

	for (;ScanParaCharDeclListIndex<ScanParaCharDeclListLen;ScanParaCharDeclListIndex++)
	{
		if (LeUuidIsEqualTo(uuidScanIntvlWdw,ScanParaCharDeclList[ScanParaCharDeclListIndex].CharDec.Uuid))
		{
			m_ScanIntervalWindowValueHandle = ScanParaCharDeclList[ScanParaCharDeclListIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID m_ScanIntervalWindow Char(0x%04x),Handle(0x%04x),ValueHandle 0x%04x"),
				AN_SCAN_INTERVAL_WINDOW,ScanParaCharDeclList[ScanParaCharDeclListIndex].Handle,m_ScanIntervalWindowValueHandle));
			RegServiceSetNotificationOn(m_ullAddr,AN_SCAN_INTERVAL_WINDOW,(RT_U8*)&m_ScanIntervalWindowValueHandle,sizeof(m_ScanIntervalWindowValueHandle));
		}
		else if (LeUuidIsEqualTo(uuidScanRefresh,ScanParaCharDeclList[ScanParaCharDeclListIndex].CharDec.Uuid))
		{
			m_ScanRefreshCharValueHandle = ScanParaCharDeclList[ScanParaCharDeclListIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID m_ScanRefresh Char(0x%04x),Handle(0x%04x),ValueHandle 0x%04x"),
				AN_SCAN_REFRESH, ScanParaCharDeclList[ScanParaCharDeclListIndex].Handle,m_ScanRefreshCharValueHandle));

			RegServiceSetNotificationOn(m_ullAddr,AN_SCAN_REFRESH,(RT_U8*)&m_ScanRefreshCharValueHandle,sizeof(m_ScanRefreshCharValueHandle));
			ScanRefreshStart = ScanParaCharDeclList[ScanParaCharDeclListIndex].Handle + 1;
			if (ScanParaCharDeclListIndex == ScanParaCharDeclListLen -1)
			{
				ScanRefreshEnd = m_ScanParaService.HandleEnd;
			}
			else
			{
				ScanRefreshEnd = ScanParaCharDeclList[ScanParaCharDeclListIndex + 1].Handle -1;
			}

		}
	}
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("StartHandle(0x%04x) EndHandle(0x%04x) to Find Descriptors in ScanRefresh"),ScanRefreshStart,ScanRefreshEnd));

	//Get Client characteristic configuration Handle of
	if (ScanRefreshStart<ScanRefreshEnd)
	{
		GATT_HANDLE_TYPE ServiceRefresHandleList[10] = {0};
		unsigned long ServiceRefresHandleListLen = 0;

		LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicDescriptors
			(ScanRefreshStart,
			ScanRefreshEnd,
			ServiceRefresHandleList,
			&ServiceRefresHandleListLen
			);
		if (LeStatus != LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID Read All Characteristis Discriptors of Scan Refresh error")));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}

		DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("RtGattClientDiscoverAllCharacteristicDescriptors OK, ServiceRefresHandleListLen(%d)"), ServiceRefresHandleListLen));
		LE_UUID uuidClientConfig;
		LeUuidSetBit16(&uuidClientConfig,AN_CLIENT_CHARACTERISTIC_CONFIGURATION);
		unsigned char ServiceRefresHandleListIndex = 0;
		for (;ServiceRefresHandleListIndex<ServiceRefresHandleListLen;ServiceRefresHandleListIndex++)
		{
			if (LeUuidIsEqualTo(uuidClientConfig,ServiceRefresHandleList[ServiceRefresHandleListIndex].Type))
			{
				m_ScanRefreshClientConfigHandle = ServiceRefresHandleList[ServiceRefresHandleListIndex].Handle;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HID m_ScanRefreshClientConfigHandle 0x%04x"),m_ScanRefreshClientConfigHandle));
			}
		}

	}
	HostWriteScanParaToHIDServer(96,48);
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status %x"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::HostWriteScanParaToHIDServer(unsigned short LeScanInterval,unsigned short LeScanWindow)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" ++ ")));

	RT_STATUS Status = RT_STATUS_SUCCESS;
	typedef struct _SCAN_PARA
	{
		unsigned short LeScanInterval;
		unsigned short LeScanWindow;
	}SCAN_PARA;

	SCAN_PARA ScanPara = {0};
	ScanPara.LeScanInterval = LeScanInterval;
	ScanPara.LeScanWindow = LeScanWindow;
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("write ScanInterval(0x%04x) ScanWindow(0x%04x)to m_ScanIntervalWindowValueHandle(0x%04x)"),
		LeScanInterval,LeScanWindow,m_ScanIntervalWindowValueHandle));
	if (m_ScanIntervalWindowValueHandle)
	{
		LE_STATUS LeStatus = m_pGattClient->RtGattClientWriteWithoutResponse(
			m_ScanIntervalWindowValueHandle,
			(unsigned char*)&ScanPara,
			sizeof(ScanPara));
		if (LeStatus!=LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID WriteWithoutRepose m_ScanIntervalWindowValueHandle 0x%04x"),m_ScanIntervalWindowValueHandle));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}

	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID No m_ScanIntervalWindowValueHandle 0x%04x"),m_ScanIntervalWindowValueHandle));

	}
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status(%x)"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::HIDHostReadReportData(unsigned char ReportType)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" ++ ")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	LE_STATUS LeStatus = LE_SUCCESS;
	unsigned char ReadReportIndex = 0;
	for (;ReadReportIndex < m_HIDReportIDMapValueListLen;ReadReportIndex++)
	{
		unsigned char ReportValue[32];
		unsigned long ReportValueLen  = 0;
		if (ReportType == m_HIDReportIDMapValueList[ReadReportIndex].ReportType)
		{
			unsigned short ReportHandle = m_HIDReportIDMapValueList[ReadReportIndex].Handle;
			LeStatus = m_pGattClient->RtGattClientReadCharacteristicValue(ReportHandle,ReportValue,&ReportValueLen);
			if (LeStatus != LE_SUCCESS)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID Read ReportData Error,Handle is(%04x)"),ReportHandle));
				Status = RT_STATUS_PROCEDURE_FAIL;
				goto exit;
			}
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Handle(%04x,ReportType(0x%02x),ReportValueLen(%d)"),ReportHandle,ReportType,ReportValueLen));
		}
	}
exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status(%x)"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::HIDHostWriteReportCharacteristic(LE_HID_SERVICE_PARAS HIDServiceParas)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" ++ ")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	LE_STATUS LeStatus = LE_SUCCESS;
	unsigned char ReportCharIndex = 0;
	LE_UUID uuidHidReport;
	LeUuidSetBit16(&uuidHidReport,AN_HID_REPORT);

	for (;ReportCharIndex<HIDServiceParas.HIDReportCharDecListLen;ReportCharIndex++)
	{
		unsigned short ReportCharValueHandle = 0;
		unsigned char ReportCharValueData[32] = {0};
		unsigned long ReportCharValueDataLen = 0;

		if (LeUuidIsEqualTo(uuidHidReport,HIDServiceParas.HIDReportCharDecList[ReportCharIndex].CharDec.Uuid))
		{
			ReportCharValueHandle = HIDServiceParas.HIDReportCharDecList[ReportCharIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Read Report Char Value, Handle(0x%04x)"),ReportCharValueHandle));
			LeStatus = m_pGattClient->RtGattClientReadCharacteristicValue(ReportCharValueHandle,ReportCharValueData,&ReportCharValueDataLen);
			if (LeStatus != LE_SUCCESS)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID Read Report Char Value ,Handle is(%04x)"),ReportCharValueHandle));
				Status = RT_STATUS_PROCEDURE_FAIL;
				goto exit;
			}
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Write Report Char Value, Handle(0x%04x)"),ReportCharValueHandle));

			LeStatus = m_pGattClient->RtGattClientWriteCharacteristicValue(ReportCharValueHandle,ReportCharValueData,ReportCharValueDataLen);
			if (LeStatus != LE_SUCCESS)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID Write Report Char Value ,Handle is(%04x)"),ReportCharValueHandle));
				Status = RT_STATUS_PROCEDURE_FAIL;
				goto exit;
			}
		}

	}
exit:	
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status(%x)"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::ReadHidInfo()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	unsigned int HIDServiceListLenIndex = 0;
	RT_STATUS Status = RT_STATUS_SUCCESS;

	if(!m_Mode)
	{// Normal
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("m_HIDServiceListLen %d"), m_HIDServiceListLen)); 
		if(m_HIDSupported)
		{
			for (HIDServiceListLenIndex=0;HIDServiceListLenIndex<m_HIDServiceListLen;HIDServiceListLenIndex++)
			{
				Status = DiscoverHIDChar(m_HIDServiceList[HIDServiceListLenIndex]);
			}
		}
		else
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID not support")));
			Status = RT_STATUS_NOT_SUPPORT;
		}

		/*unsigned int BatteryServiceListLenIndex = 0;
		for (;BatteryServiceListLenIndex<m_BatteryServiceListLen;BatteryServiceListLenIndex++)
		{
			DiscoverBatteryServiceInfo(m_BatteryServiceList[BatteryServiceListLenIndex]);
		}

		if (m_ScanParaService.HandleStart !=0)
		{
			DiscoverScanParaServiceInfo();
		}*/

		if ( m_AlertService.HandleStart != 0)
		{
			DiscoverAlertSrvChar();
		}

	}
	else
	{
		DiscoverAlertSrvChar();
	}
	
	
	return Status;
}

RT_STATUS CLEHIDClient::DiscoverDevInfoSrvChar()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;

	if(!m_Mode)
	{// Normal
		LE_STATUS LeStatus = LE_SUCCESS;
		GATT_HANDLE_CHAR_DECLARATION DevInfoCharList[GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX];
		unsigned long DevInfoCharDeclListLen = 0;

		if(m_DeviceInfoService.HandleStart == 0)
			return RT_STATUS_RESOURCE;

		LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
			m_DeviceInfoService.HandleStart,
			m_DeviceInfoService.HandleEnd,
			DevInfoCharList,
			&DevInfoCharDeclListLen
			);
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Get Device Info Service Characteristic Error")));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}

		LE_UUID uuidHWVersion, uuidSWVersion, uuidFWVersion;
		LeUuidSetBit16(&uuidHWVersion,AN_HARDWARE_REVISION_STRING);
		LeUuidSetBit16(&uuidSWVersion,AN_SOFTWARE_REVISION_STRING);
		LeUuidSetBit16(&uuidFWVersion,AN_FIRMWARE_REVISION_STRING);

		unsigned long CharDeclarIndex = 0;
		for (CharDeclarIndex=0;CharDeclarIndex<DevInfoCharDeclListLen;CharDeclarIndex++)
		{
			if (LeUuidIsEqualTo(uuidHWVersion, DevInfoCharList[CharDeclarIndex].CharDec.Uuid))
			{
				m_HWVersionValueHandle =  DevInfoCharList[CharDeclarIndex].CharDec.ValueHandle;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Hardware Revision Enable Handle(0x%x)"), m_HWVersionValueHandle));
			}
			else if(LeUuidIsEqualTo(uuidSWVersion, DevInfoCharList[CharDeclarIndex].CharDec.Uuid))
			{
				m_SWVersionValueHandle = DevInfoCharList[CharDeclarIndex].CharDec.ValueHandle;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Software Revision Handle(0x%x)"), m_SWVersionValueHandle));
			}
			else if(LeUuidIsEqualTo(uuidFWVersion, DevInfoCharList[CharDeclarIndex].CharDec.Uuid))
			{
				m_FWVersionValueHandle = DevInfoCharList[CharDeclarIndex].CharDec.ValueHandle;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Firmware Revision Handle(0x%x)"), m_FWVersionValueHandle));
			}
		}
	}
	else
	{// MP
		//m_SWVersionValueHandle = 0x55;
		//m_FWVersionValueHandle = 0x53;
		//m_HWVersionValueHandle = 0x51;
		/*m_SWVersionValueHandle = 0x46;
		m_FWVersionValueHandle = 0x44;
		m_HWVersionValueHandle = 0x42;*/

		// Using MP Setting
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status %x"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::ReadDeviceInfo(CString &HWVersion, CString &SWVersion, CString &FWVersion )
{
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;

	DiscoverDevInfoSrvChar();

	if( m_HWVersionValueHandle > 0 && m_HWVersionValueHandle < 65535)
	{
		unsigned char HWVerValue[128] = {0};
		unsigned long HWVerValueLen = 0;
		LeStatus =  m_pGattClient->RtGattClientReadCharacteristicValue(
			m_HWVersionValueHandle,
			HWVerValue,
			&HWVerValueLen);
		if (LeStatus != LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read HW Version Fail, Status(%d)"), LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
		}
		else
		{
			HWVersion = CString(HWVerValue);
		}
	}

	if( m_SWVersionValueHandle > 0 && m_SWVersionValueHandle < 65535)
	{
		unsigned char SWVerValue[128] = {0};
		unsigned long SWVerValueLen = 0;
		LeStatus =  m_pGattClient->RtGattClientReadCharacteristicValue(
			m_SWVersionValueHandle,
			SWVerValue,
			&SWVerValueLen);
		if (LeStatus != LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read SW Version Fail, Status(%d)"), LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
		}
		else
		{
			SWVersion = CString(SWVerValue);
		}
	}

	if( m_FWVersionValueHandle > 0 && m_FWVersionValueHandle < 65535)
	{
		unsigned char FWVerValue[128] = {0};
		unsigned long FWVerValueLen = 0;
		LeStatus =  m_pGattClient->RtGattClientReadCharacteristicValue(
			m_FWVersionValueHandle,
			FWVerValue,
			&FWVerValueLen);
		if (LeStatus != LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read FW Version Fail, Status(%d)"), LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
		}
		else
		{
			FWVersion = CString(FWVerValue);
		}
	}
	return Status;
}

RT_STATUS CLEHIDClient::HostEnableDeviceNotification()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;

	unsigned int i = 0;
	unsigned int j = 0;
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T(" m_InputClientCharConfigValueHandleListLen %d"),m_InputClientCharConfigValueHandleListLen));	
	if(m_InputClientCharConfigValueHandleListLen != 0)
	{	
		RegServiceSetNotificationOn(m_ullAddr, AN_HID_REPORT,(RT_U8*)&m_InputClientCharConfigValueHandleList, m_InputClientCharConfigValueHandleListLen * sizeof(RT_U16));
		m_bIsClientCharConfigHandlesReady = TRUE;
		unsigned short EnableNotificationValue = 0x01;
		unsigned short CurrentNotifyValue  = 0;
		unsigned long CurrenctNotifyValueLen = 0;
		if (m_InputClientCharConfigValueHandleListLen>0)
		{
			unsigned char ClientCharConfigValueHandleIndex = 0;
			for (;ClientCharConfigValueHandleIndex<m_InputClientCharConfigValueHandleListLen;ClientCharConfigValueHandleIndex++)
			{
				unsigned short HandleForEnableNotify = m_InputClientCharConfigValueHandleList[ClientCharConfigValueHandleIndex];
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("HandleForEnableNotify (0x%04x)"),HandleForEnableNotify));
				//First Read current Value in HID Device
				/*CurrentNotifyValue = 0;
				CurrenctNotifyValueLen = 0;
				LeStatus = m_pGattClient->RtGattClientReadCharacteristicDescriptors(HandleForEnableNotify,(unsigned char *)&CurrentNotifyValue,&CurrenctNotifyValueLen);
				if (LE_SUCCESS != LeStatus)
				{
					DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID:Read client Configuration Value Handle error(0x%04x) "),HandleForEnableNotify));
					Status = RT_STATUS_PROCEDURE_FAIL;
					goto exit;
				}*/

				LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(HandleForEnableNotify,(unsigned char *)&EnableNotificationValue,sizeof(EnableNotificationValue));
				if (LE_SUCCESS != LeStatus)
				{
					DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("write Client config Handle(0x%04x) error"),HandleForEnableNotify));
					Status = RT_STATUS_PROCEDURE_FAIL;
					goto exit;
				}
				else
				{
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("EnableNotification Handle(0x%04x)success"),HandleForEnableNotify));
				}
			}
		}

		m_pGattClient->RtGattClientExchangeMtu(123);
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Exchange MTU")));

		//Enable Battery Level Client Char configuration
		//CurrentNotifyValue = 0;
		//CurrenctNotifyValueLen = 0;
		//unsigned int BatteryServiceListLenIndex = 0;
		//for (;BatteryServiceListLenIndex<m_BatteryServiceListLen;BatteryServiceListLenIndex++)
		//{
		//	if (m_BatteryServiceList[BatteryServiceListLenIndex].m_BatteryLevelClientCharConfigHandle)
		//	{
		//		//First Read current Value in HID Device
		//		DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Enable m_BatteryLevelClientCharConfigHandle (0x%04x)"),m_BatteryServiceList[BatteryServiceListLenIndex].m_BatteryLevelClientCharConfigHandle));

		//		LeStatus = m_pGattClient->RtGattClientReadCharacteristicDescriptors(m_BatteryServiceList[BatteryServiceListLenIndex].m_BatteryLevelClientCharConfigHandle,(unsigned char *)&CurrentNotifyValue,&CurrenctNotifyValueLen);
		//		if (LE_SUCCESS != LeStatus)
		//		{
		//			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID:Read Battery client Configuration Value Handle(0x%04x) "),m_BatteryServiceList[BatteryServiceListLenIndex].m_BatteryLevelClientCharConfigHandle));
		//			goto exit;
		//		}
		//		//if ((CurrentNotifyValue&0x01) != EnableNotificationValue)
		//		{
		//			LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(m_BatteryServiceList[BatteryServiceListLenIndex].m_BatteryLevelClientCharConfigHandle,(unsigned char *)&EnableNotificationValue,sizeof(EnableNotificationValue));
		//			if (LE_SUCCESS != LeStatus)
		//			{
		//				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("write Client config Handle(0x%04x)"),m_BatteryServiceList[BatteryServiceListLenIndex].m_BatteryLevelClientCharConfigHandle));
		//				goto exit;
		//			}
		//		}
		//	}//END of Enable Enable Battery Level Client Char configuration
		//}
		
		

		//Enable Scan Parameters Refresh Chac Client Char configuration
		//CurrentNotifyValue = 0;
		//CurrenctNotifyValueLen = 0;
		//if (m_ScanRefreshClientConfigHandle)
		//{
		//	//First Read current Value in HID Device
		//	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Enable m_ScanRefreshClientConfigHandle (0x%04x)"),m_ScanRefreshClientConfigHandle));

		//	LeStatus = m_pGattClient->RtGattClientReadCharacteristicDescriptors(m_ScanRefreshClientConfigHandle,(unsigned char *)&CurrentNotifyValue,&CurrenctNotifyValueLen);
		//	if (LE_SUCCESS != LeStatus)
		//	{
		//		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("HID:Read Scan Para Refresh client Configuration Value Handle(0x%04x)"),m_ScanRefreshClientConfigHandle));

		//		goto exit;
		//	}
		//	//	if ((CurrentNotifyValue&0x01) != EnableNotificationValue)
		//	{
		//		LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(m_ScanRefreshClientConfigHandle,(unsigned char *)&EnableNotificationValue,sizeof(EnableNotificationValue));
		//		if (LE_SUCCESS != LeStatus)
		//		{
		//			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("write Client config Handle(0x%04x)"),m_ScanRefreshClientConfigHandle));
		//			goto exit;
		//		}
		//	}
		//}
		////END of Enable Scan Parameters Refresh Charc Client Char configuration
		//if (LE_SUCCESS == LeStatus)
		//{
		//	m_bIsEnableClientConfig = TRUE;
		//	RegServiceSetNotificationOn(m_ullAddr, AN_CLIENT_CHARACTERISTIC_CONFIGURATION,(RT_U8*)&m_bIsEnableClientConfig, sizeof(m_bIsEnableClientConfig));
		//}
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status(%x)"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::EnterOTAMode()
{
	if( m_OTAModeEnableHandle > 0 && m_OTAModeEnableHandle < 65535)
	{
		unsigned char OTAEnable = 1;
		return WriteCmdToDevice(m_OTAModeEnableHandle, &OTAEnable, 1);
	}
	return RT_STATUS_NOT_SUPPORT;
}

RT_STATUS CLEHIDClient::QuiteTestMode()
{
	if( m_TestModeHandle > 0 && m_TestModeHandle < 65535)
	{
		unsigned char TestModeEnable = 2;
		return WriteCmdToDevice(m_TestModeHandle, &TestModeEnable, 1);
	}
	return RT_STATUS_NOT_SUPPORT;
}

RT_STATUS CLEHIDClient::GetOTAInfo(DWORD &PatchVer, DWORD &AppVer)
{
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;

	if(!bOTASupported())
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("OTA not support")));
		return RT_STATUS_NOT_SUPPORT;
	}
	
	Status = DiscoverOTAChar();
	if( Status != RT_STATUS_SUCCESS)
		return Status;

	if( m_BDAddressHandle > 0 && m_BDAddressHandle < 65535)
	{
		unsigned char BDAddrValue[10] = {0};
		unsigned long BDAddrValueLen = 0;
		LeStatus =  m_pGattClient->RtGattClientReadCharacteristicValue(
			m_BDAddressHandle,
			BDAddrValue,
			&BDAddrValueLen);
		if (LeStatus != LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read BDAddress Fail, Status(%d)"), LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
		}
		else
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("BDAddrValueLen(%d)"), BDAddrValueLen));
		}
	}

	if( m_PatchVersionHandle > 0 && m_PatchVersionHandle < 65535)
	{
		PatchVer = 0;
		unsigned char PatchVerValue[10] = {0};
		unsigned long PatchVerValueLen = 0;
		LeStatus =  m_pGattClient->RtGattClientReadCharacteristicValue(
			m_PatchVersionHandle,
			PatchVerValue,
			&PatchVerValueLen);
		if (LeStatus != LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read Patch Version Fail, Status(%d)"), LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
		}
		else
		{
			PatchVer = (PatchVerValue[1]<<8) + PatchVerValue[0];
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("PatchVersion(%d), PatchVerValueLen(%d)"), PatchVer, PatchVerValueLen));
		}
	}

	if( m_AppVersionHandle > 0 && m_AppVersionHandle < 65535)
	{
		AppVer = 0;
		unsigned char AppVerValue[10] = {0};
		unsigned long AppVerValueLen = 0;
		LeStatus =  m_pGattClient->RtGattClientReadCharacteristicValue(
			m_AppVersionHandle,
			AppVerValue,
			&AppVerValueLen);
		if (LeStatus != LE_SUCCESS)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Read App Version Fail, Status(%d)"), LeStatus));
			Status = RT_STATUS_PROCEDURE_FAIL;
		}
		else
		{
			AppVer = (AppVerValue[1]<<8) + AppVerValue[0];
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("AppVersion(%d), AppVerValueLen(%d)"), AppVer,AppVerValueLen));
		}
	}
	
	return Status;
}

unsigned char attRxBuffer[128] = {0};
unsigned long attRxBufLen = 0;
unsigned int CLEHIDClient::WaitHIDReportDataThread(PVOID Param)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	CLEHIDClient * pThis = (CLEHIDClient *) Param;
	unsigned short NotificationHandle;
	unsigned char KeyDownValue;

	CString StrDbg;

#define INPUT_TRANS_TYPE 0xa1
#define FEATUER_TRANS_TYPE 0xa3

	LE_STATUS LeStatus;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	while (pThis->m_bNotify == true)
	{
		// Call client procedure.
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("Wait For ReportData")));
		LeStatus = pThis->m_pGattClient->RtGattWaitForNotification();
		if(pThis->m_bNotify == false)
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("m_bNotify is False, must exit thread")));
			return RT_STATUS_CONNECT_RESET;
		}
		if(LeStatus == LE_SUCCESS)
		{
			pThis->m_pGattClient->RtGattGetNotificationData(attRxBuffer, &attRxBufLen);
			{
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Receive Notification Data")));
				unsigned char* pBuf = &attRxBuffer[0];
				NotificationHandle = (unsigned short)(attRxBuffer[1] | (attRxBuffer[2] << 8));
				KeyDownValue = (unsigned char)attRxBuffer[5];

				::SendMessage(pThis->m_Mainhwnd, WM_BUTTON_RAWDATA, (WPARAM)attRxBufLen, (LPARAM)pBuf);

				if ( NotificationHandle == pThis->m_KeyNotifyHandle )
				{// Button Notification
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Receive HID Report Data,handle(0x%04x) length(0x%04x)"),NotificationHandle,attRxBufLen-3));
					::SendMessage(pThis->m_Mainhwnd, WM_BUTTON_STATE, KeyDownValue, NULL);
				}

				if( NotificationHandle == pThis->m_VoiceNotifyHandle ) // Haixin
				{// Voice Notification
					unsigned char* voiceBuf = &attRxBuffer[3];
					unsigned long voiceBufLen = attRxBufLen - 3;
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("voice data in")));
					::PostMessage(pThis->m_Mainhwnd, WM_VOICE_DATA, (WPARAM)voiceBufLen, (LPARAM)voiceBuf);
				}

				//find the REPORT_ID according to the NotificationHandle
				//unsigned char i = 0;
				//for (i=0;i<pThis->m_HIDReportIDMapValueListLen;i++)
				//{
				//	if ((NotificationHandle == pThis->m_HIDReportIDMapValueList[i].Handle))
				//	{
				//		if (REPORT_INPUT == pThis->m_HIDReportIDMapValueList[i].ReportType)
				//		{
				//			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Receive HID Report Data,handle(0x%04x) length(0x%04x)"),NotificationHandle,attRxBufLen-3));
				//			pThis->ParseHIDReport(ReportValue);
				//			break;//break out of for-loop
				//		}
				//	}
				//	///TODO:	Voice Data Type???
				//}

				/*unsigned int BatteryServiceListIndex = 0;
				for (BatteryServiceListIndex = 0;BatteryServiceListIndex < pThis->m_BatteryServiceListLen;BatteryServiceListIndex++)
				{
					if (NotificationHandle == pThis->m_BatteryServiceList[BatteryServiceListIndex].m_BatteryLevelValueHandle)
					{
						unsigned long BatteryLevelReportDataLength = attRxBufLen-3;
						DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Receive HID Battery Level Report,handle(0x%04x) length(0x%04x)"),
							NotificationHandle,BatteryLevelReportDataLength));			
						ASSERT(BatteryLevelReportDataLength == 1);
						pThis->m_BatteryServiceList[BatteryServiceListIndex].m_BatteryLevelValue = attRxBuffer[3];
						DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Receive HID m_BatteryLevelValue(%d)"),pThis->m_BatteryServiceList[BatteryServiceListIndex].m_BatteryLevelValue));
					}
				}*/

				//if (NotificationHandle == pThis->m_ScanRefreshCharValueHandle)
				//{
				//	unsigned long ScanRefreshReportDataLength = attRxBufLen-3;
				//	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Receive HID Scan Refresh Data,handle(0x%04x) length(0x%04x) "),
				//		NotificationHandle,ScanRefreshReportDataLength));			
				//	//ASSERT(ScanRefreshReportDataLength == 1);
				//	pThis->m_ScanRefreshCharValue = attRxBuffer[3];
				//	DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Receive HID m_ScanRefreshCharValue(%d)"),pThis->m_ScanRefreshCharValue));
				//	pThis->HostWriteScanParaToHIDServer(96,48);
				//}
			}
		}
		else if(LeStatus == LE_ERROR_L2CAP)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("WaitHIDReportDataThread -- with status(0x%x)"),LeStatus));
			return RT_STATUS_NOT_CONNECT;
		}
		else
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("WaitHIDReportDataThread -- with status(0x%x)"),LeStatus));
			return RT_STATUS_NOT_CONNECT;
		}

	}

	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("--")));
	return RT_STATUS_SUCCESS;

}

RT_STATUS CLEHIDClient::BeginWaitReportDataThread()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;
	m_bNotify = TRUE;
	if (NULL == m_hThreadNotify)
	{
		m_hThreadNotify = (HANDLE)_beginthreadex(NULL,0,WaitHIDReportDataThread,this,0,NULL);
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("Start WaitHIDReportDataThread \n")));
		if(NULL == m_hThreadNotify)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("start WaitHIDReportDataThread Error")));
			Status = RT_STATUS_FAILURE;
			goto exit;
		}

		RT_U32 TempLen = sizeof(m_bIsEnableClientConfig);
		if(RegServiceGetNotificationValue(m_ullAddr, AN_CLIENT_CHARACTERISTIC_CONFIGURATION,(RT_U8*)&m_bIsEnableClientConfig, TempLen))
		{
			if (m_bIsEnableClientConfig == FALSE)
			{
				HostEnableDeviceNotification();
			}
		}
		else
		{
			HostEnableDeviceNotification();
		}

	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("BeginWaitReportDataThread is runing")));
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status(%x)"),Status));
	return Status;
}

void CLEHIDClient::SetKeyMap(map<BYTE, BYTE> &mapKey)
{
	if(mapKey.size() == 0)
	{
		// Default
		m_KeyMap[POWERKEY] = MAP_POWERKEY;
		m_KeyMap[MICKEY] = MAP_MICKEY;
		m_KeyMap[UPKEY] = MAP_UPKEY;
		m_KeyMap[DOWNKEY] = MAP_DOWNKEY;
		m_KeyMap[RIGHTKEY] = MAP_RIGHTKEY;
		m_KeyMap[LEFTKEY] = MAP_LEFTKEY;
		m_KeyMap[HOMEKEY] = MAP_HOMEKEY;
		m_KeyMap[MENUKEY] = MAP_MENUKEY;
		m_KeyMap[OKKEY] = MAP_OKKEY;
		m_KeyMap[VOLDOWN] = MAP_VOLDOWN;
		m_KeyMap[VOLUP] = MAP_VOLUP;
		m_KeyMap[BACKKEY] = MAP_BACKKEY;
		m_KeyMap[ALLKEYUP] = MAP_ALLKEYUP;
	}
	else
	{
		m_KeyMap.clear();
		m_KeyMap = mapKey;
	}	
}

RT_STATUS CLEHIDClient::ParseHIDReport(unsigned char Value)
{
	HWND hwnd = m_Mainhwnd;//::FindWindow(NULL, L"HWC");//
	switch(m_KeyMap[Value])
	{
	case MAP_POWERKEY:
		SendMessage(hwnd, WM_CUSTOM_POWERDOWN, 1, NULL);
		break;
	case MAP_HOMEKEY:
		SendMessage(hwnd, WM_CUSTOM_HOMEDOWN, 1, NULL);
		break;
	case MAP_UPKEY:
		SendMessage(hwnd, WM_CUSTOM_UPDOWN, 1, NULL);
		break;
	case MAP_DOWNKEY:
		SendMessage(hwnd, WM_CUSTOM_DOWNDOWN, 1, NULL);
		break;
	case MAP_LEFTKEY:
		SendMessage(hwnd, WM_CUSTOM_LEFTDOWN, 1, NULL);
		break;
	case MAP_RIGHTKEY:
		SendMessage(hwnd, WM_CUSTOM_RIGHTDOWN, 1, NULL);
		break;
	case MAP_OKKEY:
		SendMessage(hwnd, WM_CUSTOM_OKDOWN, 1, NULL);
		break;
	case MAP_BACKKEY:
		SendMessage(hwnd, WM_CUSTOM_BACKDOWN, 1, NULL);
		break;
	case MAP_MICKEY:
		SendMessage(hwnd, WM_CUSTOM_VOICEDOWN, 1, NULL);
		break;
	case MAP_MENUKEY:
		SendMessage(hwnd, WM_CUSTOM_MENUDOWN, 1, NULL);
		break;
	case MAP_VOLUP:
		SendMessage(hwnd, WM_CUSTOM_VOLUPDOWN, 1, NULL);
		break;
	case MAP_VOLDOWN:
		SendMessage(hwnd, WM_CUSTOM_VOLDOWNDOWN, 1, NULL);
		break;
	case MAP_ALLKEYUP:
		SendMessage(hwnd, WM_ALLUP, NULL, NULL);
	default:
		//SendMessage(hwnd, WM_ALLUP, NULL, NULL);
		break;
	}
	return RT_STATUS_SUCCESS;
}

RT_STATUS CLEHIDClient::DiscoverOTAChar()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	GATT_HANDLE_CHAR_DECLARATION OTACharList[GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX];

	unsigned long OTACharDeclListLen = 0;

	LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
		m_OTAService.HandleStart,
		m_OTAService.HandleEnd,
		OTACharList,
		&OTACharDeclListLen
		);
	if (LE_SUCCESS != LeStatus)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Get RCU OTA Service Characteristic Error")));
		Status = RT_STATUS_PROCEDURE_FAIL;
		goto exit;
	}

	LE_UUID uuidOTAModeEnable, uuidDeviceMac, uuidPatchVer, uuidAppVer, uuidTestMode;
	LeUuidSetBit16(&uuidOTAModeEnable,GATT_UUID_CHAR_OTA);
	LeUuidSetBit16(&uuidDeviceMac,GATT_UUID_CHAR_MAC);
	LeUuidSetBit16(&uuidPatchVer,GATT_UUID_CHAR_PATCH);
	LeUuidSetBit16(&uuidAppVer,GATT_UUID_CHAR_APP_VERSION);
	LeUuidSetBit16(&uuidTestMode,GATT_UUID_CHAR_TEST_MODE);

	unsigned long CharDeclarIndex = 0;
	for (CharDeclarIndex=0;CharDeclarIndex<OTACharDeclListLen;CharDeclarIndex++)
	{
		if (LeUuidIsEqualTo(uuidOTAModeEnable, OTACharList[CharDeclarIndex].CharDec.Uuid))
		{
			m_OTAModeEnableHandle =  OTACharList[CharDeclarIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("OTA Mode Enable Handle(0x%x)"), m_OTAModeEnableHandle));
		}
		else if(LeUuidIsEqualTo(uuidDeviceMac, OTACharList[CharDeclarIndex].CharDec.Uuid))
		{
			m_BDAddressHandle = OTACharList[CharDeclarIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("BDAddress Handle(0x%x)"), m_BDAddressHandle));
		}
		else if(LeUuidIsEqualTo(uuidPatchVer, OTACharList[CharDeclarIndex].CharDec.Uuid))
		{
			m_PatchVersionHandle = OTACharList[CharDeclarIndex].CharDec.ValueHandle;	
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Patch Version Handle(0x%x)"), m_PatchVersionHandle));
		}
		else if (LeUuidIsEqualTo(uuidAppVer, OTACharList[CharDeclarIndex].CharDec.Uuid))
		{
			m_AppVersionHandle = OTACharList[CharDeclarIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("App Version Handle(0x%x)"), m_AppVersionHandle));
		}
		else if (LeUuidIsEqualTo(uuidTestMode, OTACharList[CharDeclarIndex].CharDec.Uuid))
		{
			m_TestModeHandle = OTACharList[CharDeclarIndex].CharDec.ValueHandle;
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("TestMode Handle(0x%x)"), m_TestModeHandle));
		}

	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status %x"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::DiscoverHandShakeSrvChar()
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

RT_STATUS CLEHIDClient::AlertSetLevel(unsigned char AlertLevel)
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("Alert Level(%d)"), AlertLevel));
	return WriteCmdToDevice(m_AlertValueHanle, &AlertLevel, sizeof(AlertLevel));
}

RT_STATUS CLEHIDClient::DiscoverAlertSrvChar()
{
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T("++")));
	RT_STATUS Status = RT_STATUS_SUCCESS;

	if(!m_Mode)
	{// Normal
		LE_STATUS LeStatus = LE_SUCCESS;
		GATT_HANDLE_CHAR_DECLARATION AlertCharList[GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX];
		unsigned long AlertCharDeclListLen = 0;

		LeStatus = m_pGattClient->RtGattClientDiscoverAllCharacteristicsOfAService(
			m_AlertService.HandleStart,
			m_AlertService.HandleEnd,
			AlertCharList,
			&AlertCharDeclListLen
			);
		if (LE_SUCCESS != LeStatus)
		{
			DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Get Alert Service Characteristic Error")));
			Status = RT_STATUS_PROCEDURE_FAIL;
			goto exit;
		}

		LE_UUID uuidAlert;
		LeUuidSetBit16(&uuidAlert,AN_ALERT_LEVEL);

		unsigned long CharDeclarIndex = 0;
		for (CharDeclarIndex=0;CharDeclarIndex<AlertCharDeclListLen;CharDeclarIndex++)
		{
			if (LeUuidIsEqualTo(uuidAlert, AlertCharList[CharDeclarIndex].CharDec.Uuid))
			{
				m_AlertValueHanle =  AlertCharList[CharDeclarIndex].CharDec.ValueHandle;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION,RTK_LE_PROFILE_RCU,(_T("Alert Char(0x%04x)"), m_AlertValueHanle));
			}
		}
	}
	else	// MP
	{
		//m_AlertValueHanle = 0x55;	//0x64;
		// Using MP Setting
	}

exit:
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE,RTK_LE_PROFILE_RCU,(_T(" -- Status %x"),Status));
	return Status;
}

RT_STATUS CLEHIDClient::HandShakeWithRCU()
{
	LE_STATUS LeStatus = LE_SUCCESS;
	RT_STATUS Status = RT_STATUS_SUCCESS;
	unsigned char attRxBuffer[128] = {0};
	UINT8 *attData;
	unsigned long attRxBufLen;
	UINT16 notifyHandle;
	UINT8 opCode;
	UINT16 dataLen = 0;

	if(m_HandShakeSupported)
	{
		Status = DiscoverHandShakeSrvChar();
		if(Status == RT_STATUS_SUCCESS)
		{
			// Get Check Data
			fnGetCheckData(HandShakeData);

			USHORT CurrentNotifyValue = GATT_CLIENT_CHAR_CONFIG_NOTIFICATION;
			LeStatus = m_pGattClient->RtGattClientWriteCharacteristicDescriptors(m_HandShakeNotificationHandle, (unsigned char *)&CurrentNotifyValue, sizeof(CurrentNotifyValue));
			if (LE_SUCCESS != LeStatus)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("RtGattClientWriteCharacteristicDescriptors Fail")));
				return RT_STATUS_PROCEDURE_FAIL;;
			}

			// Send the random data to RCU
			WriteCmdToDevice(m_HandShakeCharValueHandle, HandShakeData, 16);
			LeStatus = m_pGattClient->RtGattWaitForNotification();
			if (LE_SUCCESS != LeStatus)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("ClientNotifyRxProc-- with L2cap Disconnected")));
				return RT_STATUS_PROCEDURE_FAIL;
			}

			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Recv Notify Data")));
			m_pGattClient->RtGattGetNotificationData(attRxBuffer, &attRxBufLen);

			DEBUG_DATA(TRACE_LEVEL_VERBOSE,attRxBuffer,attRxBufLen);

			opCode = attRxBuffer[0];
			notifyHandle = (unsigned short)(attRxBuffer[1] | (attRxBuffer[2] << 8));
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("NotifyHandle (%04x)"),notifyHandle));

			dataLen = attRxBufLen - 3;
			if (m_HandShakeCharValueHandle == notifyHandle && dataLen == 16)
			{// ignore header
				attData = attRxBuffer+3;
				memcpy(HandShakeConfirm, attData, 16);
				// Check confirm value
				if(fnHandShakeConfirm(HandShakeConfirm))
					Status = RT_STATUS_SUCCESS;
				else
					Status = RT_STATUS_FAILURE;
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
	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("m_HandShakeSupported is False")));
		Status = RT_STATUS_NOT_SUPPORT;
	}
	
	return Status;
}

