#if !defined(AFX_BAUDRATESELECTOR_H__EA3297B7_05C8_40F9_9566_DE077E1A07AD__INCLUDED_)
#define AFX_BAUDRATESELECTOR_H__EA3297B7_05C8_40F9_9566_DE077E1A07AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaudRateSelector.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBaudRateSelector dialog

class CBaudRateSelector : public CDialog
{
// Construction
public:
	CBaudRateSelector(CWnd* pParent = NULL);   // standard constructor
//	void InitPointer(int* ptr);
	int m_pSpeedIndex;

// Dialog Data
	//{{AFX_DATA(CBaudRateSelector)
	enum { IDD = IDD_BAUDRATE_SELECTOR };
	int		m_ConnectionSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaudRateSelector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBaudRateSelector)
	virtual void OnOK();
	afx_msg void OnBaud115200();
	afx_msg void OnBaud14400();
	afx_msg void OnBaud19200();
	afx_msg void OnBaud28800();
	afx_msg void OnBaud38400();
	afx_msg void OnBaud57600();
	afx_msg void OnBaud9600();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAUDRATESELECTOR_H__EA3297B7_05C8_40F9_9566_DE077E1A07AD__INCLUDED_)
