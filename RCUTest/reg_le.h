#include "rt_type.h"

#pragma  once

//Configuration Key name
#define BT_LE_CONFIGURATION_PATH_KEY_NAME	 _T("SOFTWARE\\Realtek\\Bluetooth\\LE")

//reg in HCUR
//Indications
BOOL RegSetIndicationOn(ULONGLONG RemoteAddr,RT_U16 ServiceUUid, RT_U16 Handle);

BOOL RegSetIndicationOff(ULONGLONG addr, RT_U16 ServiceUUid);

BOOL RegIsIndicationOn(ULONGLONG RemoteAddr, RT_U16 ServiceUUid);

BOOL RegGetIndicationValue(ULONGLONG RemoteAddr, RT_U16 ServiceUUid, RT_U16 &Handle);

//Notifications
BOOL RegSetNotificationOn(ULONGLONG RemoteAddr,RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 Length);

BOOL RegGetNotificationValue(ULONGLONG RemoteAddr, RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 &Length);


//Notifications in HKLM
BOOL RegServiceSetNotificationOn(ULONGLONG RemoteAddr,RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 Length);

BOOL RegServiceGetNotificationValue(ULONGLONG RemoteAddr, RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 &Length);

BOOL RegRtkBleServiceRemoveDeviceConfiguration(ULONGLONG RemoteAddr);

BOOL LeIsSmpKeyMissing( ULONGLONG ullAddr);