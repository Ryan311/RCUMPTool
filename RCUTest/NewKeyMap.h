#pragma once
#include "afxwin.h"
#include "CNumberEdit.h"

// CNewKeyMap �Ի���

class CNewKeyMap : public CDialog
{
	DECLARE_DYNAMIC(CNewKeyMap)

public:
	CNewKeyMap(CString KeyName, CString KeyValue, CWnd* pParent = NULL);   // ��׼���캯��
	CNewKeyMap(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewKeyMap();
	BOOL OnInitDialog();
// �Ի�������
	enum { IDD = IDD_DIALOG_NEW_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_KeyNameEdit;
	CNumberEdit m_KeyValueEdit;
	CString m_KeyName;
	CString m_KeyValue;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

};




