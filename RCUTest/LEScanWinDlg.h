#pragma once
#include "resource.h"
#include <map>
#include "afxwin.h"
using namespace std;

// CLEScanWinDlg �Ի���

class CLEScanWinDlg : public CDialog
{
	DECLARE_DYNAMIC(CLEScanWinDlg)

public:
	CLEScanWinDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CLEScanWinDlg(int ScanDeviceInOTAM, ULONGLONG NormalAddress, CWnd* pParent = NULL);   // ���캯��
	virtual ~CLEScanWinDlg();

// �Ի�������
	enum { IDD = IDD_LESCAN_WIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedScanStart();

private:
	map<CString, CString> m_scanList;
	map<int, CString> m_AdvType;
	map<CString, int> m_AdvTypeR;
	static CLEScanWinDlg* pThis;   //��̬����ָ��
	static int* number;
	static void LeGapScanCallBack(unsigned char* pAddress, const int nSizeAddress, unsigned char* pPkt, const int nSizePkt, void* pUserData);
public:
	afx_msg void OnNMDblclkDeviceList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedScanStop();
	int m_OTAMScan;
	ULONGLONG m_BDAddress;
	ULONGLONG m_NormalAddress;	// Original Address saved by RCU tool
	ULONGLONG m_OriginalAddress;	// Original Address in advertising packet

	CString m_BDAddrStr;
	CString m_BTDevNameStr;
	CString m_AdvTypeStr;
	CString m_RSSIStr;

	CButton m_ScanEnable;
	CButton m_ScanStop;
};

