/*++

Module Name:

	Trace.h

Abstract:

	Header file for the debug tracing related function definitions and macros.

Environment:

	User mode

--*/

//
// Define the tracing flags.
//
// Tracing GUID - {14B5FCEA-6E11-405b-B6E2-1C6076D4AA82}
////
#ifndef TRACE_
#define TRACE_

#include "windows.h"
//bit32 and above is not defined.
#ifndef BIT
#define BIT(_I)   (RT_U32)(1<<(_I))
#endif

#define BIT0		0x00000001
#define BIT1		0x00000002
#define BIT2		0x00000004
#define BIT3		0x00000008
#define BIT4		0x00000010
#define BIT5		0x00000020
#define BIT6		0x00000040
#define BIT7		0x00000080
#define BIT8		0x00000100
#define BIT9		0x00000200
#define BIT10	0x00000400
#define BIT11	0x00000800
#define BIT12	0x00001000
#define BIT13	0x00002000
#define BIT14	0x00004000
#define BIT15	0x00008000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000

#define RTK_LE_DLL							BIT7
#define RTK_LE_PROFILE_FIND_ME				BIT8	
#define RTK_LE_PROFILE_PROXIMITY			BIT9
#define RTK_LE_PROFILE_HTP					BIT10
#define RTK_LE_PROFILE_AN					BIT11	
#define RTK_LE_SERVICE_BATTERY				BIT12	
#define RTK_LE_PROFILE_NETWORK_AVAIABILITY	BIT13
#define RTK_LE_PROFILE_PHONE_ALERT_STATUS	BIT14
#define RTK_LE_PROFILE_HEART_RATE			BIT15	
#define RTK_LE_PROFILE_BLOOD_PRESSURE		BIT16
#define RTK_LE_PROFILE_HID					BIT17
#define RTK_LE_SERVICE_DEVICE_INFORMATION	BIT18
#define RTK_LE_PROFILE_GLUCOSE              BIT19
#define RTK_LE_PROFILE_RSC					BIT20
#define RTK_LE_PROFILE_CSC					BIT21
#define RTK_LE_PROFILE_DFU					BIT22
#define RTK_LE_PROFILE_RCU					BIT23

#define TRACE_LEVEL_NONE        0   // Tracing is not on
#define TRACE_LEVEL_CRITICAL    1   // Abnormal exit or termination
#define TRACE_LEVEL_FATAL       1   // Deprecated name for Abnormal exit or termination
#define TRACE_LEVEL_ERROR       2   // Severe errors that need logging
#define TRACE_LEVEL_WARNING     3   // Warnings such as allocation failure
#define TRACE_LEVEL_INFORMATION 4   // Includes non-error cases(e.g.,Entry-Exit)
#define TRACE_LEVEL_VERBOSE     5   // Detailed traces from intermediate steps
#define TRACE_LEVEL_RESERVED6   6
#define TRACE_LEVEL_RESERVED7   7
#define TRACE_LEVEL_RESERVED8   8
#define TRACE_LEVEL_RESERVED9   9

extern TCHAR  gDebugOutputBuffer[2048];

extern unsigned int gDebugComponent;    
extern unsigned int gDebugLevel;

#define TRACE_LEVEL_NUM 10
extern TCHAR* StringLevel[TRACE_LEVEL_NUM];
void DebugStringFormat(TCHAR* pFormat, ... );
void OutputDebugStringFormat(TCHAR* pFormat, ... );
void DbgUpdateParameter(unsigned int DbgLevel, unsigned int DbgComp);
BOOL ReadDebugComponentValueFromReg(DWORD *value);

#define DEBUG_TRACE(_Level, _Flags, _Fmt)	  \
	do{								  \
	if (((_Flags) & gDebugComponent) && (_Level >= gDebugLevel)) { \
		DebugStringFormat ##_Fmt; \
		OutputDebugStringFormat((_T("\n%s: (%hs(), %d):    %s")), StringLevel[_Level],__FUNCTION__, __LINE__,gDebugOutputBuffer);        \
		} \
	} while(0)


#define DEBUG_DATA(_Level, _DATA, _LENGTH)   \
		do{         \
			{\
				UINT32 _i = 0, _LENGTH_TO_PRINT = _LENGTH; \
				OutputDebugStringFormat(_T("\n%hs(%hs(), %d):    Length:0x%0x : "), __TIME__, __FUNCTION__, __LINE__, _LENGTH);\
				for (;_i<(UINT32)(_LENGTH_TO_PRINT); _i++)       \
				{\
					OutputDebugStringFormat(_T("0x%02x "), (((UINT8*)(_DATA))[_i]));\
				}\
			}\
		}while(0)	

#endif