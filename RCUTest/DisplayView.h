/************************************************************************/
/*  Copying ringht (C) 2003
/*
/*	Name:	      DisplayView.h
/*  Function:     Implement the Specil function for all view,
/*                In this appliction all View must Inherit from 
/*                this View.
/*  
/*  Author:       lava_sdb
/*  Create Date:  2003-8-19
/*  Version:	  1.0                                                                 
/*
/*  Modify:   
/*     Author:
/*	   Change:
/*     Date:
/*	
/*
/************************************************************************/



#if !defined(AFX_DISPLAYVIEW_H__1D338CFA_3F56_4B82_A8C7_649C7792C6C8__INCLUDED_)
#define AFX_DISPLAYVIEW_H__1D338CFA_3F56_4B82_A8C7_649C7792C6C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayView.h : header file
//
//#include "SplitterControl.h"

/////////////////////////////////////////////////////////////////////////////
// CDisplayView view

class CDisplayView : public CView
{
protected:
	CDisplayView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDisplayView)
public:
	CStatic *m_Split;
	
	CRgn m_rgnMenu;

// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDisplayView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplayView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYVIEW_H__1D338CFA_3F56_4B82_A8C7_649C7792C6C8__INCLUDED_)
