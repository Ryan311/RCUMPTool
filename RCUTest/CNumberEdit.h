#pragma once

// NumberEdit
class CNumberEdit : public CEdit
{
public:
	CNumberEdit();
	~CNumberEdit();

	void SetWindowTextInteger(int nNum); 
	void SetWindowTextFloat(float fNum);
	void SetTextLength(int maxLen, int minLen);

	int GetWindowTextInteger();
	float GetWindowTextFloat();

private:
	bool m_bEnChange;
	int m_maxLen;
	int m_minLen;
	int m_focusStart;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	afx_msg void OnEnSetfocus();  
};