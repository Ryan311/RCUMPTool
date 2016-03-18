// LESampleGuiDlg.h : ͷ�ļ�
//
#pragma once
#include <map>
using namespace std;

// CLESampleGuiDlg �Ի���
class CLESampleGuiDlg : public CDialog
{
// ����
public:
	CLESampleGuiDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SCAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedScan();
	afx_msg void OnBnClickedStop();

private:
	map<CString, CString> m_scanList;
	static CLESampleGuiDlg* pThis;   //��̬����ָ��
	static int* number;
	static void LeGapScanCallBack(unsigned char* pAddress, const int nSizeAddress, unsigned char* pPkt, const int nSizePkt, void* pUserData);
public:
	afx_msg void OnNMDblclkDeviceList(NMHDR *pNMHDR, LRESULT *pResult);
};
