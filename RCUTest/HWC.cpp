
// HWC.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "Global.h"
#include "HWC.h"
#include "HWCDlg.h"

#include <bthsdpdef.h>
#include <BluetoothAPIs.h>
#pragma comment(lib, "Bthprops.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHWCApp
BEGIN_MESSAGE_MAP(CHWCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CHWCApp construction

CHWCApp::CHWCApp()
{
	// support Restart Manager
	// m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CHWCApp object

CHWCApp theApp;


// CHWCApp initialization

BOOL CHWCApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	// Bluetooth is pluged in ???
	HANDLE hRadio = NULL;
	BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(btfrp) };
	HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&btfrp, &hRadio);
	if (NULL == hFind || hRadio == NULL)
	{
		CString str, strTitle;
		str.LoadString(IDS_NO_BTH_RADIO);
		strTitle.LoadString(IDS_TITLE);
		::MessageBox(NULL, str, strTitle, MB_OK | MB_ICONERROR);
		BluetoothFindRadioClose(hFind);
		CloseHandle(hRadio);
		return FALSE;
	}
	BluetoothFindRadioClose(hFind);
	CloseHandle(hRadio);

	// Filter Driver Exist???
	CString RtkFilterDevPath = L"\\\\.\\BtFilter\\RCUTool";
	HANDLE hBtDeviceHandle = CreateFile(
		RtkFilterDevPath,  
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if(hBtDeviceHandle == INVALID_HANDLE_VALUE)
	{// Filter Driver not exist
		CString str, strTitle;
		str.LoadString(IDS_NO_RTK_FLTDRIVER);
		strTitle.LoadString(IDS_TITLE);
		::MessageBox(NULL, str, strTitle, MB_OK | MB_ICONERROR);
		return FALSE;
	}
	CloseHandle(hBtDeviceHandle);

	CHWCDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CHWCApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}
