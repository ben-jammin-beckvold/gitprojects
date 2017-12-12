#ifndef FTUSBDEV_H
#define FTUSBDEV_H

#include "ftd2xx.h"


class TFtUsbDev
{

public:

    FT_HANDLE ftdiHandle;
    HANDLE  Handle;

    __fastcall TFtUsbDev(void);
    __fastcall ~TFtUsbDev(void);
    BOOL Open(void);
    BOOL Open(int DeviceIndex);
    BOOL Read(LPVOID lpBuffer,
	    		 DWORD nBufferSize,
		         LPDWORD lpBytesReturned);
    BOOL Write(LPVOID lpBuffer,
   	    		  DWORD nBufferSize,
		          LPDWORD lpBytesWritten);
    BOOL Close(void);
};

#endif


