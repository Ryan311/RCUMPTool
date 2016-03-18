// LESampleGuiDlg.h : 头文件
//
#pragma once
#include <map>
using namespace std;

// CLESampleGuiDlg 对话框
class CLESampleGuiDlg : public CDialog
{
// 构造
public:
	CLESampleGuiDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SCAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedScan();
	afx_msg void OnBnClickedStop();

private:
	map<CString, CString> m_scanList;
	static CLESampleGuiDlg* pThis;   //静态对象指针
	static int* number;
	static void LeGapScanCallBack(unsigned char* pAddress, const int nSizeAddress, unsigned char* pPkt, const int nSizePkt, void* pUserData);
public:
	afx_msg void OnNMDblclkDeviceList(NMHDR *pNMHDR, LRESULT *pResult);
};
