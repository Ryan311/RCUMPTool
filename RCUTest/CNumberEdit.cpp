#include "stdafx.h"
#include "CNumberEdit.h"

/**********************************************************
CNumberEdit
***********************************************************/
BEGIN_MESSAGE_MAP(CNumberEdit, CEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CNumberEdit::OnEnSetfocus)  
END_MESSAGE_MAP()

CNumberEdit::CNumberEdit()
{

}

CNumberEdit::~CNumberEdit()
{
}

void CNumberEdit::SetWindowTextInteger(int nNum)
{
	m_bEnChange = false;
	CString strData;
	strData.Format(_T("%d"), nNum);
	SetWindowText(strData);
	m_bEnChange = true;
}

int CNumberEdit::GetWindowTextInteger()
{
	CString strData;
	GetWindowText(strData); 
	int nResult = _tstoi(strData);
	return nResult;
}

void CNumberEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	// get original string of edit control
	CString strOrigin;
	GetWindowText(strOrigin);

	if( strOrigin.GetLength() >= 4 && (nChar != VK_BACK))
	{// length should be <4
		// pass it
	}
	else if( strOrigin.GetLength() <= 2 && (nChar == VK_BACK))
	{// length should be >2
		// pass it
	}
	else if((nChar == 'x')  || (nChar >= '0' && nChar <= '9') || (nChar >= 'a' && nChar <= 'f') || (nChar >= 'A' && nChar <= 'F') || (nChar == VK_BACK)) 
	{
		// beside dot and minus, it allows number in HEX, backespace
		CEdit::OnChar(nChar, nRepCnt, nFlags); 
	}
	// pass the other keys
}

BOOL CNumberEdit::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	if(message == WM_COMMAND && HIWORD(wParam) == EN_CHANGE)
	{
		if(!m_bEnChange)
		{
			// in this case, don't send the message to the parent window
			return TRUE;
		}
	}

	return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}


void CNumberEdit::OnEnSetfocus()  
{  
	// TODO: Add your control notification handler code here  
	SetSel(2,-1); // Focus: Start -- End
}  