
// HWCDlg.cpp : implementation file
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;

#include "sbc.h"
#include "HWC.h"
#include "LEScanWinDlg.h"
#include "afxdialogex.h"
#include "GattPublic.h"
#include "ConfigDlg.h"
#include "ConfigFile.h"
#include "trace.h"
#include "aes.h"
#include "AudioFunc.h"
#include "WaveDoc.h"
#include "DisplayView.h"
#include "HWCDlg.h"

extern "C"{
#include "LEGap.h"
}
#pragma comment(lib, "LEGap.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About
unsigned long long BDAddress = 0;
CString BDAddrStr;
CString BTDevNameStr;

// Globel work dir
CString g_ModuleDir;
CString g_WorkDir;
unsigned char g_alertLevel = 1;
bool g_bVoice = TRUE;
unsigned char MaskButtonState[18] = {0};
unsigned int ButtonCheckDown = 0;
bool g_bMicDown = false;
unsigned char RSSI;
BOOLEAN g_bModeSel = FALSE;


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHWCDlg dialog
CHWCDlg* CHWCDlg::pThis = NULL;

CHWCDlg::CHWCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHWCDlg::IDD, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CHWCDlg::pThis = this;
	m_hScanEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_rightDev = FALSE;
}

CHWCDlg::~CHWCDlg()
{
	SaveDefaultConfig();
	SetEvent(m_hScanEvent);
	DeleteCriticalSection(&m_waveCriticalSection);
	freeBlocks(m_waveBlocks);
}

void CHWCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_Power);
	DDX_Control(pDX, IDC_BUTTON2, m_Home);
	DDX_Control(pDX, IDC_BUTTON9, m_Mic);
	DDX_Control(pDX, IDC_BUTTON11, m_VolDown);
	DDX_Control(pDX, IDC_BUTTON12, m_VolUp);
	DDX_Control(pDX, IDC_BUTTON6, m_Up);
	DDX_Control(pDX, IDC_BUTTON7, m_Left);
	DDX_Control(pDX, IDC_BUTTON5, m_Right);
	DDX_Control(pDX, IDC_BUTTON4, m_Down);
	DDX_Control(pDX, IDC_BUTTON3, m_OK);
	DDX_Control(pDX, IDC_BUTTON8, m_Back);
	DDX_Control(pDX, IDC_BUTTON10, m_Menu);
	DDX_Control(pDX, IDC_SHOW, m_static);
	DDX_Control(pDX, IDC_AUTOTEST_START, m_Start);
	DDX_Control(pDX, IDC_SCAN, m_ScanState);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_ButtonShow);
	DDX_Control(pDX, IDC_BUTTON_STATE, m_ButtonState);
	DDX_Control(pDX, IDC_BUTTON_RCU_CONNECT, m_RCUConn);
	DDX_Control(pDX, IDC_STOP, m_DevDisconn);
	DDX_Control(pDX, IDC_PROGRESS_OTA, m_OTAProcess);
	DDX_Control(pDX, IDC_MP_MODE_AUTO_CONNECT, m_MPConnect);
	DDX_Control(pDX, IDC_EDIT_MP_LINKKEY, m_MPLinkKeyEdit);
	DDX_Control(pDX, IDC_EDIT_MP_ADDR, m_MPBDAddressEdit);
	DDX_Control(pDX, IDC_BUTTON_ENABLE_OTA_MODE, m_UpdateButton);
	DDX_Control(pDX, IDC_CLEAR_LOG, m_LogClear);
	DDX_Control(pDX, IDC_BUTTON_CONN_STATE, m_MPModeSelect);
	DDX_Control(pDX, IDC_BUTTON_FIINDME, m_AlertButton);
	DDX_Control(pDX, IDC_BUTTON_VOICERECORD, m_VoiceButton);
	DDX_Control(pDX, IDC_WAVE_SHOW, m_WaveShow);
	DDX_Control(pDX, IDC_EDIT_DEV_RSSI, m_EditRSSI);
}

BEGIN_MESSAGE_MAP(CHWCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_MESSAGE(WM_BUTTON_STATE, &CHWCDlg::OnCustomButtonState)
	ON_MESSAGE(WM_BUTTON_STATE, &CHWCDlg::OnCustomButtonState2)
	ON_MESSAGE(WM_BUTTON_RAWDATA, &CHWCDlg::OnCustomNotifyRawData)
	ON_MESSAGE(WM_CUSTOM_POWERDOWN, &CHWCDlg::OnCustomPowerdown)
	ON_MESSAGE(WM_CUSTOM_HOMEDOWN, &CHWCDlg::OnCustomHomedown)
	ON_MESSAGE(WM_CUSTOM_UPDOWN, &CHWCDlg::OnCustomeUpdown)
	ON_MESSAGE(WM_CUSTOM_DOWNDOWN, &CHWCDlg::OnCustomeDowndown)
	ON_MESSAGE(WM_CUSTOM_LEFTDOWN, &CHWCDlg::OnCustomeLeftdown)
	ON_MESSAGE(WM_CUSTOM_RIGHTDOWN, &CHWCDlg::OnCustomeRightdown)
	ON_MESSAGE(WM_CUSTOM_OKDOWN, &CHWCDlg::OnCustomeOkdown)
	ON_MESSAGE(WM_CUSTOM_BACKDOWN, &CHWCDlg::OnCustomeBackdown)
	ON_MESSAGE(WM_CUSTOM_VOICEDOWN, &CHWCDlg::OnCustomeMicdown)
	ON_MESSAGE(WM_CUSTOM_MENUDOWN, &CHWCDlg::OnCustomeMenudown)
	ON_MESSAGE(WM_CUSTOM_VOLDOWNDOWN, &CHWCDlg::OnCustomeVoldowndown)
	ON_MESSAGE(WM_CUSTOM_VOLUPDOWN, &CHWCDlg::OnCustomeVolupdown)
	ON_MESSAGE(WM_ALLUP, &CHWCDlg::OnAllup)
	ON_MESSAGE(WM_VOICE_DATA, &CHWCDlg::OnVoiceDataStore)
	ON_MESSAGE(WM_LEDEV_CONNECT, &CHWCDlg::ConnectAndGetInfo)
	ON_MESSAGE(WM_LEDEV_DISCONNECT, &CHWCDlg::DisconnectDevLink)
	ON_MESSAGE(WM_PLAY_RECORD, &CHWCDlg::PlayVoiceRecord)
	ON_MESSAGE(WM_ALL_BUTTON_ACTIVE, &CHWCDlg::OnAllButtonActive)
	ON_MESSAGE(WM_OTA_DOWNLOAD_PROCESS, &CHWCDlg::OnOTAPatchDownloadProcess)
	ON_BN_CLICKED(IDC_SCAN, &CHWCDlg::OnBnClickedScan)
	ON_BN_CLICKED(IDC_AUTOTEST_START, &CHWCDlg::OnBnClickedAutotestStart)
	ON_BN_CLICKED(IDC_STOP, &CHWCDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_CONFIG, &CHWCDlg::OnBnClickedConfig)
	ON_BN_CLICKED(IDC_BUTTON_REPLAY, &CHWCDlg::OnBnClickedButtonReplay)
	ON_BN_CLICKED(IDC_BUTTON_RCU_CONNECT, &CHWCDlg::OnBnClickedButtonRcuConnect)
	ON_BN_CLICKED(IDC_BUTTON_LOADPATCH, &CHWCDlg::OnBnClickedButtonLoadpatch)
	ON_BN_CLICKED(IDC_OPEN_CONFIG_PATH, &CHWCDlg::OnBnClickedOpenConfigPath)
	ON_BN_CLICKED(IDC_BUTTON_ENABLE_OTA_MODE, &CHWCDlg::OnBnClickedButtonEnableOtaMode)
	ON_BN_CLICKED(IDC_MP_MODE_AUTO_CONNECT, &CHWCDlg::OnBnClickedMpModeAutoConnect)
	ON_BN_CLICKED(IDC_CLEAR_LOG, &CHWCDlg::OnBnClickedClearLog)
	ON_BN_CLICKED(IDC_CLEAR_QUIT_MP, &CHWCDlg::OnBnClickedClearQuitMp)
	ON_BN_CLICKED(IDC_BUTTON_FIINDME, &CHWCDlg::OnBnClickedButtonFindme)
	ON_BN_CLICKED(IDC_BUTTON_VOICERECORD, &CHWCDlg::OnBnClickedButtonVoicerecord)
	ON_BN_CLICKED(IDC_BUTTON_ADDR1, &CHWCDlg::OnBnClickedButtonAddr1)
	ON_BN_CLICKED(IDC_BUTTON_ADDR2, &CHWCDlg::OnBnClickedButtonAddr2)
	ON_BN_CLICKED(IDC_BUTTON_ADDR3, &CHWCDlg::OnBnClickedButtonAddr3)
	ON_BN_CLICKED(IDC_BUTTON_ADDR4, &CHWCDlg::OnBnClickedButtonAddr4)
	ON_BN_CLICKED(IDC_BUTTON_ADDR5, &CHWCDlg::OnBnClickedButtonAddr5)
END_MESSAGE_MAP()

// CHWCDlg message handlers

BOOL CHWCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_DeviceSelected = FALSE;

	// TODO: Add extra initialization here
	m_Power.SetSkin(IDB_POWERUP,IDB_POWERDOWN,0,0,0,IDB_MASK,0,0,0);
	m_Home.SetSkin(IDB_HOMEUP,IDB_HOMEDOWN,0,0,0,IDB_MASK,0,0,0);
	m_Mic.SetSkin(IDB_MICUP,IDB_MICDOWN,0,0,0,IDB_MASK,0,0,0);
	m_VolDown.SetSkin(IDB_VOLDOWNUP,IDB_VOLDOWNDOWN,0,0,0,IDB_MASK,0,0,0);
	m_VolUp.SetSkin(IDB_VOLUPUP,IDB_VOLUPDOWN,0,0,0,IDB_MASK,0,0,0);
	m_Left.SetSkin(IDB_LeftUp,IDB_LeftDown,0,0,0,IDB_MASK,0,0,0);
	m_Right.SetSkin(IDB_RightUp,IDB_RightDown,0,0,0,IDB_MASK,0,0,0);
	m_Up.SetSkin(IDB_UpUp,IDB_UpDown,0,0,0,IDB_MASK,0,0,0);
	m_Down.SetSkin(IDB_DownUp,IDB_DownDown,0,0,0,IDB_MASK,0,0,0);
	m_OK.SetSkin(IDB_OKUp,IDB_OKDown,0,0,0,IDB_MASK,0,0,0);
	m_Back.SetSkin(IDB_BackUp,IDB_BackDown,0,0,0,IDB_MASK,0,0,0);
	m_Menu.SetSkin(IDB_MenuUp,IDB_MenuDown,0,0,0,IDB_MASK,0,0,0);
	
	// Init Button Style
	InitButtonStyle();

	//Save Module Dir
	GetModuleFileName(NULL,g_ModuleDir.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	g_ModuleDir.ReleaseBuffer();
	int nPos;
	nPos=g_ModuleDir.ReverseFind(L'\\');
	g_ModuleDir=g_ModuleDir.Left(nPos);

	//Save Working Dir, create if not exist
	TCHAR szDocument[MAX_PATH]={0};
	LPITEMIDLIST pidl=NULL;
	SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl);
	if (pidl && SHGetPathFromIDList(pidl, szDocument))
	{
		g_WorkDir = szDocument;
	} 
	g_WorkDir += RCU_VOICE_DIR;
	CFileFind fileFind;
	if (!fileFind.FindFile(g_WorkDir))
	{
		if(!CreateDirectory(g_WorkDir, NULL))  
		{  
			MessageBox(L"Cannot Create Work directory");  
		}   
	}

	// get all supported Voice Decoders;
	LoadSupportedDecoders();
	
	// Load Key Map from Configuration file or using default map
	if(!LoadDefaultConfig())
	{
		m_Keymap[POWERKEY]	= MAP_POWERKEY;
		m_Keymap[MICKEY]	= MAP_MICKEY;
		m_Keymap[UPKEY]		= MAP_UPKEY;
		m_Keymap[DOWNKEY]	= MAP_DOWNKEY;
		m_Keymap[RIGHTKEY]	= MAP_RIGHTKEY;
		m_Keymap[LEFTKEY]	= MAP_LEFTKEY;
		m_Keymap[HOMEKEY]	= MAP_HOMEKEY;
		m_Keymap[MENUKEY]	= MAP_MENUKEY;
		m_Keymap[OKKEY]		= MAP_OKKEY;
		m_Keymap[VOLDOWN]	= MAP_VOLDOWN;
		m_Keymap[VOLUP]		= MAP_VOLUP;
		m_Keymap[BACKKEY]	= MAP_BACKKEY;
		m_Keymap[ALLKEYUP]	= MAP_ALLKEYUP;

		// Reverse Key Map
		m_KeymapR.clear();
		m_KeymapR[MAP_POWERKEY]	= POWERKEY;
		m_KeymapR[MAP_MICKEY]	= MICKEY;
		m_KeymapR[MAP_UPKEY]	= UPKEY;
		m_KeymapR[MAP_DOWNKEY]	= DOWNKEY;
		m_KeymapR[MAP_RIGHTKEY]	= RIGHTKEY;
		m_KeymapR[MAP_LEFTKEY]	= LEFTKEY;
		m_KeymapR[MAP_HOMEKEY]	= HOMEKEY;
		m_KeymapR[MAP_MENUKEY]	= MENUKEY;
		m_KeymapR[MAP_OKKEY]	= OKKEY;
		m_KeymapR[MAP_VOLDOWN]	= VOLDOWN;
		m_KeymapR[MAP_VOLUP]	= VOLUP;
		m_KeymapR[MAP_BACKKEY]	= BACKKEY;
		m_KeymapR[MAP_ALLKEYUP]	= ALLKEYUP;

		// Default Voice Decoder
		m_DecoderSelected = CString(VOICE_DECODER_MSBC);
	}

	//if(m_DecoderSelected.Find(L"mSBC") == -1)
	{// load the selected decoder from the dll
		CString VoiceDecoder;
		VoiceDecoder = g_ModuleDir + _T("\\Decoder\\") + m_DecoderSelected + _T(".dll");
		if(!LoadDecoder(VoiceDecoder))
		{// Fail, load default NULL decoder, do nothing
			m_DecoderSelected = CString(VOICE_DECODER_NULL);
			m_DecoderFunc = NULL_AudioDecoder;
			m_PlayFunc = NULL_AudioPlay;
			ShowEditPrintInfo(CString(L"No voice decoder selected"), 0);
			m_VoicePlayMode = 0;	// only save the raw data
		}
	}

	// Load YaoKongQi Picture and Button Mask
	YKQPictureAndMaskLoad();	

	// for direct voice play
	InitializeCriticalSection(&m_waveCriticalSection);

	m_pView = NULL;
	AddRootView();

	m_AdvTypeR[CString(L"ADV_IND")] = ADV_IND;
	m_AdvTypeR[CString(L"ADV_DIRECT_IND")] = ADV_DIRECT_IND;
	m_AdvTypeR[CString(L"ADV_SCAN_IND")] = ADV_SCAN_IND;
	m_AdvTypeR[CString(L"ADV_NONCONN_IND")] = ADV_NONCONN_IND;
	m_OTAProcess.EnableWindow(FALSE);
	m_waveBlocks = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);
	m_ScanStr.Format(_T("Scan LE Device ......\n\n"));
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHWCDlg::InitButtonStyle()
{
	// Config State Button 
	CFont* pbf=m_ButtonState.GetFont();
	LOGFONT lf0;
	pbf->GetLogFont(&lf0);
	lf0.lfHeight = 36;
	m_ButtonStateFont.CreateFontIndirect(&lf0);
	m_ButtonState.SetFont(&m_ButtonStateFont);
	m_ButtonState.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(208,208,208));
	m_ButtonState.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255,0,0));
	m_ButtonState.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(208,208,208));
	m_ButtonState.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(208,208,208));
	m_ButtonState.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(208,208,208));
	m_ButtonState.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255,0,0));

	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));

	SetSTButtonColor(255,0,0);

	//Config Static Button
	CFont* ptf=m_static.GetFont();
	LOGFONT lf1;
	ptf->GetLogFont(&lf1);
	lf1.lfHeight = 15;
	m_editFont.CreateFontIndirect(&lf1);
	m_static.SetFont(&m_editFont);

	//Config Show Edit Button 
	ptf=m_ButtonShow.GetFont();
	LOGFONT lf2;
	memset(&lf2,0,sizeof(LOGFONT));
	ptf->GetLogFont(&lf2);
	lf2.lfHeight = 11;
	wcscpy_s(lf2.lfFaceName, _countof(lf2.lfFaceName), L"Courier");
	m_ButtonShowFont.CreateFontIndirect(&lf2);
	m_ButtonShow.SetFont(&m_ButtonShowFont);

	//Config Start Button 
	ptf=m_Start.GetFont();
	LOGFONT lf3;
	ptf->GetLogFont(&lf3);
	lf3.lfHeight = 30;
	m_StartButtonFont.CreateFontIndirect(&lf3);
	m_Start.SetFont(&m_StartButtonFont);

	//Config Alert Button
	ptf=m_AlertButton.GetFont();
	LOGFONT lf4;
	ptf->GetLogFont(&lf4);
	lf4.lfHeight = 25;
	m_AlertButtonFont.CreateFontIndirect(&lf4);
	m_AlertButton.SetFont(&m_AlertButtonFont);

	//Config Voice Button
	ptf=m_VoiceButton.GetFont();
	LOGFONT lf5;
	ptf->GetLogFont(&lf5);
	lf5.lfHeight = 25;
	m_VoiceButtonFont.CreateFontIndirect(&lf5);
	m_VoiceButton.SetFont(&m_VoiceButtonFont);

	//Config MP Conn Button
	ptf=m_MPModeSelect.GetFont();
	LOGFONT lf6;
	ptf->GetLogFont(&lf6);
	lf6.lfHeight = 25;
	m_MPModeFont.CreateFontIndirect(&lf6);
	m_MPModeSelect.SetFont(&m_MPModeFont);

	//Config Log Clear Button
	ptf=m_LogClear.GetFont();
	LOGFONT lf7;
	ptf->GetLogFont(&lf7);
	lf7.lfHeight = 25;
	m_LogClearFont.CreateFontIndirect(&lf7);
	m_LogClear.SetFont(&m_LogClearFont);

}

