//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ftusbdev.h"
#include "ftd2xx.h"
//---------------------------------------------------------------------
__fastcall TFtUsbDev::TFtUsbDev(void)
{
    ftdiHandle = 0;
}


__fastcall TFtUsbDev::~TFtUsbDev(void)
{
}


BOOL TFtUsbDev::Open(void)
{
    BOOL rval = Open(0);
    return rval;
}

BOOL TFtUsbDev::Open(int DeviceIndex)
{
    FT_STATUS status = FT_Open(DeviceIndex,&ftdiHandle);

    return FT_SUCCESS(status);
}

BOOL TFtUsbDev::Read(LPVOID lpBuffer,
                     DWORD nBufferSize,
                     LPDWORD lpBytesReturned)
{
    FT_STATUS status = FT_Read(
                            ftdiHandle,
                            lpBuffer,
                            nBufferSize,
                            lpBytesReturned
                            );

    return FT_SUCCESS(status);
}

BOOL TFtUsbDev::Write(LPVOID lpBuffer,
                      DWORD nBufferSize,
                      LPDWORD lpBytesWritten)
{
    FT_STATUS status = FT_Write(
                            ftdiHandle,
                            lpBuffer,
                            nBufferSize,
                            lpBytesWritten
                            );

    return FT_SUCCESS(status);
}

BOOL TFtUsbDev::Close(void)
{
    FT_STATUS status = FT_Close(ftdiHandle);

	return FT_SUCCESS(status);
}


