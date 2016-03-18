// HWCDlg.h : header file
//

#pragma once
#include "xSkinButton.h"
#include "LEHIDClient.h"
#include "afxbutton.h"
#include "sbc.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <map>
using namespace std;

#include "BtnST.h"
#include "DFU_Def.h"
#include "CNumberEdit.h"
#include "ConfigFile.h"
#include "DisplayView.h"
#include "WaveView.h"

#define RCU_CONNECTION 0
#define OTA_CONNECTION 1

// Decoder Function type
typedef int (*AUDIO_DECODER)(LPCTSTR);
typedef int (*AUDIO_PLAY)(void);

// custom struct define
typedef struct{
	BYTE GUIKey;
	BYTE ReserveKey;
	BYTE Key1;
	BYTE Key2;
	BYTE Key3;
	BYTE Key4;
	BYTE Key5;
	BYTE Key6;
}HIDReportStruct;

typedef struct{
	sbc_t *sbc;
	void *input;
	int input_len;
	void *output;
	int output_len;
	int *written;
}MsbcDecordPrameter;

typedef struct{
	RT_U8 CmdHead;
	RT_U8 CmdValue;
	RT_U8 WakeupSignal;
	RT_U8 ChannelID;
	RT_U8 CustomID0;
	RT_U8 CustomID1;
	RT_U8 Reserved0;
	RT_U8 Reserved1;
}ControlData;

