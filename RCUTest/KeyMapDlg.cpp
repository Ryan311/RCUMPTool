// KeyMapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <map>
using namespace std;
#include "KeyMapDlg.h"
#include "NewKeyMap.h"


// CKeyMapDlg 对话框

IMPLEMENT_DYNAMIC(CKeyMapDlg, CDialog)

CKeyMapDlg::CKeyMapDlg(map<CString, CString> &KeyMap, CWnd* pParent /*=NULL*/)
	: CDialog(CKeyMapDlg::IDD, pParent)
{
	m_KeyMap = KeyMap;
}

CKeyMapDlg::CKeyMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyMapDlg::IDD, pParent)
{

}

CKeyMapDlg::~CKeyMapDlg()
{
}

void CKeyMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEYMAP_LIST, m_KeyMapList);
}

BOOL CKeyMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 在此添加额外的初始化代码
	m_KeyMapList.ModifyStyle( 0, LVS_REPORT );               // 报表模式  
	//LVS_EX_GRIDLINES是希望显示网格；LVS_EX_FULLROWSELECT是希望被选中时整行反色显 示；
	//LVS_EX_HEADERDRAGDROP是让其支持点击表头排序；LVS_EX_TWOCLICKACTIVATE是希望有鼠标在未被选中的行上移动的时候有一些效果
	m_KeyMapList.SetExtendedStyle(m_KeyMapList.GetExtendedStyle() 
		| LVS_EX_GRIDLINES 
		| LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP 
		| LVS_EX_TWOCLICKACTIVATE); 

	m_KeyMapList.InsertColumn(0,_T("Key")); 
	m_KeyMapList.InsertColumn(1,_T("Value (Hex: 0x00 ~ 0xFF)"));  

	CRect m_rect;  
	m_KeyMapList.GetClientRect(&m_rect); //获取list的客户区  
	m_KeyMapList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT); //设置list风格
	m_KeyMapList.SetColumnWidth(0, m_rect.Width() / 2); //设置列的宽度。  
	m_KeyMapList.SetColumnWidth(1, m_rect.Width() / 2);  

	// Add the existing list
	map<CString,CString>::iterator iter = m_KeyMap.begin();  
	while (iter!=m_KeyMap.end())
	{  
		m_KeyMapList.InsertItem(0,iter->first);  
		m_KeyMapList.SetItemText(0,1,iter->second);
		iter++;
	}

	needSave = false; //初始化为FLASE;
	m_rowCount = m_KeyMapList.GetItemCount();  //获取行数

	/*if (m_Edit.m_hWnd == NULL)  */
	{// Create Edit Control
		//IDC_EDIT已近在头文件中定义,这个很重要,很多时候会忽略,  
		//定义为#define  IDC_EDIT 0xffff  
		m_Edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN,	//|WS_BORDER,  
			CRect(0,0,0,0),this,IDC_EDIT);  

		m_Edit.ShowWindow(SW_HIDE); //Edit创建完后隐藏  
		m_Edit.SetFont(this->GetFont(),FALSE);//设置字体  

		//将list control设置为父窗口,生成的Edit才能正确定位,这个也很重要,  
		m_Edit.SetParent(&m_KeyMapList); 
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CKeyMapDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_KEYMAP_LIST, &CKeyMapDlg::OnNMDblclkKeymapList)
	ON_BN_CLICKED(IDOK, &CKeyMapDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CKeyMapDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CKeyMapDlg::OnBnClickedButtonDel)
END_MESSAGE_MAP()

// CKeyMapDlg 消息处理程序

void CKeyMapDlg::OnNMDblclkKeymapList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR; 

	m_Item = pEditCtrl->iItem;  
	m_SubItem = pEditCtrl->iSubItem; 

	if( m_Item == -1)
	{
		// add new one
		OnBnClickedButtonAdd();
	}
	else if(m_Item < m_rowCount)  
	{
		CString KeyName, KeyValue;
		KeyName = m_KeyMapList.GetItemText(m_Item, 0);
		KeyValue = m_KeyMapList.GetItemText(m_Item, 1);
		CNewKeyMap* newKeyDlg = new CNewKeyMap(KeyName, KeyValue);
		if(IDOK == newKeyDlg->DoModal())
		{
			// delete old key map
			map<CString,CString>::iterator iter = m_KeyMap.begin();  
			while (iter!=m_KeyMap.end())
			{  
				if (iter->first == KeyName)  
				{  
					m_KeyMap.erase(iter++);
				}  
				else  
				{  
					iter++;  
				}
			}
			// add new key map
			CString newKeyName = newKeyDlg->m_KeyName;
			CString newKeyValue = newKeyDlg->m_KeyValue;
			m_KeyMapList.SetItemText(m_Item,0,newKeyName);
			m_KeyMapList.SetItemText(m_Item,1,newKeyValue);
			m_KeyMap[newKeyName] = newKeyValue;	// add to map
		} 
	}  
	
	*pResult = 0;
}


void CKeyMapDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CKeyMapDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CNewKeyMap* newKeyDlg = new CNewKeyMap;
	if(IDOK == newKeyDlg->DoModal())
	{
		CString newKeyName = newKeyDlg->m_KeyName;
		CString newKeyValue = newKeyDlg->m_KeyValue;
		m_KeyMapList.InsertItem(m_rowCount,newKeyName);  
		m_KeyMapList.SetItemText(m_rowCount,1,newKeyValue);

		m_KeyMap[newKeyName] = newKeyValue;	// add to map
		m_rowCount++;
	}
}

void CKeyMapDlg::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	POSITION pos = m_KeyMapList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		return;
	}
	else
	{
		int nItem = m_KeyMapList.GetNextSelectedItem(pos);
		// deal with map
		
		CString KeyName = m_KeyMapList.GetItemText(nItem, 0);
		map<CString,CString>::iterator iter = m_KeyMap.begin();  
		while (iter!=m_KeyMap.end())
		{  
			if (iter->first == KeyName)  
			{  
				m_KeyMap.erase(iter++);
			}  
			else  
			{  
				iter++;  
			}
		}
		m_KeyMapList.DeleteItem(nItem);
		m_rowCount = m_KeyMapList.GetItemCount();  
	}
}