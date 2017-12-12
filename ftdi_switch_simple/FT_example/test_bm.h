//----------------------------------------------------------------------------
#ifndef TestBmH
#define TestBmH
//----------------------------------------------------------------------------
#include "FtUsbDev.h"
//----------------------------------------------------------------------------
class TTestBM
{
private:
	HANDLE hComm1;		// COM1
	HANDLE hComm2;		// COM2
    BOOL Running;
    DWORD WaitStatusTime;
    TFtUsbDev *Dev;

public:
	__fastcall TTestBM(void);
	__fastcall ~TTestBM(void);
    BOOL IsRunning(void);
    void Reset(void);
    void ActCTS(void);
    void DeactCTS(void);
    void ActDSR(void);
    void DeactDSR(void);
	void ActCD(void);
	void DeactCD(void);
	void ActRI(void);
	void DeactRI(void);
    DWORD ReadStatus(void);
    DWORD WaitStatus(void);
    BOOL Set300(void);
    BOOL Set115K(void);
    BOOL DoRxTest(unsigned char *WriteBuffer,
    			  unsigned char *ReadBuffer,
                  DWORD BufLen);
    BOOL DoTxTest(unsigned char *WriteBuffer,
    			  unsigned char *ReadBuffer,
                  DWORD BufLen);
    DWORD ReadLegacyStatus(void);
    DWORD WaitLegacyStatus(void);
    void ActDTR(void);
	void DeactDTR(void);
	void ActRTS(void);
	void DeactRTS(void);
};
//----------------------------------------------------------------------------
#endif