#define PICTURE_YKQ_HIGHT 714
#define PICTURE_YKQ_LENGTH 313

#define PICTURE_PASS_HIGHT 158
#define PICTURE_PASS_LENGTH 130

#define BUTTON_MASK_HIGHT_BIG 50
#define BUTTON_MASK_LENGTH_BIG 50

#define BUTTON_MASK_HIGHT_SMALL 45
#define BUTTON_MASK_LENGTH_SMALL 45


int CHWCDlg::YKQPictureAndMaskLoad()
{
	{// Yao Kong Qi Picture as Background
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_BACKGROUND); // 得到 Picture Control 句柄
		//CImage image;
		//image.Load(_T("res/YKQ.bmp"));
		//HBITMAP hBmp = image.Detach();
		HBITMAP hBitmap;
		hBitmap = ::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_HAIXIN_YKQ)); 
		pPic->SetBitmap(hBitmap);
		pPic->SetWindowPos(NULL, 0, 0, PICTURE_YKQ_LENGTH, PICTURE_YKQ_HIGHT, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}

	{// Pass Picture
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_PASS); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/Pass.jpg"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, PICTURE_PASS_HIGHT, PICTURE_PASS_LENGTH, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}

	{// Button 1
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B1); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B19); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}

	{// Button 2
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B2); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B20); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 3
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B3); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B21); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 4
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B4); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B22); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 5
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B5); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B23); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 6
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B6); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B24); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 7
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B7); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B25); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 8
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B8); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B26); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_BIG, BUTTON_MASK_LENGTH_BIG, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 9
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B9); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B27); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 10
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B10); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B28); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 11
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B11); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B29); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 12
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B12); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B30); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 13
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B13); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B31); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 14
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B14); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B32); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 15
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B15); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B33); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 16
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B16); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B34); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 17
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B17); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B35); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	{// Button 18
		CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B18); // 得到 Picture Control 句柄
		CImage image;
		image.Load(_T("res/greenRight.png"));
		HBITMAP hBmp = image.Detach();
		pPic->SetBitmap(hBmp);
		pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		{
			CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B36); // 得到 Picture Control 句柄
			CImage image;
			image.Load(_T("res/blueRight.png"));
			HBITMAP hBmp = image.Detach();
			pPic->SetBitmap(hBmp);
			pPic->SetWindowPos(NULL, 0, 0, BUTTON_MASK_HIGHT_SMALL, BUTTON_MASK_LENGTH_SMALL, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	
	return 0;
}

int CHWCDlg::LoadDefaultConfig()
{
	CString ModulePathName;
	CString WorkPathName;
	WorkPathName = g_WorkDir + CONFIG_FILE_PATH;
	ModulePathName = g_ModuleDir + CONFIG_FILE_PATH;
	
	if(PathFileExists(WorkPathName))
	{
		ReadCinfigFromIni(WorkPathName, m_KeymapR, m_UserDefinedKeymap, m_DecoderSelected, m_bHandShake, m_VoicePlayMode, m_MPConfig);
		//m_MPBDAddressEdit.SetWindowText(m_MPConfig.BDAdress);
		//m_MPLinkKeyEdit.SetWindowText(m_MPConfig.LinkKey);
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Read configuration from Work dir")));
	}
	else
	{
		if(PathFileExists(ModulePathName))
		{
			ReadCinfigFromIni(ModulePathName, m_KeymapR, m_UserDefinedKeymap, m_DecoderSelected, m_bHandShake, m_VoicePlayMode, m_MPConfig);
			//m_MPBDAddressEdit.SetWindowText(m_MPConfig.BDAdress);
			//m_MPLinkKeyEdit.SetWindowText(m_MPConfig.LinkKey);
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Read configuration from Install dir")));
		}
		else// config file not exist
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Read configuration Fail")));
			return 0;
		}
	}

	m_Keymap[m_KeymapR[MAP_POWERKEY]]	= MAP_POWERKEY;
	m_Keymap[m_KeymapR[MAP_MICKEY]]		= MAP_MICKEY;
	m_Keymap[m_KeymapR[MAP_UPKEY]]		= MAP_UPKEY;
	m_Keymap[m_KeymapR[MAP_DOWNKEY]]	= MAP_DOWNKEY;
	m_Keymap[m_KeymapR[MAP_RIGHTKEY]]	= MAP_RIGHTKEY;
	m_Keymap[m_KeymapR[MAP_LEFTKEY]]	= MAP_LEFTKEY;
	m_Keymap[m_KeymapR[MAP_HOMEKEY]]	= MAP_HOMEKEY;
	m_Keymap[m_KeymapR[MAP_MENUKEY]]	= MAP_MENUKEY;
	m_Keymap[m_KeymapR[MAP_OKKEY]]		= MAP_OKKEY;
	m_Keymap[m_KeymapR[MAP_VOLDOWN]]	= MAP_VOLDOWN;
	m_Keymap[m_KeymapR[MAP_VOLUP]]		= MAP_VOLUP;
	m_Keymap[m_KeymapR[MAP_BACKKEY]]	= MAP_BACKKEY;
	m_Keymap[m_KeymapR[MAP_ALLKEYUP]]	= MAP_ALLKEYUP;
	
	return 1;
}

int CHWCDlg::LoadSupportedDecoders()
{
	//CString WorkDir(WORK_DIR);
	CString DecoderDir(L"\\Decoder");
	CFileFind fileFind;
	if (!fileFind.FindFile(g_ModuleDir+DecoderDir))
	{
		return 0;	// no decoder
	}
	else
	{
		// read all decoder's dll
		//CStringArray DecoderSet;
		CString DecoderFile = g_ModuleDir+DecoderDir+L"\\*.dll";
		CFileFind ff;
		BOOL ret = ff.FindFile(DecoderFile);
		while(ret)
		{
			ret = ff.FindNextFile();
			CString FileName = ff.GetFileName();
			m_DecoderSet.Add(FileName.Left(FileName.GetLength()-4));
		}
		ff.Close();
		return 1;
	}
}

int CHWCDlg::SaveDefaultConfig()
{
	CString FilePathName;
	FilePathName = g_WorkDir + CONFIG_FILE_PATH;
	m_MPConfig.bFlag = 1;
	WriteCinfigToIni(FilePathName, m_KeymapR, m_UserDefinedKeymap, m_DecoderSelected, m_VoicePlayMode, m_MPConfig);
	return 1;
}

void CHWCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHWCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHWCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHWCDlg::ShowEditPrintInfo(CString &info, BOOL bRaw)
{
	CString OriginStr;
	if(!bRaw)
	{
		m_ButtonShow.GetWindowText(OriginStr);
		m_ButtonShow.SetWindowText(L">"+info+L"\r\n"+OriginStr);
	}
	else
	{
		m_ButtonShow.GetWindowText(OriginStr);
		m_ButtonShow.SetWindowText(info+OriginStr);
	}
}

afx_msg LRESULT CHWCDlg::OnCustomNotifyRawData(WPARAM wParam, LPARAM lParam)
{
	unsigned char* RawBuffer = (unsigned char*)lParam;
	int BufLen = (int)wParam;
	unsigned short NotificationHandle;
	NotificationHandle = (unsigned short)(RawBuffer[1] | (RawBuffer[2] << 8));

	if( NotificationHandle != 0x0029 && NotificationHandle != 0x0025)
	{	// raw data
		CString RawBuf(L"Rev Data:");
		for(int i=0; i<BufLen; i++)
		{
			CString temp;
			temp.Format(_T(" %x"), RawBuffer[i]);
			RawBuf += temp;
		}
		ShowEditPrintInfo(RawBuf, 0);
	}
	{
		// voice data, not show
	}
	
	return 0;
}

