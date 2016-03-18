
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes
#include <Mmsystem.h>


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#include <afxsock.h>            // MFC socket extensions
#include <afxdhtml.h>
//#include <BluetoothAPIs.h>

// custom message
#define WM_CUSTOM_POWERDOWN		WM_USER + 1
#define WM_CUSTOM_HOMEDOWN		WM_USER + 2
#define WM_CUSTOM_UPDOWN		WM_USER + 3
#define WM_CUSTOM_DOWNDOWN		WM_USER + 4
#define WM_CUSTOM_LEFTDOWN		WM_USER + 5
#define WM_CUSTOM_RIGHTDOWN		WM_USER + 6
#define WM_CUSTOM_OKDOWN		WM_USER + 7
#define WM_CUSTOM_BACKDOWN		WM_USER + 8
#define WM_CUSTOM_VOICEDOWN		WM_USER + 9
#define WM_CUSTOM_MENUDOWN		WM_USER + 10
#define WM_CUSTOM_VOLDOWNDOWN	WM_USER + 11
#define WM_CUSTOM_VOLUPDOWN		WM_USER + 12
#define WM_ALLUP				WM_USER + 13
#define WM_VOICE_DATA			WM_USER + 14
#define WM_LEDEV_CONNECT		WM_USER + 15
#define WM_LEDEV_DISCONNECT		WM_USER + 16
#define WM_PLAY_RECORD			WM_USER + 17
#define WM_ALL_BUTTON_ACTIVE	WM_USER + 18

// For Configuration Window
#define WM_OTA_DOWNLOAD_PROCESS	WM_USER + 19

// For All Button State
#define WM_BUTTON_STATE			WM_USER + 20
#define WM_BUTTON_RAWDATA		WM_USER + 21


#define RCU_VOICE_DIR _T("\\BTRCU")

// Configuration File
#define CONFIG_FILE_PATH _T("\\RCUTest.ini")

// Audio Record Path
#define VOICE_RECORD_PATH _T("\\AudioRecord.dat")

// Audio Record Path
#define VOICE_RECORD_PATH _T("\\AudioRecord.dat")
// mSBC
#define MSBC_WAVPATH _T("\\mSBC.wav")
#define MSBC_PCMPATH _T("\\mSBC.pcm")
#define MSBC_DECODER_PATH _T("\\mSBC.dec")

#define SBC_PCM_PATH _T("\\sbc_hx.pcm")
#define SBC_WAVE_PATH _T("\\sbc_hx.wav")

#define SERVICE_LE_DATABASE_KEY         L"System\\CurrentControlSet\\Services\\RtkBtFilter\\LeDatabase\\"

// Default Keys for Huawei
#define POWERKEY	0x66
#define MICKEY		0x3E
#define UPKEY		0x52
#define DOWNKEY		0x51
#define LEFTKEY		0x50
#define RIGHTKEY	0x4F
#define OKKEY		0x28
#define BACKKEY		0x29
#define HOMEKEY		0x4A
#define MENUKEY		0x76
#define VOLDOWN		0x81
#define VOLUP		0x80
#define ALLKEYUP	0x00

// Key-Value Map for HaiXin
#define HX_LOVEKEY		0xF6			// Picture Control 1
#define HX_POWERKEY		0x66			// Picture Control 2
#define HX_MICKEY		0xEA			// Picture Control 3

#define HX_OKKEY		0x28			// Picture Control 4
#define HX_LEFTKEY		0x50			// Picture Control 5
#define HX_RIGHTKEY		0x4F			// Picture Control 6
#define HX_UPKEY		0x52			// Picture Control 7
#define HX_DOWNKEY		0x51			// Picture Control 8

#define HX_BACKKEY		0x71			// Picture Control 9
#define HX_HOMEKEY		0xE3			// Picture Control 10
#define HX_MENUKEY		0x72			// Picture Control 11

#define HX_VOLDOWN		0x81			// Picture Control 12
#define HX_Voice		0xXX			// Picture Control 13		useless
#define HX_VOLUP		0x80			// Picture Control 14

#define HX_DIRECTSHOW	0xB0			// Picture Control 15
#define HX_MOVIE		0xB1			// Picture Control 16
#define HX_GAME			0xB2			// Picture Control 17
#define HX_EDUCATION	0xB4			// Picture Control 18

#define HX_ALLKEYUP		0x00

// MAP Position
#define MAP_ALLKEYUP	0x00	// Key Up	
#define MAP_POWERKEY	0x01
#define MAP_MICKEY		0x02
#define MAP_UPKEY		0x03
#define MAP_DOWNKEY		0x04
#define MAP_LEFTKEY		0x05
#define MAP_RIGHTKEY	0x06
#define MAP_OKKEY		0x07
#define MAP_BACKKEY		0x08
#define MAP_HOMEKEY		0x09
#define MAP_MENUKEY		0x10
#define MAP_VOLDOWN		0x11
#define MAP_VOLUP		0x12

#define VOICE_DECODER_MSBC	L"mSBC"
#define VOICE_DECODER_NULL	L"null"

typedef struct _MP_CONFIG
{
	int bFlag;
	CString BDAdress;
	CString LinkKey;
	CString Ediv;

	CString AlertHandleStr;
	CString SWHandleStr;
	CString FWHandleStr;
	CString HWHandleStr;
	CString KeyNHandleStr;
	CString VoiceNHandleStr;
}MP_CONFIG;


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


