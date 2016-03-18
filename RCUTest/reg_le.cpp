#include "stdafx.h"
#include <atlbase.h>
#include "reg_le.h"

#ifdef WPP_TRACING_ENABLE
#include "reg_le.tmh"
#endif


BOOL RegSetIndicationOn(ULONGLONG RemoteAddr,RT_U16 ServiceUUid, RT_U16 Handle)
{
	CRegKey key;

	CString strIndicationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Indication"),RemoteAddr);

	CString strIndicationRemoteAddress = strIndicationRoot+strRemoteAddr;
	LONG ret = key.Create(HKEY_CURRENT_USER, strIndicationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUuid;
	strServiceUuid.Format(_T("%x"),ServiceUUid);

	ULONG nBytes = sizeof(Handle);

	ret = key.SetBinaryValue(strServiceUuid.GetBuffer(),&Handle,nBytes);

	if (ret != ERROR_SUCCESS)
	{
		key.Close();
		return FALSE;
	}

	return TRUE;
}
BOOL RegSetIndicationOff(ULONGLONG RemoteAddr,RT_U16 ServiceUUid)
{
	CRegKey key;

	CString strIndicationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Indication"),RemoteAddr);

	CString strIndicationRemoteAddress = strIndicationRoot+strRemoteAddr;
	LONG ret = key.Open(HKEY_CURRENT_USER, strIndicationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUuid;
	strServiceUuid.Format(_T("%x"),ServiceUUid);

	ret = key.DeleteSubKey(strServiceUuid);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}
	return TRUE;
}


BOOL RegIsIndicationOn(ULONGLONG RemoteAddr, RT_U16 ServiceUUid)
{
	RT_U16 Handle = 0;
	return RegGetIndicationValue(RemoteAddr, ServiceUUid, Handle);
}

BOOL RegGetIndicationValue(ULONGLONG RemoteAddr, RT_U16 ServiceUUid, RT_U16 &Handle)
{
	CRegKey key;

	CString strNotificationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Indication"),RemoteAddr);

	CString strNotificationRemoteAddress = strNotificationRoot+strRemoteAddr;
	LONG ret = key.Open(HKEY_CURRENT_USER, strNotificationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUUid;
	ULONG length = 2;
	strServiceUUid.Format(_T("%x"),ServiceUUid);

	ret = key.QueryBinaryValue(strServiceUUid, &Handle,&length);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	return TRUE;

}


BOOL RegSetNotificationOn(ULONGLONG RemoteAddr,RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 Length)
{
	CRegKey key;

	CString strNotificationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Notification"),RemoteAddr);

	CString strNotificationRemoteAddress = strNotificationRoot+strRemoteAddr;
	LONG ret = key.Create(HKEY_CURRENT_USER, strNotificationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUuid;
	strServiceUuid.Format(_T("%x"),ServiceUUid);

	ret = key.SetBinaryValue(strServiceUuid.GetBuffer(),pNotificationData,Length);

	if (ret != ERROR_SUCCESS)
	{
		key.Close();
		return FALSE;
	}

	return TRUE;
}
BOOL RegSetNotificationOff(ULONGLONG RemoteAddr,RT_U16 ServiceUUid)
{
	CRegKey key;

	CString strNotificationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Notification"),RemoteAddr);

	CString strNotificationRemoteAddress = strNotificationRoot+strRemoteAddr;
	LONG ret = key.Open(HKEY_CURRENT_USER, strNotificationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUuid;
	strServiceUuid.Format(_T("%x"),ServiceUUid);

	ret = key.DeleteSubKey(strServiceUuid);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}
	return TRUE;
}



BOOL RegGetNotificationValue(ULONGLONG RemoteAddr, RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 &Length)
{
	CRegKey key;

	CString strNotificationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Notification"),RemoteAddr);

	CString strNotificationRemoteAddress = strNotificationRoot+strRemoteAddr;
	LONG ret = key.Open(HKEY_CURRENT_USER, strNotificationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUUid;

	strServiceUUid.Format(_T("%x"),ServiceUUid);

	ret = key.QueryBinaryValue(strServiceUUid.GetBuffer(), pNotificationData,&Length);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	return TRUE;

}

BOOL RegServiceSetNotificationOn(ULONGLONG RemoteAddr,RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 Length)
{
	CRegKey key;

	CString strNotificationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Notification"),RemoteAddr);

	CString strNotificationRemoteAddress = strNotificationRoot+strRemoteAddr;
	LONG ret = key.Create(HKEY_LOCAL_MACHINE, strNotificationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUuid;
	strServiceUuid.Format(_T("%x"),ServiceUUid);

	ret = key.SetBinaryValue(strServiceUuid.GetBuffer(),pNotificationData,Length);

	if (ret != ERROR_SUCCESS)
	{
		key.Close();
		return FALSE;
	}
	return TRUE;
}
BOOL RegServiceSetNotificationOff(ULONGLONG RemoteAddr,RT_U16 ServiceUUid)
{
	CRegKey key;

	CString strNotificationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Notification"),RemoteAddr);

	CString strNotificationRemoteAddress = strNotificationRoot+strRemoteAddr;
	LONG ret = key.Open(HKEY_LOCAL_MACHINE, strNotificationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUuid;
	strServiceUuid.Format(_T("%x"),ServiceUUid);

	ret = key.DeleteSubKey(strServiceUuid);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}
	return TRUE;
}



BOOL RegServiceGetNotificationValue(ULONGLONG RemoteAddr, RT_U16 ServiceUUid, RT_U8 *pNotificationData, RT_U32 &Length)
{
	CRegKey key;

	CString strNotificationRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("\\%I64x\\Notification"),RemoteAddr);

	CString strNotificationRemoteAddress = strNotificationRoot+strRemoteAddr;
	LONG ret = key.Open(HKEY_LOCAL_MACHINE, strNotificationRemoteAddress);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	CString strServiceUUid;

	strServiceUUid.Format(_T("%x"),ServiceUUid);

	ret = key.QueryBinaryValue(strServiceUUid.GetBuffer(), pNotificationData,&Length);
	if (ret != ERROR_SUCCESS) 
	{
		key.Close();
		return FALSE;
	}

	return TRUE;

}


BOOL RegRtkBleServiceRemoveDeviceConfiguration(ULONGLONG RemoteAddr)
{
	BOOL bRetRegDelete = FALSE;
	CRegKey key;

	CString strLeDeviceRoot = BT_LE_CONFIGURATION_PATH_KEY_NAME;
	CString strRemoteAddr;
	strRemoteAddr.Format(_T("%I64x"),RemoteAddr);


	LONG ret = key.Open(HKEY_LOCAL_MACHINE, strLeDeviceRoot);
	if (ret != ERROR_SUCCESS) 
	{
		bRetRegDelete= FALSE;
		goto TagEND;
	}

	ret = key.RecurseDeleteKey(strRemoteAddr);
	if (ret != ERROR_SUCCESS) 
	{
		bRetRegDelete= FALSE;
		goto TagEND;
	}
	else
	{
		bRetRegDelete = TRUE;
	}

TagEND:

	key.Close();
	return bRetRegDelete;
}


//BOOL LeIsSmpKeyMissing( ULONGLONG ullAddr)
//{
//	BOOL bRet = FALSE;
//	CString strKey;
//	strKey.Format(_T("%s\\%I64x"), LE_DATABASE_KEY, ullAddr);
//	CRegKey key;
//	LONG ret = key.Open(HKEY_LOCAL_MACHINE, strKey, KEY_READ); 
//	if (ret != ERROR_SUCCESS)
//	{		
//		bRet = TRUE;
//		goto TagEND;
//	}
//	DWORD dwValue = 0;
//	ret = key.QueryDWORDValue(L"KeyMissing", dwValue);
//	if (ret != ERROR_SUCCESS)
//	{
//		bRet = FALSE;
//	}
//	else
//	{
//		bRet = TRUE;	
//	}
//
//TagEND:
//
//	key.Close();
//
//	return bRet;
//}
//
