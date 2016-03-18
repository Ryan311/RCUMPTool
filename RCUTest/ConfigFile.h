#pragma once

#include <map>
using namespace std;

// Session
#define KEY_MAP_SESSION L"KeyMap"
#define USER_DEFINED_KEY_MAP L"UserDefinedKeyMap"
#define VOICE_CONFIG	L"VoiceConfig"
#define HAND_SHAKE		L"HandShake"
#define MP_MODE			L"MPConfig"

// Key Name
#define KEY_POWER	L"Key_Power "
#define KEY_HOME	L"Key_Home  "
#define KEY_MENU	L"Key_Menu  "
#define KEY_BACK	L"Key_Back  "
#define KEY_OK		L"Key_OK    "
#define KEY_UP		L"Key_UP    "
#define KEY_DOWN	L"Key_Down  "
#define KEY_RIGHT	L"Key_Right "
#define KEY_LEFT	L"Key_Left  "
#define KEY_MIC		L"Key_Record"
#define KEY_VolP	L"Key_VolPP "
#define KEY_VolM	L"Key_VolMM "
#define KEY_AllUP	L"Key_AllUp "

#define VOICE_DECORDER L"Decorder"
#define VOICE_PLAY_MODE L"Mode"
#define HAND_SHAKE_EANBLE L"Enable"
#define USER_DEFINED_KEY_NAMES L"AllUserKeyNames"

// MP Configuration
#define MP_BTADDRESS L"BDAdress"
#define MP_LINKKEY	 L"LinkKey"
#define MP_EDIV		 L"Ediv"
#define MP_ALERT_HANDLE L"AlertHandle"
#define MP_SW_HANDLE	L"SoftwareHandle"
#define MP_FW_HANDLE	L"FirmwareHandle"
#define MP_HW_HANDLE	L"HardwareHandle"
#define MP_KEY_NOTIFY	L"KeyNotifyHandle"
#define MP_VOICE_NOTIFY	L"VoiceNotifyHandle"

// For HaiXin
#define MP_ADDRESS_01 L"00e04c238801"
#define MP_ADDRESS_02 L"00e04c238802"
#define MP_ADDRESS_03 L"00e04c238803"
#define MP_ADDRESS_04 L"00e04c238804"
#define MP_ADDRESS_05 L"00e04c238805"
#define MP_LINK_KEY L"cb84aa4d6642d5a233a16a519a50b5ac"

int WriteCinfigToIni(CString lpPath, 
					 map<BYTE, BYTE> &KeyMap, 
					 map<CString, CString>&u_KeyMap, 
					 CString &Decoder, int &VoiceMode, 
					 MP_CONFIG &MpConfig);
int ReadCinfigFromIni(CString lpPath, 
					  map<BYTE, BYTE> &KeyMap, 
					  map<CString, CString>&u_KeyMap, 
					  CString &Decoder, int &bHandShake, 
					  int &VoiceMode, MP_CONFIG &MpConfig);
