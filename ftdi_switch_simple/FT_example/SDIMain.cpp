//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SDIMain.h"
#include "test_bm.h"
#include "ftd2xx.h"
#include "copt.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TSDIAppForm *SDIAppForm;
//---------------------------------------------------------------------
__fastcall TSDIAppForm::TSDIAppForm(TComponent *AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TSDIAppForm::FormCreate(TObject *Sender)
{
	SDIAppForm->Caption = FT_STR_DEF_TITLE;
    Application->OnHint = ShowHint;
}
//---------------------------------------------------------------------------
void __fastcall TSDIAppForm::FormActivate(TObject *Sender)
{
    char Buf[64];

    ManuLabel->Caption = FT_STR_DEF_MANUFACTURER;
    DescLabel->Caption = FT_STR_DEF_PRODUCT_DESC;

    wsprintf(Buf,"VID_%04.04x&&PID_%04.04x",FT_INT_DEF_VENDOR_ID,FT_INT_DEF_PRODUCT_ID);
    DevIdLabel->Caption = Buf;
    ProgramBitBtn->SetFocus();
}
//---------------------------------------------------------------------
void __fastcall TSDIAppForm::ShowHint(TObject *Sender)
{
	StatusBar->Panels->Items[0]->Text = Application->Hint;
}
//---------------------------------------------------------------------------
BOOLEAN __fastcall TSDIAppForm::Program(TObject *Sender)
{
	FT_PROGRAM_DATA ftData = {
		0x0,			// Header - must be 0x00000000
		0xffffffff,		// Header - must be 0xffffffff
		2,				// Header - FT_PROGRAM_DATA version
						//          0 = original
                        //          1 = FT2232C extensions
                        //          2 = FT232R extensions

		FT_INT_DEF_VENDOR_ID,               //VID
		FT_INT_DEF_PRODUCT_ID,			    //PID
		FT_STR_DEF_MANUFACTURER,            //Manufacturer string
		FT_STR_DEF_MANUFACTURER_ID,		    //Manufacturer ID string
		FT_STR_DEF_PRODUCT_DESC,    	    //description string
		FT_STR_DEF_FIXED_SERNO,			    //serial number
		FT_INT_DEF_MAX_POWER, 			    //max current
		FT_INT_DEF_PNP, 				    //PNP
		FT_INT_DEF_SELF_POWERED,		    //Self Powered
		FT_INT_DEF_REMOTE_WAKEUP,		    //Remote wakeup
		//
		// Rev4 extensions
		//
		FT_BOOL_DEF_BM,                     //true if FT232BM
		FT_BOOL_DEF_ISO_IN, 			    //true if in endpoint is isochronous
		FT_BOOL_DEF_ISO_OUT,			    //true if out endpoint is isochronous
		FT_BOOL_DEF_PULL_DOWN_ENABLE,	    //true if pulldown enabled
		FT_BOOL_DEF_SERNO_ENABLE,  		    //true serial number to be used
		FT_BOOL_DEF_USB_VERSION_ENABLE,	    //USB version select
		0,               				    //USB version (BCD) (0x0200 => USB2)
        //
        // FT2232C extensions
        //
        FT_BOOL_DEF_C,				        // non-zero if Rev5 chip, zero otherwise
	    FT_BOOL_DEF_A_ISO_IN,			    // non-zero if in endpoint is isochronous
	    FT_BOOL_DEF_B_ISO_IN,			    // non-zero if in endpoint is isochronous
	    FT_BOOL_DEF_A_ISO_OUT,			    // non-zero if out endpoint is isochronous
	    FT_BOOL_DEF_B_ISO_OUT,			    // non-zero if out endpoint is isochronous
	    FT_BOOL_DEF_PULL_DOWN_ENABLE_C,	    // non-zero if pull down enabled
	    FT_BOOL_DEF_SERNO_ENABLE_C,		    // non-zero if serial number to be used
	    FT_BOOL_DEF_USB_VERSION_ENABLE_C,	// non-zero if chip uses USBVersion
	    0,                  			    // BCD (0x0200 => USB2)
	    FT_BOOL_DEF_A_IS_HIGH_CURRENT,		// non-zero if interface is high current
	    FT_BOOL_DEF_B_IS_HIGH_CURRENT,		// non-zero if interface is high current
	    FT_BOOL_DEF_A_IS_FIFO,			    // non-zero if interface is 245 FIFO
	    FT_BOOL_DEF_A_IS_FIFO_TARGET,		// non-zero if interface is 245 FIFO CPU target
	    FT_BOOL_DEF_A_IS_FAST_SERIAL,	   	// non-zero if interface is Fast serial
	    FT_BOOL_DEF_A_IS_VCP,				// non-zero if interface is to use VCP drivers
	    FT_BOOL_DEF_B_IS_FIFO,			    // non-zero if interface is 245 FIFO
	    FT_BOOL_DEF_B_IS_FIFO_TARGET,		// non-zero if interface is 245 FIFO CPU target
	    FT_BOOL_DEF_B_IS_FAST_SERIAL,	   	// non-zero if interface is Fast serial
	    FT_BOOL_DEF_B_IS_VCP,				// non-zero if interface is to use VCP drivers
	    //
	    // FT232R extensions
	    //
	    FT_BOOL_DEF_USE_EXT_OSC,			// Use External Oscillator
	    FT_BOOL_DEF_HIGH_DRIVE_IOS_R,		// High Drive I/Os
	    64,                      			// Endpoint size
	    FT_BOOL_DEF_PULL_DOWN_ENABLE_R,		// non-zero if pull down enabled
	    FT_BOOL_DEF_SERNO_ENABLE_R,		    // non-zero if serial number to be used
	    FT_BOOL_DEF_INVERT_TXD,			    // non-zero if invert TXD
	    FT_BOOL_DEF_INVERT_RXD,			    // non-zero if invert RXD
	    FT_BOOL_DEF_INVERT_RTS,		    	// non-zero if invert RTS
	    FT_BOOL_DEF_INVERT_CTS,	    		// non-zero if invert CTS
	    FT_BOOL_DEF_INVERT_DTR, 			// non-zero if invert DTR
	    FT_BOOL_DEF_INVERT_DSR, 			// non-zero if invert DSR
	    FT_BOOL_DEF_INVERT_DCD,		    	// non-zero if invert DCD
	    FT_BOOL_DEF_INVERT_RI,				// non-zero if invert RI
	    FT_INT_DEF_CBUS_0,		    		// Cbus Mux control
	    FT_INT_DEF_CBUS_1,  				// Cbus Mux control
	    FT_INT_DEF_CBUS_2,  				// Cbus Mux control
	    FT_INT_DEF_CBUS_3,  				// Cbus Mux control
	    FT_INT_DEF_CBUS_4,  				// Cbus Mux control
	    FT_BOOL_DEF_R_IS_NOT_VCP			// non-zero if using D2XX drivers
	};

    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;

    wsprintf(InfoText,"Program EEPROM");

    if (FT_Open(0,&ftHandle) != FT_OK)
        return false;

    ftStatus = FT_EE_Program(ftHandle,&ftData);

    FT_Close(ftHandle);

    return ftStatus == FT_OK;
}
//---------------------------------------------------------------------------
BOOLEAN __fastcall TSDIAppForm::Test(TObject *Sender)
{
    const int MaxBufferSize = 256;
    int CurrentBufferSize;
    unsigned char WriteBuffer[MaxBufferSize];
    unsigned char ReadBuffer[MaxBufferSize];
	TTestBM t;
    BOOL fStatus;

    SDIAppForm->Refresh();

	if (!t.IsRunning()) {
	    strcpy(InfoText,"Failed to initialize tester");
        return false;
    }

        t.Reset();

 	//
    // Test0.1 : Activate DTR
    //

    wsprintf(InfoText,"Activate DTR");
    t.ActDTR();
    if (!(MS_DSR_ON & t.WaitLegacyStatus()))
        return false;

 	//
    // Test0.2 : Deactivate DTR
    //

    wsprintf(InfoText,"Deactivate DTR");
    t.DeactDTR();
    if (MS_DSR_ON & t.WaitLegacyStatus())
        return false;

 	//
    // Test0.3 : Activate RTS
    //

    wsprintf(InfoText,"Activate RTS");
    t.ActRTS();
    if (!(MS_CTS_ON & t.WaitLegacyStatus()))
        return false;

 	//
    // Test0.4 : Deactivate RTS
    //

    wsprintf(InfoText,"Deactivate RTS");
    t.DeactRTS();
    if (MS_CTS_ON & t.WaitLegacyStatus())
        return false;

	//
    // Test1 : Activate CTS
    //

    wsprintf(InfoText,"Activate CTS");
    t.ActCTS();
    if (!(MS_CTS_ON & t.WaitStatus()))
        return false;

	//
    // Test2 : Deactivate CTS
    //

    wsprintf(InfoText,"Deactivate CTS");
   	t.DeactCTS();
    if (MS_CTS_ON & t.WaitStatus())
        return false;

	//
    // Test3 : Activate DSR
    //

    wsprintf(InfoText,"Activate DSR");
    t.ActDSR();
    if (!(MS_DSR_ON & t.WaitStatus()))
        return false;

	//
    // Test4 : Deactivate DSR
    //

    wsprintf(InfoText,"Deactivate DSR");
   	t.DeactDSR();
    if (MS_DSR_ON & t.WaitStatus())
        return false;

	//
    // Test5 : Activate CD
    //

    wsprintf(InfoText,"Activate CD");
    t.ActCD();
    if (!(MS_RLSD_ON & t.WaitStatus()))
        return false;

	//
    // Test6 : Deactivate CD
    //

    wsprintf(InfoText,"Deactivate CD");
   	t.DeactCD();
    if (MS_RLSD_ON & t.WaitStatus())
        return false;

	//
    // Test7 : Activate RI
    //

    wsprintf(InfoText,"Activate RI");
    t.ActRI();
    if (!(MS_RING_ON & t.WaitStatus()))
        return false;

	//
    // Test8 : Deactivate RI
    //

    wsprintf(InfoText,"Deactivate RI");
   	t.DeactRI();
    if (MS_RING_ON & t.WaitStatus())
        return false;

    //
    // Test 9: RX/TX at 300 baud
    //
    // Perform any buffer initialisation in DoRxTest and DoTxTest.
    //

    wsprintf(InfoText,"Rx/Tx at 300 Baud");

    if (!t.Set300())
        return false;

    CurrentBufferSize = 16;

    fStatus = t.DoRxTest(WriteBuffer,ReadBuffer,CurrentBufferSize);
    if (fStatus)
		fStatus = t.DoTxTest(WriteBuffer,ReadBuffer,CurrentBufferSize);

    if (!fStatus)
        return false;

    //
    // Test 10: RX/TX at 115200 baud
    //
    // Perform any buffer initialisation in DoRxTest and DoTxTest.
    //

    wsprintf(InfoText,"Rx/Tx at 115200 Baud");

    if (!t.Set115K())
        return false;

    CurrentBufferSize = 256;

    fStatus = t.DoRxTest(WriteBuffer,ReadBuffer,CurrentBufferSize);
    if (fStatus)
		fStatus = t.DoTxTest(WriteBuffer,ReadBuffer,CurrentBufferSize);

    return (BOOLEAN) fStatus;
}
//---------------------------------------------------------------------------
void __fastcall TSDIAppForm::ProgramBitBtnClick(TObject *Sender)
{
    ProgramBitBtn->Enabled = false;

    PassLabel->Visible = false;
    FailLabel->Visible = false;

    if (!DisableEEPromCB->Checked) {

        StatusBar->Panels->Items[0]->Text = "Program";

        if (!Program(Sender)) {
            StatusBar->Panels->Items[0]->Text = InfoText;
            FailLabel->Visible = true;
            goto quit;
        }

    }

    StatusBar->Panels->Items[0]->Text = "Test";

    if (!Test(Sender)) {
        StatusBar->Panels->Items[0]->Text = InfoText;
        FailLabel->Visible = true;
        goto quit;
    }

    StatusBar->Panels->Items[0]->Text = "";
    PassLabel->Visible = true;

quit:
    Application->ProcessMessages();

    ProgramBitBtn->Enabled = true;
    ProgramBitBtn->SetFocus();
}
//---------------------------------------------------------------------------