afx_msg LRESULT CHWCDlg::OnCustomButtonState2(WPARAM wParam, LPARAM lParam)
{
	unsigned char KeyValue = (unsigned char)wParam;
	switch(KeyValue)
	{
	case HX_LOVEKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B1);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B19);
			if(!MaskButtonState[0])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[0] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[0] = 0;
			}
			ButtonCheckDown |= 0x01 << 0;
		}
		break;
	case HX_POWERKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B2);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B20);
			if(!MaskButtonState[1])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[1] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[1] = 0;
			}
			ButtonCheckDown |= 0x01 << 1;
		}
		break;
	case HX_MICKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B3);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B21);
			if(!MaskButtonState[2])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[2] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[2] = 0;
			}
			OnCustomeMicdown(1, NULL);
			ButtonCheckDown |= 0x01 << 2;
		}
		break;	
	case HX_OKKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B4);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B22);
			if(!MaskButtonState[3])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[3] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[3] = 0;
			}
			ButtonCheckDown |= 0x01 << 3;
		}
		break;
	case HX_LEFTKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B5);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B23);
			if(!MaskButtonState[4])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[4] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[4] = 0;
			}
			ButtonCheckDown |= 0x01 << 4;
		}
		break;
	case HX_RIGHTKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B6);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B24);
			if(!MaskButtonState[5])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[5] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[5] = 0;
			}
			ButtonCheckDown |= 0x01 << 5;
		}
		break;
	case HX_UPKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B7);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B25);
			if(!MaskButtonState[6])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[6] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[6] = 0;
			}
			ButtonCheckDown |= 0x01 << 6;
		}
		break;
	case HX_DOWNKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B8);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B26);
			if(!MaskButtonState[7])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[7] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[7] = 0;
			}
			ButtonCheckDown |= 0x01 << 7;
		}
		break;
	case HX_BACKKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B9);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B27);
			if(!MaskButtonState[8])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[8] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[8] = 0;
			}
			ButtonCheckDown |= 0x01 << 8;
		}
		break;
	case HX_HOMEKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B10);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B28);
			if(!MaskButtonState[9])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[9] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[9] = 0;
			}
			ButtonCheckDown |= 0x01 << 9;
		}
		break;
	case HX_MENUKEY:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B11);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B29);
			if(!MaskButtonState[10])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[10] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[10] = 0;
			}
			ButtonCheckDown |= 0x01 << 10;
		}
		break;
	case HX_VOLDOWN:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B12);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B30);
			if(!MaskButtonState[11])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[11] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[11] = 0;
			}
			ButtonCheckDown |= 0x01 << 11;
		}
		break;
	case HX_VOLUP:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B14);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B32);
			if(!MaskButtonState[13])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[13] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[13] = 0;
			}
			ButtonCheckDown |= 0x01 << 12;
			ButtonCheckDown |= 0x01 << 13;
		}
		break;
	case HX_DIRECTSHOW:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B15);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B33);
			if(!MaskButtonState[14])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[14] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[14] = 0;
			}
			ButtonCheckDown |= 0x01 << 14;
		}
		break;
	case HX_MOVIE:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B16);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B34);
			if(!MaskButtonState[15])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[15] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[15] = 0;
			}
			ButtonCheckDown |= 0x01 << 15;
		}
		break;
	case HX_GAME:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B17);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B35);
			if(!MaskButtonState[16])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[16] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[16] = 0;
			}
			ButtonCheckDown |= 0x01 << 16;
		}
		break;
	case HX_EDUCATION:
		{
			CStatic* pPic1 = (CStatic*)GetDlgItem(IDC_STATIC_B18);
			CStatic* pPic2 = (CStatic*)GetDlgItem(IDC_STATIC_B36);
			if(!MaskButtonState[17])
			{// 0

				pPic1->ShowWindow(TRUE);
				pPic2->ShowWindow(FALSE);
				MaskButtonState[17] = 1;
			}
			else
			{// 1
				pPic1->ShowWindow(FALSE);
				pPic2->ShowWindow(TRUE);
				MaskButtonState[17] = 0;
			}
			ButtonCheckDown |= 0x01 << 17;
		}
		break;
	case HX_ALLKEYUP:
		{
			// Check All button Down???
			SendMessage(WM_CUSTOM_VOICEDOWN,0,0);
			if(ButtonCheckDown == 0x3FFFF)
				GetDlgItem(IDC_STATIC_PASS)->ShowWindow(TRUE);
		}
	}

	return 0;
}

void CHWCDlg::PicButtonMaskClear()
{
	CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC_B1);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B2);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B3);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B4);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B5);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B6);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B7);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B8);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B9);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B10);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B11);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B12);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B13);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B14);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B15);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B16);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B17);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B18);
	pPic->ShowWindow(FALSE);

	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B19);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B20);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B21);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B22);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B23);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B24);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B25);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B26);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B27);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B28);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B29);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B30);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B31);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B32);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B33);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B34);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B35);
	pPic->ShowWindow(FALSE);
	pPic = (CStatic*)GetDlgItem(IDC_STATIC_B36);
	pPic->ShowWindow(FALSE);

}

afx_msg LRESULT CHWCDlg::OnCustomButtonState(WPARAM wParam, LPARAM lParam)
{
	unsigned char KeyValue = (unsigned char)wParam;
	CString KeyValueStr;
	KeyValueStr.Format(_T("0x%02x"), KeyValue);

	map<BYTE, BYTE>::iterator it1 = m_Keymap.find(KeyValue);
	if(it1 != m_Keymap.end()) 
	{// find it
		switch(m_Keymap[KeyValue])
		{
		case MAP_POWERKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Power");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Power Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_HOMEKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Home");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Home Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_UPKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Up");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Up Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_DOWNKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Down");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Down Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_LEFTKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Left");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Left Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_RIGHTKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Right");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Right Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_OKKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    OK");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("OK Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_BACKKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Back");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Back Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_MICKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Mic");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Mic Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
				OnCustomeMicdown(1, NULL);
			}
			break;
		case MAP_MENUKEY:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Menu");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Menu Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_VOLUP:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Vol++");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Vol++ Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_VOLDOWN:
			{
				m_ButtonState.SetWindowText(KeyValueStr+L"    Vol--");
				::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
				CString infoStr;
				infoStr.Format(_T("Vol-- Button Down, KeyValue is 0x%02x"), KeyValue);
				ShowEditPrintInfo(infoStr, 0);
			}
			break;
		case MAP_ALLKEYUP:
			OnAllup(1, NULL);
			break;
		}
	}
	else 
	{// Use user defined Key
		CString TestModeStr(L"0xff");
		if(TestModeStr == KeyValueStr)
		{// disconnect the link
			ShowEditPrintInfo(CString(L"RCU Device Quit MP Mode Successfully"), 0);
			::PostMessage(m_hWnd, WM_LEDEV_DISCONNECT, 0, (LPARAM)&m_LeDevAddress);
			return 0;
		}
		map<CString, CString>::iterator it = m_UserDefinedKeymapR.find(KeyValueStr);
		if(it != m_UserDefinedKeymapR.end()) 
		{// find it
			m_ButtonState.SetWindowText(it->first + L"    " + it->second);
			::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONDOWN, 0, 0);	//向Button发送按键消息
			ShowEditPrintInfo(it->second + L" Button Down, KeyValue is" + it->first, 0);
		}
		else 
		{// not find it
			CString infoStr;
			infoStr.Format(_T("Unmapped KeyValue(0x%02x)"), KeyValue);
			m_ButtonState.SetWindowText(_T("Unmapped"));
			ShowEditPrintInfo(infoStr, 0);
		}
	}

	return 0;
}

afx_msg LRESULT CHWCDlg::OnCustomPowerdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	static bool s_PowerStatus = FALSE;
	if (wParam == 1)
	{
		m_Power.OnLButtonDown(nflags,point);
		s_PowerStatus = TRUE;
	}
	else
	{
		m_Power.OnLButtonUp(nflags,point);
		if(s_PowerStatus)
		{
			s_PowerStatus = FALSE;
		}
	}
	return 0;
}

afx_msg LRESULT CHWCDlg::OnCustomHomedown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	static bool s_HomeStatus = FALSE;
	if (wParam == 1)
	{
		m_Home.OnLButtonDown(nflags,point);
		s_HomeStatus = TRUE;
	}
	else
	{
		if(s_HomeStatus)
		{
			m_Home.OnLButtonUp(nflags,point);
			s_HomeStatus = FALSE;
		}
	}
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeUpdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_Up.OnLButtonDown(nflags,point);
	}
	else
	{
		m_Up.OnLButtonUp(nflags,point);
	}
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeDowndown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_Down.OnLButtonDown(nflags,point);
	}
	else
	{
		m_Down.OnLButtonUp(nflags,point);
	}
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeLeftdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if(wParam == 1)
	{
		m_Left.OnLButtonDown(nflags,point);
	}
	else
	{
		m_Left.OnLButtonUp(nflags,point);
	}
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeRightdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_Right.OnLButtonDown(nflags,point);
	}
	else
	{
		m_Right.OnLButtonUp(nflags,point);
	}
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeOkdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_OK.OnLButtonDown(nflags,point);
	}
	else
	{
		m_OK.OnLButtonUp(nflags,point);
	}
	
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeBackdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_Back.OnLButtonDown(nflags,point);
	}
	else
	{
		m_Back.OnLButtonUp(nflags,point);
	}
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeMicdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1 && g_bMicDown == false)
	{
		m_Mic.OnLButtonDown(nflags,point);
		StartToRecordVoice();

		if(m_VoicePlayMode == 2)
			WavePlayStart();
		ControlData RecStart = {0};
		RecStart.CmdHead = 0x5a;
		RecStart.CmdValue = 0x01;
		RecStart.WakeupSignal = 0xff;
		RecStart.ChannelID = 0xff;
		RecStart.CustomID0 = 0xff;
		RecStart.CustomID1 = 0xff;
		RecStart.Reserved0 = 0xff;
		RecStart.Reserved1 = 0xff;

		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));	

		RT_STATUS Status = LEHIDSvr.WriteCmdToDevice(0x002d, (unsigned char*)&RecStart, sizeof(RecStart));
		g_bMicDown = true;
		CString infoStr;
		infoStr.Format(_T("Receive voice data ..."));
		ShowEditPrintInfo(infoStr, 0);
	}
	else if(wParam == 0 && g_bMicDown == true)
	{
		m_Mic.OnLButtonUp(nflags,point);
		UpdateWindow();
		// Start to record voice
		ControlData RecStart = {0};
		RecStart.CmdHead = 0x5a;
		RecStart.CmdValue = 0x00;
		RecStart.WakeupSignal = 0x00;
		RecStart.ChannelID = 0xff;
		RecStart.CustomID0 = 0xff;
		RecStart.CustomID1 = 0xff;
		RecStart.Reserved0 = 0xff;
		RecStart.Reserved1 = 0xff;

		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
		m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));

		RT_STATUS Status = LEHIDSvr.WriteCmdToDevice(0x002d, (unsigned char*)&RecStart, sizeof(RecStart));
		if( m_VoicePlayMode == 0)
		{
			StopToRecordVoice();
		}
		else if( m_VoicePlayMode == 1 )
		{
			StopToRecordVoice();
			::PostMessage(m_hWnd, WM_PLAY_RECORD, 0, 0);
		}
		else if ( m_VoicePlayMode == 2 )
		{
			StopToRecordVoice();
			WavePlayStop();
		}
		g_bMicDown = false;
		g_bVoice = TRUE;
	}
	return 0;
}

int CHWCDlg::StartToRecordVoice()
{
	CString AudioFilePath;
	AudioFilePath = g_WorkDir + VOICE_RECORD_PATH;
	
	_wfopen_s(&m_MicFile, AudioFilePath, _T("wb"));
	m_VoiceStoreProecss.EnableWindow(TRUE);
	return 1;
}

int CHWCDlg::StopToRecordVoice()
{
	fclose(m_MicFile);
	m_VoiceStoreProecss.EnableWindow(FALSE);
	return 1;
}


afx_msg LRESULT CHWCDlg::OnCustomeMenudown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_Menu.OnLButtonDown(nflags,point);
	}
	else
	{
		m_Menu.OnLButtonUp(nflags,point);
	}
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeVoldowndown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_VolDown.OnLButtonDown(nflags,point);
	}
	else
	{
		m_VolDown.OnLButtonUp(nflags,point);
	}
	
	return 0;
}


afx_msg LRESULT CHWCDlg::OnCustomeVolupdown(WPARAM wParam, LPARAM lParam)
{
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	if (wParam == 1)
	{
		m_VolUp.OnLButtonDown(nflags,point);
	}
	else
	{
		m_VolUp.OnLButtonUp(nflags,point);
	}
	return 0;

}

afx_msg LRESULT CHWCDlg::OnAllup(WPARAM wParam, LPARAM lParam)
{
	bool value = 0;
	SendMessage(WM_CUSTOM_POWERDOWN,value,0);
	SendMessage(WM_CUSTOM_HOMEDOWN,value,0);
	SendMessage(WM_CUSTOM_LEFTDOWN,value,0);
	SendMessage(WM_CUSTOM_RIGHTDOWN,value,0);
	SendMessage(WM_CUSTOM_UPDOWN,value,0);
	SendMessage(WM_CUSTOM_DOWNDOWN,value,0);
	SendMessage(WM_CUSTOM_OKDOWN,value,0);
	SendMessage(WM_CUSTOM_BACKDOWN,value,0);
	SendMessage(WM_CUSTOM_VOICEDOWN,value,0);
	SendMessage(WM_CUSTOM_MENUDOWN,value,0);
	SendMessage(WM_CUSTOM_VOLUPDOWN,value,0);
	SendMessage(WM_CUSTOM_VOLDOWNDOWN,value,0);
	m_ButtonState.SetWindowText(L"");
	::SendMessage(m_ButtonState.m_hWnd, WM_LBUTTONUP, 0, 0);	//向Button发送按键消息
	CString infoStr;
	infoStr.Format(_T("Button Up"));
	ShowEditPrintInfo(infoStr, 0);
	return 0;
}


typedef struct
{
	unsigned char header;  //0xaa
	unsigned char flag;       //0x55
	unsigned char data_sn;
	unsigned char frame_sn;
	unsigned char data[16];
} t_report_voice_packet0;
typedef struct
{
	unsigned char flag;       //0x55
	unsigned char data_sn;
	unsigned char frame_sn;
	unsigned char data[17];
} t_report_voice_packet1;

unsigned char FrameData[36] = {0x9c, 0x31, 0x0e};
afx_msg LRESULT CHWCDlg::OnVoiceDataStore(WPARAM wParam, LPARAM lParam)
{
	unsigned char* voiceBuf = (unsigned char*)lParam;
	unsigned long bufLen = (unsigned long)wParam;
	static int dataSn = 0;
	static int frameSn = 0;

	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("voiceBuf is %p, bufLen is %d"), voiceBuf, bufLen));

	if( *voiceBuf == 0x6e && *(voiceBuf+1) == 0x61 ) return 0;  // the first info data package, dismiss

	if( *voiceBuf == 0xaa ) 
	{
		t_report_voice_packet0 *pFirstFrame = (t_report_voice_packet0*)voiceBuf;
		dataSn = pFirstFrame->data_sn;
		frameSn = pFirstFrame->frame_sn;
		memcpy(FrameData+3, pFirstFrame->data, 16);
	}
	else
	{
		t_report_voice_packet1 *pSecondFrame = (t_report_voice_packet1*)voiceBuf;
		if(dataSn == pSecondFrame->data_sn && (frameSn+1 == pSecondFrame->frame_sn))
		{
			memcpy(FrameData+19, pSecondFrame->data, 17);
			fwrite(FrameData, sizeof(char), 36, m_MicFile);
		}
	}

	//fwrite(voiceBuf, sizeof(char), bufLen, m_MicFile);
	//if(m_VoicePlayMode == 2)
	//	WaveWriteAudio((LPSTR)voiceBuf, bufLen);
	
	return 0;
}

