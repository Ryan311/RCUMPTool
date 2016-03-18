#pragma once
#include "resource.h"
#include "afxcmn.h"

#define  IDC_EDIT 0xffff

// CKeyMapDlg 对话框
class CKeyMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CKeyMapDlg)

public:
	CKeyMapDlg(map<CString, CString> &KeyMap, CWnd* pParent = NULL);   // 标准构造函数
	CKeyMapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CKeyMapDlg();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_KEYMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	map<CString, CString> m_KeyMap;
	CListCtrl m_KeyMapList;
	CEdit m_Edit;  
	int m_Item;		//行
	int m_SubItem;	//列
	int m_rowCount;	//总行数
	bool  needSave; //用于保存标志,如果有过编写则为TURE,当下次编辑时会在原来的位置输入原来输入的内容
	afx_msg void OnNMDblclkKeymapList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
};
