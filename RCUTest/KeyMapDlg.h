#pragma once
#include "resource.h"
#include "afxcmn.h"

#define  IDC_EDIT 0xffff

// CKeyMapDlg �Ի���
class CKeyMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CKeyMapDlg)

public:
	CKeyMapDlg(map<CString, CString> &KeyMap, CWnd* pParent = NULL);   // ��׼���캯��
	CKeyMapDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CKeyMapDlg();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_KEYMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	map<CString, CString> m_KeyMap;
	CListCtrl m_KeyMapList;
	CEdit m_Edit;  
	int m_Item;		//��
	int m_SubItem;	//��
	int m_rowCount;	//������
	bool  needSave; //���ڱ����־,����й���д��ΪTURE,���´α༭ʱ����ԭ����λ������ԭ�����������
	afx_msg void OnNMDblclkKeymapList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
};