DWORD WINAPI CHWCDlg::s_DeviceConnectThreadProc(LPVOID lpv)
{
	CHWCDlg *pThis = (CHWCDlg*)lpv;
	return pThis->DeviceConnectThreadProc();
}

DWORD CHWCDlg::DeviceConnectThreadProc(void)
{
	RT_STATUS Status = RT_STATUS_SUCCESS;
	LEScanStop();
	m_LEDevConnected = TRUE;

	m_RCUConn.EnableWindow(FALSE);
	m_UpdateButton.EnableWindow(FALSE);

	if( RCU_CONNECTION == m_ConnType )
	{
		if(!LEHIDSvr.Init(m_LeDevAddress))
		{
			DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("LEHIDSvr init Fail")));	
			DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("++")));
			m_LEDevConnected = FALSE;
			return RT_STATUS_NOT_CONNECT;
		}

		//ShowEditPrintInfo(CString(L"Start to connect the device"), 0);
		if(RT_STATUS_SUCCESS == LEHIDSvr.ConnectTo())
		{
			// Discover data channel service from remote device
			ShowEditPrintInfo(CString(L"Connect to Device Successfully"), 0);
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
			LEHIDSvr.SetKeyMap(m_Keymap);
			Status = LEHIDSvr.DiscoverService();
			if (Status != RT_STATUS_SUCCESS)
			{
				DEBUG_TRACE(TRACE_LEVEL_ERROR,RTK_LE_PROFILE_RCU,(_T("Discover Service Failed, Status(0x%x)"), Status));
				return Status;
			}
			else
			{
				// TODO: add result
				m_bHandShake = 0; // For HaiXin, don't do handshake
				if(m_bHandShake)
				{
					Status = LEHIDSvr.HandShakeWithRCU();
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Hand Shake Result %x"), Status));
					if(Status = RT_STATUS_SUCCESS)
						ShowEditPrintInfo(CString(L"HandShake Successfully"), 0);
					else
					{
						ShowEditPrintInfo(CString(L"HandShake Fail, Disconnect"), 0);
						CString InfoStr(_T("Disconnect, HandShake Fail\n\n") + m_ConnectedDev);
						m_static.SetWindowText(InfoStr);
						LEHIDSvr.Disconnect();
						m_LEDevConnected = FALSE;
						m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
						m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
						m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
						m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
						m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
						m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
						DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Disconnect the LE Link")));
						return Status;
					}
				}

				m_DevDisconn.EnableWindow(TRUE);
				m_ButtonState.SetWindowText(L"");
				m_ScanState.EnableWindow(FALSE);

				/*DWORD PatchVersion, AppVersion;
				Status = LEHIDSvr.GetOTAInfo(PatchVersion, AppVersion);
				if(Status == RT_STATUS_SUCCESS)
				{
					CString VersionInfo;
					VersionInfo.Format(_T("OTA support, current info:\r\n\tPatch Version: 0x%x\r\n\tApp Version:   0x%x"), PatchVersion, AppVersion);
					ShowEditPrintInfo(VersionInfo, 0);
					CString InfoStr(_T("Device Connected\n\n") + m_ConnectedDev);
					m_static.SetWindowText(InfoStr);
				}*/

				CString HWStr, SWStr, FWStr;
				Status = LEHIDSvr.ReadDeviceInfo(HWStr, SWStr, FWStr);
				if(Status == RT_STATUS_SUCCESS)
				{
					//ShowEditPrintInfo(CString(L"Read Device Info Successfully"), 0);
					GetDlgItem(IDC_EDIT_DEVINFO_HW)->SetWindowText(HWStr);
					GetDlgItem(IDC_EDIT_DEVINFO_SW)->SetWindowText(SWStr);
					GetDlgItem(IDC_EDIT_DEVINFO_FW)->SetWindowText(FWStr);
					GetDlgItem(IDC_EDIT_DEV_RSSI)->SetWindowText(m_RssiStr);
				}

				Status = LEHIDSvr.ReadHidInfo();
				if(Status == RT_STATUS_SUCCESS)
				{
					m_AlertButton.EnableWindow(TRUE);
					m_VoiceButton.EnableWindow(TRUE);
					m_LogClear.EnableWindow(TRUE);

					//ShowEditPrintInfo(CString(L"Get HID Service Info Successfully"), 0);
					ButtonGroupEnable(m_LEDevConnected);	// Enable Buttons
					LEHIDSvr.BeginWaitReportDataThread();
					CString DecoderInfo(_T("\nVoice Decoder:	 ") + m_DecoderSelected);
					CString InfoStr(_T("Device Connected\n") + m_ConnectedDev + DecoderInfo);
					m_static.SetWindowText(InfoStr);
					memset(MaskButtonState, 0, 18);
					g_bVoice = TRUE;
					//ShowEditPrintInfo(CString(L"Start to test OK"), 0);
				}
				else
				{
					ShowEditPrintInfo(CString(L"HID not support"), 0);
					LEHIDSvr.Disconnect();
					m_LEDevConnected = FALSE;
					m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
					m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
					m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
					m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
					m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
					m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Disconnect the LE Link")));
					return Status;
				}
			}
		}
		else
		{
			CString InfoStr(_T("Connected Fail\n\n") + m_ConnectedDev);
			m_static.SetWindowText(InfoStr);
			m_LEDevConnected = FALSE;
			m_rightDev = FALSE;
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
			m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
			ShowEditPrintInfo(CString(L"Connected Fail"), 0);
		}
	}
	else if( OTA_CONNECTION == m_ConnType )
	{
		if(!dfu.Init(m_LeDevAddress))
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("DFU init Fail")));
			m_LEDevConnected = FALSE;
			return RT_STATUS_NOT_CONNECT;
		}

		LRESULT Status = dfu.ConnectTo(m_LeOTAMAddress);
		if(RT_STATUS_SUCCESS == Status)
		{
			// Do Hand Shake
			ShowEditPrintInfo(CString(L"OTA Connect Successfully"), 0);
			// TODO: add result
			if(m_bHandShake)
			{
				Status = dfu.HandShakeWithRCU();
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Hand Shake Result %x"), Status));
				if(Status = RT_STATUS_SUCCESS)
					ShowEditPrintInfo(CString(L"HandShake Successfully"), 0);
				else
					ShowEditPrintInfo(CString(L"HandShake Fail, Disconnect"), 0);
			}
			// Enable Buttons

			if(Status == RT_STATUS_SUCCESS)
			{
				CString InfoStr(_T("OTA Connected\n") + m_ConnectedDev);
				m_static.SetWindowText(InfoStr);

				m_RCUConn.EnableWindow(FALSE);
				m_ScanState.EnableWindow(FALSE);
				m_DevDisconn.EnableWindow(TRUE);
				m_RCUConn.EnableWindow(FALSE);
				m_MPConnect.EnableWindow(FALSE);
				m_ButtonState.ShowWindow(FALSE);
				m_OTAProcess.ShowWindow(TRUE);
				UpdateWindow();

				ShowEditPrintInfo(CString(L"Start DFU procedure"), 0);
				OnBnClickedButtonGetRemoteinfo();
				OnBnClickedButtonDownloadPatch();
				OnBnClickedButtonActiveFw();
				ShowEditPrintInfo(CString(L"DFU procedure over"), 0);
				dfu.Disconnect(m_LeOTAMAddress);
				InfoStr.Format(_T("OTA Over, Link disconnected\n") + m_OTAConnectedDev);
				m_static.SetWindowText(InfoStr);

				m_ButtonState.ShowWindow(TRUE);
				m_OTAProcess.ShowWindow(FALSE);
				UpdateWindow();

			}
			else
			{
				CString InfoStr(_T("Disconnect, HandShake Fail\n") + m_OTAConnectedDev);
				m_static.SetWindowText(InfoStr);
				dfu.Disconnect(m_LeOTAMAddress);
				m_LEDevConnected = FALSE;
				DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Disconnect the LE Link")));
			}
		}
		else
		{
			CString InfoStr(_T("Connected Fail\n") + m_OTAConnectedDev);
			m_static.SetWindowText(InfoStr);
			m_LEDevConnected = FALSE;
			m_rightDev = FALSE;
		}
	}

	return RT_STATUS_SUCCESS;
}

LRESULT CHWCDlg::ConnectAndGetInfo(WPARAM wParam, LPARAM lParam)
{
	RT_STATUS Status = RT_STATUS_SUCCESS;
	m_ConnType = (int)wParam;
	unsigned long long BTAddr = *(unsigned long long*)lParam;
	switch(m_ConnType)
	{
	case RCU_CONNECTION:
		m_LeDevAddress = BTAddr;
	case OTA_CONNECTION:
		m_LeOTAMAddress = BTAddr;
	}
	
	m_hDeviceConnThread = CreateThread(
		NULL, 
		0, 
		s_DeviceConnectThreadProc, 
		(LPVOID)this, 
		0, 
		NULL);
	if(NULL != m_hDeviceConnThread)
	{
		m_RCUConn.EnableWindow(TRUE);
	}
	CloseHandle(m_hDeviceConnThread);

	return Status;
}


LRESULT CHWCDlg::DisconnectDevLink(WPARAM wParam, LPARAM lParam)
{
	if(!m_LEDevConnected) 
		return RT_STATUS_SUCCESS;

	CString InfoStr(_T("Disconnecting......\n\n") + m_ConnectedDev);
	m_static.SetWindowText(InfoStr);
	m_rightDev = FALSE;
	m_LEDevConnected = FALSE;
	LEHIDSvr.Disconnect();
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Disconnect the LE Link")));

	SetEvent(m_hScanEvent);
	ButtonGroupEnable(m_LEDevConnected);
	InfoStr.Format(_T("Link disconnected\n\n") + m_ConnectedDev);
	m_static.SetWindowText(InfoStr);
	GetDlgItem(IDC_EDIT_DEVINFO_HW)->SetWindowText(L"");
	GetDlgItem(IDC_EDIT_DEVINFO_SW)->SetWindowText(L"");
	GetDlgItem(IDC_EDIT_DEVINFO_FW)->SetWindowText(L"");
	GetDlgItem(IDC_EDIT_DEV_RSSI)->SetWindowText(L"");
	ButtonCheckDown = 0;
	GetDlgItem(IDC_STATIC_PASS)->ShowWindow(FALSE);

	if(g_bMicDown)
	{
		StopToRecordVoice();
	}
	g_bMicDown = FALSE;

	PicButtonMaskClear();
	m_ButtonShow.SetWindowText(L"");
	SetSTButtonColor(255, 0, 0);
	g_alertLevel = 1;
	g_bVoice = TRUE;
	if(m_pView)
		m_pView->ClearView();

	// Back to Read
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));

	m_AlertButton.EnableWindow(FALSE);
	m_VoiceButton.EnableWindow(FALSE);
	m_LogClear.EnableWindow(FALSE);

	return RT_STATUS_SUCCESS;
}

LRESULT CHWCDlg::OnAllButtonActive(WPARAM wParam, LPARAM lParam)
{
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("OnAllButtonActive +++")));
	UINT nflags = 1;
	CPoint point;
	point.x = 29;
	point.y = 26;
	
	// Down!!!
	{
		m_Right.OnLButtonDown(nflags, point);
		m_Left.OnLButtonDown(nflags,point);
		m_OK.OnLButtonDown(nflags, point);
		m_Mic.OnLButtonDown(nflags, point);
		m_Up.OnLButtonDown(nflags, point);
		m_Down.OnLButtonDown(nflags, point);
		m_VolDown.OnLButtonDown(nflags, point);
		m_VolUp.OnLButtonDown(nflags, point);
		m_Home.OnLButtonDown(nflags, point);
		m_Power.OnLButtonDown(nflags, point);
		m_Back.OnLButtonDown(nflags, point);
		m_Menu.OnLButtonDown(nflags, point);
		UpdateWindow();
	}

	Sleep(800);
	// Up!!!
	{
		m_Left.OnLButtonUp(nflags,point);
		m_Right.OnLButtonUp(nflags,point);
		m_OK.OnLButtonUp(nflags, point);
		m_Mic.OnLButtonUp(nflags, point);
		m_Up.OnLButtonUp(nflags, point);
		m_Down.OnLButtonUp(nflags, point);
		m_VolDown.OnLButtonUp(nflags, point);
		m_VolUp.OnLButtonUp(nflags, point);
		m_Home.OnLButtonUp(nflags, point);
		m_Power.OnLButtonUp(nflags, point);
		m_Back.OnLButtonUp(nflags, point);
		m_Menu.OnLButtonUp(nflags, point);
		UpdateWindow();
	}
	return 0;


	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("OnAllButtonActive ---")));
	return RT_STATUS_SUCCESS;
}

LRESULT CHWCDlg::PlayVoiceRecord(WPARAM wParam, LPARAM lParam)
{
	ShowEditPrintInfo(CString(L"Replay recorded voice data"), 0);
	UpdateWindow();

	CString AudioFilePath;
	AudioFilePath = g_WorkDir + VOICE_RECORD_PATH;

	LPTSTR lpAudioFilePath =(LPTSTR)(LPCTSTR)AudioFilePath;
	m_DecoderFunc(lpAudioFilePath);
	DrawVoiceWave();	
	m_PlayFunc();

	ShowEditPrintInfo(CString(L"Replay over"), 0);
	UpdateWindow();
	return RT_STATUS_SUCCESS;
}

