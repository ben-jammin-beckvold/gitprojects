// ComDlg.cpp : implementation file
// The dialog file only handles dialog issues, the commands are 
// sent in the file SerialCommands.cpp... which is heavily commented

//#include "stdafx.h"
#include "FPC SerialCom.h"
#include "BaudRateSelector.h"
#include "ComDlg.h"
#include <sys/stat.h>
#include "crc.h"
#include <Dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PROCESSMESSAGES_AFX { MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}
#define PROCESSMESSAGES_STD { MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_REMOVE)) { ::TranslateMessage(&msg); ::DispatchMessage(&msg); } }


//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
// CAboutDlg dialog used for App About
// Only added to show the image on screen after an image upload.
//****************************************************************************
//////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers			
		ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//This is used to paint the image on screen after image upload..
void CAboutDlg::OnPaint()
{
	CDialog::OnPaint();

	
	struct stat status;
	stat("testImage.bmp", &status);
	const int imSize = status.st_size;
	unsigned char *imageBuffer = (unsigned char *)malloc(imSize);
	FILE *fil = 0;;
	fopen_s(&fil, "testImage.bmp","rb+");

	if (fil){
		fread(imageBuffer, sizeof(char), imSize, fil);
		fclose(fil);

		int paintLeft	= 10;
		int paintTop	= 10;

		//Draw it...
		BITMAPINFO *bDIB = (BITMAPINFO *)(imageBuffer + sizeof(BITMAPFILEHEADER));
		HDC dc =::GetDC(this->GetSafeHwnd());
		RECT    lpRect;
		
		GetWindowRect(&lpRect);
		int width	= lpRect.right - lpRect.left - 15;
		paintLeft	= (width - bDIB->bmiHeader.biWidth) / 2;

		::StretchDIBits(dc, 
			paintLeft, 
			paintTop, // paintTop + bDIB->bmiHeader.biHeight,
			bDIB->bmiHeader.biWidth, //bDIB->bmiHeader.biWidth, 
			bDIB->bmiHeader.biHeight, //-bDIB->bmiHeader.biHeight,
			0, 
			0, 
			bDIB->bmiHeader.biWidth, 
			bDIB->bmiHeader.biHeight,
			(void *)(imageBuffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)), 
			bDIB,
			DIB_RGB_COLORS, 
			SRCCOPY);
	}
	free(imageBuffer);
}

