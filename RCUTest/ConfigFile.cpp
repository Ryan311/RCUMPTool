#include "stdafx.h"
#
#include "ConfigFile.h"

int WriteCinfigToIni(CString lpPath, map<BYTE, BYTE> &KeyMap, 
					 map<CString, CString>&u_KeyMap, CString &Decoder, 
					 int &VoiceMode, MP_CONFIG &MpConfig)
{
	if(lpPath.IsEmpty())
		return -1;
	else
	{
		//DeleteFile(lpPath);
	}
	CString KeyPower;
	CString KeyMic;
	CString KeyUp;
	CString KeyDown;
	CString KeyRight;
	CString KeyLeft;
	CString KeyHome;
	CString KeyMenu;
	CString KeyOk;
	CString KeyVolM;
	CString KeyVolP;
	CString KeyBack;
	CString KeyAllUp;

	KeyPower.Format(L"%x", KeyMap[MAP_POWERKEY]);
	KeyMic.Format(L"%x", KeyMap[MAP_MICKEY]);
	KeyUp.Format(L"%x", KeyMap[MAP_UPKEY]);
	KeyDown.Format(L"%x", KeyMap[MAP_DOWNKEY]);
	KeyRight.Format(L"%x", KeyMap[MAP_RIGHTKEY]);
	KeyLeft.Format(L"%x", KeyMap[MAP_LEFTKEY]);
	KeyHome.Format(L"%x", KeyMap[MAP_HOMEKEY]);
	KeyMenu.Format(L"%x", KeyMap[MAP_MENUKEY]);
	KeyOk.Format(L"%x", KeyMap[MAP_OKKEY]);
	KeyVolM.Format(L"%x", KeyMap[MAP_VOLDOWN]);
	KeyVolP.Format(L"%x", KeyMap[MAP_VOLUP]);
	KeyBack.Format(L"%x", KeyMap[MAP_BACKKEY]);
	KeyAllUp.Format(L"%x", KeyMap[MAP_ALLKEYUP]);

	WritePrivateProfileString(KEY_MAP_SESSION, KEY_POWER, (LPCTSTR)KeyPower, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_HOME, (LPCTSTR)KeyHome, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_BACK, (LPCTSTR)KeyBack, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_MENU, (LPCTSTR)KeyMenu, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_OK, (LPCTSTR)KeyOk, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_UP, (LPCTSTR)KeyUp, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_DOWN, (LPCTSTR)KeyDown, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_RIGHT, (LPCTSTR)KeyRight, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_LEFT, (LPCTSTR)KeyLeft, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_MIC, (LPCTSTR)KeyMic, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_VolP, (LPCTSTR)KeyVolP, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_VolM, (LPCTSTR)KeyVolM, lpPath);
	WritePrivateProfileString(KEY_MAP_SESSION, KEY_AllUP, (LPCTSTR)KeyAllUp, lpPath);

	CString UserKeys(L"");
	map<CString,CString>::iterator iter1 = u_KeyMap.begin();  
	while (iter1!=u_KeyMap.end())
	{  
		UserKeys += iter1->first + L",";
		iter1++;
	}
	WritePrivateProfileString(USER_DEFINED_KEY_MAP, USER_DEFINED_KEY_NAMES, (LPCTSTR)UserKeys, lpPath);
	map<CString,CString>::iterator iter2 = u_KeyMap.begin();  
	while (iter2!=u_KeyMap.end())
	{  
		WritePrivateProfileString(USER_DEFINED_KEY_MAP, iter2->first, (LPCTSTR)iter2->second, lpPath);
		iter2++;
	}

	WritePrivateProfileString(VOICE_CONFIG, VOICE_DECORDER, (LPCTSTR)Decoder, lpPath);

	CString VoicePlayMode;
	VoicePlayMode.Format(L"%x", VoiceMode);
	WritePrivateProfileString(VOICE_CONFIG, VOICE_PLAY_MODE, (LPCTSTR)VoicePlayMode, lpPath);

	// Save MP Config
	if(MpConfig.bFlag)
	{// have Mp info
		WritePrivateProfileString(MP_MODE, MP_BTADDRESS, (LPCTSTR)MpConfig.BDAdress, lpPath);
		WritePrivateProfileString(MP_MODE, MP_LINKKEY, (LPCTSTR)MpConfig.LinkKey, lpPath);
		WritePrivateProfileString(MP_MODE, MP_ALERT_HANDLE, (LPCTSTR)MpConfig.AlertHandleStr, lpPath);
		WritePrivateProfileString(MP_MODE, MP_SW_HANDLE, (LPCTSTR)MpConfig.SWHandleStr, lpPath);
		WritePrivateProfileString(MP_MODE, MP_FW_HANDLE, (LPCTSTR)MpConfig.FWHandleStr, lpPath);
		WritePrivateProfileString(MP_MODE, MP_HW_HANDLE, (LPCTSTR)MpConfig.HWHandleStr, lpPath);
		WritePrivateProfileString(MP_MODE, MP_KEY_NOTIFY, (LPCTSTR)MpConfig.KeyNHandleStr, lpPath);
		WritePrivateProfileString(MP_MODE, MP_VOICE_NOTIFY, (LPCTSTR)MpConfig.VoiceNHandleStr, lpPath);
	}

	return 0;
}

