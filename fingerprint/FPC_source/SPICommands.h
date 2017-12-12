// SerialCommands.h: interface for the CSerialCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPICOMMANDS_H__05049468_EBF7_46B8_97B3_CE39FFEAD73E__INCLUDED_)
#define AFX_SPICOMMANDS_H__05049468_EBF7_46B8_97B3_CE39FFEAD73E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "spi_interface.h"

class CSPICommand  
{
public:

	spi_interface *m_spi;
	unsigned char m_CommandArray [6];	//Array containing the 6 byte command structure, sent from host
	unsigned char m_Response [4];		//Array containing received 4 byte response structure
	unsigned char m_CRC [4];			//Array receiving the CRC value

	bool IsOpen();	//True if SPI comm is open
	void Close();	//Close SPI comm
	bool Open(CString Port, HANDLE dlg); //Initialize SPI Communication
	void KillCurrentCommunication();

	CSPICommand();
	virtual ~CSPICommand();

protected:

	bool m_bIsOpen;

////////////////////////////////////////////////////////////////////
//	API functions
////////////////////////////////////////////////////////////////////
public:
	
	//Image handling
	bool ApiCaptureImage(bool checkFingerPresent = false);									//Capture an image from sensor, put it in RAM and return fingerpresent true or false
	bool ApiUploadImage(unsigned char **image, DWORD *size);			//Upload image from RAM to PC
	bool ApiDownloadImage(unsigned char *img, int size, int indexValue);	//Download image from PC to RAM

	//Enrol, verify & identify
	bool ApiCaptureEnrolRAM();								//Capture an image and enrol it into RAM
	bool ApiEnrolRAM();										//Create a template from the image in RAM 

	bool ApiCaptureVerifyRAM();								//Capture an image and verify it against template in RAM
	bool ApiVerifyRAM();									//Verify image in RAM against template in RAM
	bool ApiCaptureVerifyFLASH(int slotNumber);				//Capture an image and verify against template in FLASH slot
	bool ApiVerifyFLASH(int slotValue);						//Verify image in RAM against template in FLASH slot

	bool ApiCaptureIdentify(int * slotValue);				//Capture an image, and identify against all templates in FLASH
	bool ApiIdentify(int * slotValue);						//Identify image in RAM against all templates in FLASH
	
	//Template handling commands
	bool ApiUploadTemplate(unsigned char *templ);			//Upload template from RAM to PC
	bool ApiUploadTemplateFLASH(unsigned char **templ, int *payload, int slot); //Upload template from FLASH slot to PC
	bool ApiDownloadTemplate( unsigned char *templ, int size);	//Download template fom PC to RAM

	bool ApiCopyTemplateRamToFLASH(int slotNumber);			//Copy template from RAM to slot in FLASH
	bool ApiDeleteTemplateSlot(int slotNumber);				//Delete a template in a FLASH slot
	bool ApiDeleteAllInFLASH();								//Delete all templates in FLASH

	//Application commands
	bool ApiTestHardware();									//Test that hardware and comm is ok..
	bool ApiFirmwareVersion();								//Get firmware version string
	bool ApiSetBaudRateRAM ( int baudIndex);				//Change baudrate. Value is stored only until unit is reset.
	bool ApiCancel();										//Cancel ongoing command
    bool ApiInitiateUpgrade();
	bool ApiUpgradeFirmwareBlock( unsigned char *data, int size, int flashSlot); //Download a new FirmwareBlock to FPC2020
	bool ApiDownloadProgram(unsigned char *buffer, int size);


};

#endif // !defined(AFX_SPICOMMANDS_H__05049468_EBF7_46B8_97B3_CE39FFEAD73E__INCLUDED_)
