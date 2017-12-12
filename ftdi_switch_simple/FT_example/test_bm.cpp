//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <windows.h>
#include <winioctl.h>
#include "test_bm.h"
#include "ftd2xx.h"
//---------------------------------------------------------------------

__fastcall TTestBM::TTestBM(void)
{
	Running = FALSE;

    //
    // open COM1
    //
    hComm1 = CreateFile("\\\\.\\COM1",
    					GENERIC_READ | GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        0,
                        0);

	if (hComm1 == INVALID_HANDLE_VALUE)
    	return;

	//
    // open COM2
    //
    hComm2 = CreateFile("\\\\.\\COM2",
    					GENERIC_READ | GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        0,
                        0);

	if (hComm2 == INVALID_HANDLE_VALUE)
    	return;

    //
    // open the device
    //
    Dev = new TFtUsbDev;
    if (!Dev->Open())
        return;

    WaitStatusTime = 50;

    Running = TRUE;
}

__fastcall TTestBM::~TTestBM(void)
{
	if (Running) {
    	DeactCTS();
        DeactDSR();
        DeactCD();
        DeactRI();
    	CloseHandle(hComm1);
    	CloseHandle(hComm2);
        Dev->Close();
        delete Dev;
        Running = FALSE;
    }
}


BOOL TTestBM::IsRunning(void)
{
	return Running;
}


void TTestBM::Reset(void)
{
	//
    // Deactivate CTS, DSR, CD, RI
    //
    DeactCTS();
    DeactDSR();
    DeactCD();
    DeactRI();
}


void TTestBM::ActCTS(void)
{
	//
    // Activate CTS - Set RTS on COM2
    //
    EscapeCommFunction(hComm2,SETRTS);
}


void TTestBM::DeactCTS(void)
{
	//
    // Deactivate CTS - Clear RTS on COM2
    //
    EscapeCommFunction(hComm2,CLRRTS);
}


void TTestBM::ActDSR(void)
{
	//
    // Activate DSR - Set DTR on COM2
    //
    EscapeCommFunction(hComm2,SETDTR);
}


void TTestBM::DeactDSR(void)
{
	//
    // Deactivate DSR - Clear DTR on COM2
    //
    EscapeCommFunction(hComm2,CLRDTR);
}


void TTestBM::ActCD(void)
{
	//
    // Activate CD - Set DTR on COM1
    //
    EscapeCommFunction(hComm1,SETDTR);
}


void TTestBM::DeactCD(void)
{
	//
    // Deactivate CD - Clear DTR on COM1
    //
    EscapeCommFunction(hComm1,CLRDTR);
}


void TTestBM::ActRI(void)
{
	//
    // Activate RI - Set RTS on COM1
    //
    EscapeCommFunction(hComm1,SETRTS);
}


void TTestBM::DeactRI(void)
{
	//
    // Deactivate RI - Clear RTS on COM1
    //
    EscapeCommFunction(hComm1,CLRRTS);
}


DWORD TTestBM::ReadStatus(void)
{
	DWORD dw;

    FT_GetModemStatus(Dev->ftdiHandle,&dw);
	return dw;
}


DWORD TTestBM::WaitStatus(void)
{
   	Sleep(WaitStatusTime);
    return ReadStatus();
}


BOOL TTestBM::Set300(void)
{
	//
    // Setup for 300 baud 8N1 communications
    //

    //
    // first setup device
    //

    FT_SetBaudRate(Dev->ftdiHandle,FT_BAUD_300);
    FT_SetDataCharacteristics(Dev->ftdiHandle,FT_BITS_8,FT_STOP_BITS_1,FT_PARITY_NONE);
    FT_SetTimeouts(Dev->ftdiHandle,300,300);

    //
    // now setup COM2
    //
    DCB dcb;
    FillMemory(&dcb,sizeof(dcb),0);
    dcb.DCBlength = sizeof(dcb);
    if (!BuildCommDCB("300,n,8,1",&dcb)) {
    	return FALSE;
    }

    if (!SetCommState(hComm2,&dcb)) {
    	return FALSE;
    }

    COMMTIMEOUTS timeouts;

    timeouts.ReadIntervalTimeout = 20;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.ReadTotalTimeoutConstant = 300;
    timeouts.WriteTotalTimeoutMultiplier = 20;
    timeouts.WriteTotalTimeoutConstant = 200;

    if (!SetCommTimeouts(hComm2,&timeouts)) {
        return FALSE;
    }

    return TRUE;
}