int ReadCinfigFromIni(CString lpPath, map<BYTE, BYTE> &KeyMap, map<CString, CString>&u_KeyMap, 
					  CString &Decoder, int &bHandShake, int &VoiceMode, MP_CONFIG &MpConfig)
{
	if(lpPath.IsEmpty())
		return -1;
	int PowerValue;	
	int HomeValue;	
	int OKValue;	
	int UpValue;
	int DownValue;	
	int RightValue;
	int LeftValue;	
	int BackValue;	
	int MenuValue;	
	int MicValue;
	int VolPValue;
	int VolMValue;
	int AllKeysUp;

	LPTSTR temp = new TCHAR[32];

	GetPrivateProfileString(KEY_MAP_SESSION, KEY_POWER, L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&PowerValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_HOME, L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&HomeValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_BACK, L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&BackValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_MENU, L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&MenuValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_OK, L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&OKValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_UP,  L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&UpValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_DOWN,  L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&DownValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_RIGHT, L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&RightValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_LEFT,  L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&LeftValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_MIC,  L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&MicValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_VolP,  L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&VolPValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_VolM, L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&VolMValue);
	GetPrivateProfileString(KEY_MAP_SESSION, KEY_AllUP,  L"0" , temp, 32, lpPath);
	swscanf_s(temp,L"%x",&AllKeysUp);

	KeyMap.clear();
	KeyMap[MAP_POWERKEY]= PowerValue;
	KeyMap[MAP_MICKEY]	= MicValue;
	KeyMap[MAP_UPKEY]	= UpValue;
	KeyMap[MAP_DOWNKEY]	= DownValue;
	KeyMap[MAP_RIGHTKEY]= RightValue;
	KeyMap[MAP_LEFTKEY]	= LeftValue;
	KeyMap[MAP_HOMEKEY]	= HomeValue;
	KeyMap[MAP_MENUKEY]	= MenuValue;
	KeyMap[MAP_OKKEY]	= OKValue;
	KeyMap[MAP_VOLDOWN]	= VolMValue;
	KeyMap[MAP_VOLUP]	= VolPValue;
	KeyMap[MAP_BACKKEY]	= BackValue;
	KeyMap[MAP_ALLKEYUP]= AllKeysUp;

	GetPrivateProfileString(VOICE_CONFIG, VOICE_DECORDER, L"mSBC" , temp, 32, lpPath);
	Decoder = CString(temp);

	bHandShake = GetPrivateProfileInt(HAND_SHAKE, HAND_SHAKE_EANBLE, 1, lpPath);

	VoiceMode = GetPrivateProfileInt(VOICE_CONFIG, VOICE_PLAY_MODE, 0, lpPath);

	LPTSTR ltmp = new TCHAR[1024];
	CString nameStr(L"");
	CString valueStr(L"");
	GetPrivateProfileString(USER_DEFINED_KEY_MAP, USER_DEFINED_KEY_NAMES, L"" , ltmp, 1024, lpPath);
	CString KeyNames = CString(ltmp);
	int i = 0;
	while (AfxExtractSubString(nameStr, KeyNames, i++, _T(',')))
	{
		if(!nameStr.IsEmpty())
		{
			GetPrivateProfileString(USER_DEFINED_KEY_MAP, nameStr, L"" , temp, 32, lpPath);
			valueStr = CString(temp);
			u_KeyMap[nameStr] = valueStr;
		}
	}

	{// load Mp info
		GetPrivateProfileString(MP_MODE, MP_BTADDRESS, L"" , ltmp, 1024, lpPath);
		MpConfig.BDAdress = CString(ltmp);
		GetPrivateProfileString(MP_MODE, MP_LINKKEY, L"" , ltmp, 1024, lpPath);
		MpConfig.LinkKey = CString(ltmp);

		GetPrivateProfileString(MP_MODE, MP_ALERT_HANDLE, L"0x55" , ltmp, 1024, lpPath);
		MpConfig.AlertHandleStr = CString(ltmp);
		GetPrivateProfileString(MP_MODE, MP_SW_HANDLE, L"0x46" , ltmp, 1024, lpPath);
		MpConfig.SWHandleStr = CString(ltmp);
		GetPrivateProfileString(MP_MODE, MP_FW_HANDLE, L"0x44" , ltmp, 1024, lpPath);
		MpConfig.FWHandleStr = CString(ltmp);
		GetPrivateProfileString(MP_MODE, MP_HW_HANDLE, L"0x42" , ltmp, 1024, lpPath);
		MpConfig.HWHandleStr = CString(ltmp);
		GetPrivateProfileString(MP_MODE, MP_KEY_NOTIFY, L"0x1e" , ltmp, 1024, lpPath);
		MpConfig.KeyNHandleStr = CString(ltmp);
		GetPrivateProfileString(MP_MODE, MP_VOICE_NOTIFY, L"0x25" , ltmp, 1024, lpPath);
		MpConfig.VoiceNHandleStr = CString(ltmp);
	}

	delete temp;
	delete ltmp;
	return 0;
}