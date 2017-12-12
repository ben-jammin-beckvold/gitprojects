// SerialPortEnumerator.cpp: implementation of the CSerialPortEnumerator class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "SerialPortEnumerator.h"

#include <setupapi.h>
#include <ntddser.h>
#include <wchar.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

GUID SerialPortClass = { 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialPortEnumerator::CSerialPortEnumerator()
{


}

CSerialPortEnumerator::~CSerialPortEnumerator()
{

}

SerialPortList
CSerialPortEnumerator::Enumerate()
{
	SerialPortList PortList;

	HDEVINFO DeviceInfo = SetupDiGetClassDevs(
		&SerialPortClass,
		0,
		0,
		DIGCF_PRESENT);

	if (INVALID_HANDLE_VALUE == DeviceInfo)
	{
		return PortList;
	}

	SP_DEVINFO_DATA DeviceInfoData = { 0 };
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	DWORD PortNameSize = 32;
	LPBYTE PortNameBuffer = (LPBYTE)malloc(PortNameSize);

	for (int i = 0; SetupDiEnumDeviceInfo(DeviceInfo, i, &DeviceInfoData); i++)
	{
		HKEY Handle = SetupDiOpenDevRegKey(
			DeviceInfo, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

		DWORD RegType = REG_SZ;
		DWORD ErrorCode;

		while (TRUE)
		{
			const wchar_t *pn = _T("PortName");
			ErrorCode = RegQueryValueEx(
				Handle, pn, 0, &RegType, PortNameBuffer, &PortNameSize);

			if (ERROR_MORE_DATA == ErrorCode)
			{
				if (PortNameBuffer)
				{
					free(PortNameBuffer);
				}

				PortNameBuffer = (LPBYTE)malloc(PortNameSize);
			}
			else
			{
				break;
			}
		}

		RegCloseKey(Handle);

		if (ERROR_SUCCESS == ErrorCode)
		{
			size_t nchar;
			char * name = (char *)malloc(PortNameSize);
			wcstombs_s(&nchar, name, PortNameSize, (const wchar_t *)PortNameBuffer, PortNameSize);
			if (name[0] == L'C' || name[0] == L'c') {
				PortList.push_back(name);
			}
			free(name);
		}
	}

	SetupDiDestroyDeviceInfoList(DeviceInfo);
	DeviceInfo = NULL;

	if (PortNameBuffer)
	{
		free(PortNameBuffer);
	}

	return PortList;
}
