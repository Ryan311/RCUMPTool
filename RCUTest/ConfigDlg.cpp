// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "ConfigDlg.h"
#include "ConfigFile.h"
#include "DFU_Def.h"
#include "LEScanWinDlg.h"
#include "KeyMapDlg.h"
#include "trace.h"

extern CString g_ModuleDir;

// CConfigDlg 对话框
IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
	, m_KeyPower(_T(""))
	, m_KeyHome(_T(""))
	, m_KeyBack(_T(""))
	, m_KeyMenu(_T(""))
	, m_KeyAllUp(_T(""))
	, m_KeyUp(_T(""))
	, m_KeyDown(_T(""))
	, m_KeyRight(_T(""))
	, m_KeyLeft(_T(""))
	, m_KeyOk(_T(""))
	, m_KeyMic(_T(""))
	, m_KeyVolP(_T(""))
	, m_KeyVolM(_T(""))
	, m_DecoderStr(_T(""))
	, m_NewDecoder(_T(""))
	, m_VoiceOpt(0)
	, m_AlertHandleStr(_T(""))
	, m_SWHandleStr(_T(""))
	, m_FWHandleStr(_T(""))
	, m_HWHandleStr(_T(""))
	, m_KeyNHandleStr(_T(""))
	, m_VoiceNHandleStr(_T(""))
{

}

