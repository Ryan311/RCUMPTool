#pragma  once

BOOL GetAppVersion( TCHAR *LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber );
LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionPointers) ;