void CHWCDlg::OnBnClickedScan()
{
	// TODO: 在此添加控件通知处理程序代码
	int nRetCode = 0;
	CLEScanWinDlg* Scandlg = new CLEScanWinDlg;
	if(IDOK == Scandlg->DoModal())
	{
		m_Start.EnableWindow(False);
		m_Start.ShowWindow(False);
		m_static.ShowWindow(True);
		m_ButtonShow.SetWindowText(L"");
		m_LeDevAddress = Scandlg->m_BDAddress;
		m_BDAddrStr = Scandlg->m_BDAddrStr;
		m_BTDevNameStr = Scandlg->m_BTDevNameStr;
		m_AdvStr = Scandlg->m_AdvTypeStr;
		m_RssiStr = Scandlg->m_RSSIStr;

		// Delete the Register
		if(m_AdvTypeR[m_AdvStr] == ADV_IND)
		{
			CRegKey LeDeviceKey;
			CString BDAddressPath = SERVICE_LE_DATABASE_KEY;
			int RetValue = LeDeviceKey.Open(HKEY_LOCAL_MACHINE, BDAddressPath);
			if( RetValue == ERROR_SUCCESS )	
			{
				//LeDeviceKey.DeleteSubKey(m_BDAddrStr);	// only delete empty subkey
				LeDeviceKey.RecurseDeleteKey(m_BDAddrStr);	// can delete non-empty subkey
				LeDeviceKey.Close();
			}
		}
		else if(m_AdvTypeR[m_AdvStr] == ADV_DIRECT_IND)
		{
			// Check Register ???
			CRegKey LeDeviceKey;
			CString BDAddressPath = SERVICE_LE_DATABASE_KEY + m_BDAddrStr;
			int RetValue = LeDeviceKey.Open(HKEY_LOCAL_MACHINE, BDAddressPath);
			if( RetValue == ERROR_SUCCESS )	
			{
				TCHAR tmp[_MAX_PATH];  
				DWORD len = _MAX_PATH;  
				LeDeviceKey.QueryStringValue(_T("DevName"), tmp, &len);
				m_BTDevNameStr = CString(tmp);
				LeDeviceKey.Close();
			}
		}

		pThis->m_EditRSSI.SetWindowText(m_RssiStr);

		m_ConnectedDev = _T("BT Address:              ") + m_BDAddrStr + _T("\nDevice Name:            ") + m_BTDevNameStr;
		CString Device(_T("Device Selected, wait to connect\n\n") + m_ConnectedDev);
		pThis->m_static.SetWindowText(Device);
		UpdateWindow();
		
		m_RCUConn.EnableWindow(TRUE);
		m_DeviceSelected = TRUE;
	}
	else
	{
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("No LE Device is sellected")));
	}
}

DWORD WINAPI CHWCDlg::s_LEScanDeviceThreadProc(LPVOID lpv)
{
	CHWCDlg * pThis = (CHWCDlg *)lpv;
	return pThis->LEScanDeviceThreadProc();
}

DWORD CHWCDlg::LEScanDeviceThreadProc()
{
	BOOL Result = FALSE;
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("LEScanDeviceThreadProc ++")));
	BleRegisterScanCallback(LeGapScanCallBack);
	while(true)
	{
		if(m_MPModeStop)
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("MP Mode Stop, exit this thread")));
			break;
		}
		if(!m_LEDevConnected)
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("ReScan Le Device")));
			swscanf_s((LPCTSTR)m_MPConfig.BDAdress, L"%I64x", &m_LeDevAddress);
			m_ConnectedDev = _T("BT Address:              ") + m_MPConfig.BDAdress + _T("\nDevice Name:            MP Mode Device");
			CString Device(_T("Scan the device in MP Mode...\n\n\n") + m_ConnectedDev);
			pThis->m_static.SetWindowText(Device);
			ShowEditPrintInfo(CString(L"LE scan again..."), 0);
			UpdateWindow();
			LEScanStart();
		}
		DWORD dw = WaitForSingleObject(m_hScanEvent, 5000);	// timeout 5s
		if(dw == WAIT_TIMEOUT || dw == WAIT_OBJECT_0)
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Timeout, continue to Wait, m_LEDevConnected(%d)"), m_LEDevConnected));
			ResetEvent(m_hScanEvent);
			continue;
		}
		else
		{// error or event signed, exit thread
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Exit thread, Status(0x%x)"), dw));
			break;
		}
	}
	
	LEScanStop();
	BleUnregisterScanCallback();
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("LEScanDeviceThreadProc --")));
	return Result;
}

void CHWCDlg::LEScanStart()
{
	// TODO: 在此添加控件通知处理程序代码
	signed char s;
	//BleRegisterScanCallback(LeGapScanCallBack);

	// set scan interval, if not call, using default value
	// Range: 0x0004 to 0x4000
	// Default: 0x0010 (10 ms)
	// Time = N * 0.625 msec
	// Time Range: 2.5 msec to 10.24 seconds
	// BleSetScanningInterval( 0x0010 ); //default value: 10ms

	// start scanning and check status
	BleIsScanning(&s);
	if(s)
	{
		//printf("Is Scanning, stop scan and restart again");
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Is Scanning, stop scan and restart again")));
		BleEnableScan(false,0x0);            
	}
	BleSetScanMode(ACTIVE);		//set active mode
	BleEnableScan(true, 0x1);
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Set scan enable")));
	BleIsScanning(&s);
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Is Scanning: %d")));
}

void CHWCDlg::LeGapScanCallBack(
			unsigned char* pAddress,	// BT Address
			const int nSizeAddress,		// Address Len = 6
			unsigned char* pPkt,		// Report data
			const int nSizePkt,			// Report Len
			void* pUserData)
{
	PADV_DATA advData = NULL;
	int advLen = 0;
	unsigned char nameArr[31] = {0};
	CString address;
	CString nameStr;
	bool PairFlag = False;
	UCHAR AdvType = (UCHAR)pUserData;	// AdvType or Disconnect Notification
	ULONGLONG BDADdress;

	memcpy(&BDADdress, pAddress, nSizeAddress);
	// Only need ADV_DIRECT_IND Package
	address.Format(_T("%02x%02x%02x%02x%02x%02x"),pAddress[5], pAddress[4], pAddress[3], pAddress[2], pAddress[1],pAddress[0]);
	
	switch(AdvType)
	{
	case ADV_DIRECT_IND:
		{
			if( pThis->m_LeDevAddress == BDADdress )
			{// find the connect direct device
				RSSI = *(pPkt);
				if( RSSI & 0x80) 
					pThis->m_RssiStr.Format(L"-%d dBm", RSSI&0x7F);	// -127 - 0
				else
					pThis->m_RssiStr.Format(L"+%d dBm", RSSI);		// 0 - 20
				//pThis->ShowEditPrintInfo(CString(L"Start to connect the right device"), 0);
				::PostMessage(pThis->m_hWnd, WM_LEDEV_CONNECT, RCU_CONNECTION, (LPARAM)&pThis->m_LeDevAddress);
			}
		}
		break;
	case LE_LINK_DISCONNECT:
		{
			if( pThis->m_LeDevAddress == BDADdress )
			{// LE Link is disconnect
				pThis->ShowEditPrintInfo(CString(L"Link disconnect from the bottom"), 0);
				::PostMessage(pThis->m_hWnd, WM_LEDEV_DISCONNECT, 0, (LPARAM)&pThis->m_LeDevAddress);
			}
		}
		break;
	//case ADV_IND:
	case ADV_SCAN_IND:
	case ADV_NONCONN_IND:
		break;
	}
}

void CHWCDlg::LEScanStop()
{
	signed char s;
	// TODO: 在此添加控件通知处理程序代码
	// unregister callback
	//BleUnregisterScanCallback();
	// stop scanning and check status
	BleEnableScan(false, 0x1);
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Set scan disable")));
	//printf("Set scan disable\n");
	BleIsScanning(&s);
	//printf("Is Scanning: %d\n", s);
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Is Scanning: %d"), s));
}


void CHWCDlg::OnBnClickedAutotestStart()
{//  Only Run Once
	// TODO: 在此添加控件通知处理程序代码
}

int CHWCDlg::ButtonGroupEnable(bool value)
{
	GetDlgItem(IDC_BUTTON1)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON6)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON7)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON9)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON10)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON11)->EnableWindow(value);
	GetDlgItem(IDC_BUTTON12)->EnableWindow(value);
	return 0;
}

void CHWCDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	// Terminate the scan thread
	SetEvent(m_hScanEvent);
	m_MPModeStop = TRUE;
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("Start to wait m_hThreadNotify")));
	DWORD dw = WaitForSingleObject(m_hLEScanDeviceThread, 1000);
	DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("Wait over, dw(%x)"), dw));
	m_hLEScanDeviceThread = NULL;
	if(m_LEDevConnected)
	{
		::SendMessage(m_hWnd, WM_LEDEV_DISCONNECT, 0, 0);
	}
	if(m_DeviceSelected)
	{
		m_RCUConn.EnableWindow(TRUE);
	}
	else
	{
		m_Start.ShowWindow(TRUE);
		m_static.ShowWindow(FALSE);
	}
	m_ScanState.EnableWindow(TRUE);
	m_ButtonState.SetWindowText(L"Test Button");
	OnBnClickedClearLog();
	MPModeReset();
}
void CHWCDlg::OnBnClickedConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	CConfigDlg* configDlg = new CConfigDlg(m_UserDefinedKeymap, m_KeymapR, m_DecoderSet, m_DecoderSelected, 
										m_VoicePlayMode, m_LeDevAddress, m_LEDevConnected, m_MPConfig);
	if(IDOK == configDlg->DoModal())
	{
		int PowerValue;		// = _ttoi(configDlg->m_KeyPower);
		int HomeValue;		// = _ttoi(configDlg->m_KeyHome);
		int OKValue;		// = _ttoi(configDlg->m_KeyOk);
		int UpValue;		// = _ttoi(configDlg->m_KeyUp);
		int DownValue;		// = _ttoi(configDlg->m_KeyDown);
		int RightValue;		// = _ttoi(configDlg->m_KeyRight);
		int LeftValue;		// = _ttoi(configDlg->m_KeyLeft);
		int BackValue;		// = _ttoi(configDlg->m_KeyBack);
		int MenuValue;		// = _ttoi(configDlg->m_KeyMenu);
		int MicValue;		// = _ttoi(configDlg->m_KeyMic);
		int VolPValue;		// = _ttoi(configDlg->m_KeyVolP);
		int VolMValue;		// = _ttoi(configDlg->m_KeyVolM);
		int AllKeysUp;		// = _ttoi(configDlg->m_KeyAllUp);
		swscanf_s(configDlg->m_KeyPower.GetBuffer(0),L"%x",&PowerValue);
		swscanf_s(configDlg->m_KeyHome.GetBuffer(0),L"%x",&HomeValue);
		swscanf_s(configDlg->m_KeyOk.GetBuffer(0),L"%x",&OKValue);
		swscanf_s(configDlg->m_KeyUp.GetBuffer(0),L"%x",&UpValue);
		swscanf_s(configDlg->m_KeyDown.GetBuffer(0),L"%x",&DownValue);
		swscanf_s(configDlg->m_KeyRight.GetBuffer(0),L"%x",&RightValue);
		swscanf_s(configDlg->m_KeyLeft.GetBuffer(0),L"%x",&LeftValue);
		swscanf_s(configDlg->m_KeyBack.GetBuffer(0),L"%x",&BackValue);
		swscanf_s(configDlg->m_KeyMenu.GetBuffer(0),L"%x",&MenuValue);
		swscanf_s(configDlg->m_KeyMic.GetBuffer(0),L"%x",&MicValue);
		swscanf_s(configDlg->m_KeyVolP.GetBuffer(0),L"%x",&VolPValue);
		swscanf_s(configDlg->m_KeyVolM.GetBuffer(0),L"%x",&VolMValue);
		swscanf_s(configDlg->m_KeyAllUp.GetBuffer(0),L"%x",&AllKeysUp);

		// Static Value Handle
		m_MPConfig.AlertHandleStr = configDlg->m_AlertHandleStr;
		m_MPConfig.SWHandleStr = configDlg->m_SWHandleStr;
		m_MPConfig.FWHandleStr = configDlg->m_FWHandleStr;
		m_MPConfig.HWHandleStr = configDlg->m_HWHandleStr;
		m_MPConfig.KeyNHandleStr = configDlg->m_KeyNHandleStr;
		m_MPConfig.VoiceNHandleStr = configDlg->m_VoiceNHandleStr;

		//unsigned short AlertValueHandle;
		//unsigned short SWValueHandle;
		//unsigned short FWValueHandle;
		//unsigned short HWValueHandle;
		//unsigned short KeyNotifyHandle;
		//unsigned short VoiceNotifyHandle;
		//swscanf_s(configDlg->m_AlertHandleStr.GetBuffer(0),L"%x",&AlertValueHandle);
		//swscanf_s(configDlg->m_SWHandleStr.GetBuffer(0),L"%x",&SWValueHandle);
		//swscanf_s(configDlg->m_FWHandleStr.GetBuffer(0),L"%x",&FWValueHandle);
		//swscanf_s(configDlg->m_HWHandleStr.GetBuffer(0),L"%x",&HWValueHandle);
		//swscanf_s(configDlg->m_KeyNHandleStr.GetBuffer(0),L"%x",&KeyNotifyHandle);
		//swscanf_s(configDlg->m_VoiceNHandleStr.GetBuffer(0),L"%x",&VoiceNotifyHandle);

		m_Keymap.clear();
		m_Keymap[PowerValue]	= MAP_POWERKEY;
		m_Keymap[MicValue]		= MAP_MICKEY;
		m_Keymap[UpValue]		= MAP_UPKEY;
		m_Keymap[DownValue]		= MAP_DOWNKEY;
		m_Keymap[RightValue]	= MAP_RIGHTKEY;
		m_Keymap[LeftValue]		= MAP_LEFTKEY;
		m_Keymap[HomeValue]		= MAP_HOMEKEY;
		m_Keymap[MenuValue]		= MAP_MENUKEY;
		m_Keymap[OKValue]		= MAP_OKKEY;
		m_Keymap[VolMValue]		= MAP_VOLDOWN;
		m_Keymap[VolPValue]		= MAP_VOLUP;
		m_Keymap[BackValue]		= MAP_BACKKEY;
		m_Keymap[AllKeysUp]		= MAP_ALLKEYUP;

		m_KeymapR.clear();
		m_KeymapR[MAP_POWERKEY]	= PowerValue;
		m_KeymapR[MAP_MICKEY]	= MicValue;
		m_KeymapR[MAP_UPKEY]	= UpValue;
		m_KeymapR[MAP_DOWNKEY]	= DownValue;
		m_KeymapR[MAP_RIGHTKEY]	= RightValue;
		m_KeymapR[MAP_LEFTKEY]	= LeftValue;
		m_KeymapR[MAP_HOMEKEY]	= HomeValue;
		m_KeymapR[MAP_MENUKEY]	= MenuValue;
		m_KeymapR[MAP_OKKEY]	= OKValue;
		m_KeymapR[MAP_VOLDOWN]	= VolMValue;
		m_KeymapR[MAP_VOLUP]	= VolPValue;
		m_KeymapR[MAP_BACKKEY]	= BackValue;
		m_KeymapR[MAP_ALLKEYUP]	= AllKeysUp;
		
		m_UserDefinedKeymap = configDlg->m_UserDefineKeyMap;	// Get UserDefined Keymap
		m_UserDefinedKeymapR.clear();
		map<CString,CString>::iterator iter = m_UserDefinedKeymap.begin();  
		while (iter!=m_UserDefinedKeymap.end())
		{  
			m_UserDefinedKeymapR[iter->second] = iter->first;
			iter++;
		}
		LEHIDSvr.SetKeyMap(m_Keymap);

		m_VoicePlayMode = configDlg->m_VoiceOpt;
		if(m_DecoderSelected != configDlg->m_DecoderStr)
		{
			m_DecoderSelected = configDlg->m_DecoderStr;
			//if(m_DecoderSelected.Find(L"mSBC") == -1)
			{// load dll test
				CString DecoderDir(L"\\Decoder\\");
				CString VoiceDecoder;
				VoiceDecoder = g_ModuleDir + DecoderDir + m_DecoderSelected + _T(".dll");
				if(!LoadDecoder(VoiceDecoder))
				{// Fail
					m_DecoderSelected = CString(VOICE_DECODER_NULL);
					m_DecoderFunc = NULL_AudioDecoder;
					m_PlayFunc = NULL_AudioPlay;
					ShowEditPrintInfo(CString(L"No voice decoder selected"), 0);
					m_VoicePlayMode = 0;
				}
			}
		}

		switch(m_VoicePlayMode)
		{
		case 0:
			ShowEditPrintInfo(CString(L"Record voice to data only"), 0);
			break;
		case 1:
			ShowEditPrintInfo(CString(L"Record voice to data and play"), 0);
			break;
		case 2:
			ShowEditPrintInfo(CString(L"Direct to play"), 0);
			break;
		}

		if(m_LEDevConnected)
		{
			CString DecoderInfo(_T("\nVoice Decoder:	 ") + m_DecoderSelected);
			CString InfoStr(_T("Connected, test...\n") + m_ConnectedDev + DecoderInfo);
			m_static.SetWindowText(InfoStr);
		}
	}

	delete configDlg;
}