// CHWCDlg dialog
class CHWCDlg : public CDialogEx
{
// Construction
public:
	CHWCDlg(CWnd* pParent = NULL);	// standard constructor
	~CHWCDlg();

// Dialog Data
	enum { IDD = IDD_HWC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	HICON m_hIcon;
	CFont m_editFont;
	CFont m_ButtonShowFont;
	CFont m_StartButtonFont;
	CFont m_ButtonStateFont;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void InitButtonStyle();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg LRESULT OnCustomButtonState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomButtonState2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomNotifyRawData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomPowerdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomHomedown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeUpdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeDowndown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeLeftdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeRightdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeOkdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeBackdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeMicdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeMenudown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeVoldowndown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomeVolupdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAllup(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnVoiceDataStore(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ConnectAndGetInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT DisconnectDevLink(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT PlayVoiceRecord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAllButtonActive(WPARAM wParam, LPARAM lParam);
private:
	CxSkinButton m_Home;
	CxSkinButton m_Mic;
	CxSkinButton m_VolDown;
	CxSkinButton m_VolUp;
	CxSkinButton m_Up;
	CxSkinButton m_Left;
	CxSkinButton m_Right;
	CxSkinButton m_Down;
	CxSkinButton m_OK;
	CxSkinButton m_Back;
	CxSkinButton m_Menu;
	CStatic m_static;
	CxSkinButton m_Power;
	CButtonST m_ButtonState;
	CButtonST m_MPModeSelect;
	CButtonST m_AlertButton;
	CButtonST m_VoiceButton;
	CButtonST m_LogClear;

	CFont m_AlertButtonFont;
	CFont m_VoiceButtonFont;
	CFont m_MPModeFont;
	CFont m_LogClearFont;

private:
	unsigned long long m_LeDevAddress;
	CString m_BDAddrStr;
	CString m_BTDevNameStr;
	CString m_AdvStr;
	CString m_RssiStr;
	bool  m_DeviceSelected;
	unsigned long long m_LeOTAMAddress;

	static CHWCDlg* pThis;   //æ≤Ã¨∂‘œÛ÷∏’Î
	CLEHIDClient LEHIDSvr;
	HANDLE  m_hLEScanDeviceThread;
	FILE *m_MicFile;
	bool m_LEDevConnected;
	int m_ConnType;	// RCU or OTA
	bool m_MPModeStop;
	HANDLE m_hScanEvent;
	bool m_rightDev;
	CString m_ScanStr;
	CString m_ConnectedDev;
	CString m_OTAConnectedDev;
	int m_VoicePlayMode;
	// Key Map BitMap
	map<BYTE, BYTE> m_Keymap;	
	map<BYTE, BYTE> m_KeymapR;
	map<CString, CString> m_UserDefinedKeymap;
	map<CString, CString> m_UserDefinedKeymapR;
	map<CString, int> m_AdvTypeR;
	// Decoder
	map<CString, int> m_DecoderMap;
	// Picture Button Mask -- KeyValue Map
	map<BYTE, BYTE> m_PicMaskmap;	

	CString m_DecoderSelected;
	int m_bHandShake;
	AUDIO_DECODER m_DecoderFunc;
	AUDIO_PLAY m_PlayFunc;
	CStringArray m_DecoderSet;

	int LoadDefaultConfig();
	int SaveDefaultConfig();
	int LoadDecoder(CString DecoderFilePath);
	int LoadSupportedDecoders();
	int YKQPictureAndMaskLoad();
	void PicButtonMaskClear();

	int StartToRecordVoice();
	int StopToRecordVoice();
	int ButtonGroupEnable(bool value);

	static DWORD WINAPI s_LEScanDeviceThreadProc(LPVOID lpv);
	DWORD LEScanDeviceThreadProc(void);
	CDisplayView * AddRootView();

	void LEScanStart();
	static void LeGapScanCallBack(unsigned char* pAddress, const int nSizeAddress, unsigned char* pPkt, const int nSizePkt, void* pUserData);
	void LEScanStop();

	void ShowEditPrintInfo(CString &info, BOOL bRaw);
	void SetSTButtonColor(int Red, int Green, int Blue);

	static DWORD WINAPI s_DeviceConnectThreadProc(LPVOID lpv);
	DWORD DeviceConnectThreadProc(void);
	HANDLE m_hDeviceConnThread;

public:
	CProgressCtrl m_VoiceStoreProecss;
	CButton m_ScanState;
	CButton m_Start;
	CEdit m_ButtonShow;
	afx_msg void OnBnClickedAutotestStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedConfig();
	afx_msg void OnBnClickedScan();
	afx_msg void OnBnClickedButtonReplay();

private:
	/*
	* audio play direct
	*/
	HWAVEOUT m_hWaveOut;
	WAVEFORMATEX m_wfx;
	CRITICAL_SECTION m_waveCriticalSection;
	WAVEHDR* m_waveBlocks;
	volatile int m_waveFreeBlockCount;
	int m_waveCurrentBlock;
	sbc_t m_sbc;
	MsbcDecordPrameter m_msbc_decode_parameter;
	CDFUClient dfu;

	static void CALLBACK WaveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);
	void WaveWriteAudio(LPSTR data, int size);
	int WavePlayStart();
	int WavePlayStop();
	void DrawVoiceWave();

public:
	afx_msg void OnBnClickedButtonRcuConnect();
	afx_msg void OnBnClickedButtonOtaConnect();
	afx_msg void OnBnClickedButtonLoadpatch();
	afx_msg void OnBnClickedButtonGetRemoteinfo();
	afx_msg void OnBnClickedButtonDownloadPatch();
	afx_msg void OnBnClickedButtonActiveFw();
	afx_msg void OnBnClickedButtonDoAll();
	afx_msg LRESULT OnOTAPatchDownloadProcess(WPARAM wParam, LPARAM lParam);

	CButton m_RCUConn;
	CButton m_DevDisconn;
	CProgressCtrl m_OTAProcess;
	afx_msg void OnBnClickedOpenConfigPath();
	afx_msg void OnBnClickedButtonEnableOtaMode();

	// MP Tool
	CButton m_MPConnect;
	afx_msg void OnBnClickedMpModeAutoConnect();
	bool m_MPModeEnable;

	CEdit m_MPLinkKeyEdit;
	CEdit m_MPBDAddressEdit;
	MP_CONFIG m_MPConfig;
	afx_msg void OnBnClickedClearLog();
	afx_msg void OnBnClickedClearQuitMp();
	CButton m_UpdateButton;
	afx_msg void OnBnClickedButtonFindme();
	afx_msg void OnBnClickedButtonVoicerecord();
	afx_msg void OnBnClickedButtonAddr1();
	afx_msg void OnBnClickedButtonAddr2();
	afx_msg void OnBnClickedButtonAddr3();
	afx_msg void OnBnClickedButtonAddr4();
	afx_msg void OnBnClickedButtonAddr5();
	void MPModeReset();
	CStatic m_WaveShow;
	CCreateContext m_pContext;
	CWaveView *m_pView;
	afx_msg void OnBnClickedButtonConnState();
	CEdit m_EditRSSI;
};