//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
// CComDlg dialog
// This class handles the main dialog window.. All the pushbutton functions are
// at the bottom of this file..
//****************************************************************************
//////////////////////////////////////////////////////////////////////////////
CComDlg::CComDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComDlg)
	m_PortValue = _T("");
	m_BaudValue = _T("");
	m_BitsValue = _T("");
	m_HardwareValue = _T("");
	m_ViewValue = _T("");
	m_ParityValue = _T("");
	m_StopValue = _T("");
	m_CustomCommand = _T("");
	m_SlotValue = 0;
	m_TemplateNameValue = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CComDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComDlg)
	DDX_Control(pDX, IDC_DOWNLOADALL, m_DownloadAll);
	DDX_Control(pDX, IDC_UPLOADALL, m_UploadAll);
	DDX_Control(pDX, IDC_COMPACT, m_CompactButton);
	//DDX_Control(pDX, IDC_INITIATE_UPGRADE, m_InitiateUpgradeCtrl);
	//DDX_Control(pDX, IDC_UPGRADE, m_UpgradeCtrl);
	DDX_Control(pDX, IDC_TEXT10, m_TextCtrl10);
	DDX_Control(pDX, IDC_TEMPLATE_NAME, m_TemplateNameCtrl);
	DDX_Control(pDX, IDC_UPLOAD_SLICE, m_UploadImageCtrl);
	DDX_Control(pDX, IDC_TEST_HARDWARE, m_TestHardwareCtrl);
	DDX_Control(pDX, IDC_CAPTURE_ENROL, m_CaptureEnrolCtrl);
	DDX_Control(pDX, IDOK, m_OkCtrl);
	DDX_Control(pDX, IDC_STATIC4, m_StaticCtrl4);
	DDX_Control(pDX, IDC_STATIC3, m_StaticCtrl3);
	DDX_Control(pDX, IDC_STATIC2, m_StaticCtrl2);
	DDX_Control(pDX, IDC_STATIC1, m_StaticCtrl1);
	DDX_Control(pDX, IDC_UPLOAD_TEMPLATE_FLASH, m_UploadTemplateFLASHCtrl);
	DDX_Control(pDX, IDC_VERT_DIVIDER, m_VertDivCtrl);
	DDX_Control(pDX, IDC_TEXT7, m_TextCtrl7);
	DDX_Control(pDX, IDC_TEXT6, m_TextCtrl6);
	DDX_Control(pDX, IDC_TEXT5, m_TextCtrl5);
	DDX_Control(pDX, IDC_TEXT4, m_TextCtrl4);
	DDX_Control(pDX, IDC_TEXT3, m_TextCtrl3);
	DDX_Control(pDX, IDC_TEXT2, m_TextCtrl2);
	DDX_Control(pDX, IDC_TEXT1, m_TextCtrl1);
	DDX_Control(pDX, IDC_SEND, m_SendCtrl);
	DDX_Control(pDX, IDC_INPUT1, m_InputCtrl);
	DDX_Control(pDX, IDC_DOWNLOAD_IMAGE, m_DownloadImageCtrl);
	DDX_Control(pDX, IDC_DIVIDER4, m_DivCtrl4);
	DDX_Control(pDX, IDC_DIVIDER3, m_DivCtrl3);
	DDX_Control(pDX, IDC_DIVIDER1, m_DivCtrl1);
	DDX_Control(pDX, IDC_DIVIDER2, m_Div1Ctrl2);
	DDX_Control(pDX, IDC_DEFAULT, m_DefaultCtrl);
	DDX_Control(pDX, IDC_CUSTOM_COMMAND, m_CustomCtrl);
	DDX_Control(pDX, IDC_CONNECT, m_ConnectCtrl);
	DDX_Control(pDX, IDC_CLEAR, m_ClearCtrl);
	DDX_Control(pDX, IDC_VERIFY_FLASH, m_VerifyFLASHCtrl);
	DDX_Control(pDX, IDC_CAPTURE_VERIFY_FLASH, m_CaptureVerifyFLASHCtrl);
	DDX_Control(pDX, IDC_VERIFY_RAM, m_VerifyRamCtrl);
	DDX_Control(pDX, IDC_UPLOAD_TEMPLATE, m_UploadTemplateCtrl);
	DDX_Control(pDX, IDC_CAPTURE_VERIFY, m_CaptureVerifyCtrl);
	DDX_Control(pDX, IDC_SET_BAUD_RATE, m_SetBaudCtrl);
	DDX_Control(pDX, IDC_IDENTIFY, m_IdentifyCtrl);
	DDX_Control(pDX, IDC_CAPTURE_IDENTIFY, m_CaptureIdentifyCtrl);
	DDX_Control(pDX, IDC_FIRMWARE_VERSION, m_FirmwareCtrl);
	DDX_Control(pDX, IDC_DOWNLOAD_TEMPLATE, m_DownloadTemplateCtrl);
	DDX_Control(pDX, IDC_DELETE_SLOT, m_DeleteSlotCtrl);
	DDX_Control(pDX, IDC_DELETE_ALL, m_DeleteAllCtrl);
	DDX_Control(pDX, IDC_ENROL_RAM, m_EnrolCtrl);
	DDX_Control(pDX, IDC_COPY_TEMPLATE_RAM_TO_FLASH, m_CopyCtrl);
	DDX_Control(pDX, IDC_CAPTURE, m_CaptureCtrl);
	DDX_Control(pDX, IDC_CANCEL, m_CancelCtrl);
	DDX_Control(pDX, IDC_VIEW, m_ViewCtrl);
	DDX_Control(pDX, IDC_HARDWARE, m_HardwareCtrl);
	DDX_Control(pDX, IDC_SLOT_EDIT, m_SlotCtrl);
	DDX_Control(pDX, IDC_CHAR, m_CharCtrl);
	DDX_Control(pDX, IDC_HEX, m_HexCtrl);
	DDX_Control(pDX, IDC_LIST1, m_ListBoxCtrl);
	DDX_Control(pDX, IDC_STATUS, m_StatusCtrl);
	DDX_Control(pDX, IDC_STOP_BITS, m_StopCtrl);
	DDX_Control(pDX, IDC_PARITY, m_ParityCtrl);
	DDX_Control(pDX, IDC_BITS, m_BitsCtrl);
	DDX_Control(pDX, IDC_BAUD, m_BaudCtrl);
	DDX_Control(pDX, IDC_PORT, m_PortCtrl);
	DDX_CBString(pDX, IDC_PORT, m_PortValue);
	DDX_CBString(pDX, IDC_BAUD, m_BaudValue);
	DDX_CBString(pDX, IDC_BITS, m_BitsValue);
	DDX_CBString(pDX, IDC_HARDWARE, m_HardwareValue);
	DDX_CBString(pDX, IDC_VIEW, m_ViewValue);
	DDX_CBString(pDX, IDC_PARITY, m_ParityValue);
	DDX_CBString(pDX, IDC_STOP_BITS, m_StopValue);
	DDX_Text(pDX, IDC_CUSTOM_COMMAND, m_CustomCommand);
	DDV_MaxChars(pDX, m_CustomCommand, 400);
	DDX_Text(pDX, IDC_SLOT_EDIT, m_SlotValue);
	DDX_CBString(pDX, IDC_TEMPLATE_NAME, m_TemplateNameValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CComDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_CAPTURE_ENROL, OnCaptureEnrolRAM)
	ON_BN_CLICKED(IDC_CAPTURE_VERIFY, OnCaptureVerifyRAM)
	ON_BN_CLICKED(IDC_SET_BAUD_RATE, OnSetBaudRate)
	ON_BN_CLICKED(IDC_DOWNLOAD_TEMPLATE, OnDownloadTemplate)
	ON_BN_CLICKED(IDC_UPLOAD_TEMPLATE, OnUploadTemplate)
	ON_BN_CLICKED(IDC_FIRMWARE_VERSION, OnFirmwareVersion)
	ON_BN_CLICKED(IDC_CAPTURE_VERIFY_FLASH, OnCaptureVerifyFLASH)
	ON_BN_CLICKED(IDC_UPLOAD_SLICE, OnUploadImage)
	ON_BN_CLICKED(IDC_CANCEL, OnCancelCommand)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_CHAR, OnChar)
	ON_BN_CLICKED(IDC_HEX, OnHex)
	ON_BN_CLICKED(IDC_COPY_TEMPLATE_RAM_TO_FLASH, OnCopyTemplateRamToFLASH)
	ON_BN_CLICKED(IDC_CAPTURE, OnCapture)
	ON_CBN_SELENDOK(IDC_VIEW, OnSelendokView)
	ON_BN_CLICKED(IDC_DELETE_SLOT, OnDeleteSlot)
	ON_BN_CLICKED(IDC_DELETE_ALL, OnDeleteAll)
	ON_BN_CLICKED(IDC_UPLOAD_TEMPLATE_FLASH, OnUploadTemplateFLASH)
	ON_BN_CLICKED(IDC_CAPTURE_IDENTIFY, OnCaptureIdentify)
	ON_BN_CLICKED(IDC_IDENTIFY, OnIdentify)
	ON_BN_CLICKED(IDC_ENROL_RAM, OnEnrolRam)
	ON_BN_CLICKED(IDC_VERIFY_FLASH, OnVerifyFLASH)
	ON_BN_CLICKED(IDC_DOWNLOAD_IMAGE, OnDownloadImage)
	ON_CBN_SELENDOK(IDC_HARDWARE, OnSelendokHardware)
	ON_BN_CLICKED(IDC_VERIFY_RAM, OnVerifyRam)
	ON_BN_CLICKED(IDC_TEST_HARDWARE, OnTestHardware)
	ON_BN_CLICKED(IDC_INITIATE_UPGRADE, OnInitiateUpgrade)
	ON_BN_CLICKED(IDC_UPGRADE, OnUpgrade)
	ON_BN_CLICKED(IDC_RUN_INSTALLER, OnRunInstaller)
	ON_BN_CLICKED(IDC_CPTENRRAMDUAL, OnCaptureEnrolRamDual)
	ON_BN_CLICKED(IDC_UPLOADALL, OnUploadall)
	ON_BN_CLICKED(IDC_DOWNLOADALL, OnDownloadall)
	ON_WM_DEVICECHANGE()
	ON_LBN_SELCHANGE(IDC_LIST1, &CComDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComDlg message handlers
// 

BOOL CComDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Extra initialization here
	m_crColor = RGB(0,0,0);//Black
	//Fill the dialog dropboxes with correct values
	m_IsInitiated=false;
	FillDropBoxes();
	//Here we set the default serial settings
	SetDefaultSettings();
	SetLastSettings();

	m_CompactButton.SetCheck(1); //Default Value - compact mode

	WINDOWPLACEMENT wndpl;
	m_ListBoxCtrl.GetWindowPlacement(&wndpl);
	m_movePixels = wndpl.rcNormalPosition.top;
	m_TextCtrl4.GetWindowPlacement(&wndpl);
	m_movePixels -= wndpl.rcNormalPosition.top;

	m_CurrentView = m_ViewCtrl.GetCurSel();
	m_CurrentHardware = m_HardwareCtrl.GetCurSel();
	m_UseSPIFlag = 0; //Use UART as default communication protocol.
	m_SwipeImageSliceCounter = -1;
	m_SwipeImageSliceCounterMax = -1;
	m_SwipeImageBuffer = 0;
	OnSelendokView();

	FillTemplateDropBox();

	m_IsInitiated=true;
	m_Command = new CSerialCommand("COM1");
	m_SPICommand = new CSPICommand();
	m_StatusCtrl.SetWindowText(_T("Status: Not connected..."));

	m_HexCtrl.EnableWindow(FALSE);
	m_CharCtrl.EnableWindow(TRUE);
	m_ListBoxCtrl.InitStorage(2500,80000);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CComDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CComDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//This is added just to change font color in the list box..
HBRUSH CComDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	

	if (pWnd->GetDlgCtrlID() == IDC_LIST1)
	{
		UpdateData(TRUE);
		// the listbox has input focus
		pDC->SetTextColor(m_crColor);//RGB(GetRValue(m_crColor),GetGValue(m_crColor),
		pDC->SetBkMode(TRANSPARENT);
		CRect rect;
		m_ListBoxCtrl.GetItemRect(0,rect);
		rect.OffsetRect(0,400);
		ValidateRect(rect);
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CComDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


//*************** Settings and init functions ***********************************************************//

//Set all serial settings to default value..
void CComDlg::SetDefaultSettings() 
{
	m_HardwareCtrl.SetCurSel(0);
	m_ViewCtrl.SetCurSel(m_ViewCtrl.FindString(-1, _T("Basic")));
	m_PortCtrl.SetCurSel(0);//Always set first found port to default
	m_BaudCtrl.SetCurSel(m_BaudCtrl.FindString(-1, _T("9600"))); //9600 baud is default
	m_BitsCtrl.SetCurSel(m_BitsCtrl.FindString(-1, _T("8"))); // 8 data bits 
	m_ParityCtrl.SetCurSel(m_ParityCtrl.FindString(-1, _T("Odd"))); //Odd parity 
	m_StopCtrl.SetCurSel(m_StopCtrl.FindString(-1, _T("1"))); //1 stop bit
	
}

//Set all serial settings to last used value..
void CComDlg::SetLastSettings() 
{
	wchar_t text[50];
	int index0=0, index1=0;
	CString value;
	CString EoL;
	EoL.Format(_T("\n"));
	
	FILE *fil;//All values are saved in a text file..
	fopen_s(&fil, "FPC_Settings.txt", "rt");
	if (fil == NULL){
		return;
	}

	/* Set pointer to beginning of file: */
    fseek( fil, 0L, SEEK_SET );

	_fgetts(text,50,fil);
	value = CString(text);
	m_HardwareCtrl.SetCurSel(m_HardwareCtrl.FindString(-1,value.Left(value.GetLength()-1)));

	_fgetts(text,50,fil);
	value = CString(text);
	m_ViewCtrl.SetCurSel(m_ViewCtrl.FindString(-1,value.Left(value.GetLength()-1)));

	_fgetts(text,50,fil);
	value = CString(text);
	m_PortCtrl.SetCurSel(m_PortCtrl.FindString(-1,value.Left(value.GetLength()-1)));

	_fgetts(text,50,fil);
	value = CString(text);
	m_BaudCtrl.SetCurSel(m_BaudCtrl.FindString(-1,value.Left(value.GetLength()-1)));

	_fgetts(text,50,fil);
	value = CString(text);
	m_BitsCtrl.SetCurSel(m_BitsCtrl.FindString(-1,value.Left(value.GetLength()-1)));

	_fgetts(text,50,fil);
	value = CString(text);
	m_StopCtrl.SetCurSel(m_StopCtrl.FindString(-1,value.Left(value.GetLength()-1)));

	_fgetts(text,50,fil);
	value = CString(text);
	m_ParityCtrl.SetCurSel(m_ParityCtrl.FindString(-1,value.Left(value.GetLength()-1)));

	fclose(fil);
  
}

//Fill the dialog dropboxes with correct values
void CComDlg::FillDropBoxes()
{



	//First dropbox is the ComPort box
	SerialPortList pPorts = m_Ports.Enumerate();//Find available ports

	m_PortCtrl.ResetContent();
		for (SerialPortListIterator it = pPorts.begin(); it != pPorts.end(); it++)
		{
			CString SerialPort = (*it).c_str();
			m_PortCtrl.AddString(SerialPort);//Add found ports to dropbox
		}
	//m_PortCtrl.AddString("USB-SPI");

	//Hardware
	m_HardwareCtrl.ResetContent();
	m_HardwareCtrl.AddString(_T("FPC-EMD")); //Also known as FPC2020 in this code...Fully compatible with FPC-AM2.
	m_HardwareCtrl.AddString(_T("FPC-AM3"));

	//Viewmode
	m_ViewCtrl.ResetContent();
	m_ViewCtrl.AddString(_T("Basic"));
	m_ViewCtrl.AddString(_T("Extended"));

	//Baud
	m_BaudCtrl.ResetContent();
	m_BaudCtrl.AddString(_T("9600"));
	m_BaudCtrl.AddString(_T("14400"));
	m_BaudCtrl.AddString(_T("19200"));
	m_BaudCtrl.AddString(_T("28800"));
	m_BaudCtrl.AddString(_T("38400"));
	m_BaudCtrl.AddString(_T("57600"));
	m_BaudCtrl.AddString(_T("76800"));		//This speed does not seem to be supported by every PC, therefore it has been removed.
	m_BaudCtrl.AddString(_T("115200"));
//	m_BaudCtrl.AddString("230400");		//I can't get this to work on my computer. Hardware related?

	//Data bits
	m_BitsCtrl.ResetContent();
	m_BitsCtrl.AddString(_T("4"));
	m_BitsCtrl.AddString(_T("5"));
	m_BitsCtrl.AddString(_T("6"));
	m_BitsCtrl.AddString(_T("7"));
	m_BitsCtrl.AddString(_T("8"));

	//Parity
	m_ParityCtrl.ResetContent();
	m_ParityCtrl.AddString(_T("Odd"));
	m_ParityCtrl.AddString(_T("Even"));
	m_ParityCtrl.AddString(_T("None"));

	//StopBits
	m_StopCtrl.ResetContent();
	m_StopCtrl.AddString(_T("1"));
	m_StopCtrl.AddString(_T("1.5"));
	m_StopCtrl.AddString(_T("2"));
}

//Enable or disable the windows
void CComDlg::EnableWindows(bool mode){

		m_TestHardwareCtrl.EnableWindow(mode);
		m_CaptureCtrl.EnableWindow(mode);
		m_VerifyRamCtrl.EnableWindow(mode);
		m_UploadImageCtrl.EnableWindow(mode);
		m_CaptureEnrolCtrl.EnableWindow(mode);
		m_CaptureVerifyCtrl.EnableWindow(mode);
		m_CancelCtrl.EnableWindow(mode);
		m_SendCtrl.EnableWindow(mode);
		m_DownloadImageCtrl.EnableWindow(mode);		
		m_CustomCtrl.EnableWindow(mode);
		m_VerifyFLASHCtrl.EnableWindow(mode);
		m_CaptureVerifyFLASHCtrl.EnableWindow(mode);
		m_UploadTemplateCtrl.EnableWindow(mode);
		m_UploadTemplateFLASHCtrl.EnableWindow(mode);
		m_SetBaudCtrl.EnableWindow(mode);
		m_IdentifyCtrl.EnableWindow(mode);
		m_CaptureIdentifyCtrl.EnableWindow(mode);
		m_FirmwareCtrl.EnableWindow(mode);
		m_DownloadTemplateCtrl.EnableWindow(mode);
		m_DeleteSlotCtrl.EnableWindow(mode);
		m_DeleteAllCtrl.EnableWindow(mode);
		m_EnrolCtrl.EnableWindow(mode);
		m_CopyCtrl.EnableWindow(mode);
		m_SlotCtrl.EnableWindow(mode);
		//m_UpgradeCtrl.EnableWindow(mode);
		//m_InitiateUpgradeCtrl.EnableWindow(mode);
		m_DownloadAll.EnableWindow(mode);
		m_UploadAll.EnableWindow(mode);
		
}
//Get all serial settings from the text file..
void CComDlg::GetSettings() 
{
	
	UpdateData();
	CString textString;
	FILE *fil;
	fopen_s(&fil, "FPC_Settings.txt","wt");

	textString.Format(_T("%s\n"), m_HardwareValue);
	_fputts(textString, fil);
	textString.Format(_T("%s\n"), m_ViewValue);
	_fputts(textString, fil);
	textString.Format(_T("%s\n"), m_PortValue);
	_fputts(textString, fil);
	textString.Format(_T("%s\n"), m_BaudValue);
	_fputts(textString, fil);
	textString.Format(_T("%s\n"), m_BitsValue);
	_fputts(textString, fil);
	textString.Format(_T("%s\n"), m_StopValue);
	_fputts(textString, fil);
	textString.Format(_T("%s\n"), m_ParityValue);
	_fputts(textString, fil);

	fclose(fil);
}


//View mode is selected, change size and hide or show some buttons
void CComDlg::OnSelendokView() 
{
	if (m_IsInitiated){
		if (m_CurrentView == m_ViewCtrl.GetCurSel()){
			return;
		}	
	}

	m_CurrentView = m_ViewCtrl.GetCurSel();
	if (m_ViewCtrl.GetCurSel()){ //If current selected is = 1, then show extended view.. all should be visible..
		
		m_StaticCtrl1.ShowWindow(SW_SHOW);
		m_StaticCtrl2.ShowWindow(SW_SHOW);
		m_StaticCtrl3.ShowWindow(SW_SHOW);
		m_StaticCtrl4.ShowWindow(SW_SHOW);
		m_TextCtrl7.ShowWindow(SW_SHOW);
		m_TextCtrl6.ShowWindow(SW_SHOW);
		m_TextCtrl5.ShowWindow(SW_SHOW);
		m_TextCtrl4.ShowWindow(SW_SHOW);
		m_TextCtrl2.ShowWindow(SW_SHOW);	
		m_SendCtrl.ShowWindow(SW_SHOW);;
		m_InputCtrl.ShowWindow(SW_SHOW);
		m_DownloadImageCtrl.ShowWindow(SW_SHOW);
		m_DivCtrl4.ShowWindow(SW_SHOW);
		m_DivCtrl3.ShowWindow(SW_SHOW);
		m_DivCtrl1.ShowWindow(SW_SHOW);

		m_DefaultCtrl.ShowWindow(SW_SHOW);
		m_CustomCtrl.ShowWindow(SW_SHOW);
		m_VerifyFLASHCtrl.ShowWindow(SW_SHOW);
		m_CaptureVerifyFLASHCtrl.ShowWindow(SW_SHOW);
		m_UploadTemplateCtrl.ShowWindow(SW_SHOW);
		m_UploadTemplateFLASHCtrl.ShowWindow(SW_SHOW);
		m_SetBaudCtrl.ShowWindow(SW_SHOW);
		m_IdentifyCtrl.ShowWindow(SW_SHOW);
		m_CaptureIdentifyCtrl.ShowWindow(SW_SHOW);
		m_FirmwareCtrl.ShowWindow(SW_SHOW);
		m_DownloadTemplateCtrl.ShowWindow(SW_SHOW);
		m_DeleteSlotCtrl.ShowWindow(SW_SHOW);
		m_DeleteAllCtrl.ShowWindow(SW_SHOW);
		m_EnrolCtrl.ShowWindow(SW_SHOW);
		m_CopyCtrl.ShowWindow(SW_SHOW);
		m_SlotCtrl.ShowWindow(SW_SHOW);
		m_VerifyRamCtrl.ShowWindow(SW_SHOW);
		m_TemplateNameCtrl.ShowWindow(SW_SHOW);
		m_TextCtrl10.ShowWindow(SW_SHOW);
		
		m_StopCtrl.ShowWindow(SW_SHOW);
		m_ParityCtrl.ShowWindow(SW_SHOW);
		m_BitsCtrl.ShowWindow(SW_SHOW);
		m_BaudCtrl.ShowWindow(SW_SHOW);

		//m_UpgradeCtrl.ShowWindow(SW_SHOW);
		//m_InitiateUpgradeCtrl.ShowWindow(SW_SHOW);
		m_DownloadAll.ShowWindow(SW_SHOW);
		m_UploadAll.ShowWindow(SW_SHOW);
		
		if (m_IsInitiated){

			RECT rect;
			WINDOWPLACEMENT wndpl;
			//Move these
			this->GetWindowRect(&rect);
			
			rect.bottom = rect.bottom + m_movePixels ;this->MoveWindow(&rect);

			m_CharCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition; m_CharCtrl.SetWindowPos(NULL,rect.left,rect.top + m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
			m_HexCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition; m_HexCtrl.SetWindowPos(NULL,rect.left,rect.top + m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
			m_StatusCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition;m_StatusCtrl.SetWindowPos(NULL,rect.left,rect.top + m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
			m_ListBoxCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition;m_ListBoxCtrl.SetWindowPos(NULL,rect.left,rect.top + m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
			m_OkCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition; m_OkCtrl.SetWindowPos(NULL,rect.left,rect.top + m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
			
			m_VertDivCtrl.GetWindowPlacement(&wndpl);rect = wndpl.rcNormalPosition; m_VertDivCtrl.SetWindowPos(NULL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top +m_movePixels, SWP_NOZORDER);
			m_ClearCtrl.GetWindowPlacement(&wndpl);rect = wndpl.rcNormalPosition; m_ClearCtrl.SetWindowPos(NULL,rect.left,rect.top + m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
			InvalidateRect(NULL);
			OnPaint();
		}


	}else{//A lot should be invisible..

		m_StaticCtrl1.ShowWindow(SW_HIDE);
		m_StaticCtrl2.ShowWindow(SW_HIDE);
		m_StaticCtrl3.ShowWindow(SW_HIDE);
		m_StaticCtrl4.ShowWindow(SW_HIDE);
		m_TextCtrl7.ShowWindow(SW_HIDE);
		m_TextCtrl6.ShowWindow(SW_HIDE);
		m_TextCtrl5.ShowWindow(SW_HIDE);
		m_TextCtrl4.ShowWindow(SW_HIDE);
		m_TextCtrl2.ShowWindow(SW_HIDE);	
		m_SendCtrl.ShowWindow(SW_HIDE);;
		m_InputCtrl.ShowWindow(SW_HIDE);
		m_DownloadImageCtrl.ShowWindow(SW_HIDE);
		m_DivCtrl4.ShowWindow(SW_HIDE);
		m_DivCtrl3.ShowWindow(SW_HIDE);
		m_DivCtrl1.ShowWindow(SW_HIDE);
		m_DefaultCtrl.ShowWindow(SW_HIDE);
		m_CustomCtrl.ShowWindow(SW_HIDE);
		m_VerifyFLASHCtrl.ShowWindow(SW_HIDE);
		m_CaptureVerifyFLASHCtrl.ShowWindow(SW_HIDE);
		m_UploadTemplateCtrl.ShowWindow(SW_HIDE);
		m_UploadTemplateFLASHCtrl.ShowWindow(SW_HIDE);
		m_SetBaudCtrl.ShowWindow(SW_HIDE);
		m_IdentifyCtrl.ShowWindow(SW_HIDE);
		m_CaptureIdentifyCtrl.ShowWindow(SW_HIDE);
		m_FirmwareCtrl.ShowWindow(SW_HIDE);
		m_DownloadTemplateCtrl.ShowWindow(SW_HIDE);
		m_DeleteSlotCtrl.ShowWindow(SW_HIDE);
		m_DeleteAllCtrl.ShowWindow(SW_HIDE);
		m_EnrolCtrl.ShowWindow(SW_HIDE);
		m_CopyCtrl.ShowWindow(SW_HIDE);
		m_SlotCtrl.ShowWindow(SW_HIDE);
		m_StopCtrl.ShowWindow(SW_HIDE);
		m_ParityCtrl.ShowWindow(SW_HIDE);
		m_BitsCtrl.ShowWindow(SW_HIDE);
		m_BaudCtrl.ShowWindow(SW_HIDE);
		m_VerifyRamCtrl.ShowWindow(SW_HIDE);
		m_TemplateNameCtrl.ShowWindow(SW_HIDE);
		m_TextCtrl10.ShowWindow(SW_HIDE);
		//m_UpgradeCtrl.ShowWindow(SW_HIDE);
		//m_InitiateUpgradeCtrl.ShowWindow(SW_HIDE);
		m_DownloadAll.ShowWindow(SW_HIDE);
		m_UploadAll.ShowWindow(SW_HIDE);

		
		//Move these
		
		RECT rect;

		WINDOWPLACEMENT wndpl;
	
		this->GetWindowRect(&rect);rect.bottom = rect.bottom - m_movePixels ;this->MoveWindow(&rect);

		m_CharCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition; m_CharCtrl.SetWindowPos(NULL,rect.left,rect.top - m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
		m_HexCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition; m_HexCtrl.SetWindowPos(NULL,rect.left,rect.top - m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
		m_StatusCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition;m_StatusCtrl.SetWindowPos(NULL,rect.left,rect.top - m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
		m_ListBoxCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition;m_ListBoxCtrl.SetWindowPos(NULL,rect.left,rect.top - m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
		m_OkCtrl.GetWindowPlacement(&wndpl); rect = wndpl.rcNormalPosition; m_OkCtrl.SetWindowPos(NULL,rect.left,rect.top - m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
		
		m_VertDivCtrl.GetWindowPlacement(&wndpl);rect = wndpl.rcNormalPosition; m_VertDivCtrl.SetWindowPos(NULL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top-m_movePixels, SWP_NOZORDER);
		m_ClearCtrl.GetWindowPlacement(&wndpl);rect = wndpl.rcNormalPosition; m_ClearCtrl.SetWindowPos(NULL,rect.left,rect.top - m_movePixels,1,1,SWP_NOSIZE| SWP_NOZORDER);
		
		if (m_IsInitiated){
			InvalidateRect(NULL);
			OnPaint();
		}

	}
	OnSelendokHardware();
}

//Hardware is selected.. show or hide appropriate buttons
void CComDlg::OnSelendokHardware() 
{
	//This variable can be used for separating between different hardware configurations, in the future...
	m_CurrentHardware = m_HardwareCtrl.GetCurSel();

	//These changes are only meaningful if extended view mode is selected
	if ( !m_CurrentView){
		return;
	}

	//Show all buttons
	FillTemplateDropBox();
	m_DownloadImageCtrl.ShowWindow(SW_SHOW);
	m_VerifyFLASHCtrl.ShowWindow(SW_SHOW);
	m_VerifyRamCtrl.ShowWindow(SW_SHOW);
	m_IdentifyCtrl.ShowWindow(SW_SHOW);
	m_CaptureIdentifyCtrl.ShowWindow(SW_SHOW);
	m_EnrolCtrl.ShowWindow(SW_SHOW);

}
	
//**************************************************************************************************//
//***************Here comes all the pushbutton functions********************************************//
//***************Roughly the same order as they appear in the dialog********************************//
//**************************************************************************************************//


//Connect to the serial port according to the user settings
void CComDlg::OnConnect() 
{
	//Check if they have selected hardware mode.. 
	if ( m_HardwareCtrl.GetCurSel() == LB_ERR ){ //No hardware selected
		AfxMessageBox(_T("Please select hardware mode."));
		return;
	}
	//First get user settings
	GetSettings();

	if (m_Command->IsOpen()){
		m_Command->Close();//Close if it's already open
	}
	if (m_SPICommand->IsOpen()){
		m_SPICommand->Close();//Close if it's already open
	}

	if (m_PortValue == "USB-SPI"){ 
		//Setup the SPI interface:
		m_StatusCtrl.SetWindowText(_T("Status: Initializing SPI Interface"));
		m_SPICommand->Open(m_PortValue, this); 
		m_UseSPIFlag=1;
		//Enable all windows..	
		EnableWindows(true);
		return;
	} else {
		m_UseSPIFlag=0;
	}
	//If we get here, go ahead and Connect and open the serial port.
	m_Command->Open(m_PortValue, strtol(CT2A(m_BaudValue),NULL,10), strtol(CT2A(m_BitsValue),NULL,10), m_ParityValue, strtol(CT2A(m_StopValue),NULL,10),this);
	if (m_Command->IsOpen()){
		CString String = " ";
		String.Format(_T("Status: Connected to port %s ( %s baud - %s data bits - %s parity - %s stop bits )"),m_PortValue,m_BaudValue,m_BitsValue,m_ParityValue,m_StopValue);
		m_StatusCtrl.SetWindowText(String);
		//Enable all windows..	
		EnableWindows(true);

	}else{
		m_StatusCtrl.SetWindowText(_T("Status: Connection failed..."));
	}	
}


//User want to set all values to default values
void CComDlg::OnDefault() 
{
	SetDefaultSettings();	
}


//***************Basic command functions *************************************************************//


//Test that communication and hardware is ok
void CComDlg::OnTestHardware() 
{
	EnableWindows(false);
	if (m_UseSPIFlag)
		m_SPICommand->ApiTestHardware();
	else
		m_Command->ApiTestHardware();
	EnableWindows(true);
}


//Capture an image from sensor
void CComDlg::OnCapture() 
{
	EnableWindows(false);
	if (m_UseSPIFlag)
		m_SPICommand->ApiCaptureImage();
	else
		m_Command->ApiCaptureImage();
	EnableWindows(true);
}


//Upload one image from the image memory.. Save it on PC and show it on screen
void CComDlg::OnUploadImage() 
{
	DWORD imSize;
	BITMAPINFO *bDIB;
	BITMAPFILEHEADER *BFH;

	LONG paintWidth, paintHeight;

	double edge, fraction, intp;

	unsigned char *imageBuffer = 0;
	
	EnableWindows(false);
	m_StatusCtrl.SetWindowText(_T("Status: Image is uploading, please wait."));

	bool ok; 
		
	if (m_UseSPIFlag)
		ok = m_SPICommand->ApiUploadImage(&imageBuffer, &imSize);
	else
		ok = m_Command->ApiUploadImage(&imageBuffer, 0, &imSize);

	if (!ok) return;

    //Save image to disk..
	FILE *fil;
	fopen_s(&fil, "testImage.bmp","wb");

	edge = sqrt((double)imSize);
	fraction = modf(edge, &intp);
	if (fraction == 0.0) {
		paintWidth	= (LONG)intp;
		paintHeight = (LONG)intp;
	}
	else {
		if (imSize == 30400) {
			paintWidth = 152;
			paintHeight = 200;
		}
		else {
			return;
		}
	}

	/** Create BMP header */ 
	BFH = (BITMAPFILEHEADER *)malloc(sizeof(BITMAPFILEHEADER));
	BFH->bfType			= 0x4D42;
	BFH->bfReserved1	= 0;
	BFH->bfReserved2	= 0;
	BFH->bfSize			= (paintWidth * paintHeight) + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + 256 * sizeof(RGBQUAD);
	BFH->bfOffBits		= sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + 256 * sizeof(RGBQUAD);

	/** DIB Header */
	bDIB = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
	bDIB->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bDIB->bmiHeader.biWidth			= paintWidth;
	bDIB->bmiHeader.biHeight		= paintHeight;
	bDIB->bmiHeader.biPlanes		= 1; // Always
	bDIB->bmiHeader.biBitCount		= 8; // 8 for BW
	bDIB->bmiHeader.biCompression	= BI_RGB; // Not compressed
	bDIB->bmiHeader.biSizeImage		= imSize; // Size in bytes
	bDIB->bmiHeader.biXPelsPerMeter = 0; // Not used
	bDIB->bmiHeader.biYPelsPerMeter = 0; // No used
	bDIB->bmiHeader.biClrUsed		= 256; // Colors actually used
	bDIB->bmiHeader.biClrImportant	= 0; // All colors are important
	for (int i = 0; i < 256; i++) {
		bDIB->bmiColors[i].rgbRed		= i;
		bDIB->bmiColors[i].rgbGreen		= i;
		bDIB->bmiColors[i].rgbBlue		= i;
		bDIB->bmiColors[i].rgbReserved	= i;
	}

	fwrite((unsigned char *)BFH, sizeof(char), sizeof(BITMAPFILEHEADER), fil);
	fwrite((unsigned char *)bDIB, sizeof(char), sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD), fil);
	fwrite(imageBuffer, sizeof(char), imSize, fil);
	fclose(fil);

	free(BFH);
	free(bDIB);

	if (imageBuffer) {
		free(imageBuffer);
	}

	m_StatusCtrl.SetWindowText(_T("Status: Image is uploaded and saved on PC."));
	//Show image on screen
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
	EnableWindows(true);
}

//Enrol a fingerprint (a template will be created and placed in RAM)
//The function will wait until a finger is present or the function is canceled
void CComDlg::OnCaptureEnrolRAM() 
{
	EnableWindows(false);
	m_CancelCtrl.EnableWindow(true);
	//Get the FLASH slot information:
	UpdateData(true);
	m_StatusCtrl.SetWindowText(_T("Please capture finger image, then wait for finger to enrol.."));

	bool commandOk=false;

	if (m_UseSPIFlag)
		commandOk = m_SPICommand->ApiCaptureEnrolRAM();
	else
		commandOk = m_Command->ApiCaptureEnrolRAM();

	if (commandOk){
		MessageBeep(MB_ICONEXCLAMATION); //OK sound
	} else {
		MessageBeep(0xFFFFFFFF); //Error sound
	}


	EnableWindows(true);
}

//Do a verification. This requires that a template is located in RAM.
//The function will wait until a finger is present or the function is canceled
//!!!!During swipe sensor verifications, the template in RAM will be destroyed, 
//so you have to download a template before every verification!!!!
void CComDlg::OnCaptureVerifyRAM() 
{
	EnableWindows(false);
	m_CancelCtrl.EnableWindow(true);
	//Get the FLASH slot information
	UpdateData(true);
	m_StatusCtrl.SetWindowText(_T("Please capture finger image, then wait for finger to verify.."));
	bool commandOk=false;
	if (m_UseSPIFlag)
		commandOk = m_SPICommand->ApiCaptureVerifyRAM();
	else 
		commandOk = m_Command->ApiCaptureVerifyRAM();
	if (commandOk)
		MessageBeep(MB_ICONEXCLAMATION); //OK sound
	else
		MessageBeep(0xFFFFFFFF); //Error sound

	EnableWindows(true);
}

//Send cancel to abort current operation..
//Only useful during enrol and verify operations with fingerpresent checks
void CComDlg::OnCancelCommand() 
{
	if (m_UseSPIFlag)
		m_SPICommand->ApiCancel();
	else
		m_Command->ApiCancel();

	//Cancel all pending IO operations 
	//(if something is left from whatever process we have cancelled).
	//This can be done by ReConnecting...
	OnConnect();
	m_StatusCtrl.SetWindowText(_T("Status: Operation Cancelled!"));
}


//*************** Template handling *************************************************************//


//Upload template from RAM and store it on PC..
//Template must first be created using enrol
void CComDlg::OnUploadTemplate() 
{
	int templSize;
	unsigned char *templBuffer = 0;


	CString currName="";
	m_TemplateNameCtrl.GetWindowText(currName);

	if (currName == ""){
		AfxMessageBox(_T("Please write a template file name before uploading template.."));
		return;
	}

	EnableWindows(false);
	
	
	m_StatusCtrl.SetWindowText(_T("Status: Uploading template.."));

	bool ok;
	if (m_UseSPIFlag)
		ok = m_SPICommand->ApiUploadTemplate(templBuffer);
	else
		ok = m_Command->ApiUploadTemplate(&templBuffer, &templSize);

	if (!templBuffer) {
		//Not a valid Area Template!
		ok = false;
		m_StatusCtrl.SetWindowText(_T("Status: Not a valid template."));
	}
	
	//Save template to disk
	if (ok) {
		currName = currName + ".tpl";
		
		FILE *fil;
		fopen_s(&fil, CT2A(currName),"wb");
		fwrite(templBuffer, sizeof(char), templSize, fil);
		fclose(fil);

		m_StatusCtrl.SetWindowText(_T("Status: Template uploaded and saved to disk.."));
	}

	/* Free template data */
	if (templBuffer != 0) {
		free(templBuffer);
	}
	FillTemplateDropBox();
	EnableWindows(true);
}


//Read template from PC and send it to RAM
void CComDlg::OnDownloadTemplate() 
{
	struct stat st;
	EnableWindows(true);
								   
	CString currName="";
	m_TemplateNameCtrl.GetWindowText(currName);

	if (currName == ""){
		AfxMessageBox(_T("Please enter a template file name from the list before downloading a template.."));
		return;
	}
	//Read template from file
	FILE *fil = 0;
	currName = currName + ".tpl";
	fopen_s(&fil, CT2A(currName),"rb");

	if (fil){
		int				templSize;
		unsigned char	*templBuffer;

		stat(CT2A(currName), &st);
		templSize	= st.st_size;
		templBuffer = (unsigned char *)malloc(sizeof(char) * templSize);
		fread(templBuffer, sizeof(char), templSize, fil);

		fclose(fil);

		//Get the FLASH slot information
		UpdateData(true);
		
		//Send template
		bool ok;
		if (m_UseSPIFlag) {
			ok = m_SPICommand->ApiDownloadTemplate(templBuffer, templSize);
		} else {
			ok = m_Command->ApiDownloadTemplate(templBuffer, templSize);
		}

		free(templBuffer);

		if (ok) {
			m_StatusCtrl.SetWindowText(_T("Status: Template downloaded.."));
		} else {
			m_StatusCtrl.SetWindowText(_T("Status: Template download failed.."));
		}
	} else {
		AfxMessageBox(_T("Template file name not found in list.."));
		return;
	}
	EnableWindows(true);
}

//Copy template from RAM to FLASH, using slot information on where to store it
//Be sure that there is a valid template in RAM before calling this function..
//Template is located in RAM after an ENROL call or after DOWNLOAD_TEMPLATE
void CComDlg::OnCopyTemplateRamToFLASH()  
{
	EnableWindows(false);
	//Get the FLASH slot information
	UpdateData(true);

	//Check if slot value is valid (0-1 for swipe)
	bool inRange = m_SlotValue >= 0 && m_SlotValue <= 49;
	if (!inRange){
		AfxMessageBox(_T("Error: Slot index out of range. Please enter a valid flash slot number."));
		EnableWindows(true);
		return;
	}
	bool commandOk=false;
	if (m_UseSPIFlag)
		commandOk = m_SPICommand->ApiCopyTemplateRamToFLASH(m_SlotValue);
	else
		commandOk = m_Command->ApiCopyTemplateRamToFLASH(m_SlotValue);

	if (commandOk){
		CString mess; mess.Format(_T("Status: Template copied to FLASH slot number %d .."), m_SlotValue);
		m_StatusCtrl.SetWindowText(mess);
	}else{
		m_StatusCtrl.SetWindowText(_T("Status: Template NOT copied, operation failed.."));
	}
	EnableWindows(true);
}

//Delete a template in FLASH slot
void CComDlg::OnDeleteSlot() 
{
	EnableWindows(false);
	//Get the FLASH slot information
	UpdateData(true);

	//Check if slot value is valid (0-1 for swipe)
	bool inRange = m_SlotValue >= 0 && m_SlotValue <= 49;
	if (!inRange){
		AfxMessageBox(_T("Error: Slot index out of range. Please enter a valid flash slot number."));
		EnableWindows(true);
		return;
	}
	bool ok;
	if (m_UseSPIFlag)
		ok = m_SPICommand->ApiDeleteTemplateSlot(m_SlotValue);
	else
		ok = m_Command->ApiDeleteTemplateSlot(m_SlotValue);

	//Status
	if (ok){
		CString mess; mess.Format(_T("Status: Template in slot number %d deleted.."), m_SlotValue);
		m_StatusCtrl.SetWindowText(mess);
	}else{
		m_StatusCtrl.SetWindowText(_T("Status: Failure, slot NOT deleted.. "));
	}
	EnableWindows(true);
}

//Delete ALL templates in FLASH
void CComDlg::OnDeleteAll() 
{
	EnableWindows(false);
	if (AfxMessageBox(_T("This command deletes ALL templates in memory. Do you wish to continue?"),MB_YESNO) == IDNO){

		m_StatusCtrl.SetWindowText(_T("Status: Operation aborted.."));
		EnableWindows(true);
		return;

	}

	bool ok;
	if (m_UseSPIFlag)
		ok = m_SPICommand->ApiDeleteAllInFLASH();
	else
		ok = m_Command->ApiDeleteAllInFLASH();

	//Status
	if (ok){
		m_StatusCtrl.SetWindowText(_T("Status: All slots deleted.."));
	}else{
		m_StatusCtrl.SetWindowText(_T("Status: Failure, slots NOT deleted.. "));
	}
	EnableWindows(true);
	
}

//Upload a template to the PC from a FLASH slot, and save it on disk.
void CComDlg::OnUploadTemplateFLASH() 
{
	int		templSize = 0;
	uint8_t	*templBuffer = 0;

	CString currName="";
	m_TemplateNameCtrl.GetWindowText(currName);

	if (currName == ""){
		AfxMessageBox(_T("Please enter a template file name before uploading template."));
		return;
	}

	EnableWindows(false);

	//Get the FLASH slot information
	UpdateData(true);

	//Check if slot value is valid 
	if (m_SlotValue < 0 && m_SlotValue > 49){
		AfxMessageBox(_T("Error: Slot index out of range. Please enter a valid flash slot number."));
		EnableWindows(true);
		return;
	}
	bool ok;
	if (m_UseSPIFlag)
		ok = m_SPICommand->ApiUploadTemplateFLASH(&templBuffer, &templSize, m_SlotValue);
	else
		ok = m_Command->ApiUploadTemplateFLASH(&templBuffer, &templSize, m_SlotValue);
	//Save template to disk
	if (templBuffer && templSize) {
		currName = currName + ".tpl";
		
		FILE *fil;
		fopen_s(&fil, CT2A(currName),"wb");
		fwrite(templBuffer, sizeof(uint8_t), templSize, fil);
		fclose(fil);
		CString mess; mess.Format(_T("Status: Template in slot number %d uploaded and saved to disc.."), m_SlotValue);
		m_StatusCtrl.SetWindowText(mess);
		free(templBuffer);
	}

	FillTemplateDropBox();

	EnableWindows(true);
}


//*************** Additional commands *************************************************************//


//Verify image in RAM against template in RAM
void CComDlg::OnVerifyRam() 
{
	EnableWindows(false);
	UpdateData(true);
	m_StatusCtrl.SetWindowText(_T("Status: Verifying image.. Please wait.."));
	if (m_UseSPIFlag)
		m_SPICommand->ApiVerifyRAM();
	else
		m_Command->ApiVerifyRAM();
	EnableWindows(true);
}

//Enrol image in RAM and place template in RAM
void CComDlg::OnEnrolRam() 
{
	EnableWindows(false);
	//Get the FLASH slot information
	UpdateData(true);
	m_StatusCtrl.SetWindowText(_T("Status: Enrolling image.. Please wait.."));
	if (m_UseSPIFlag)
		m_SPICommand->ApiEnrolRAM();
	else
		m_Command->ApiEnrolRAM();

		
	EnableWindows(true);
}

//Wait for finger present, capture an image, identify it against all templates in FLASH
void CComDlg::OnCaptureIdentify() 
{
	int slotNumber;
	EnableWindows(false);
	m_CancelCtrl.EnableWindow(true);
	m_StatusCtrl.SetWindowText(_T("Status: Waiting for finger to identify.."));

	int IdentifyOK;
	if (m_UseSPIFlag)
		IdentifyOK = m_SPICommand->ApiCaptureIdentify( &slotNumber);
	else
		IdentifyOK = m_Command->ApiCaptureIdentify( &slotNumber);

	if (IdentifyOK){
		CString mess="";
		mess.Format(_T("Status: Identification successful for slot number %d"), slotNumber);
		MessageBeep(MB_ICONEXCLAMATION); //OK sound
		m_StatusCtrl.SetWindowText(mess);
	} else {
		CString mess="";
		mess.Format(_T("Status: Identification failed!"));
		MessageBeep(0xFFFFFFFF); //Error sound
	}
	EnableWindows(true);
}


//Wait for finger present, capture an image and verify it against a template in FLASH..
//Slot information is needed to know which template to verify against
void CComDlg::OnCaptureVerifyFLASH() 
{
	EnableWindows(false);
	m_CancelCtrl.EnableWindow(true);
	//Get the FLASH slot information
	UpdateData(true);
	m_StatusCtrl.SetWindowText(_T("Status: Waiting for finger to verify.."));

	//Check if slot value is valid (0-1 for swipe)
	bool inRange = m_SlotValue >= 0 && m_SlotValue <= 49;
	if (!inRange){
		AfxMessageBox(_T("Error: Slot index out of range. Please enter a valid flash slot number."));
		EnableWindows(true);
		return;
	}
	bool commandOk=false;
	if (m_UseSPIFlag)
		commandOk = m_SPICommand->ApiCaptureVerifyFLASH(m_SlotValue);
	else
		commandOk = m_Command->ApiCaptureVerifyFLASH(m_SlotValue);
	if (commandOk)
		MessageBeep(MB_ICONEXCLAMATION); //OK sound
	else
		MessageBeep(0xFFFFFFFF); //Error sound

	if (commandOk){
		CString mess=""; mess.Format(_T("Status: Verification successful for slot number %d"), m_SlotValue);
		m_StatusCtrl.SetWindowText(mess);
	}
	EnableWindows(true);
}


//Get application firmware version
void CComDlg::OnFirmwareVersion()  
{
	EnableWindows(false);
	if (m_UseSPIFlag)
		m_SPICommand->ApiFirmwareVersion(); 
	else
		m_Command->ApiFirmwareVersion(); 

	EnableWindows(true);
				
}

int baudrateActual(int index){
	switch (index){
	case 0x10:
		return 9600;
	case 0x20:
		return 14400;
	case 0x30:
		return 19200;
	case 0x40:
		return 28800;
	case 0x50:
		return 34800;
	case 0x60:
		return 57600;
	case 0x80:
		return 115200;
	}
	return 0; 
}


//Change the baud rate.. stored in RAM, will be reset on power-off
void CComDlg::OnSetBaudRate() 
{
	EnableWindows(false);
	int baudIndex; //Index corresponding to the chosen baud rate, 

	CBaudRateSelector dlg;	
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		baudIndex = dlg.m_pSpeedIndex;
	}
	else if (nResponse == IDCANCEL)
	{
		//No action is necessary here.
		m_StatusCtrl.SetWindowText(_T("Status: Command cancelled, baud rate not changed."));
		EnableWindows(true);
		return;
	}
	if (baudIndex == 0){
		m_StatusCtrl.SetWindowText(_T("Status: No speed selected, baud rate not changed."));
		EnableWindows(true);
		return;
	}

	//Set the modules baud rate
	bool ok;
	if (m_UseSPIFlag)
		ok = m_SPICommand->ApiSetBaudRateRAM(baudIndex);
	else
		ok = m_Command->ApiSetBaudRateRAM(baudIndex);

	//if ok, then change the pc baudrate
	if (ok){
		CString baudString; 
		baudString.Format(_T("%d"),baudrateActual(baudIndex));
		m_BaudCtrl.SelectString(0,baudString);

		//And reconnect the PC with new BaudRate
		OnConnect();
		m_StatusCtrl.SetWindowText(_T("Status: New baudrate is set.."));
	}else{
		m_StatusCtrl.SetWindowText(_T("Status: ApiSetBaudRate failed.."));
	}
	EnableWindows(true);
}

//Verify image in RAM against template in FLASH slot
void CComDlg::OnVerifyFLASH() 
{
	EnableWindows(false);
	//Get the FLASH slot information
	UpdateData(true);

	//Check if slot value is valid (0-1 for swipe)
	bool inRange = m_SlotValue >= 0 && m_SlotValue <=49;
	if (!inRange){
		AfxMessageBox(_T("Error: Slot index out of range. Please enter a valid flash slot number."));
		EnableWindows(true);
		return;
	}
	if (m_UseSPIFlag)
		m_SPICommand->ApiVerifyFLASH(m_SlotValue);
	else
		m_Command->ApiVerifyFLASH(m_SlotValue);

	EnableWindows(true);
}

//Download image from PC to module. 
void CComDlg::OnDownloadImage() 
{
	EnableWindows(false);

	//Select a file through a dialog box:
	CFileDialog file_open(true, _T(".bmp"), 0, OFN_FILEMUSTEXIST,_T("BMP Files (*.bmp)|*.bmp||"), NULL );

    // Show the dialog and wait for the result - verify that the OK button was pressed
    if (file_open.DoModal() == IDOK ) {
		// Get the entire path name to the file that was selected
		CString				path	= file_open.GetPathName();
		CFile				image;
		UINT				filesize;
		bool				ok;
		uint8_t				*fileBuffer;



		if (image.Open(path, CFile::modeRead) == FALSE) {
			return;
		}
		
		filesize = (UINT)image.GetLength();
		fileBuffer = (uint8_t *)malloc(filesize);
		
		if (image.Read(fileBuffer, filesize)) {
			BITMAPFILEHEADER	*BFH		= (BITMAPFILEHEADER *)fileBuffer;
			BITMAPINFO			*bDIB		= (BITMAPINFO *)(fileBuffer + sizeof(BITMAPFILEHEADER));
			uint8_t				*imagedata	= fileBuffer + BFH->bfOffBits;
			uint16_t			imagesize	= (uint16_t)(bDIB->bmiHeader.biHeight * bDIB->bmiHeader.biWidth);

			m_StatusCtrl.SetWindowText(_T("Status: Downloading image, please wait.."));

			if (m_UseSPIFlag)
				ok = m_SPICommand->ApiDownloadImage(imagedata, imagesize, 1); //Index 1 to skip the CRC check during download
			else
				ok = m_Command->ApiDownloadImage(imagedata, imagesize, 0, 0); //Index one for a non CRC-check!

			if (ok) {
				m_StatusCtrl.SetWindowText(_T("Status: Image downloaded.."));
			}
			else {
				m_StatusCtrl.SetWindowText(_T("Status: Download Error!"));
			}	
		}
		free(fileBuffer);
		image.Close();
	}
	EnableWindows(true);
}
	
//Identify image in RAM against all templates in FLASH
void CComDlg::OnIdentify() 
{

	int slotNumber;
	EnableWindows(false);
	m_StatusCtrl.SetWindowText(_T("Status: Identifying RAM image.."));

	int IdentifyOK;
	if (m_UseSPIFlag)
		IdentifyOK = m_SPICommand->ApiIdentify(&slotNumber);
	else
		IdentifyOK = m_Command->ApiIdentify(&slotNumber);

	CString mess="";
	if (IdentifyOK){
		mess.Format(_T("Status: Identification successful for slot number %d"), slotNumber);

		m_StatusCtrl.SetWindowText(mess);
	}
	EnableWindows(true);
}

void CComDlg::OnInitiateUpgrade()
{
	bool InitiateOk = false;

	m_StatusCtrl.SetWindowText(_T("Status: Initiating Firmware Upgrade."));

	if (m_UseSPIFlag)
	{
		InitiateOk = m_SPICommand->ApiInitiateUpgrade();
	}
	else
	{
		InitiateOk = m_Command->ApiInitiateUpgrade();
	}

	if (InitiateOk)
	{
		m_StatusCtrl.SetWindowText(_T("Status: Initiating firmware upgrade was successful."));
	}
	else
	{
		m_StatusCtrl.SetWindowText(_T("Status: Initiating firmware upgrade failed."));
	}
}

void CComDlg::OnUpgrade() 
{
	//Let the user select a file containing the new Program
	//to be copied to the Flash memory in chunks of 256 bytes.
	
	m_StatusCtrl.SetWindowText(_T("Status: Preparing to download new Firmware."));

	//Select a file through a dialog box:
	CFileDialog file_open(true, _T(".bin"), 0, OFN_FILEMUSTEXIST,_T("Bin Files (*.bin)|*.bin||"), NULL );

    // Show the dialog and wait for the result - verify that the OK button was pressed
    if(file_open.DoModal() == IDOK )
    {
		// Get the entire path name to the file that was selected
		CString ext		= file_open.GetFileExt();
		CString name	= file_open.GetFileName();
		CString path	= file_open.GetPathName();

		CFile			flFirmware;
		int				filesize;
		int				i;
		bool			ok;
		unsigned char*	fileBuffer;
		TCHAR			txt[128]	= { 0 };
		int				size		= sizeof(txt);

		if (flFirmware.Open(path, CFile::modeRead) == TRUE) {
			CArchive ar(&flFirmware, CArchive::load);
			filesize = (int)flFirmware.GetLength();
			if (filesize % 256) {
				m_StatusCtrl.SetWindowText(_T("Error: Firmware file must be a multiple of 256 bytes long!"));
				return;
			}
			fileBuffer = (unsigned char*)new unsigned int[filesize];
			ar.Read(fileBuffer, filesize);
			//Now, the file is in memory. Now, we can send it through the UART, in chunks of 256 bytes!
			//Start downloading:
			for (i = 0; i < filesize / 256; i++) {
				_stprintf_s(txt, 128, _T("Status: Firmware being downloaded. (Block %d of %d)."), i, (filesize / 256) - 1);
				m_StatusCtrl.SetWindowText(txt); // Convert char array to wchar_t  array 
				Sleep(5);
				if (m_UseSPIFlag)
					ok = m_SPICommand->ApiUpgradeFirmwareBlock(fileBuffer + 256 * i, 256, i);
				else
					ok = m_Command->ApiUpgradeFirmwareBlock(fileBuffer + 256 * i, 256, i);
				if (!ok) {
					m_StatusCtrl.SetWindowText(_T("Error: Firmware Download Failed! Try again!"));
					return;
				}
			}
			m_StatusCtrl.SetWindowText(_T("Status: Firmware Upgraded. Reset FPC2020!"));
		}
	}
}


void CComDlg::OnRunInstaller() 
{
	// Sends a special command to the FPC2020 BootCode (which defaults at 9600 baud),
	// which contains a complete program in its payload.
	// This program is dumped into the code memory, and when complete, the application is started automatically!
	//
	// Note that originally, it was the installer program that was started this way - that explains the function name!
	int size=0;
	FILE* fil;
	unsigned char prgBuffer[100000];

	EnableWindows(true);

	//Select a file through a dialog box:
	CFileDialog file_open(true, _T(".bin"), 0, OFN_FILEMUSTEXIST,_T("Bin Files (*.bin)|*.bin||"), NULL );

    // Show the dialog and wait for the result - verify that the OK button was pressed
    if(file_open.DoModal() == IDOK )
    {
		// Get the entire path name to the file that was selected
		CString ext = file_open.GetFileExt();
		CString name = file_open.GetFileName();
		CString path = file_open.GetPathName();

		{
			fopen_s(&fil, CT2A(name),"rb");
			bool fileExist = (fil != NULL);

			if (fileExist){

				while (!feof(fil)){
					fread(prgBuffer+size, 1, 1, fil);
					size++;
				}

				size--; //We went one step too far!

				fclose(fil);

				m_StatusCtrl.SetWindowText(_T("Status: Program download in progress.."));
			
				//Send template
				bool ok;
				if (m_UseSPIFlag)
					ok = m_SPICommand->ApiDownloadProgram(prgBuffer, size);
				else
					ok = m_Command->ApiDownloadProgram(prgBuffer, size);

				if (ok){
					m_StatusCtrl.SetWindowText(_T("Status: Program downloaded and is now Running.."));
				}else{
					m_StatusCtrl.SetWindowText(_T("Status: Program download failed.."));
				}
			}else{
				AfxMessageBox(_T("Error: Installer Code file could not be found!"));
				return;
			}
		}
	}
	EnableWindows(true);
}

// This example shows how we can combine two existing commands to get a desired behaviour!
// We first issue a capture image command, and when it returns, we immediately make a call to 
// the enrol function. It is in principle the same functionality as the 'Capture Enrol' command,
// but in the following case, the host gets notified when the actual enrol processing begins,
// i.e. when the user of the system can remove the finger from the sensor surface.
// Note: This function is only valid on a Area Sensor System
void CComDlg::OnCaptureEnrolRamDual() 
{
	EnableWindows(false);
	m_CancelCtrl.EnableWindow(true);
	//Get the FLASH slot information:
	UpdateData(true);
	m_StatusCtrl.SetWindowText(_T("Please capture finger image..."));

	bool commandOk=false;

	//Start by executing a Capture Image (with fingerpresent check)!
	if (m_UseSPIFlag)
		commandOk = m_SPICommand->ApiCaptureImage(true);
	else
		commandOk = m_Command->ApiCaptureImage(true);
	MessageBeep(MB_ICONEXCLAMATION); //OK sound
	m_StatusCtrl.SetWindowText(_T("Finger can be removed. Enrol in progress..."));
	//Next, go ahead and enrol the captured fingerprint image!
	if (m_UseSPIFlag)
		commandOk = m_SPICommand->ApiEnrolRAM();
	else
		commandOk = m_Command->ApiEnrolRAM();
	if (commandOk)
		MessageBeep(MB_ICONEXCLAMATION); //OK sound
	else
		MessageBeep(0xFFFFFFFF); //Error sound

	EnableWindows(true);
}

//*************** Non-api functions and buttons *************************************************************//

		
		
//This is used to send custom bytes in HEX format..
//It always tries to read 4 bytes as response..
void CComDlg::OnSend() 
{
	
	//Send the byte string corresponding to the hex code 
	//Get the custom string to be sent..
	UpdateData();
	//Check the format and translate string to int values
	int byteNumber=0;
	int index=-1;
	unsigned char ByteList [5000];
	index=m_CustomCommand.Find(_T("0x"),0);
	if (index < 0){ //The string was not found
		AfxMessageBox(_T("No valid HEX bytes to send.. HEX values must use format '0x02 0xFE' "));
		return;
	}
	CString tempStr;
	while (index>=0){
		
		ByteList[byteNumber] = (unsigned char)strtol(CT2A(m_CustomCommand.Mid(index,4)),NULL,16);//Convert Hex to int
		index++;
		byteNumber++;
		index=m_CustomCommand.Find(_T("0x"),index);
		
	}
	
	//Send the custom command, and check for an immediate response!
	unsigned char Response [4];
	unsigned char CRC [4];
	unsigned char *dataBuffer = 0;
	DWORD nWritten;
	DWORD nRead=0;
	int payload=0;
	m_StatusCtrl.SetWindowText(_T("Custom command is sent.. Waiting for response.."));

	PrintData(ByteList, byteNumber, true);
	if (!m_UseSPIFlag){
		if (!WriteFile(m_Command->m_hComm, ByteList, byteNumber, &nWritten, NULL))
		{
			return;
		}
	} else {
		m_SPICommand->m_spi->SendData((char*)ByteList,byteNumber,0,0);
		nWritten = byteNumber;
	}

	//Read the response bytes
	if (!m_UseSPIFlag){
		while (!nRead){
			if (ReadFile(m_Command->m_hComm, &Response, 4, &nRead, NULL) == 0){
				return;
			}
		}
	} else {
		m_SPICommand->m_spi->ReadDataWhenNotBusy(0,0,(char*)Response,4);
		nRead = 4;
	}
	//For now, let this application act just as it does for FPC-AM.
	//Add a 5 second timeout to wait for fingerpresent and other stuff that might be happening, if the sent bytes looks like a command
	if (nRead == 0 ){ //This can only happen for the UART implementation. 
		m_StatusCtrl.SetWindowText(_T("No response received.."));
		if (nWritten >= 6 && ByteList[0] == 0x02){
			Sleep(5000);
			if (ReadFile(m_Command->m_hComm, &Response, 4, &nRead, NULL) == 0)
			{
				return;
			}
			if (nRead == 0){
				m_StatusCtrl.SetWindowText(_T("Status: No response bytes found. Read operation timed out.."));
			}
		}
	}
	
	if (Response[0] == 0x02){  //If this is a correct response:
		//Read the payload data, if any..
		payload = Response[2] + Response[3] * 256;
		if ((payload > 0) && (nRead == 4)){
			dataBuffer	= (unsigned char *)malloc(payload * sizeof(char));
			if (!m_UseSPIFlag){
				if (ReadFile(m_Command->m_hComm, dataBuffer, payload, &nRead, NULL) == 0)
				{
					return;
				}
			} else {
				m_SPICommand->m_spi->ReadData(0,0,(char*)dataBuffer,payload);
				nRead = payload;
			}
			
			//Read CRC
			if (!m_UseSPIFlag)
				ReadFile(m_Command->m_hComm, &CRC, 4, &nRead, NULL);
			else {
				m_SPICommand->m_spi->ReadData(0,0,(char*)&CRC,4);
				nRead = 4;
			}
		}
	}
	PrintData(Response, 4, false);
	if (payload){
		PrintData(dataBuffer, payload, false);
		PrintData(CRC, 4, false);
		free(dataBuffer);
	}
}


//Print data in the listbox, different colors for transmit (TX) och receive (RX)
void CComDlg::PrintData(unsigned char * byteStream, int NrOfBytes, bool TX){

	const int BytesPerRow = 37;
	int NoOfNewRows = NrOfBytes / BytesPerRow +1;
	int RemBytes = NrOfBytes;
	CString byteString;
	CString temp;
	m_crColor=RGB(0,0,255);

	//Check that we infact have something to transmit!
	//If a fingerpresent capture has been cancelled, this will happen.
	if (!NrOfBytes) return;

	//Check that there isn't more than 2500 rows..
	int rowsTooMany = m_ListBoxCtrl.GetCount() + NoOfNewRows - 2500;
	for (int i = 0; i < rowsTooMany; i++){
		m_ListBoxCtrl.DeleteString(i);
	}

	//If the data to be displayed covers more than one row
	//compactify it, for better readability.
	if (m_CompactButton.GetCheck() && NoOfNewRows>1){

		if (TX){
			byteString="TX: ";
		}else{
			byteString="RX: ";
		}

		for (int i=0;i<15 ;i++){ //The first 15 bytes!
			if (m_HexCtrl.IsWindowEnabled()){//Want to print characters
				if (*(byteStream+i) == 0) {
					temp.Format(_T("%c"),248); //To get a sign instead of NULL, which cannot be printed
				}else{
					temp.Format(_T("%c"),*(byteStream+i));
				}
			}else{ //Print HEX values
				temp.Format(_T(" %.2X"),*(byteStream+i));
			}
			byteString += temp;
		}

		temp.Format(_T("... [%d bytes] ..."),NrOfBytes);
		byteString += temp;

		for (int i=15;i>=1;i--){ //The last 15 bytes!
			if (m_HexCtrl.IsWindowEnabled()){//Want to print characters
				if (*(byteStream+NrOfBytes-i) == 0) {
					temp.Format(_T("%c"),248); //To get a sign instead of NULL, which cannot be printed
				}else{
					temp.Format(_T("%c"),*(byteStream+NrOfBytes-i));
				}
			}else{ //Print HEX values
				temp.Format(_T(" %.2X"),*(byteStream+NrOfBytes-i));
			}
			byteString += temp;
		}
		int row = m_ListBoxCtrl.AddString(byteString);
		if (!TX) m_ListBoxCtrl.SetSel(row);

		NoOfNewRows = 0; //So that we skip the remaining part.
	}

	for (int j=0;j<NoOfNewRows;j++){
		if (TX){
			byteString="TX: ";
		}else{
			byteString="RX: ";
		}

		for (int i=0;i<RemBytes && i < BytesPerRow ;i++){
			if (m_HexCtrl.IsWindowEnabled()){//Want to print characters
				if (*(byteStream+j*BytesPerRow+i) == 0) {
					temp.Format(_T("%c"),248); //To get a sign instead of NULL, which cannot be printed
				}else{
					temp.Format(_T("%c"),*(byteStream+j*BytesPerRow+i));
				}
			}else{ //Print HEX values
				temp.Format(_T(" %.2X"),*(byteStream+j*BytesPerRow+i));
			}
			byteString += temp;
		}
		RemBytes -= BytesPerRow;
		int row = m_ListBoxCtrl.AddString(byteString);
		if (!TX) m_ListBoxCtrl.SetSel(row);
	}

	CString status = "";
	CString message[24] = {	"Operation failed" ,
							"Operation was successful" ,
							"Image captured, but no finger present",
							"Image captured, with finger present",
							"Verification successful",
							"Verification failed",
							"Enrol successful",
							"Enrol failed",
							"Hardware test successful",
							"Hardware test failed",
							"CRC checksum failed",
							"Payload was longer than expected for this command",
							"Payload was shorter than expected for this command",
							"Command not recognised",
							"No template is present in memory",
							"Identification successful",
							"Identification failed",
							"Slot number out of range",
							"Operation canceled successfully",
							"Application has already been started",
							"Firmware CRC checksum test failed",
							"Input Buffer Overflow",
							"System CRC checksum test failed",
							"Failure - Low Voltage Warning. Replace battery and Reset."
							};

	if (!TX && (*byteStream  == 0x02) && (*(byteStream + 1) < 24) && NrOfBytes==4){
		
		status.Format(_T("Status: %s. "),message[*(byteStream + 1)]);
		m_StatusCtrl.SetWindowText(status);
	}

	PROCESSMESSAGES_AFX;
}

//Display bytes as characters in the listbox..
void CComDlg::OnChar() 
{
	m_HexCtrl.EnableWindow(TRUE);
	m_CharCtrl.EnableWindow(FALSE);
	int NrOfStrings = m_ListBoxCtrl.GetCount();
	CString HexString;
	CString CharString;
	for (int j=0;j<NrOfStrings;j++){
		
		m_ListBoxCtrl.GetText(j,HexString);
		//Check the format and translate string to int values
		int byteNumber=0;
		int index=0;
		bool TX = (HexString.Left(2)=="TX");

		unsigned char ByteList [200];
		
		CString tempStr;
		for (int k = 5; k < HexString.GetLength() ;){
			ByteList[byteNumber] = (unsigned char)strtol(CT2A(HexString.Mid(k,2)),NULL,16);//Convert Hex to int		
			byteNumber++;
			k+=3;			
		}

		//Translate values to character string..
		CString temp;
		if (TX){
			CharString="TX: ";
		}else{
			CharString="RX: ";
		}

		for (int i=0;i< byteNumber ;i++){
			
			if (*(ByteList+i) == 0) {
				temp.Format(_T("%c"),248); //To get a sign instead of NULL, which cannot be printed
			}else{
				temp.Format(_T("%c"),*(ByteList+i));
			}
		
			CharString += temp;		
		}

		m_ListBoxCtrl.DeleteString(j);
		int row = m_ListBoxCtrl.InsertString(j,CharString);
		if (!TX) m_ListBoxCtrl.SetSel(row);
	}
}

//Transform character values to HEX values in Listbox
void CComDlg::OnHex() 
{
	m_HexCtrl.EnableWindow(FALSE);
	m_CharCtrl.EnableWindow(TRUE);

	int NrOfStrings = m_ListBoxCtrl.GetCount();
	CString HexString;
	CString CharString;
	for (int j=0;j<NrOfStrings;j++){
		
		m_ListBoxCtrl.GetText(j,CharString);
		//Check the format and translate string to int values
		int byteNumber=CharString.GetLength()-4;
		int index=-1;
		bool TX = (CharString.Left(2)=="TX");

		unsigned char ByteList [200];
		
		
		CString tempStr;
		for (int i=4; i<CharString.GetLength();i++){		
			ByteList[i-4] = (unsigned char)(CharString.GetAt(i));
		}

		//Translate values to character string..
		CString temp;
		if (TX){
			HexString="TX: ";
		}else{
			HexString="RX: ";
		}

		for (int i=0;i< byteNumber ;i++){
			if (*(ByteList+i) == 248) {
				temp.Format(_T(" %.2X"),0); //To get a sign instead of NULL, which cannot be printed.. This is a bit unorthodox of course.
			}else{
				temp.Format(_T(" %.2X"),*(ByteList+i));
			}
			HexString += temp;
		}

		m_ListBoxCtrl.DeleteString(j);
		int row = m_ListBoxCtrl.InsertString(j,HexString);
		if (!TX) m_ListBoxCtrl.SetSel(row);
	}
}

//Delete all characters in listbox
void CComDlg::OnClear() 
{
	m_ListBoxCtrl.ResetContent();
	
}

//Close program
void CComDlg::OnOK() 
{
	GetSettings();
	CDialog::OnOK();
}

void CComDlg::FillTemplateDropBox(){

	WIN32_FIND_DATA fd;
	CString wildOrig;
	wildOrig = "*.tpl";
	HANDLE h = FindFirstFile(wildOrig, &fd);
	CString fileName;
	int tplSize;
	tplSize = 1070;

	BOOL ok=h != INVALID_HANDLE_VALUE;

	CString currName="";
	m_TemplateNameCtrl.GetWindowText(currName);

	m_TemplateNameCtrl.ResetContent();


	while (ok){
		
		if ( fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) { //No directory
			fileName = fd.cFileName;
			if ((fileName != ".") && (fileName != "..") ){
				if (fd.nFileSizeLow ==  tplSize){
					m_TemplateNameCtrl.AddString(fileName.Left(fileName.GetLength()-4));//Just to remove the extension ".tpl"
				}
			}
		}
		ok = FindNextFile(h, &fd);
	}
	//CString currName="";
	m_TemplateNameCtrl.SetWindowText(currName);
}

//A helper-function
void CComDlg::Scramble(unsigned char *p, int lines, int points) {
	int xxx =  0x359A6C5A;  // 2 bits set per nibble
	int i, nWords;
	unsigned int *wp = (unsigned int *)p;

	nWords = lines*points / 4;

	for (i = 0; i < nWords; i++) {
		*wp ^= xxx;
		wp++;
		if (xxx < 0 ) {
			xxx = ((xxx << 1) | 1);
		} else {
			xxx = (xxx << 1);
		}
	}
}

void CComDlg::KillCurrentCommunication(){
	m_Command->KillCurrentCommunication();
}


void CComDlg::OnButton1() 
{
//Test function, not used.

}

void CComDlg::OnUploadall() 
{
		// Full database upload, up to the Flash slot number given in the edit box!
	unsigned char *templ = 0;
	int loopCounter;
	int templSize;
	CString fileName;
	bool commandOk;
	CString currName="";
	char number[5];
	FILE *fil;

	//Get the FLASH slot information
	UpdateData(true);
	m_TemplateNameCtrl.GetWindowText(currName);

	if (currName == "") {
		AfxMessageBox(_T("Please enter a template file name from the list before uploading a template.."));
		return;
	}
	
	m_StatusCtrl.SetWindowText(_T("Command is sent.. Waiting for response.."));
	EnableWindows(false);


	//Now, for each time through the loop, we must update
	//filename and slot number!
	for (loopCounter = 0; loopCounter <= 49; loopCounter++) {

		if (m_UseSPIFlag)
			commandOk = m_SPICommand->ApiUploadTemplateFLASH(&templ, &templSize, loopCounter);
		else
			commandOk = m_Command->ApiUploadTemplateFLASH(&templ, &templSize, loopCounter);

		if (commandOk && templ) {
			sprintf_s(number,"_%d",loopCounter); 
			fileName = currName + number + ".tpl";
			
			fopen_s(&fil, CT2A(fileName),"wb");
			fwrite(templ, sizeof(char), templSize, fil);
			fclose(fil);
			CString mess; mess.Format(_T("Status: Template in slot number %d uploaded and saved to disc."), loopCounter);
			m_StatusCtrl.SetWindowText(mess);
		}

		if (templ) {
			free(templ);
			templ = 0;
		}
	}

	//Now, we are done
	EnableWindows(true);
	
}

void CComDlg::OnDownloadall() 
{
	// Full database download, up to the Flash slot number given in the edit box!
	// This function is currently only implemented for FPC2020

	unsigned char *tplBuffer; //Large enough to handle either area or swipe 2020-templates.
	int loopCounter;
	int templSize;
	CString fileName;
	bool commandOk;
	CString currName="";
	char number[5];
	FILE *fil;
	struct stat st;

	//Get the FLASH slot information
	UpdateData(true);
	m_TemplateNameCtrl.GetWindowText(currName);

	if (currName == ""){
		AfxMessageBox(_T("Please enter a template file name from the list before downloading a template.."));
		return;
	}

	m_StatusCtrl.SetWindowText(_T("Command is sent.. Waiting for response.."));
	EnableWindows(false);

	for (loopCounter = 0; loopCounter <= 49; loopCounter++) {
		//Create correct filename:
		sprintf_s(number,"_%d",loopCounter); 
		fileName = currName + number + ".tpl";

		//Read template from file
		stat(CT2A(fileName), &st);
		templSize = st.st_size;
		fopen_s(&fil, CT2A(fileName),"rb");
		
		if (fil) {
			tplBuffer = (unsigned char *)malloc(templSize);
			fread(tplBuffer, sizeof(char), templSize, fil);
			fclose(fil);

			if (m_UseSPIFlag)
				commandOk = m_SPICommand->ApiDownloadTemplate(tplBuffer, templSize);
			else
				commandOk = m_Command->ApiDownloadTemplate(tplBuffer, templSize);
			
			free(tplBuffer);

			//Next - move the template from RAM to Flash!
			if (commandOk){
				if (m_UseSPIFlag)
					commandOk = m_SPICommand->ApiCopyTemplateRamToFLASH(loopCounter);
				else
					commandOk = m_Command->ApiCopyTemplateRamToFLASH(loopCounter);
			}
			if (commandOk){
				CString mess; mess.Format(_T("Status: Template in slot number %d downloaded and saved to Flash."), loopCounter);
				m_StatusCtrl.SetWindowText(mess);
			} else {
				CString mess; mess.Format(_T("Status: Process Failed."));
				m_StatusCtrl.SetWindowText(mess);
			}
		}
	}

	//Now, we are done
	EnableWindows(true);
	
	
}

BOOL CComDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	switch (nEventType)
	{
	case DBT_DEVICEARRIVAL:
	case DBT_DEVICEREMOVECOMPLETE: {
		CString c_port = m_PortValue;

		//First dropbox is the ComPort box
		SerialPortList pPorts = m_Ports.Enumerate(); //Find available ports

		m_PortCtrl.ResetContent();
		for (SerialPortListIterator it = pPorts.begin(); it != pPorts.end(); it++) {
			CString SerialPort = (*it).c_str();
			m_PortCtrl.AddString(SerialPort);//Add found ports to dropbox
		}
		if (c_port.GetLength() > 0) {
			int index = m_PortCtrl.FindString(-1, c_port);
			if (index != -1) {
				m_PortCtrl.SetCurSel(index);
			}
			else {
				if (m_Command->IsOpen()) {
					m_Command->Close();
					m_StatusCtrl.SetWindowText(_T("Status: Connection terminated, device on " + c_port + " removed."));
				}
			}
		}
		break;
	}
	default:
		break;
	}

	return TRUE;
}


void CComDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
}