void CHWCDlg::DrawVoiceWave()
{
	m_pView->ClearView();
	m_pView->UpdateView();
}

void CHWCDlg::OnBnClickedButtonReplay()
{
	// TODO: 在此添加控件通知处理程序代码
#define VOICE_REPLAY	0	// Function
#define BUTTON_DOWN		0	// Test
#define AES_ENCRYPT		0	// Test
#define PIC_DRAW		0	// Draw
#define WAVE_DOCVIEW	0	// Doc-View
#define DEV_INFO		0

	if(DEV_INFO)
	{
		CString HWStr, SWStr, FWStr;
		LEHIDSvr.ReadDeviceInfo(HWStr, SWStr, FWStr);
	}

	if(WAVE_DOCVIEW)
	{
		DrawVoiceWave();
	}

	if(PIC_DRAW)
	{
		CString pcmFilePath;
		pcmFilePath = g_WorkDir + SBC_PCM_PATH;
		CFile pcmSbcFile(pcmFilePath,CFile::modeRead);
		ULONGLONG filelength = pcmSbcFile.GetLength();

		CWnd *pPicWnd=GetDlgItem(IDC_WAVE_SHOW);
		CRect rc;
		pPicWnd->GetClientRect(rc);
		CClientDC dc(pPicWnd);
		int Height = rc.Height();
		int Width = rc.Width();

		unsigned short *pcmbuffer = NULL;
		pcmbuffer = (unsigned short *)HeapAlloc(GetProcessHeap(),HEAP_GENERATE_EXCEPTIONS,SIZE_T(filelength));
		if (pcmbuffer == NULL)
		{
			_tprintf(_T("Error -> HeapAlloc!"));
			pcmSbcFile.Close();
			return;
		}

		SIZE_T size = pcmSbcFile.Read((unsigned char *)pcmbuffer,(UINT)filelength);
		if (size != filelength)
		{
			_tprintf(_T("Error -> pcmfile Read!"));
			HeapFree(GetProcessHeap(),0,pcmbuffer);
			pcmSbcFile.Close();
			return;
		}

#define TIME_LINE 5*16000	// 5s
#define WAVE_MAX  0xffff	// 2Bytes
		int a = TIME_LINE/Width;
		int y = 0;
		float dx;
		unsigned short dy = TIME_LINE/Width;
		unsigned short waveMax = WAVE_MAX;
		unsigned short pcmWave = 0;
		y = waveMax;
		for(int x=0; x<Width; x=x+2)
		{
			int pos = dy*x;
			pos = (pos%2) ? (pos-1) : pos;
			if(pos < filelength-1)
				pcmWave = *(pcmbuffer+pos/2);
			else
				pcmWave = waveMax*0.5-2;
			dx = (pcmWave-waveMax*0.5)/waveMax;
			y = (int)(Height*0.5 + dx*Height);

			dc.MoveTo(x,Height/2);
			dc.LineTo(x,y);
		}
		HeapFree(GetProcessHeap(),0,pcmbuffer);
		pcmSbcFile.Close();		
	}

	if(VOICE_REPLAY)
	{
		if(m_VoicePlayMode == 2)
		{// Audio Direct Play
			ShowEditPrintInfo(CString(L"Replay last recorded voice data"), 0);

			CString AudioFilePath;
			AudioFilePath = g_WorkDir + VOICE_RECORD_PATH;
			LPTSTR lpAudioFilePath =(LPTSTR)(LPCTSTR)AudioFilePath;
			CFile msbcfile(AudioFilePath,CFile::modeRead);
			ULONGLONG filelength = msbcfile.GetLength();
			int mSBCFrameSize = 120;
			void *pcmbuffer = NULL;
			void *msbcbuffer = NULL;
			ULONGLONG frame_count = filelength / mSBCFrameSize;
			msbcbuffer = (void *)malloc(mSBCFrameSize);
			WavePlayStart();
			
			for (int i=0;i<frame_count;i++)
			{
				int size = msbcfile.Read(msbcbuffer,mSBCFrameSize);
				if (size != mSBCFrameSize)
				{
					DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("Error -> Read msbc file")));
				}
				WaveWriteAudio((LPSTR)msbcbuffer, mSBCFrameSize);
			}
			WavePlayStop();
			ShowEditPrintInfo(CString(L"Replay over"), 0);
		}
		else if ( m_VoicePlayMode == 1 )
		{// Audio Replay
			ShowEditPrintInfo(CString(L"Replay last recorded voice data"), 0);

			CString AudioFilePath;
			AudioFilePath = g_WorkDir + VOICE_RECORD_PATH;
			LPTSTR lpAudioFilePath =(LPTSTR)(LPCTSTR)AudioFilePath;
			m_DecoderFunc(lpAudioFilePath);
			m_PlayFunc();
			ShowEditPrintInfo(CString(L"Replay over"), 0);
		}
		else if ( m_VoicePlayMode == 0 )
		{
			ShowEditPrintInfo(CString(L"Silent Mode, Only have raw data"), 0);
		}
	}

	if(BUTTON_DOWN)
	{
		unsigned char KeyDownValue = 0x77; 
		HBITMAP   hBitmap;   
		hBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BackDown)); 
		m_ButtonState.SetBitmap(hBitmap);  

		::SendMessage(m_hWnd, WM_BUTTON_STATE, KeyDownValue, NULL);
		Sleep(500);
		::SendMessage(m_hWnd, WM_ALLUP, NULL, NULL);
		//m_ButtonState.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_TEST));
	}

	if(AES_ENCRYPT)
	{// AES Test
		aes_context ctx;
		unsigned char AES_Key32[32] ={ 
			0x97, 0x86, 0xc1, 0x98, 0x70, 0xd3, 0x4d, 0xcf,
			0xaf, 0xda, 0x11, 0x3a, 0x80, 0xbc, 0xd1, 0x2a,
			0x4b, 0x1f, 0x9b, 0xd6, 0x4b, 0xa7, 0x4e, 0xa2,
			0x96, 0x41, 0xb1, 0x35, 0xfd, 0xc4, 0x27, 0x6b
		}; 

		unsigned char AES_Key16[16] = {
			0x00, 0xe0, 0x4c, 0x23, 0x47, 0xe7, 0x42, 0x5b, 0xbf, 0x8f, 0xfd, 0xd4, 0xff, 0xff, 0x00, 0x00
		};

		unsigned char PlainData[16] = {
			0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xff, 0xff, 0xed, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21
		};

		UINT8 TestData1[16] = {0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
			0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02};

		// 加密之后：
		UINT8 TestData2[16] = {0x23, 0xe3, 0x22, 0xcb, 0x91, 0x72, 0xc3, 0x2d,
			0x4f, 0xd8, 0x89, 0x30, 0x3, 0xe3, 0xfe, 0xe};

		//RCU回的数据
		UINT8 TestData3[16] = {0x40, 0x24, 0x29, 0x4e, 0xbc, 0x90, 0xd0, 0x8f,
			0x5b, 0x95, 0xb2, 0x39, 0xa8, 0xa0, 0x62, 0x19};

		//解密之后：
		UINT8 TestData4[16] = {0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03,
			0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03};

		
		{// AES 128
			unsigned char key[16];
			int n = 0;
			memcpy(key, AES_Key16, 16 + n * 8);
			aes_set_key( &ctx, key, 128 + n * 64);

			unsigned char PlainDataEncrypt[16];
			unsigned char PlainDataDecrypt[16];
			aes_encrypt(&ctx, PlainData, PlainDataEncrypt);
			aes_decrypt(&ctx, PlainDataEncrypt, PlainDataDecrypt);
		}

		{//AES 256
			// init Key
			unsigned char key[32];
			int n = 3 - 1;
			memcpy(key, AES_Key16, 16 + n * 8);
			aes_set_key( &ctx, key, 128 + n * 64);

			unsigned char TestData1_Enc[16];
			unsigned char TestData3_Des[16];
			aes_encrypt(&ctx, TestData1, TestData1_Enc);	// TestData1_Enc ?= TestData2 
			aes_decrypt(&ctx, TestData3, TestData3_Des);	// TestData3_Des ?= TestData4 

	#define random(x) (rand()%x)	//产生[0, x)区间内的随机整数
			unsigned char RandData[16];
			unsigned char RandData_Enc[16];
			unsigned char RandData_Des[16];
			srand((unsigned)time(NULL));	//设置当前时间为随机种子

			// 产生16个随机数
			for( int i = 0;  i < 16; i++ )
				RandData[i] = random(255);
			aes_encrypt(&ctx, RandData, RandData_Enc);		// 
			aes_decrypt(&ctx, RandData_Enc, RandData_Des);	// RandData ?= RandData_Des 

			for(int i=0; i<16; i++)
			{
				if(RandData[i] != (RandData_Des[i]))
				{
					DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("Rand Data Compare Fail")));
				}
			}
		}
	}
}

int CHWCDlg::LoadDecoder(CString DecoderFilePath)
{
	HINSTANCE hDll = AfxLoadLibrary(DecoderFilePath);
	if(NULL == hDll)
	{
		MessageBox(_T("Load Decoder Dll Fail"));
		return 0;
	}

	//查找DecoderFunc函数得到算法描述
	m_DecoderFunc = (AUDIO_DECODER)GetProcAddress(hDll,"AudioDecoder");
	m_PlayFunc = (AUDIO_PLAY)GetProcAddress(hDll,"AudioPlay");
	if(m_DecoderFunc && m_PlayFunc)
	{
		return 1; // Load OK
	}
	else
	{
		MessageBox(_T("Load Decoder Func Fail"));
		AfxFreeLibrary(hDll);
		return 0; // Load Fail
	}
}

/**
*	wave play direct
*/
int CHWCDlg::WavePlayStart()
{
	/*
	* Initialise the module variables
	*/
	m_waveFreeBlockCount = BLOCK_COUNT;
	m_waveCurrentBlock = 0;

	/*
	* set up the WAVEFORMATEX structure.
	*/
	m_wfx.nSamplesPerSec = 16000; /* sample rate */
	m_wfx.wBitsPerSample = 16; /* sample size */
	m_wfx.nChannels = 1; /* channels*/
	m_wfx.cbSize = 0; /* size of _extra_ info */
	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_wfx.nBlockAlign = (m_wfx.wBitsPerSample * m_wfx.nChannels) >> 3;
	m_wfx.nAvgBytesPerSec = m_wfx.nBlockAlign * m_wfx.nSamplesPerSec;

	/*
	* try to open the default wave device. WAVE_MAPPER is
	* a constant defined in mmsystem.h, it always points to the
	* default wave device on the system (some people have 2 or
	* more sound cards).
	*/
	if (waveOutOpen(
		&m_hWaveOut,
		WAVE_MAPPER,
		&m_wfx,
		(DWORD_PTR)WaveOutProc,
		(DWORD_PTR)&m_waveFreeBlockCount,
		CALLBACK_FUNCTION
		) != MMSYSERR_NOERROR)
	{
		DEBUG_TRACE(TRACE_LEVEL_ERROR, RTK_LE_PROFILE_RCU, (_T("waveOutOpen error")));
		return 0;
	}

	m_msbc_decode_parameter.sbc = &m_sbc;
	m_msbc_decode_parameter.sbc->bitpool = 26;
	m_msbc_decode_parameter.sbc->blocks = 15;
	m_msbc_decode_parameter.sbc->channels = 1;
	m_msbc_decode_parameter.sbc->joint = 0;
	m_msbc_decode_parameter.sbc->subbands = 8;
	m_msbc_decode_parameter.sbc->rate = 16000;
	sbc_init(m_msbc_decode_parameter.sbc,0);

	m_msbc_decode_parameter.input_len = MSBCFRAMESIZE - 3;
	m_msbc_decode_parameter.output_len = PCMFRAMESIZE;

	CString sbcFilePath;
	sbcFilePath = g_WorkDir + MSBC_DECODER_PATH;
	CFile sbcfile(sbcFilePath, CFile::modeCreate | CFile::modeReadWrite);
	sbcfile.Close();

	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("WavePlayStart OK")));

	return 1;
}

