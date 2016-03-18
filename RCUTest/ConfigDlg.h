#pragma once

#include <map>
#include "CNumberEdit.h"
#include "afxwin.h"
#include "afxcmn.h"
using namespace std;


// CConfigDlg 对话框

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	CConfigDlg(map<CString, CString> &UserDefinedKeyMap,
		map<BYTE, BYTE>&KeyMap, 
		CStringArray &Decoders, 
		CString &CurDecoder, 
		int VoicePlayMode, 
		ULONGLONG LeDevAddress, 
		bool ConnectState, MP_CONFIG &MPConfig,
		CWnd* pParent = NULL);
	virtual ~CConfigDlg();
	BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonMoreKey();
	afx_msg void OnBnClickedButtonDefault();
	afx_msg void OnBnClickedButtonAddDecoder();
	afx_msg void OnBnClickedRadioVoiceOption();
	afx_msg void OnCbnSelchangeComboDecoder();

	CNumberEdit m_EditPower;
	CNumberEdit m_EditHome;
	CNumberEdit m_EditBack;
	CNumberEdit m_EditMenu;
	CNumberEdit m_EditKeyUp;
	CNumberEdit m_EditUp;
	CNumberEdit m_EditDown;
	CNumberEdit m_EditRight;
	CNumberEdit m_EditLeft;
	CNumberEdit m_EditOK;
	CNumberEdit m_EditMic;
	CNumberEdit m_EditVolP;
	CNumberEdit m_EditVolM;
	CString m_KeyPower;
	CString m_KeyHome;
	CString m_KeyBack;
	CString m_KeyMenu;
	CString m_KeyAllUp;
	CString m_KeyUp;
	CString m_KeyDown;
	CString m_KeyRight;
	CString m_KeyLeft;
	CString m_KeyOk;
	CString m_KeyMic;
	CString m_KeyVolP;
	CString m_KeyVolM;
	CNumberEdit m_EditTest;
	CComboBox m_ComboBoxDecoder;
	CString m_DecoderStr;
	CStringArray m_Decoders;
	ULONGLONG m_LeDevAddress;
	bool m_ConnectState;

	map<BYTE, BYTE> m_CurKeyMap;
	map<CString, CString> m_UserDefineKeyMap;
	CEdit m_EditNewDecoder;
	CString m_NewDecoder;
	int m_VoiceOpt;
	CEdit m_PatchFilePathStr;
	CProgressCtrl m_PatchDownloadProcess;
	CEdit m_AlertValueHandle;
	CEdit m_SWValueHandle;
	CEdit m_FWValueHandle;
	CEdit m_HWValueHandle;
	CString m_AlertHandleStr;
	CString m_SWHandleStr;
	CString m_FWHandleStr;
	CString m_HWHandleStr;
	CEdit m_KeyNotifyHandleEdit;
	CEdit m_VoiceNotifyHandleEdit;
	CString m_KeyNHandleStr;
	CString m_VoiceNHandleStr;
};