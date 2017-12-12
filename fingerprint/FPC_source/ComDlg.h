// ComDlg.h : header file
// Contains mostly stuff for the windows GUI
// See the source (.cpp) for more comments on the serial API related functions
//

#include "Resource.h"
#include "SerialPortEnumerator.h"
#include "SerialCommands.h"
#include "SPICommands.h"

#if !defined(AFX_COMDLG_H__98846A20_45E8_4212_9E0A_AAC8369B3674__INCLUDED_)
#define AFX_COMDLG_H__98846A20_45E8_4212_9E0A_AAC8369B3674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CComDlg dialog

class CComDlg : public CDialog
{
// Construction
public:
	CComDlg(CWnd* pParent = NULL);	// standard constructor	
	COLORREF m_crColor; //Just to change font color
	void PrintData(unsigned char *byteStream, int NrOfBytes, bool TX);
	void EnableWindows(bool mode);
	void KillCurrentCommunication(); //Instantly stops all serial port communication.

// Dialog Data
	//{{AFX_DATA(CComDlg)
	enum { IDD = IDD_FPCSERIALCOM_DIALOG };
	CButton	m_DownloadAll;
	CButton	m_UploadAll;
	CButton	m_CompactButton;
	CButton	m_UpgradeCtrl;
    CButton m_InitiateUpgradeCtrl;
	CStatic	m_TextCtrl10;
	CComboBox	m_TemplateNameCtrl;
	CButton	m_UploadImageCtrl;
	CButton	m_TestHardwareCtrl;
	CButton	m_CaptureEnrolCtrl;
	CButton	m_OkCtrl;
	CStatic	m_StaticCtrl4;
	CStatic	m_StaticCtrl3;
	CStatic	m_StaticCtrl2;
	CStatic	m_StaticCtrl1;
	CButton	m_UploadTemplateFLASHCtrl;
	CStatic	m_VertDivCtrl;
	CStatic	m_TextCtrl8;
	CStatic	m_TextCtrl7;
	CStatic	m_TextCtrl6;
	CStatic	m_TextCtrl5;
	CStatic	m_TextCtrl4;
	CStatic	m_TextCtrl3;
	CStatic	m_TextCtrl2;
	CStatic	m_TextCtrl1;
	CButton	m_SendCtrl;
	CButton	m_InputCtrl;
	CButton	m_DownloadImageCtrl;
	CStatic	m_DivCtrl4;
	CStatic	m_DivCtrl3;
	CStatic	m_DivCtrl1;
	CStatic	m_Div1Ctrl2;
	CButton	m_DefaultCtrl;
	CEdit	m_CustomCtrl;
	CButton	m_ConnectCtrl;
	CButton	m_ClearCtrl;
	CButton	m_VerifyFLASHCtrl;
	CButton	m_CaptureVerifyFLASHCtrl;
	CButton	m_VerifyRamCtrl;
	CButton	m_UploadTemplateCtrl;
	CButton	m_UploadSliceCtrl;
	CButton	m_CaptureVerifyCtrl;
	CButton	m_SetBaudCtrl;
	CButton	m_IdentifyCtrl;
	CButton	m_CaptureIdentifyCtrl;
	CButton	m_FirmwareCtrl;
	CButton	m_DownloadTemplateCtrl;
	CButton	m_DeleteSlotCtrl;
	CButton	m_DeleteAllCtrl;
	CButton	m_EnrolCtrl;
	CButton	m_CopyCtrl;
	CButton	m_CaptureCtrl;
	CButton	m_CancelCtrl;
	CComboBox	m_ViewCtrl;
	CComboBox	m_HardwareCtrl;
	CEdit	m_SlotCtrl;
	CButton	m_CharCtrl;
	CButton	m_HexCtrl;
	CListBox	m_ListBoxCtrl;
	CStatic	m_StatusCtrl;
	CComboBox	m_StopCtrl;
	CComboBox	m_ParityCtrl;
	CComboBox	m_BitsCtrl;
	CComboBox	m_BaudCtrl;
	CComboBox	m_PortCtrl;
	CString	m_PortValue;
	CString	m_BaudValue;
	CString	m_BitsValue;
	CString m_HardwareValue;
	CString m_ViewValue;
	CString	m_ParityValue;
	CString	m_StopValue;
	CString	m_CustomCommand;
	int		m_SlotValue;
	CString	m_TemplateNameValue;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	CSerialPortEnumerator m_Ports;
    CSerialCommand *m_Command;
	CSPICommand *m_SPICommand;

	

// Implementation
protected:
	HICON m_hIcon;
	CBrush m_brush;

	void SetDefaultSettings();
	void GetSettings();
	void FillDropBoxes();
	void FillTemplateDropBox();

	void SetLastSettings();
	bool m_IsInitiated;
	int m_movePixels;
	int m_CurrentView;
	int m_CurrentHardware; //Not used in this code. Can be used in the future to separate different hardware.
	int m_SwipeImageSliceCounter;
	int m_SwipeImageSliceCounterMax;
	unsigned char* m_SwipeImageBuffer;
	int m_UseSPIFlag; //=0 if we are using UART as our communication method, and =1 if we are using SPI.
	

	// Generated message map functions
	//{{AFX_MSG(CComDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDefault();
	afx_msg void OnClear();
	afx_msg void OnConnect();
	afx_msg void OnCaptureEnrolRAM();
	afx_msg void OnCaptureVerifyRAM();
	afx_msg void OnSetBaudRate();
	afx_msg void OnDownloadTemplate();
	afx_msg void OnUploadTemplate();
	afx_msg void OnFirmwareVersion();
	afx_msg void OnCaptureVerifyFLASH();
	afx_msg void OnUploadImage();
	afx_msg void OnCancelCommand();
	afx_msg void OnSend();
	afx_msg void OnChar();
	afx_msg void OnHex();
	afx_msg void OnCopyTemplateRamToFLASH();
	afx_msg void OnCapture();
	afx_msg void OnSelendokView();
	virtual void OnOK();
	afx_msg void OnDeleteSlot();
	afx_msg void OnDeleteAll();
	afx_msg void OnUploadTemplateFLASH();
	afx_msg void OnCaptureIdentify();
	afx_msg void OnIdentify();
	afx_msg void OnEnrolRam();
	afx_msg void OnVerifyFLASH();
	afx_msg void OnDownloadImage();
	afx_msg void OnSelendokHardware();
	afx_msg void OnVerifyRam();
	afx_msg void OnTestHardware();
    afx_msg void OnInitiateUpgrade();
	afx_msg void OnUpgrade();
	afx_msg void OnRunInstaller();
	afx_msg void OnCaptureEnrolRamDual();
	afx_msg void OnButton1();
	afx_msg void OnUploadall();
	afx_msg void OnDownloadall();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void Scramble(unsigned char *p, int lines, int points);	//Helper function for descrambling area images.


public:
	afx_msg void OnLbnSelchangeList1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMDLG_H__98846A20_45E8_4212_9E0A_AAC8369B3674__INCLUDED_)