int CHWCDlg::WavePlayStop()
{
	while (m_waveFreeBlockCount < BLOCK_COUNT)
		Sleep(1000);
	/*
	* unprepare any blocks that are still prepared
	*/
	for (int i = 0; i < m_waveFreeBlockCount; i++)
	{
		if (m_waveBlocks[i].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(m_hWaveOut, &m_waveBlocks[i], sizeof(WAVEHDR));
	}

	waveOutClose(m_hWaveOut);
	return 1;
}

void CHWCDlg::WaveOutProc(
	 HWAVEOUT hWaveOut,
	 UINT uMsg,
	 DWORD dwInstance,
	 DWORD dwParam1,
	 DWORD dwParam2
	 )
{
	/*
	* pointer to free block counter
	*/
	int* freeBlockCounter = (int*)dwInstance;

	switch(uMsg)
	{
	case WOM_DONE:
		{
			EnterCriticalSection(&(pThis->m_waveCriticalSection));
			(*freeBlockCounter)++;
			LeaveCriticalSection(&(pThis->m_waveCriticalSection));
		}
		break;
	case WOM_OPEN:
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Wave audio device Open OK")));
		}
		break;
	case WOM_CLOSE:
		{
			DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Wave audio device Close OK")));
		}
		break;
	default:
		break;
	}
}


void CHWCDlg::WaveWriteAudio(LPSTR data, int size)
{
	// decode mSBC to PCM
	unsigned char *pcmbuffer = NULL;
	unsigned char *sbcbuffer = NULL;
	pcmbuffer = (unsigned char *)malloc(PCMFRAMESIZE);	
	ULONGLONG frame_count = size / MSBCFRAMESIZE;

	int written = 0;
	m_msbc_decode_parameter.written = &written;

	for (int i=0;i<frame_count;i++)
	{
		int decoded = 0;
		unsigned char *sbcbuffer = (unsigned char*)data + MSBCFRAMESIZE * i;
		m_msbc_decode_parameter.input =  (void *)((BYTE *)sbcbuffer + 2);	//?
		m_msbc_decode_parameter.output = pcmbuffer;
		int ret = sbc_decode(m_msbc_decode_parameter.sbc,
			m_msbc_decode_parameter.input,
			m_msbc_decode_parameter.input_len,
			m_msbc_decode_parameter.output,
			m_msbc_decode_parameter.output_len,
			m_msbc_decode_parameter.written);	// smbc decode
		if (m_msbc_decode_parameter.output_len != *(m_msbc_decode_parameter.written) || written != PCMFRAMESIZE)
		{
			DEBUG_TRACE(TRACE_LEVEL_VERBOSE, RTK_LE_PROFILE_RCU, (_T("Error -> msbc decode")));
			free(pcmbuffer);
			return;
		}

		CString sbcFilePath;
		sbcFilePath = g_WorkDir + MSBC_DECODER_PATH;
		CFile sbcfile(sbcFilePath, CFile::modeReadWrite);
		sbcfile.SeekToEnd();
		sbcfile.Write(pcmbuffer, written);
		sbcfile.Close();

		if(1)
		{
			WAVEHDR* current;
			int remain;
			current = &m_waveBlocks[m_waveCurrentBlock];
			unsigned char *pcmOut = pcmbuffer;
			int pcmSize = PCMFRAMESIZE;
			while (pcmSize > 0) {
				/*
				* first make sure the header we're going to use is unprepared
				*/
				if (current->dwFlags & WHDR_PREPARED)
					waveOutUnprepareHeader(m_hWaveOut, current, sizeof(WAVEHDR));

				if (pcmSize < (int)(BLOCK_SIZE - current->dwUser)) {
					memcpy(current->lpData + current->dwUser, pcmOut, pcmSize);
					current->dwUser += pcmSize;
					break;
				}
				remain = BLOCK_SIZE - current->dwUser;
				memcpy(current->lpData + current->dwUser, pcmOut, remain);
				pcmSize -= remain;
				pcmOut += remain;
				current->dwBufferLength = BLOCK_SIZE;
				waveOutPrepareHeader(m_hWaveOut, current, sizeof(WAVEHDR));
				waveOutWrite(m_hWaveOut, current, sizeof(WAVEHDR));
				EnterCriticalSection(&m_waveCriticalSection);
				m_waveFreeBlockCount--;
				LeaveCriticalSection(&m_waveCriticalSection);
				/*
				* wait for a block to become free
				*/
				while (!m_waveFreeBlockCount)
					Sleep(100);
				/*
				* point to the next block
				*/
				m_waveCurrentBlock++;
				m_waveCurrentBlock %= BLOCK_COUNT;
				current = &m_waveBlocks[m_waveCurrentBlock];
				current->dwUser = 0;
			}
		}
	}

	free(pcmbuffer);
}

void CHWCDlg::OnBnClickedButtonRcuConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Device(_T("Start to connect...\n\n") + m_ConnectedDev);
	pThis->m_static.SetWindowText(Device);
	UpdateWindow();

	LEHIDSvr.SetConnectMode(0, m_MPConfig);
	::PostMessage(m_hWnd, WM_LEDEV_CONNECT, RCU_CONNECTION, (LPARAM)&m_LeDevAddress);
}

void CHWCDlg::OnBnClickedButtonOtaConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	//CString Device(_T("Start to connect...\n\n") + m_ConnectedDev);
	//pThis->m_static.SetWindowText(Device);
	//UpdateWindow();
	//::PostMessage(m_hWnd, WM_LEDEV_CONNECT, OTA_CONNECTION, (LPARAM)&m_LeDevAddress);	//
	LEHIDSvr.EnterOTAMode();
}


void CHWCDlg::OnBnClickedButtonLoadpatch()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePath;
	CFileDialog dlg(TRUE, L"*", L"*.*");	///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePath=dlg.GetPathName();
		ShowEditPrintInfo(FilePath, 0);
		dfu.OTALoadFWImageFromFile(FilePath);
		CString ImgInfo;
		ImgInfo.Format(_T("Local Patch Info: \r\n\toffset(0x%x)\r\n\tsignature(0x%x)\r\n\tversion(0x%x)\r\n\tchecksum(0x%x)\r\n\tlength(0x%x)\r\n\tota_flag(0x%x)\r\n\treserved_8(0x%x)"), 
			dfu.ImgHeader.offset, dfu.ImgHeader.signature, dfu.ImgHeader.version, dfu.ImgHeader.checksum, dfu.ImgHeader.length, dfu.ImgHeader.ota_flag, dfu.ImgHeader.reserved_8);
		ShowEditPrintInfo(ImgInfo, 0);
		m_UpdateButton.EnableWindow(TRUE);
		UpdateWindow();
	}
}

void CHWCDlg::OnBnClickedButtonGetRemoteinfo()
{
	// TODO: 在此添加控件通知处理程序代码
	dfu.OTAGetTargetImageInfo();
	CString TargetImgInfo;
	TargetImgInfo.Format(_T("Remote Target Patch Info: \r\n\tOriginalFWVersion(0x%x)\r\n\tnImageUpdateOffset(0x%x)"), 
		dfu.m_DfuDataInfo.CurInfo.OriginalVersion, dfu.m_DfuDataInfo.CurInfo.ImageUpdateOffset);
	ShowEditPrintInfo(TargetImgInfo, 0);
}

void CHWCDlg::OnBnClickedButtonDownloadPatch()
{
	// TODO: 在此添加控件通知处理程序代码
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("start to push image to remote le device...")));
	CString DownloadInfo;
	DownloadInfo.Format(L"Start to download patch to le device...");
	ShowEditPrintInfo(DownloadInfo, 0);
	m_OTAProcess.ShowWindow(TRUE);
	m_ButtonState.ShowWindow(FALSE);
	dfu.OTAStartDFU();
	if( RT_STATUS_SUCCESS == dfu.OTAValidFW())
	{
		DownloadInfo.Format(L"Patch Download Successfully");
	}
	else
	{
		DownloadInfo.Format(L"Patch Download Fail");
	}
	ShowEditPrintInfo(DownloadInfo, 0);
	m_OTAProcess.ShowWindow(FALSE);
	m_ButtonState.ShowWindow(TRUE);
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_DFU, (_T("operation end:")));
}

void CHWCDlg::OnBnClickedButtonActiveFw()
{
	// TODO: 在此添加控件通知处理程序代码
	if( RT_STATUS_SUCCESS == dfu.OTAActiveAndReset())
	{// result to disconnect
		ShowEditPrintInfo(CString(L"Active new patch successfully"), 0);
	}
	else
	{
		ShowEditPrintInfo(CString(L"Active new patch fail"), 0);
	}
	ShowEditPrintInfo(CString(L"Le Link Disconnect"), 0);
	m_RCUConn.EnableWindow(FALSE);
	m_ScanState.EnableWindow(TRUE);
}

void CHWCDlg::OnBnClickedButtonDoAll()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowEditPrintInfo(CString(L"Start DFU procedure"), 0);
	OnBnClickedButtonGetRemoteinfo();
	OnBnClickedButtonDownloadPatch();
	OnBnClickedButtonActiveFw();
	ShowEditPrintInfo(CString(L"DFU procedure over"), 0);
}

afx_msg LRESULT CHWCDlg::OnOTAPatchDownloadProcess(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码
	int i = (int)wParam;
	int Count = (int)lParam;

	if(i == 0)
	{
		m_OTAProcess.SetRange32(0, Count);
		m_OTAProcess.SetStep(0);
		CString DLInfo;
		DLInfo.Format(_T("Start to download...  \n"));
		m_static.SetWindowText(DLInfo+m_OTAConnectedDev);
	}
	else if( i < Count )
	{
		m_OTAProcess.SetPos(i);
		//m_OTAProcess.StepIt();
	}
	else if( i >= Count )
	{
		m_OTAProcess.SetPos(Count);
	}
	
	//CString DLInfo;
	//DLInfo.Format(_T("i(%d), Count(%d)"), i, Count);
	//ShowEditPrintInfo(DLInfo, 0);
	Sleep(1);	// must have it
	return 0;
}
void CHWCDlg::OnBnClickedOpenConfigPath()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, L"explore", g_WorkDir, NULL, NULL, SW_SHOWNORMAL);
}


void CHWCDlg::OnBnClickedButtonEnableOtaMode()
{
	// TODO: 在此添加控件通知处理程序代码
	RT_STATUS Status = RT_STATUS_SUCCESS;
	Status = LEHIDSvr.EnterOTAMode(); 
	if (LE_SUCCESS == Status)
	{
		//OnBnClickedStop();  // must disconnect the link
		ShowEditPrintInfo(CString(L"Device Enter OTA Mode"), 0);
		ShowEditPrintInfo(CString(L"Scan the device..."), 0);
		int nRetCode = 0;
		CString OTA_BDAddrStr;
		CLEScanWinDlg* Scandlg = new CLEScanWinDlg(1, m_LeDevAddress);
		if(IDOK == Scandlg->DoModal())
		{			
			m_Start.EnableWindow(False);
			m_Start.ShowWindow(False);
			m_static.ShowWindow(True);
			m_ButtonShow.SetWindowText(L"");

			//OnBnClickedStop();// disconnect the original link

			OTA_BDAddrStr = Scandlg->m_BDAddrStr;
			m_LeOTAMAddress = Scandlg->m_BDAddress;

			m_OTAConnectedDev = _T("BT Address:              ") + OTA_BDAddrStr + _T("\nNormal Address:         ") + m_BDAddrStr+ _T("\nNormal Name:            ") + m_BTDevNameStr ;
			CString Device(_T("Device in OTA mode, connect...\n") + m_OTAConnectedDev);
			pThis->m_static.SetWindowText(Device);
			UpdateWindow();
			m_RCUConn.EnableWindow(FALSE);
			
			{// add register
				CRegKey LeDeviceKey;
				CString BDAddressPath = SERVICE_LE_DATABASE_KEY + OTA_BDAddrStr;
				int RetValue = LeDeviceKey.Create(HKEY_LOCAL_MACHINE, SERVICE_LE_DATABASE_KEY);
				if( RetValue == ERROR_SUCCESS )
				{// Todo: need to delete the register info after OTA mode is off
					RetValue = LeDeviceKey.Create(HKEY_LOCAL_MACHINE, BDAddressPath);
					if( RetValue == ERROR_SUCCESS )
					{
						LeDeviceKey.Close();
						RetValue = LeDeviceKey.Open(HKEY_LOCAL_MACHINE, BDAddressPath);
						if( RetValue == ERROR_SUCCESS )
						{
							RetValue = LeDeviceKey.SetStringValue(_T("DevName"), CString(L"MP Mode Device"));
							RetValue = LeDeviceKey.SetDWORDValue(_T("SecLevel"), 0);	// Security Level = 0
							RetValue = LeDeviceKey.SetDWORDValue(_T("PeerAddrType"), 1);	// Address Type = Random
							RetValue = LeDeviceKey.SetDWORDValue(_T("bSupportBond"), 0);	// Not support bond
						}
						LeDeviceKey.Close();
						::PostMessage(m_hWnd, WM_LEDEV_CONNECT, OTA_CONNECTION, (LPARAM)&m_LeOTAMAddress);
					}
				}
			}
		}
	}
}

