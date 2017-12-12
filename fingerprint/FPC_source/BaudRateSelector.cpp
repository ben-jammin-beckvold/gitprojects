// BaudRateSelector.cpp : implementation file
//

#include "StdAfx.h"
#include "FPC serialCom.h"
#include "BaudRateSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaudRateSelector dialog


CBaudRateSelector::CBaudRateSelector(CWnd* pParent /*=NULL*/)
	: CDialog(CBaudRateSelector::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBaudRateSelector)
	m_ConnectionSpeed = -1;
	//}}AFX_DATA_INIT
	m_pSpeedIndex = 0x00;
}


void CBaudRateSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBaudRateSelector)
	DDX_Radio(pDX, IDC_BAUD_9600, m_ConnectionSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBaudRateSelector, CDialog)
	//{{AFX_MSG_MAP(CBaudRateSelector)
	ON_BN_CLICKED(IDC_BAUD_115200, OnBaud115200)
	ON_BN_CLICKED(IDC_BAUD_14400, OnBaud14400)
	ON_BN_CLICKED(IDC_BAUD_19200, OnBaud19200)
	ON_BN_CLICKED(IDC_BAUD_28800, OnBaud28800)
	ON_BN_CLICKED(IDC_BAUD_38400, OnBaud38400)
	ON_BN_CLICKED(IDC_BAUD_57600, OnBaud57600)
	ON_BN_CLICKED(IDC_BAUD_9600, OnBaud9600)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaudRateSelector message handlers

/*void CBaudRateSelector::InitPointer(int* ptr){
	m_pSpeedIndex = ptr;
}*/

void CBaudRateSelector::OnOK() 
{
	CDialog::OnOK();
}

void CBaudRateSelector::OnBaud115200() 
{
	m_pSpeedIndex = 0x80;
}

void CBaudRateSelector::OnBaud14400() 
{
	m_pSpeedIndex = 0x20;
}

void CBaudRateSelector::OnBaud19200() 
{
	m_pSpeedIndex = 0x30;
}

void CBaudRateSelector::OnBaud28800() 
{
	m_pSpeedIndex = 0x40;
}

void CBaudRateSelector::OnBaud38400() 
{
	m_pSpeedIndex = 0x50;
}

void CBaudRateSelector::OnBaud57600() 
{
	m_pSpeedIndex = 0x60;
}

void CBaudRateSelector::OnBaud9600() 
{
	m_pSpeedIndex = 0x10;
}
