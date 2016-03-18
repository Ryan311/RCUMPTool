#include "stdafx.h"
#include "trace.h"
#include <windows.h>


#include <tchar.h>
#include <strsafe.h>

unsigned int gDebugComponent = RTK_LE_PROFILE_RCU;    
unsigned int gDebugLevel = TRACE_LEVEL_NONE;

extern TCHAR    gDebugOutputBuffer[2048] = {0};

#define KEY_VALUE_NAME_COMPONENT_LE_PROFILES				(L"LE_PROFILES_COMPONENTS")
#define PROTOCOL_BT4STACK				_T("SOFTWARE\\Realtek\\Bluetooth\\BT4Stack")


TCHAR* StringLevel[TRACE_LEVEL_NUM] = 
{
	_T("NONE"),
	_T("CRITICAL_OR_FATAL"),
	_T("ERROR"),
	_T("WARNING"),
	_T("INFORMATION"),
	_T("VERBOSE"),
	_T("RESERVED6"),
	_T("RESERVED7"),
	_T("RESERVED8"),
	_T("RESERVED9")
};


//****************************************************************************
// Public Function Definitions
//****************************************************************************

void 
DbgUpdateParameter(unsigned int DbgLevel, unsigned int DbgComp)
{
	gDebugLevel = DbgLevel;
	gDebugComponent = DbgComp;
	return;
}

void OutputDebugStringFormat( TCHAR* pFormat, ... )
{
	TCHAR buffer[2048];
	va_list args;
	va_start( args, pFormat );
	StringCchVPrintf( buffer, sizeof(buffer), pFormat, args );
	OutputDebugString( buffer );
}

void DebugStringFormat( TCHAR* pFormat, ... )
{
	va_list args;
	va_start( args, pFormat );
	StringCchVPrintf( gDebugOutputBuffer, sizeof(gDebugOutputBuffer), pFormat, args );
}



BOOL ReadDebugComponentValueFromReg(DWORD *value)  
{  
	DWORD type;  
	DWORD s;  
	DWORD len = sizeof(DWORD);  
	HKEY key;  

	RegOpenKeyW(HKEY_LOCAL_MACHINE,PROTOCOL_BT4STACK, &key);  

	if (RegQueryValueEx(key, KEY_VALUE_NAME_COMPONENT_LE_PROFILES, 0, &type, (LPBYTE)&s, &len)==ERROR_SUCCESS)  
	{  
		*value = s;  
		return TRUE;  
	}  
	return FALSE;  
}  


