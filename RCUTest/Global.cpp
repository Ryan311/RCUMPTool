#include "stdafx.h"
#include <atlbase.h>
#include <Setupapi.h>
#include <Strsafe.h>
#include <Dbghelp.h>
#include "Global.h"

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "Version.lib")

BOOL GetAppVersion( TCHAR *LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber )
{ 
	DWORD dwHandle, dwLen; 
	UINT BufLen; 
	LPTSTR lpData;
	VS_FIXEDFILEINFO *pFileInfo; 
	dwLen = GetFileVersionInfoSize( LibName, &dwHandle ); 
	if (!dwLen)   
		return FALSE; 
	lpData = (LPTSTR) malloc (dwLen); 
	if (!lpData)  
		return FALSE; 
	if( !GetFileVersionInfo( LibName, dwHandle, dwLen, lpData ) ) 
	{ 
		free (lpData); 
		return FALSE; 
	} 
	if( VerQueryValue( lpData, L"\\", (LPVOID *) &pFileInfo, (PUINT)&BufLen ) )  
	{  
		*MajorVersion = HIWORD(pFileInfo->dwFileVersionMS);  
		*MinorVersion = LOWORD(pFileInfo->dwFileVersionMS);  
		*BuildNumber = HIWORD(pFileInfo->dwFileVersionLS); 
		*RevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);  

		free (lpData);
		return TRUE;
	} 
	free (lpData); 
	return FALSE;
}

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionPointers) 
{ 
	SetErrorMode( SEM_NOGPFAULTERRORBOX ); 
	CString strBuild; 
	strBuild.Format(_T("Build: %s %s"), __DATE__, __TIME__); 
	CString strError; 
	HMODULE hModule; 
	TCHAR szModuleName[MAX_PATH] = _T(""); 
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)pExceptionPointers->ExceptionRecord->ExceptionAddress, &hModule); 
	GetModuleFileName(hModule, szModuleName, ARRAYSIZE(szModuleName)); 
	strError.AppendFormat(_T("%s 0x%x , 0x%x , 0x%x."), szModuleName,pExceptionPointers->ExceptionRecord->ExceptionCode, pExceptionPointers->ExceptionRecord->ExceptionFlags, pExceptionPointers->ExceptionRecord->ExceptionAddress); 

	WORD MajorVersion = 0; 
	WORD MinorVersion = 0; 
	WORD BuildNumber = 0; 
	WORD RevisionNumber = 0;

	TCHAR* szAppName = _tcsrchr(szModuleName, '\\');
	szAppName++;

	GetAppVersion(szModuleName, &MajorVersion, &MinorVersion, &BuildNumber, &RevisionNumber);

	CString strVersion;
	strVersion.Format (_T("v%hu.%hu.%hu.%hu"), MajorVersion, MinorVersion, BuildNumber, RevisionNumber);

	//Éú³É mini crash dump 
	BOOL bMiniDumpSuccessful; 
	TCHAR szPath[MAX_PATH]; 
	TCHAR szFileName[MAX_PATH]; 
	DWORD dwBufferSize = MAX_PATH; 
	HANDLE hDumpFile; 
	SYSTEMTIME stLocalTime; 
	MINIDUMP_EXCEPTION_INFORMATION ExpParam; 
	GetLocalTime( &stLocalTime ); 
	GetTempPath( dwBufferSize, szPath ); 
	StringCchPrintf( szFileName, MAX_PATH, _T("%s%s"), szPath,  szAppName ); 
	CreateDirectory( szFileName, NULL ); 
	StringCchPrintf( szFileName, MAX_PATH, _T("%s%s\\%s-%04d%02d%02d-%02d%02d%02d-P%ld-T%ld.dmp"), 
		szPath, szAppName, strVersion, 
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
		GetCurrentProcessId(), GetCurrentThreadId()); 
	hDumpFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0); 

	MINIDUMP_USER_STREAM UserStream[2]; 
	MINIDUMP_USER_STREAM_INFORMATION UserInfo; 
	UserInfo.UserStreamCount = 1; 
	UserInfo.UserStreamArray = UserStream; 
	UserStream[0].Type = CommentStreamW; 
	UserStream[0].BufferSize = strBuild.GetLength()*sizeof(TCHAR); 
	UserStream[0].Buffer = strBuild.GetBuffer(); 
	UserStream[1].Type = CommentStreamW; 
	UserStream[1].BufferSize = strError.GetLength()*sizeof(TCHAR); 
	UserStream[1].Buffer = strError.GetBuffer(); 

	ExpParam.ThreadId = GetCurrentThreadId(); 
	ExpParam.ExceptionPointers = pExceptionPointers; 
	ExpParam.ClientPointers = TRUE; 

	MINIDUMP_TYPE MiniDumpWithDataSegs = (MINIDUMP_TYPE)(MiniDumpNormal
		| MiniDumpWithHandleData 
		| MiniDumpWithUnloadedModules 
		| MiniDumpWithIndirectlyReferencedMemory 
		| MiniDumpScanMemory 
		| MiniDumpWithProcessThreadData 
		| MiniDumpWithThreadInfo); 
	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
		hDumpFile, 
		MiniDumpWithDataSegs,
		&ExpParam, NULL, NULL); 

	return EXCEPTION_CONTINUE_SEARCH;
} 