BOOL TTestBM::Set115K(void)
{
	//
    // Setup for 115200 baud 8N1 communications
    //

    //
    // first setup device
    //

    FT_SetBaudRate(Dev->ftdiHandle,FT_BAUD_115200);
    FT_SetDataCharacteristics(Dev->ftdiHandle,FT_BITS_8,FT_STOP_BITS_1,FT_PARITY_NONE);
    FT_SetTimeouts(Dev->ftdiHandle,300,300);

    //
    // now setup COM2
    //
    DCB dcb;
    FillMemory(&dcb,sizeof(dcb),0);
    dcb.DCBlength = sizeof(dcb);
    if (!BuildCommDCB("115200,n,8,1",&dcb)) {
    	return FALSE;
    }

    if (!SetCommState(hComm2,&dcb)) {
    	return FALSE;
    }

    COMMTIMEOUTS timeouts;

    timeouts.ReadIntervalTimeout = 20;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.ReadTotalTimeoutConstant = 300;
    timeouts.WriteTotalTimeoutMultiplier = 20;
    timeouts.WriteTotalTimeoutConstant = 200;

    if (!SetCommTimeouts(hComm2,&timeouts)) {
        return FALSE;
    }

    return TRUE;
}


BOOL TTestBM::DoRxTest(unsigned char *WriteBuffer,
					   unsigned char *ReadBuffer,
                       DWORD BufLen)
{
/*++
    Device receive test
    ===================

    Transmit data from COM2 and read the device. If the read and write buffers
    are the same, the test is successful.

    Device read failures and timeouts are supported.
--*/

    unsigned char *source = WriteBuffer;
    DWORD len = BufLen;

    for (DWORD i = 0; i < BufLen; i++)
   	  	WriteBuffer[i] = (unsigned char) i;

    for (;;) {

	    DWORD dwWritten;

        if (!WriteFile(hComm2,source,len,&dwWritten,NULL))
            return FALSE;    // Write failed

        if (dwWritten == len)
            break;           // Finished

        if (dwWritten == 0)
            return FALSE;    // Write timeout

        if (dwWritten != len) { // Written part of data
            len -= dwWritten;
            source += dwWritten;
        }

    }

    unsigned char *dest = ReadBuffer;
    len = BufLen;

    memset(ReadBuffer,0,BufLen);

    for (;;) {

        DWORD dwRead;

        if (!Dev->Read(dest,len,&dwRead))
            return FALSE;    // Read failed

        if (dwRead == len)
            break;           // Finished

#if 1
        if (dwRead == 0)
            return FALSE;    // Read timeout
#endif

        if (dwRead != len) { // Read part of data
            len -= dwRead;
            dest += dwRead;
        }

    }

	return memcmp(WriteBuffer,ReadBuffer,BufLen) == 0;
}


BOOL TTestBM::DoTxTest(unsigned char *WriteBuffer,
					   unsigned char *ReadBuffer,
                       DWORD BufLen)
{
/*++
    Device transmit test
    ====================

    Transmit data from device and read COM2. If the read and write buffers
    are the same, the test is successful.

    Device write failures and timeouts are supported.
--*/

    BOOL fStatus;

    ActCTS();
    fStatus = MS_CTS_ON & WaitStatus();
    if (!fStatus)
        return FALSE;

    ActDSR();
    fStatus = MS_DSR_ON & WaitStatus();
    if (!fStatus)
        return FALSE;

    unsigned char *source = WriteBuffer;
    DWORD len = BufLen;

    for (;;) {

	    DWORD dwWritten;

        if (!Dev->Write(source,len,&dwWritten))
            return FALSE;    // Write failed

        if (dwWritten == len)
            break;           // Finished

        if (dwWritten == 0)
            return FALSE;    // Write timeout

        if (dwWritten != len) { // Written part of data
            len -= dwWritten;
            source += dwWritten;
        }

    }

    unsigned char *dest = ReadBuffer;
    len = BufLen;

    memset(ReadBuffer,0,BufLen);

    for (;;) {

	    DWORD dwRead;

        if (!ReadFile(hComm2,dest,len,&dwRead,NULL))
            return FALSE;    // Read failed

        if (dwRead == len)
            break;           // Finished

        if (dwRead == 0)
            return FALSE;    // Read timeout

        if (dwRead != len) { // Read part of data
            len -= dwRead;
            dest += dwRead;
        }

    }

	return memcmp(WriteBuffer,ReadBuffer,BufLen) == 0;
}


//---------------------------------------------------------------------
DWORD TTestBM::ReadLegacyStatus(void)
{
	DWORD dw;

    GetCommModemStatus(hComm2,&dw);
	return dw;
}


DWORD TTestBM::WaitLegacyStatus(void)
{
   	Sleep(WaitStatusTime);
    return ReadLegacyStatus();
}

void TTestBM::ActDTR(void)
{
	//
    // Activate DTR - Set DTR on device
    //
    FT_SetDtr(Dev->ftdiHandle);
}


void TTestBM::DeactDTR(void)
{
	//
    // Deactivate DTR - Reset DTR on device
    //
    FT_ClrDtr(Dev->ftdiHandle);
}

void TTestBM::ActRTS(void)
{
	//
    // Activate RTS - Set RTS on device
    //
    FT_SetRts(Dev->ftdiHandle);
}


void TTestBM::DeactRTS(void)
{
	//
    // Deactivate RTS - Reset RTS on device
    //
    FT_ClrRts(Dev->ftdiHandle);
}


//---------------------------------------------------------------------


