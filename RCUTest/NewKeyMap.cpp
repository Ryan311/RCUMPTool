// NewKeyMap.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "NewKeyMap.h"


// CNewKeyMap �Ի���

IMPLEMENT_DYNAMIC(CNewKeyMap, CDialog)

CNewKeyMap::CNewKeyMap(CString KeyName, CString KeyValue, CWnd* pParent /*= NULL*/)
	: CDialog(CNewKeyMap::IDD, pParent)
{
	m_KeyName = KeyName;
	m_KeyValue = KeyValue;
}

CNewKeyMap::CNewKeyMap(CWnd* pParent /*=NULL*/)
	: CDialog(CNewKeyMap::IDD, pParent)
{
	m_KeyValue.Format(_T("0x"));
}

CNewKeyMap::~CNewKeyMap()
{
}

BOOL CNewKeyMap::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_KeyNameEdit.SetWindowText(m_KeyName);
	m_KeyValueEdit.SetWindowText(m_KeyValue);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CNewKeyMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_KEY_NAME, m_KeyNameEdit);
	DDX_Control(pDX, IDC_EDIT_KEY_VALUE, m_KeyValueEdit);
}


BEGIN_MESSAGE_MAP(CNewKeyMap, CDialog)
	ON_BN_CLICKED(IDOK, &CNewKeyMap::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CNewKeyMap::OnBnClickedCancel)
END_MESSAGE_MAP()


void CNewKeyMap::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_KeyNameEdit.GetWindowText(m_KeyName);
	m_KeyValueEdit.GetWindowText(m_KeyValue);
	if(m_KeyName.IsEmpty() || m_KeyValue.IsEmpty())
	{
		MessageBox(_T("KeyName and KeyValue cannot be empty either"));
		return;
	}
	else
	{
		OnOK();
	}
}

void CNewKeyMap::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}