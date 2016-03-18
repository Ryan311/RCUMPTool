#ifndef LE_HID_CLIENT_
#define LE_HID_CLIENT_
#include "gatt_api.h"
#include <map>
using namespace std;

#define  REPORT_DESCRIPTOR_VALUE_LEN_MAX 512
#define  REPORT_ID_MAP_LIST_LEN_MAX    32
#define	 HID_SERVICE_LIST_LEN_MAX 5

//
// Characteristic UUID
//
// GAP
// Battery
// Device Info
// Scan Parameter
// #define AN_****	0x2a**

class CGattEntity;
class CGattInstance;

typedef struct 
{	
	unsigned short Handle;
	GATT_CHAR_DESCRIPTOR Descriptor;
}GATT_HID_REPORT_ID_MAPPING;

typedef struct 
{
	unsigned char ReportId;
	unsigned char ReportType;
	unsigned short Handle;
}HID_REPORT_ID_MAP_VALUE;

typedef struct _LE_HID_SERVICE_PARAS
{
	unsigned short m_InputClientCharConfigValueHandleList[REPORT_ID_MAP_LIST_LEN_MAX];//for INPUT report
	unsigned char m_InputClientCharConfigValueHandleListLen;
	bool m_bIsClientCharConfigHandlesReady;
	unsigned short m_HIDControlPointCharValueHandle;//need to bake-up

	//get HID Service
	GATT_GROUP_HANDLE_VALUE m_HIDService;

	//All Characteristic of HID Service
	GATT_HANDLE_CHAR_DECLARATION m_CharDeclarOfHIDList[GATT_HANDLE_CHAR_DECLARATION_LIST_LEN_MAX];// = {0};
	unsigned long m_CharDeclarOfHIDListLen;// = 0;

	//find HID_Report_Descriptor Declaration,  in All characteristics of HID Service
	GATT_HANDLE_CHAR_DECLARATION HIDReportDiscriptorCharDec;
	unsigned char HIDReportDiscriptorCharDecIndex;// = 0xff;

	// find HID_Report Declaration,  in All characteristics of HID Service
	GATT_HANDLE_CHAR_DECLARATION HIDReportCharDecList[32];// = {0};
	unsigned char HIDReportCharDecListLen;// = 0xff;

	// Keyboard Input 
	GATT_HANDLE_CHAR_DECLARATION HIDBootKeyboardInputReportCharDec;// = {0};
	unsigned char HIDBootKeyboardInputReportCharDecIndex;// = 0xff;
	unsigned short HIDBootKeyboardInputClientConfigHandle;

	// Keyboard Output
	GATT_HANDLE_CHAR_DECLARATION HIDBootKeyboardOutputReportCharDec;// = {0};
	unsigned char HIDBootKeyboardOutputReportCharDecIndex;// = 0xff;
	unsigned short HIDBootKeyboardOutputClientConfigHandle;

	// Mouse Input
	GATT_HANDLE_CHAR_DECLARATION HIDBootMouseInputReportCharDec;// = {0};
	unsigned char HIDBootMouseInputReportCharDecIndex;// = 0xff;
	unsigned short HIDBootMouseInputClientConfigHandle;

	//find HID_Control_point Declaration, in All characteristics of HID Service
	GATT_HANDLE_CHAR_DECLARATION HIDControlPointCharDec;// = {0};
	unsigned char HIDControlPointCharDecIndex;// = 0xff;

	// find Protocol Mode
	GATT_HANDLE_CHAR_DECLARATION ProtocolModeCharDec;// = {0};
	unsigned char HIDProtocolModeCharDecIndex;// = 0xff;

	//find HID Information Declaration,  in All characteristics of HID Service
	GATT_HANDLE_CHAR_DECLARATION HIDInfoCharDec;// = {0};
	unsigned char HIDInfoCharDecIndex;
	unsigned char m_HIDInfoValue[8];
}LE_HID_SERVICE_PARAS;

typedef struct _LE_BATTERY_SERVICE_PARAS
{
	GATT_GROUP_HANDLE_VALUE m_BatteryService;
	unsigned short m_BatteryLevelValueHandle;	//Need to bake-up
	unsigned char m_BatteryLevelValue;			//0~100:0%~100%
	HID_REPORT_ID_MAP_VALUE m_BatteryLevelValueReportIdMap;
	unsigned short m_BatteryLevelClientCharConfigHandle;	//need to bake up
}LE_BATTERY_SERVICE_PARAS;


class CLEHIDClient
{
public:
	CLEHIDClient();
	~CLEHIDClient();

	BOOL Init(unsigned long long ServerAddr);
	VOID UnInit();

	/**
		0  - success with indication not set
		1  - success with indication has been set
		-1 - fail with connect
	*/
	RT_STATUS ConnectTo();
	RT_STATUS ReadHidInfo();
	void Disconnect();
	void SetConnectMode(int Mode,  MP_CONFIG &MPConfig);

	RT_STATUS DiscoverService();
	BOOLEAN bHIDSupported();
	RT_STATUS DiscoverHIDChar(LE_HID_SERVICE_PARAS& HIDServiceParas);
	RT_STATUS GetDescriptorsInfoOfACharacteristic(unsigned short StartHandle,unsigned short EndHandle,unsigned long CharDeclarIndex,LE_HID_SERVICE_PARAS ThisHIDServiceParas);
	RT_STATUS DiscoverBatteryServiceInfo(LE_BATTERY_SERVICE_PARAS& BatteryServiceInstance);
	RT_STATUS GetBatteryLevelValue(LE_BATTERY_SERVICE_PARAS BatteryServiceInstance);
	RT_STATUS DiscoverScanParaServiceInfo();
	RT_STATUS HIDHostWriteReportCharacteristic(LE_HID_SERVICE_PARAS HIDServiceParas);
	RT_STATUS EnableHidNotification();
	RT_STATUS HostWriteScanParaToHIDServer(unsigned short LeScanInterval,unsigned short LeScanWindow);

