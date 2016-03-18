/*********************************************************************

Copyright @ Realtek Semiconductor Corp. All Rights Reserved.

THIS CODE AND INFORMATION IS PROVIDED [AS] [IS] WITHOUT WARRANTY OF ANY
KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.


\file :     DFU_def.h

\brief  :   Implementation of DFU Client

Rui Huang

Environment:    Win7 only

Comment:



Revision History:
When            Who                         What                   
-------------   ------------------------    -----------------------
2014/10/27      rui_huang
********************************************************************/

#ifndef DFU_DEF_H_
#define DFU_DEF_H_

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#include "gatt_api.h"

// DFU Opcode
#define	OPCODE_DFU_START_DFU							0x01
#define OPCODE_DFU_RECEIVE_FW_IMAGE						0x02
#define OPCODE_DFU_VALIDATE_FW_IMAGE					0x03
#define OPCODE_DFU_ACTIVE_IMAGE_RESET					0x04
#define OPCODE_DFU_RESET_SYSTEM							0x05
#define OPCODE_DFU_REPORT_RECEIVED_IMAGE_INFO			0x06
#define OPCODE_DFU_PACKET_RECEIPT_NOTIFICATION_REQUEST	0x07
#define OPCODE_DFU_RESPONSE_CODE						0x10
#define OPCODE_DFU_PACKET_RECEIPT_NOTIFICATION			0x11

// ERROR Code for OTA command response
#define ERROR_STATE_SUCCESS						0x01
#define ERROR_STATE_OPCODE_NOT_SUPPORTED		0x02
#define ERROR_STATE_INVALID_PARA				0x03
#define ERROR_STATE_OPRERATION_FAILED			0x04
#define ERROR_STATE_DATA_SIZE_EXCEEDS			0x05
#define ERROR_STATE_CRC_ERROR					0x06

#pragma pack(push,1)
// DFU Response Header
typedef struct _DEF_RESPONSE_HEADER
{
	UINT8	Opcode;
	UINT8	ReqOpcode;
	UINT8	RspValue;
}DEF_RESPONSE_HEADER;


// DFU Op structure
// OPCODE_DFU_START_DFU
#if 1
typedef struct _DFU_START_DFU{
	UINT8	Opcode;
	UINT16	offset;
	UINT16	signature;
	UINT16	version;
	UINT16	checksum;         
	UINT16	length;     
	UINT8	ota_flag;    // default: 0xff
	UINT8	reserved_8;  // default: 0xff
	UINT32  reservedForAes;//should be 0
}DFU_START_DFU;
#else
typedef struct _DFU_START_DFU{
	UINT8	Opcode;
	UINT16	offset;
	UINT16	signature;
	UINT16	version;
	UINT16	checksum;         
	UINT16	length;     
	UINT8	ota_flag;    // default: 0xff
	UINT8	reserved_8;  // default: 0xff
}DFU_START_DFU;
#endif

typedef struct _DFU_START_DFU_RESPONSE{
	UINT8	Opcode;
	UINT8	ReqOpcode;
	UINT8	RspValue;
}DFU_START_DFU_RESPONSE;


// OPCODE_DFU_RECEIVE_FW_IMAGE
typedef struct _RECEIVE_FW_IMAGE{
	UINT8	Opcode;
	UINT16	nSignature;
	UINT32	nImageUpdateOffset;
}RECEIVE_FW_IMAGE;

typedef struct _RECEIVE_FW_IMAGE_RESPONSE{
	UINT8	Opcode;
	UINT8	ReqOpcode;
	UINT8	RspValue;
}RECEIVE_FW_IMAGE_RESPONSE;


// OPCODE_DFU_VALIDATE_FW_IMAGE
typedef struct _VALIDATE_FW_IMAGE{
	UINT8	Opcode;
	UINT16	nSignature;
}VALIDATE_FW_IMAGE;

typedef struct _VALIDATE_FW_IMAGE_RESPONSE{
	UINT8	Opcode;
	UINT8	ReqOpcode;
	UINT8	RspValue;
}VALIDATE_FW_IMAGE_RESPONSE;

//OPCODE_DFU_ACTIVE_IMAGE_RESET
typedef struct _ACTIVE_IMAGE_RESET{
	UINT8	Opcode;
}ACTIVE_IMAGE_RESET;

//OPCODE_DFU_RESET_SYSTEM
typedef struct _RESET_SYSTEM{
	UINT8	Opcode;
}RESET_SYSTEM;

// OPCODE_DFU_REPORT_RECEIVED_IMAGE_INFO
typedef struct _REPORT_RECEIVED_IMAGE_INFO{
	UINT8	Opcode;
	UINT16	nSignature;
}REPORT_RECEIVED_IMAGE_INFO;

typedef struct _REPORT_RECEIVED_IMAGE_INFO_RESPONSE{
	UINT8	Opcode;
	UINT8	ReqOpcode;
	UINT8	RspValue;
	UINT16	OriginalFWVersion;
	UINT32	nImageUpdateOffset;
}REPORT_RECEIVED_IMAGE_INFO_RESPONSE;

