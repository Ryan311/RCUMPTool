#pragma once
#include "afxwin.h"
#include "CNumberEdit.h"

// CNewKeyMap 对话框

class CNewKeyMap : public CDialog
{
	DECLARE_DYNAMIC(CNewKeyMap)

public:
	CNewKeyMap(CString KeyName, CString KeyValue, CWnd* pParent = NULL);   // 标准构造函数
	CNewKeyMap(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewKeyMap();
	BOOL OnInitDialog();
// 对话框数据
	enum { IDD = IDD_DIALOG_NEW_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_KeyNameEdit;
	CNumberEdit m_KeyValueEdit;
	CString m_KeyName;
	CString m_KeyValue;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

};




