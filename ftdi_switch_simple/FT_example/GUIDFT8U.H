/*++

Copyright (c) 2000  Future Technology Devices International Ltd.

Module Name:

    guidft8u.h

Abstract:

    Native USB device driver for FTDI FT8U232
    The GUID is used to generate symbolic links to
    driver instances created from user mode

Environment:

    kernel & user mode

Revision History:

    18/09/00    awm     Created.

--*/


#ifndef GUIDFT8U_H
#define GUIDFT8U_H

#include <initguid.h>


// {219D0508-57A8-4ff5-97A1-BD86587C6C7E}
DEFINE_GUID(GUID_CLASS_FT8U245_BULK, 
0x219d0508, 0x57a8, 0x4ff5, 0x97, 0xa1, 0xbd, 0x86, 0x58, 0x7c, 0x6c, 0x7e);

#define IOCTL_FT_READ_EE			CTL_CODE(FILE_DEVICE_UNKNOWN,\
											 0x0820,\
		                                     METHOD_BUFFERED,\
        		                             FILE_ANY_ACCESS)
#define IOCTL_FT_WRITE_EE			CTL_CODE(FILE_DEVICE_UNKNOWN,\
											 0x0821,\
		                                     METHOD_BUFFERED,\
        		                             FILE_ANY_ACCESS)
#define IOCTL_FT_ERASE_EE			CTL_CODE(FILE_DEVICE_UNKNOWN,\
											 0x0822,\
		                                     METHOD_BUFFERED,\
        		                             FILE_ANY_ACCESS)

#endif

