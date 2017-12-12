// FPC SerialCom.h : main header file for the FPC SERIALCOM application
//

#if !defined(AFX_FPCSERIALCOM_H__0543BDCA_420B_48EA_BB1A_8B6678DD35B2__INCLUDED_)
#define AFX_FPCSERIALCOM_H__0543BDCA_420B_48EA_BB1A_8B6678DD35B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CComApp:
// See FPC SerialCom.cpp for the implementation of this class
//

class CComApp : public CWinApp
{
public:
	CComApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CComApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FPCSERIALCOM_H__0543BDCA_420B_48EA_BB1A_8B6678DD35B2__INCLUDED_)
