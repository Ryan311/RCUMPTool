// KeyMapDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <map>
using namespace std;
#include "KeyMapDlg.h"
#include "NewKeyMap.h"


// CKeyMapDlg �Ի���

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

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_KeyMapList.ModifyStyle( 0, LVS_REPORT );               // ����ģʽ  
	//LVS_EX_GRIDLINES��ϣ����ʾ����LVS_EX_FULLROWSELECT��ϣ����ѡ��ʱ���з�ɫ�� ʾ��
	//LVS_EX_HEADERDRAGDROP������֧�ֵ����ͷ����LVS_EX_TWOCLICKACTIVATE��ϣ���������δ��ѡ�е������ƶ���ʱ����һЩЧ��
	m_KeyMapList.SetExtendedStyle(m_KeyMapList.GetExtendedStyle() 
		| LVS_EX_GRIDLINES 
		| LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP 
		| LVS_EX_TWOCLICKACTIVATE); 

	m_KeyMapList.InsertColumn(0,_T("Key")); 
	m_KeyMapList.InsertColumn(1,_T("Value (Hex: 0x00 ~ 0xFF)"));  

	CRect m_rect;  
	m_KeyMapList.GetClientRect(&m_rect); //��ȡlist�Ŀͻ���  
	m_KeyMapList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT); //����list���
	m_KeyMapList.SetColumnWidth(0, m_rect.Width() / 2); //�����еĿ�ȡ�  
	m_KeyMapList.SetColumnWidth(1, m_rect.Width() / 2);  

	// Add the existing list
	map<CString,CString>::iterator iter = m_KeyMap.begin();  
	while (iter!=m_KeyMap.end())
	{  
		m_KeyMapList.InsertItem(0,iter->first);  
		m_KeyMapList.SetItemText(0,1,iter->second);
		iter++;
	}

	needSave = false; //��ʼ��ΪFLASE;
	m_rowCount = m_KeyMapList.GetItemCount();  //��ȡ����

	/*if (m_Edit.m_hWnd == NULL)  */
	{// Create Edit Control
		//IDC_EDIT�ѽ���ͷ�ļ��ж���,�������Ҫ,�ܶ�ʱ������,  
		//����Ϊ#define  IDC_EDIT 0xffff  
		m_Edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN,	//|WS_BORDER,  
			CRect(0,0,0,0),this,IDC_EDIT);  

		m_Edit.ShowWindow(SW_HIDE); //Edit�����������  
		m_Edit.SetFont(this->GetFont(),FALSE);//��������  

		//��list control����Ϊ������,���ɵ�Edit������ȷ��λ,���Ҳ����Ҫ,  
		m_Edit.SetParent(&m_KeyMapList); 
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BEGIN_MESSAGE_MAP(CKeyMapDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_KEYMAP_LIST, &CKeyMapDlg::OnNMDblclkKeymapList)
	ON_BN_CLICKED(IDOK, &CKeyMapDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CKeyMapDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CKeyMapDlg::OnBnClickedButtonDel)
END_MESSAGE_MAP()

// CKeyMapDlg ��Ϣ�������

void CKeyMapDlg::OnNMDblclkKeymapList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

void CKeyMapDlg::OnBnClickedButtonAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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