	RT_STATUS WriteCmdToDevice(unsigned short targetHandle, unsigned char*Buf, int BufLen);
	RT_STATUS WriteRequestToDevice(unsigned short targetHandle, unsigned char* Buf, int BufLen);
	RT_STATUS WriteDataToChannel(unsigned char *Buf, int BufLen);
	RT_STATUS HIDHostReadReportData(unsigned char ReportType);
	RT_STATUS HostEnableDeviceNotification();

	static unsigned int WINAPI WaitHIDReportDataThread(PVOID Param);
	RT_STATUS BeginWaitReportDataThread();
	void SetKeyMap(map<BYTE, BYTE> &mapKey);
	RT_STATUS ParseHIDReport(unsigned char value);

	// Device Info
	RT_STATUS DiscoverDevInfoSrvChar();
	RT_STATUS ReadDeviceInfo(CString &HWVersion, CString &SWVersion, CString &FWVersion );
	RT_STATUS GetDevInfoValueHandle(unsigned short &HWValueHandle, unsigned short &SWValueHandle, unsigned short &FWValueHandle );

	// HandShake
	BOOLEAN bHandShakeSupported();
	RT_STATUS DiscoverHandShakeSrvChar();
	RT_STATUS HandShakeWithRCU();

	// Alert
	RT_STATUS DiscoverAlertSrvChar();
	RT_STATUS AlertSetLevel(unsigned char AlertLevel);

	// OTA
	BOOLEAN bOTASupported();
	RT_STATUS DiscoverOTAChar();
	RT_STATUS GetOTAInfo(DWORD &PatchVer, DWORD &AppVer);
	RT_STATUS EnterOTAMode();
	RT_STATUS QuiteTestMode();

	CGattInstance *m_pGattInstance;
	CGattEntity * m_pGattClient;
	BOOLEAN m_init;
	HWND m_Mainhwnd;

private:

	UINT8 m_SecLevel;
	ULONGLONG m_ullAddr;			// Server Address
	map<BYTE, BYTE> m_KeyMap;		// Key Map
	BOOLEAN m_HIDSupported;
	BOOLEAN m_OTASupported;
	BOOLEAN m_HandShakeSupported;
	int m_Mode;	// 0 Normal   1 MP

protected:

	HANDLE m_hThreadNotify;
	bool m_bNotify;
	bool m_bReadGattDataSuccess;
	bool m_bMSStackInitConnToHIDDevice;
	HANDLE m_hHandleAutoConnectThread;
	unsigned char m_NotifyNumCount;
	UCHAR m_bIsEnableClientConfig;
	HANDLE m_hEventExitAutoConnectThread;
	HANDLE m_hEventStartToConnect;
	HANDLE m_hEventBondDeviceFinished;

	// GAP Service
	GATT_GROUP_HANDLE_VALUE m_GapService;

	// HID Service
	HID_REPORT_ID_MAP_VALUE m_HIDReportIDMapValueList[REPORT_ID_MAP_LIST_LEN_MAX];
	unsigned char m_HIDReportIDMapValueListLen;

	unsigned short m_InputClientCharConfigValueHandleList[REPORT_ID_MAP_LIST_LEN_MAX];//for INPUT report
	unsigned char m_InputClientCharConfigValueHandleListLen;
	bool m_bIsClientCharConfigHandlesReady;
	unsigned short m_HIDControlPointCharValueHandle;//need to bake-up
	LE_HID_SERVICE_PARAS m_HIDServiceList[HID_SERVICE_LIST_LEN_MAX];
	unsigned long m_HIDServiceListLen;
	unsigned short m_KeyNotifyHandle;
	unsigned short m_VoiceNotifyHandle;

	unsigned char m_ReportDescriptorValue[REPORT_DESCRIPTOR_VALUE_LEN_MAX];
	unsigned long m_ReportDescriptorValueLen;

	//get Battery Service
	LE_BATTERY_SERVICE_PARAS m_BatteryServiceList[HID_SERVICE_LIST_LEN_MAX];
	unsigned long m_BatteryServiceListLen;

	//Scan Parameters Service
	GATT_GROUP_HANDLE_VALUE m_ScanParaService;
	unsigned short m_ScanIntervalWindowValueHandle;//need to bake-up in Registry
	unsigned char m_ScanRefreshCharValue;
	unsigned short m_ScanRefreshCharValueHandle;//need to bake-up in Registry
	unsigned short m_ScanRefreshClientConfigHandle;//need to bake up

	//DeviceInformation
	GATT_GROUP_HANDLE_VALUE m_DeviceInfoService;
	unsigned short m_SWVersionValueHandle;
	unsigned short m_FWVersionValueHandle;
	unsigned short m_HWVersionValueHandle;

	//Alert Service
	GATT_GROUP_HANDLE_VALUE m_AlertService;
	unsigned short m_AlertValueHanle;

	// OTA Service
	GATT_GROUP_HANDLE_VALUE m_OTAService;
	unsigned short m_OTAModeEnableHandle;
	unsigned short m_BDAddressHandle;
	unsigned short m_PatchVersionHandle;
	unsigned short m_AppVersionHandle;
	unsigned short m_TestModeHandle;

	// HandShake Service
	GATT_GROUP_HANDLE_VALUE m_HandShakeService;
	unsigned short m_HandShakeCharValueHandle;
	unsigned short m_HandShakeNotificationHandle;

	// MP Static Handle
	CRITICAL_SECTION m_csNotifyConnectHidDeviceLock;
};
#endif