//	OPCODE_DFU_PACKET_RECEIPT_NOTIFICATION_REQUEST
typedef struct _PACKET_RECEIPT_NOTIFICATION_REQUEST{
	UINT8	Opcode;
	UINT16	PacketNum;
}PACKET_RECEIPT_NOTIFICATION_REQUEST;

typedef struct _PACKET_RECEIPT_NOTIFICATION_REQUEST_RESPONSE{
	UINT8	Opcode;
	UINT8	ReqOpcode;
	UINT8	RspValue;
	UINT16	PacketNum;
}PACKET_RECEIPT_NOTIFICATION_REQUEST_RESPONSE;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _DFU_FW_IMAGE_INFO
{
	// Image file info
	UINT16 FWImageSize;
	UINT16 FWVersion;
	
	// Remote device info
	UINT16 OriginalVersion;
	UINT32 ImageUpdateOffset;
}DFU_FW_IMAGE_INFO;

typedef struct _DFU_DATA_INFO
{
	UCHAR Flag;						// ???
	DFU_FW_IMAGE_INFO CurInfo;		// read from OPCODE_DFU_REPORT_RECEIVED_IMAGE_INFO
	DFU_FW_IMAGE_INFO ImgInfo;		// patch info loaded from file
}DFU_DATA_INFO;

typedef struct _IMAGE_HEADER{
	UINT16 offset;
	UINT16 signature;
	UINT16 version;
	UINT16 checksum;         
	UINT16 length;     
	UINT8  ota_flag;    // default: 0xff
	UINT8  reserved_8;  // default: 0xff   
}IMAGE_HEADER;
#pragma pack(pop)

class CGattEntity;

#pragma pack(push,1)
typedef struct _DFU_HANDLES
{
	USHORT DFUDataHandle;
	USHORT ControlPointValueHandle;
	USHORT ControlPointClientConfigHandle;
	BOOL   bIsEnableNotify;
}DFU_HADNLES;
#pragma  pack(pop)

class CGattEntity;
class CGattInstance;

class CDFUClient
{
public:
    CDFUClient();
    ~CDFUClient();

	BOOL Init(unsigned long long ServerAddr);
	VOID UnInit();
	VOID SetSecLevel(UINT8 SecLevel);

    RT_STATUS ConnectTo(IN unsigned long long ServerAddress);
	void Disconnect(IN unsigned long long ServerAddress);

	int SetIndicationOn(IN unsigned long long ServerAddress);
	int SetIndicationOff(IN unsigned long long ServerAddress);
//	VOID RegistDisconnCallBack(UPPER_UI_DISCONN_CALLBACK pUpperUICallBack, LPVOID pUpperUI);
	BOOL UnRegistDisconnCallBack();
//	void RegisterDFUValueCB(IN DFU_MEASURE_ROUTINE MeasureProcessCB,IN PVOID Private);
	BOOL UnRegisterDFUValueCB();
	VOID SetLeControlUIClosed(bool Value);

	// DFU variable
//	UPPER_UI_DISCONN_CALLBACK m_pCallBackNotifyUpperUIDisconnect;
	LPVOID m_pUpperUI;
	DFU_DATA_INFO  m_DfuDataInfo;
	DFU_HADNLES m_DFUHandles;
	BOOL m_bIsSetOnNotify;
	BOOL m_bIsDisconnect;
	CGattInstance *m_pGattInstance;
	CGattEntity * m_pGattClient;

	// HandShake Service
	GATT_GROUP_HANDLE m_HandShakeService;
	unsigned short m_HandShakeCharValueHandle;
	unsigned short m_HandShakeNotificationHandle;
	
	// DFU Procedure
	RT_STATUS OTALoadFWImageFromFile(CString &pszFileName);
	RT_STATUS OTAGetTargetImageInfo();
	RT_STATUS OTAStartDFU();
	RT_STATUS OTAValidFW();
	RT_STATUS OTAActiveAndReset();
	RT_STATUS OTAGetControlPointResponseCode(UINT8 Opcode, UINT8** RepData);
	RT_STATUS HandShakeWithRCU();
	RT_STATUS WriteCmdToDevice(unsigned short targetHandle, unsigned char* Buf, int BufLen);

private:
    PVOID  m_pPrivate;                     // call back context.
	UINT8 m_SecLevel;
    //RT_STATUS DiscoverDFUService(IN CGattEntity * pGattClient);
	RT_STATUS DiscoverDFUServices(IN CGattEntity * pGattClient);
    RT_STATUS DiscoverDFUDataChar(IN CGattEntity * pGattClient);
	RT_STATUS GetDFUFeature(IN CGattEntity * pGattClient);
    RT_STATUS DiscoverControlPointChar(IN CGattEntity * pGattClient);
	BOOL  IsNotificationOn(ULONGLONG BtAddr);
	RT_STATUS EnableNotifyIndicate();
	RT_STATUS DFUPushImageToTarget();
	RT_STATUS DiscoverHandShakeSrvChar();

    USHORT m_DFUServerHandleStart;    // start handle.
    USHORT m_DFUServerHandleEnd;      // end handle.
	ULONGLONG m_ullAddr;
	BOOL m_bIsUIClosing;

public:
	CFile ImageFile;
	IMAGE_HEADER ImgHeader;
	UINT32 DataLen;
	UINT8  DataBuf[1024*256];
};

#endif