void CHWCDlg::OnBnClickedMpModeAutoConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	// Auto Active Scan
	m_MPBDAddressEdit.GetWindowText(m_MPConfig.BDAdress);
	m_MPLinkKeyEdit.GetWindowText(m_MPConfig.LinkKey);
	if(m_MPConfig.BDAdress.IsEmpty() || m_MPConfig.LinkKey.IsEmpty() )
	{
		MessageBox(L"BDAddress or Link Key should not be Empty");
		return;
	}

	CRegKey LeDeviceKey;
	CString BDAddressPath = SERVICE_LE_DATABASE_KEY + m_MPConfig.BDAdress;
	int RetValue = LeDeviceKey.Create(HKEY_LOCAL_MACHINE, SERVICE_LE_DATABASE_KEY);
	if( RetValue == ERROR_SUCCESS )
	{
		RetValue = LeDeviceKey.Create(HKEY_LOCAL_MACHINE, BDAddressPath);
		if( RetValue == ERROR_SUCCESS )
		{
			LeDeviceKey.Close();
			UCHAR RandNum[8] = {0}; // Rand
			UCHAR LtkValue[16] = {0};	// LTK
			{// change Ltk from string to binary
				int nLength = m_MPConfig.LinkKey.GetLength();
				int nBytes = WideCharToMultiByte(CP_ACP,0,m_MPConfig.LinkKey,nLength,NULL,0,NULL,NULL);
				char* Linktemp = new char[ nBytes + 1];
				memset(Linktemp,0,nLength + 1);
				WideCharToMultiByte(CP_OEMCP, 0, m_MPConfig.LinkKey, nLength, Linktemp, nBytes, NULL, NULL);
				Linktemp[nBytes] = 0; 
				if(nLength != 32)
				{
					MessageBox(L"Link Key should be 16 Bytes!!!");
					return;
				}
				int i = 0;
				for(i = 0 ; i < nLength ; i++)
				{
					UCHAR tempByte = Linktemp[i];
					UCHAR temp = 0;
					if(tempByte >= '0' && tempByte <= '9')
						temp = tempByte - '0';
					else if( tempByte >= 'a' && tempByte <= 'f' )
						temp = tempByte - 'a' + 10;
					else if( tempByte >= 'A' && tempByte <= 'F' )
						temp = tempByte - 'A' + 10;
					if( i%2 == 0)
					{
						LtkValue[i/2] = temp;
					}
					else
					{
						LtkValue[i/2] = (LtkValue[i/2]<<4) + temp;
					}
					
				}
			}
			{// generate Rand numbers
				srand((unsigned)time(NULL)); 
				for(int i = 0; i < 8;i++ ) 
					RandNum[i] = rand() % 256;  // rand number between [0-256)
			}
			RetValue = LeDeviceKey.Open(HKEY_LOCAL_MACHINE, BDAddressPath);
			if( RetValue == ERROR_SUCCESS )
			{
				RetValue = LeDeviceKey.SetStringValue(_T("DevName"), CString(L"MP Mode Device"));
				RetValue = LeDeviceKey.SetDWORDValue(_T("SecLevel"), 1);
				RetValue = LeDeviceKey.SetDWORDValue(_T("PeerAddrType"), 0);
				RetValue = LeDeviceKey.SetDWORDValue(_T("bSupportBond"), 1);
				RetValue = LeDeviceKey.SetDWORDValue(_T("Ediv"), 0);
				RetValue = LeDeviceKey.SetBinaryValue(_T("Rand"), &RandNum, sizeof(RandNum));
				RetValue = LeDeviceKey.SetBinaryValue(_T("Ltk"), &LtkValue, sizeof(LtkValue));
				if( RetValue == ERROR_SUCCESS )
				{
					m_LEDevConnected = FALSE;
					m_MPModeEnable = TRUE;
					
					ShowEditPrintInfo(CString(L"MP Mode Start..."), 0);
					ShowEditPrintInfo(m_BDAddrStr, 0);
					ShowEditPrintInfo(m_MPConfig.LinkKey, 0);

					m_Start.EnableWindow(False);
					m_Start.ShowWindow(False);
					m_static.ShowWindow(True);
					m_ButtonShow.SetWindowText(L"");

					swscanf_s((LPCTSTR)m_MPConfig.BDAdress, L"%I64x", &m_LeDevAddress);
					m_ConnectedDev = _T("BT Address:              ") + m_MPConfig.BDAdress + _T("\nDevice Name:            MP Mode Device");
					CString Device(_T("Scan the device in MP Mode...\n\n\n") + m_ConnectedDev);
					pThis->m_static.SetWindowText(Device);
					UpdateWindow();
					
					ShowEditPrintInfo(CString(_T("MP Mode Start")), 0);
					ShowEditPrintInfo(CString(_T("Addr:"))+m_MPConfig.BDAdress, 0);
					ShowEditPrintInfo(CString(_T("PeerAddrType:"))+CString(L"0"), 0);
					ShowEditPrintInfo(CString(_T("Ltk :"))+m_MPConfig.LinkKey, 0);
					ShowEditPrintInfo(CString(_T("Ediv:"))+CString(L"0"), 0);
					ShowEditPrintInfo(CString(_T("SecLevel:"))+CString(L"1"), 0);
					ShowEditPrintInfo(CString(_T("bSupportBond:"))+CString(L"1"), 0);

					ResetEvent(m_hScanEvent);
					if(!m_hLEScanDeviceThread)
					{
						m_MPModeStop = FALSE;
						m_hLEScanDeviceThread = CreateThread(
							NULL,
							0,
							s_LEScanDeviceThreadProc,
							(LPVOID)this,
							0,
							NULL);
						if (NULL != m_hLEScanDeviceThread)
						{
							CloseHandle(m_hLEScanDeviceThread);
							DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Start scan thread ++")));
							m_MPConnect.EnableWindow(FALSE);
							m_ScanState.EnableWindow(FALSE);
							m_RCUConn.EnableWindow(FALSE);
							m_DeviceSelected = FALSE;
						}
					}
				}
			}
			LeDeviceKey.Close();
		}
		else
		{
			LeDeviceKey.Close();
		}
		m_MPConnect.EnableWindow(FALSE);
	}
	else
	{
		MessageBox(L"Create Key Fail");
	}
}

void CHWCDlg::OnBnClickedClearLog()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_LEDevConnected)
	{
		::SendMessage(m_hWnd, WM_LEDEV_DISCONNECT, 0, 0);
	}
}

void CHWCDlg::OnBnClickedClearQuitMp()
{
	// TODO: 在此添加控件通知处理程序代码
	LEHIDSvr.QuiteTestMode();
}

void CHWCDlg::OnBnClickedButtonFindme()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_LEDevConnected) return;
	LEHIDSvr.AlertSetLevel(g_alertLevel);
	if(g_alertLevel)
	{
		g_alertLevel = 0;
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	}
	else
	{
		g_alertLevel = 1;
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
		m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	}
}

void CHWCDlg::OnBnClickedButtonVoicerecord()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_LEDevConnected) return;
	if(g_bVoice)
	{
		g_bVoice = FALSE;
		OnCustomeMicdown(1, NULL);
	}
	else
	{
		g_bVoice = TRUE;
		OnCustomeMicdown(0, NULL);
	}
}


void CHWCDlg::OnBnClickedButtonAddr1()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_bModeSel) return;
	g_bModeSel = TRUE;

	CString mpAddress(MP_ADDRESS_01);
	CString mpLinkKey(MP_LINK_KEY);
	m_MPBDAddressEdit.SetWindowText(mpAddress);
	m_MPLinkKeyEdit.SetWindowText(mpLinkKey);
	SetSTButtonColor(255, 0, 0);
	
	//GetDlgItem(IDC_BUTTON_ADDR1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR5)->EnableWindow(FALSE);
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	m_MPModeSelect.SetWindowText(L"1");

	LEHIDSvr.SetConnectMode(1, m_MPConfig);
	OnBnClickedMpModeAutoConnect();
}


void CHWCDlg::OnBnClickedButtonAddr2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_bModeSel) return;
	g_bModeSel = TRUE;

	CString mpAddress(MP_ADDRESS_02);
	CString mpLinkKey(MP_LINK_KEY);
	m_MPBDAddressEdit.SetWindowText(mpAddress);
	m_MPLinkKeyEdit.SetWindowText(mpLinkKey);
	SetSTButtonColor(255, 0, 0);

	GetDlgItem(IDC_BUTTON_ADDR1)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_ADDR2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR5)->EnableWindow(FALSE);
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	m_MPModeSelect.SetWindowText(L"2");

	LEHIDSvr.SetConnectMode(1,m_MPConfig);
	OnBnClickedMpModeAutoConnect();
}


void CHWCDlg::OnBnClickedButtonAddr3()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_bModeSel) return;
	g_bModeSel = TRUE;

	CString mpAddress(MP_ADDRESS_03);
	CString mpLinkKey(MP_LINK_KEY);
	m_MPBDAddressEdit.SetWindowText(mpAddress);
	m_MPLinkKeyEdit.SetWindowText(mpLinkKey);
	SetSTButtonColor(255, 0, 0);

	GetDlgItem(IDC_BUTTON_ADDR1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR2)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_ADDR3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR5)->EnableWindow(FALSE);
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	m_MPModeSelect.SetWindowText(L"3");

	LEHIDSvr.SetConnectMode(1, m_MPConfig);
	OnBnClickedMpModeAutoConnect();
}

void CHWCDlg::OnBnClickedButtonAddr4()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_bModeSel) return;
	g_bModeSel = TRUE;

	CString mpAddress(MP_ADDRESS_04);
	CString mpLinkKey(MP_LINK_KEY);
	m_MPBDAddressEdit.SetWindowText(mpAddress);
	m_MPLinkKeyEdit.SetWindowText(mpLinkKey);
	SetSTButtonColor(255, 0, 0);

	GetDlgItem(IDC_BUTTON_ADDR1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR3)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_ADDR4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR5)->EnableWindow(FALSE);
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	m_MPModeSelect.SetWindowText(L"4");

	LEHIDSvr.SetConnectMode(1, m_MPConfig);
	OnBnClickedMpModeAutoConnect();
}

void CHWCDlg::OnBnClickedButtonAddr5()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_bModeSel) return;
	g_bModeSel = TRUE;

	CString mpAddress(MP_ADDRESS_05);
	CString mpLinkKey(MP_LINK_KEY);
	m_MPBDAddressEdit.SetWindowText(mpAddress);
	m_MPLinkKeyEdit.SetWindowText(mpLinkKey);
	SetSTButtonColor(255, 0, 0);

	GetDlgItem(IDC_BUTTON_ADDR1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADDR4)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_ADDR5)->EnableWindow(FALSE);
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(0,255,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	m_MPModeSelect.SetWindowText(L"5");

	LEHIDSvr.SetConnectMode(1, m_MPConfig);
	OnBnClickedMpModeAutoConnect();
}

void CHWCDlg::SetSTButtonColor(int Red, int Green, int Blue)
{
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(Red,Green,Blue));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(Red,Green,Blue));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(Red,Green,Blue));
	m_LogClear.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));

	m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(Red,Green,Blue));
	m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(Red,Green,Blue));
	m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_AlertButton.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(Red,Green,Blue));
	m_AlertButton.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));

	m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(Red,Green,Blue));
	m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(Red,Green,Blue));
	m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(Red,Green,Blue));
	m_VoiceButton.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,Blue));
}

CDisplayView * CHWCDlg::AddRootView()
{
	ASSERT(splitter!=NULL);
	CWnd *pPicWnd=GetDlgItem(IDC_WAVE_SHOW);
	CRect rect;
	pPicWnd->GetWindowRect(&rect);	//获取控件的屏幕坐标
	ScreenToClient(&rect);			//转换为对话框上的客户坐标

	// create wave view
	CString PathName; 
	PathName = g_WorkDir + SBC_WAVE_PATH;
	DeleteFile(PathName);
	m_pContext.m_pCurrentDoc = new CWaveDoc(PathName);

	m_pContext.m_pNewViewClass = RUNTIME_CLASS(CWaveView);
	m_pView =(CWaveView *) ((CFrameWnd*)this)->CreateView(&m_pContext);		
	m_pView->m_Split=&m_WaveShow;
	m_pView->MoveWindow(rect);

	return NULL;
}

void CHWCDlg::MPModeReset()
{
	// TODO: 在此添加控件通知处理程序代码
	CString mpAddress(MP_ADDRESS_01);
	CString mpLinkKey(MP_LINK_KEY);
	m_MPBDAddressEdit.SetWindowText(L"");
	m_MPLinkKeyEdit.SetWindowText(L"");
	SetSTButtonColor(255, 0, 0);
	m_MPConnect.EnableWindow(FALSE);
	m_AlertButton.EnableWindow(FALSE);
	m_VoiceButton.EnableWindow(FALSE);
	m_LogClear.EnableWindow(TRUE);
	g_bModeSel = FALSE;

	GetDlgItem(IDC_BUTTON_ADDR1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ADDR2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ADDR3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ADDR4)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ADDR5)->EnableWindow(TRUE);
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(255,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(255,0,0));
	m_MPModeSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0,0,0));
	m_MPModeSelect.SetWindowText(L"");

	LEHIDSvr.SetConnectMode(0, m_MPConfig);
}

BOOL CHWCDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_NUMPAD0:
		case VK_LEFT:
			{
				OnBnClickedButtonFindme();
				ShowEditPrintInfo(CString(L"Findme Enable/Disable"), 0);
			}
			break;
		case VK_DECIMAL:
		case VK_RIGHT:
			{
				OnBnClickedButtonVoicerecord();
				ShowEditPrintInfo(CString(L"Voice Record Enable/Enable"), 0);
			}
			break;
		case VK_SUBTRACT:
		case VK_UP:
			{
				OnBnClickedClearLog();
				ShowEditPrintInfo(CString(L"Disconnect LE Link"), 0);
			}
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}