CConfigDlg::CConfigDlg(map<CString, CString> &UserDefinedKeyMap, 
					   map<BYTE, BYTE> &KeyMap, CStringArray &Decoders, 
					   CString &CurDecoder, int VoicePlayMode,  ULONGLONG LeDevAddress, 
					   bool ConnectState, MP_CONFIG &MPConfig, CWnd* pParent/*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	m_KeyPower.Format(L"0x%02x", KeyMap[MAP_POWERKEY]);
	m_KeyMic.Format(L"0x%02x", KeyMap[MAP_MICKEY]);
	m_KeyUp.Format(L"0x%02x", KeyMap[MAP_UPKEY]);
	m_KeyDown.Format(L"0x%02x", KeyMap[MAP_DOWNKEY]);
	m_KeyRight.Format(L"0x%02x", KeyMap[MAP_RIGHTKEY]);
	m_KeyLeft.Format(L"0x%02x", KeyMap[MAP_LEFTKEY]);
	m_KeyHome.Format(L"0x%02x", KeyMap[MAP_HOMEKEY]);
	m_KeyMenu.Format(L"0x%02x", KeyMap[MAP_MENUKEY]);
	m_KeyOk.Format(L"0x%02x", KeyMap[MAP_OKKEY]);
	m_KeyVolM.Format(L"0x%02x", KeyMap[MAP_VOLDOWN]);
	m_KeyVolP.Format(L"0x%02x", KeyMap[MAP_VOLUP]);
	m_KeyBack.Format(L"0x%02x", KeyMap[MAP_BACKKEY]);
	m_KeyAllUp.Format(L"0x%02x", KeyMap[MAP_ALLKEYUP]);
	m_Decoders.Copy(Decoders);
	m_DecoderStr = CurDecoder;
	m_CurKeyMap = KeyMap;
	m_UserDefineKeyMap = UserDefinedKeyMap;
	m_VoiceOpt = VoicePlayMode;
	m_ConnectState = ConnectState;
	m_LeDevAddress = LeDevAddress;

	m_AlertHandleStr = MPConfig.AlertHandleStr;
	m_SWHandleStr = MPConfig.SWHandleStr;
	m_FWHandleStr = MPConfig.FWHandleStr;
	m_HWHandleStr = MPConfig.HWHandleStr;
	m_KeyNHandleStr = MPConfig.KeyNHandleStr;
	m_VoiceNHandleStr = MPConfig.VoiceNHandleStr;
}

CConfigDlg::~CConfigDlg()
{
}

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	for( int i = 0; i < m_Decoders.GetSize() ; i++)
	{
		m_ComboBoxDecoder.AddString(m_Decoders[i]);	
	}
	// 总是向前插入
	m_ComboBoxDecoder.SetWindowText(m_DecoderStr);

	return TRUE;
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POWER, m_EditPower);
	DDX_Control(pDX, IDC_EDIT_HOME, m_EditHome);
	DDX_Control(pDX, IDC_EDIT_BACK, m_EditBack);
	DDX_Control(pDX, IDC_EDIT_MENU, m_EditMenu);
	DDX_Control(pDX, IDC_EDIT_KEYUP, m_EditKeyUp);
	DDX_Control(pDX, IDC_EDIT_UP, m_EditUp);
	DDX_Control(pDX, IDC_EDIT_DOWN, m_EditDown);
	DDX_Control(pDX, IDC_EDIT_RIGHT, m_EditRight);
	DDX_Control(pDX, IDC_EDIT_LEFT, m_EditLeft);
	DDX_Control(pDX, IDC_EDIT_OK, m_EditOK);
	DDX_Control(pDX, IDC_EDIT_MIC, m_EditMic);
	DDX_Control(pDX, IDC_EDIT_VOLP, m_EditVolP);
	DDX_Control(pDX, IDC_EDIT_VOLM, m_EditVolM);
	DDX_Text(pDX, IDC_EDIT_POWER, m_KeyPower);
	DDX_Text(pDX, IDC_EDIT_HOME, m_KeyHome);
	DDX_Text(pDX, IDC_EDIT_BACK, m_KeyBack);
	DDX_Text(pDX, IDC_EDIT_MENU, m_KeyMenu);
	DDX_Text(pDX, IDC_EDIT_KEYUP, m_KeyAllUp);
	DDX_Text(pDX, IDC_EDIT_UP, m_KeyUp);
	DDX_Text(pDX, IDC_EDIT_DOWN, m_KeyDown);
	DDX_Text(pDX, IDC_EDIT_RIGHT, m_KeyRight);
	DDX_Text(pDX, IDC_EDIT_LEFT, m_KeyLeft);
	DDX_Text(pDX, IDC_EDIT_OK, m_KeyOk);
	DDX_Text(pDX, IDC_EDIT_MIC, m_KeyMic);
	DDX_Text(pDX, IDC_EDIT_VOLP, m_KeyVolP);
	DDX_Text(pDX, IDC_EDIT_VOLM, m_KeyVolM);
	DDX_Control(pDX, IDC_COMBO_DECODER, m_ComboBoxDecoder);
	DDX_CBString(pDX, IDC_COMBO_DECODER, m_DecoderStr);
	DDX_Control(pDX, IDC_EDIT1, m_EditNewDecoder);
	DDX_Text(pDX, IDC_EDIT1, m_NewDecoder);
	DDX_Radio(pDX, IDC_RADIO_VOICE_OPTION1, m_VoiceOpt);
	DDX_Control(pDX, IDC_EDIT_ALERT, m_AlertValueHandle);
	DDX_Control(pDX, IDC_EDIT_DIS_SW, m_SWValueHandle);
	DDX_Control(pDX, IDC_EDIT_DIS_FW, m_FWValueHandle);
	DDX_Control(pDX, IDC_EDIT_DIS_HW, m_HWValueHandle);
	DDX_Text(pDX, IDC_EDIT_ALERT, m_AlertHandleStr);
	DDX_Text(pDX, IDC_EDIT_DIS_SW, m_SWHandleStr);
	DDX_Text(pDX, IDC_EDIT_DIS_FW, m_FWHandleStr);
	DDX_Text(pDX, IDC_EDIT_DIS_HW, m_HWHandleStr);
	DDX_Control(pDX, IDC_EDIT_KEY_NOTIFY, m_KeyNotifyHandleEdit);
	DDX_Control(pDX, IDC_EDIT_VOICE_NOTIFY, m_VoiceNotifyHandleEdit);
	DDX_Text(pDX, IDC_EDIT_KEY_NOTIFY, m_KeyNHandleStr);
	DDX_Text(pDX, IDC_EDIT_VOICE_NOTIFY, m_VoiceNHandleStr);
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CConfigDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CConfigDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD_DECODER, &CConfigDlg::OnBnClickedButtonAddDecoder)
	ON_BN_CLICKED(IDC_RADIO_VOICE_OPTION1, &CConfigDlg::OnBnClickedRadioVoiceOption)
	ON_BN_CLICKED(IDC_RADIO_VOICE_OPTION2, &CConfigDlg::OnBnClickedRadioVoiceOption)
	ON_BN_CLICKED(IDC_RADIO_VOICE_OPTION3, &CConfigDlg::OnBnClickedRadioVoiceOption)
	ON_BN_CLICKED(IDC_BUTTON_MORE_KEY, &CConfigDlg::OnBnClickedButtonMoreKey)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, &CConfigDlg::OnBnClickedButtonDefault)
	ON_CBN_SELCHANGE(IDC_COMBO_DECODER, &CConfigDlg::OnCbnSelchangeComboDecoder)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序
void CConfigDlg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
	CFileDialog dlg(FALSE, L"ini", L"*.ini");	///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		MP_CONFIG MpConfig;
		MpConfig.bFlag = 0;
		WriteCinfigToIni(FilePathName, m_CurKeyMap, m_UserDefineKeyMap, m_DecoderStr, m_VoiceOpt, MpConfig);
		MessageBox(L"Save Current Configuration to File: " + FilePathName);
	}
}

void CConfigDlg::OnBnClickedButtonLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE, L"ini", L"*.ini");	///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		int bHandleShake;
		MP_CONFIG MpConfig;
		ReadCinfigFromIni(FilePathName, m_CurKeyMap, m_UserDefineKeyMap, m_DecoderStr, bHandleShake, m_VoiceOpt, MpConfig);
		MessageBox(L"Load Configuration From File: " + FilePathName);
	}
}

void CConfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	
	int numZero = 0;
	if(m_KeyPower.IsEmpty())
		m_KeyPower.Format(L"0x%x", numZero);
	if(m_KeyHome.IsEmpty())
		m_KeyHome.Format(L"0x%x", numZero);
	if(m_KeyOk.IsEmpty())
		m_KeyOk.Format(L"0x%x", numZero);
	if(m_KeyUp.IsEmpty())
		m_KeyUp.Format(L"0x%x", numZero);
	if(m_KeyDown.IsEmpty())
		m_KeyDown.Format(L"0x%x", numZero);
	if(m_KeyRight.IsEmpty())
		m_KeyRight.Format(L"0x%x", numZero);
	if(m_KeyLeft.IsEmpty())
		m_KeyLeft.Format(L"0x%x", numZero);
	if(m_KeyBack.IsEmpty())
		m_KeyBack.Format(L"0x%x", numZero);
	if(m_KeyMenu.IsEmpty())
		m_KeyMenu.Format(L"0x%x", numZero);
	if(m_KeyMic.IsEmpty())
		m_KeyMic.Format(L"0x%x", numZero);
	if(m_KeyVolP.IsEmpty())
		m_KeyVolP.Format(L"0x%x", numZero);
	if(m_KeyVolM.IsEmpty())
		m_KeyVolM.Format(L"0x%x", numZero);
	if(m_KeyAllUp.IsEmpty())
		m_KeyAllUp.Format(L"0x%2x", numZero);

	UpdateData(false);
	OnOK();
}

void CConfigDlg::OnBnClickedButtonAddDecoder()
{
	// TODO: 在此添加控件通知处理程序代码
	m_EditNewDecoder.GetWindowText(m_NewDecoder);
	if(!m_NewDecoder.IsEmpty())
	{
		m_ComboBoxDecoder.AddString(m_NewDecoder);
		m_EditNewDecoder.SetWindowText(L"");
		m_Decoders.Add(m_NewDecoder);
	}
}


void CConfigDlg::OnBnClickedRadioVoiceOption()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch(m_VoiceOpt)
	{
	case 0:
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Record voice to file")));
		break;
	case 1:
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Record voice to file and then play")));
		break;
	case 2:
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Derect to play")));
		break;
	}
}

void CConfigDlg::OnBnClickedButtonMoreKey()
{
	// TODO: 在此添加控件通知处理程序代码
	CKeyMapDlg* keyMapDlg = new CKeyMapDlg(m_UserDefineKeyMap);
	if(IDOK == keyMapDlg->DoModal())
	{
		m_UserDefineKeyMap = keyMapDlg->m_KeyMap;
	}
}

void CConfigDlg::OnBnClickedButtonDefault()
{
	// TODO: 在此添加控件通知处理程序代码
	m_UserDefineKeyMap.clear();
	m_CurKeyMap.clear();
	m_DecoderStr = L"";
	m_VoiceOpt = 0;

	CString ModulePathName;
	ModulePathName = g_ModuleDir + CONFIG_FILE_PATH;

	if(PathFileExists(ModulePathName))
	{
		int bHandleShake;
		MP_CONFIG MpConfig;
		ReadCinfigFromIni(ModulePathName, m_CurKeyMap, m_UserDefineKeyMap, m_DecoderStr, bHandleShake, m_VoiceOpt, MpConfig);
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Read configuration from Install dir")));
	}
	else	// config file not exist
	{
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Read configuration Fail")));
	}

	m_KeyPower.Format(L"0x%02x", m_CurKeyMap[MAP_POWERKEY]);
	m_KeyMic.Format(L"0x%02x", m_CurKeyMap[MAP_MICKEY]);
	m_KeyUp.Format(L"0x%02x", m_CurKeyMap[MAP_UPKEY]);
	m_KeyDown.Format(L"0x%02x", m_CurKeyMap[MAP_DOWNKEY]);
	m_KeyRight.Format(L"0x%02x", m_CurKeyMap[MAP_RIGHTKEY]);
	m_KeyLeft.Format(L"0x%02x", m_CurKeyMap[MAP_LEFTKEY]);

	m_KeyHome.Format(L"0x%02x", m_CurKeyMap[MAP_HOMEKEY]);
	m_KeyMenu.Format(L"0x%02x", m_CurKeyMap[MAP_MENUKEY]);
	m_KeyOk.Format(L"0x%02x", m_CurKeyMap[MAP_OKKEY]);
	m_KeyVolM.Format(L"0x%02x", m_CurKeyMap[MAP_VOLDOWN]);
	m_KeyVolP.Format(L"0x%02x", m_CurKeyMap[MAP_VOLUP]);
	m_KeyBack.Format(L"0x%02x", m_CurKeyMap[MAP_BACKKEY]);
	m_KeyAllUp.Format(L"0x%02x", m_CurKeyMap[MAP_ALLKEYUP]);

	UpdateData(FALSE);
	return;
}


void CConfigDlg::OnCbnSelchangeComboDecoder()
{
	// TODO: 在此添加控件通知处理程序代码
	DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("OnCbnSelchangeComboDecoder")));
	m_ComboBoxDecoder.GetLBText(m_ComboBoxDecoder.GetCurSel(), m_DecoderStr);
	if(m_DecoderStr.Find(L"mSBC") == -1)
	{

	}

	return;
}
