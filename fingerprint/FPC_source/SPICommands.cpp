// SerialFpcConnection.cpp: implementation of the CSerialCommand class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"


#include "crc.h"
#include "ComDlg.h"

#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "SPICommands.h"
#include "Definitions.h"
#include "spi_interface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CComDlg *gDlg; 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSPICommand::CSPICommand()
{
	m_bIsOpen = FALSE;
	m_spi=new spi_interface();
	

}

CSPICommand::~CSPICommand()
{
	delete m_spi;
}


////////////////////////////////////////////////////////////////////////
//Open the SPI port
////////////////////////////////////////////////////////////////////////
bool CSPICommand::Open(CString Port, HANDLE dlg)
{
	gDlg = (CComDlg*)dlg;

	if (m_spi->Open(1))
		m_bIsOpen = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//Close the SPI port
////////////////////////////////////////////////////////////////////////
void CSPICommand::Close()
{
	m_bIsOpen = FALSE;
	m_spi->Close();

}

////////////////////////////////////////////////////////////////////////
//Check if the port is open
////////////////////////////////////////////////////////////////////////
bool CSPICommand::IsOpen()
{
	return m_bIsOpen;
}

//Cancels all SPI Communication:
void CSPICommand::KillCurrentCommunication(){

	Close();
}




/////////////////////////////////////////////////////////////////////////////////////
//***********************************************************************************
//Below begins the API functions
//They are made as simple as possible, with very few calls to external functions.
//***********************************************************************************
/////////////////////////////////////////////////////////////////////////////////////


/****** Image handling functions ****************************************************************/


////////////////////////////////////////////////////////////////////////////////////////
//ApiCaptureImage
//Capture an image from the sensor.. Will place the image in internal RAM, and respond with
//true if a finger is detected in the image. 
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiCaptureImage(bool checkFingerPresent)
{
	//CommandStructure: Startbyte 0x02, index 0 , command 0x80, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CAPTURE_IMAGE; //0x80;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	m_Response[0] = 0;
	m_Response[1] = 0;
	m_Response[2] = 0;
	m_Response[3] = 0;

	if (checkFingerPresent) m_CommandArray [3] = API_CAPTURE_IMAGE_FINGERPRESENT; 
	
	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive, 

	//Read 4 response bytes:
	if (m_CommandArray[3] == API_CAPTURE_IMAGE){
		m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);
	} else {
		//We have to do it the following way, if we want to be able to cancel the operation:
		while (!m_spi->ReadDataEx((char*)m_Response,4) && (m_CommandArray[3] != API_CANCEL)){ //Try to read 4 bytes, while the cancel button not has been pressed!
			//Enable the cancelling function:
			{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
		}
	}

	//Print data to screen (unless cancelled)
	if (m_CommandArray[3] != API_CANCEL)
		gDlg->PrintData(m_Response, 4, false);

	return m_Response[1]==API_FINGER_PRESENT;  //0x03

}


/////////////////////////////////////////////////////////////////////
//ApiUploadImage
//Upload image from the image memory to PC..
/////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiUploadImage(unsigned char **image, DWORD *payload)
{
	//CommandBuffer: Startbyte 0x02, no index, command 0x90, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x01; //Will skip the CRC generation if set to 1.
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_UPLOAD_IMAGE; //0x90;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	gDlg->m_StatusCtrl.SetWindowText(_T("Status: Image is uploading, please wait."));

	//If we turn off the CRC we can transmit at a high rate!
//	m_spi->SetDelay1(CHEETAH_VERYLOW_SPEED/*CHEETAH_HIGH_SPEED*/); 
	m_spi->SetDelay1(CHEETAH_HIGH_SPEED);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);
	
	if ( m_Response[1]==API_SUCCESS){  //0x01	
		*payload = m_Response[2] + m_Response[3] * 256;//Read the image (payload data);
		*image = (unsigned char *)malloc(*payload);
		m_spi->ReadData(0, 0, (char*)(*image), *payload);
	}
	//Read CRC
	m_spi->ReadData(0, 0, (char*)m_CRC, 4);

	m_spi->SetDelay1(CHEETAH_DEFAULT_SPEED); //Back to normal speed again!
	
	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);
	gDlg->PrintData(*image, *payload, false);
	gDlg->PrintData(m_CRC, 4, false);

	return m_Response [1] == API_SUCCESS; //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiDownloadImage
//Download image from PC to module.. Only relevant for FPC6610 module and FPC2020
//Special restriction for FPC6610: We have to delay between the 6 byte command and the 30400 byte payload.
//This is due to the limited RAM memory in the module..
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiDownloadImage( unsigned char *img, int imgSize, int indexValue)
{

	unsigned char payload_msb = imgSize / 256;        
	unsigned char payload_lsb = imgSize - imgSize / 256 *256;    

	//CommandBuffer: Startbyte 0x02, no index, command 0x91, imgSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = (indexValue&0xFF);	//If we use a non-zero here, the CRC check will be skipped, and everything will be faster!
	m_CommandArray [2] = (indexValue>>8);
	m_CommandArray [3] = API_DOWNLOAD_IMAGE; //0x91;
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb;


	gDlg->m_StatusCtrl.SetWindowText(_T("Status: Image is downloading, please wait."));

	//If we turn off the CRC we can transmit at a high rate!
	if (indexValue) m_spi->SetDelay1(CHEETAH_HIGH_SPEED);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);
	//Send Image
	m_spi->SendData((char*)img,imgSize,0,0);

	//Calculate and send CRC:
	unsigned int intCRC = crcCalculate(img, imgSize);
	m_CRC[0]= intCRC&0xFF; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	m_spi->SendData((char*)m_CRC,4,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	m_spi->SetDelay1(CHEETAH_DEFAULT_SPEED); //Back to normal speed again!

	//Dump Communicated data to Screen:
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(img, imgSize, true);
	gDlg->PrintData(m_CRC, 4, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response [1] == API_SUCCESS; //0x01
}

bool CSPICommand::ApiInitiateUpgrade()
{
    m_CommandArray[0] = 0x02;
    m_CommandArray[1] = 0;
    m_CommandArray[2] = 0;
    m_CommandArray[3] = API_FIRMWARE_INITIATE_UPGRADE; //0xC4;
    m_CommandArray[4] = 0x00;
    m_CommandArray[5] = 0x00;

    m_spi->SendData((char*)m_CommandArray, 6, 0, 0);

    Sleep(5); //A short pause between send and receive,

    //Read 4 response bytes:
    //We have to do it the following way, if we want to be able to cancel the operation:
    while (!m_spi->ReadDataEx((char*)m_Response, 4) && (m_CommandArray[3] != API_CANCEL)) { //Try to read 4 bytes, while the cancel button not has been pressed!
                                                                                            //Enable the cancelling function:
        { MSG msg; while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) { AfxGetApp()->PumpMessage(); }}//Pump message queue
    }

    //Print bytes on screen (if this response was not cancelled!)
    if (m_CommandArray[3] != API_CANCEL) {
        gDlg->PrintData(m_Response, 4, false);
    }

    return m_Response[1] == 0x00;
}

/****** Enrol, verify & identify functions ****************************************************************/

/////////////////////////////////////////////////////////////////////
//ApiCaptureEnrolRAM
//function waits for finger present, captures an image, 
//and enrols it into RAM
/////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiCaptureEnrolRAM()
{

	//CommandStructure: Startbyte 0x02, no index, command 0x81, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0;
	m_CommandArray [2] = 0;
	m_CommandArray [3] = API_CAPTURE_AND_ENROL_RAM; //0x81;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	//We have to do it the following way, if we want to be able to cancel the operation:
	while (!m_spi->ReadDataEx((char*)m_Response,4) && (m_CommandArray[3] != API_CANCEL)){ //Try to read 4 bytes, while the cancel button not has been pressed!
		//Enable the cancelling function:
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}

	//Print bytes on screen (if this response was not cancelled!)
	if (m_CommandArray[3] != API_CANCEL)
		gDlg->PrintData(m_Response, 4, false);

	return m_Response[1]==0x06; //API_ENROL_OK
}

////////////////////////////////////////////////////////////////////////////////////////
//ApiEnrolRAM
//uses the image in RAM, enrols it, and places template in RAM
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiEnrolRAM()
{
//	int payload_size;
//	unsigned char localBuffer [30400]; //Large enough to handle a full image if necessary.	

	//CommandStructure: Startbyte 0x02, index 0 , command 0x85, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0;
	m_CommandArray [2] = 0;
	m_CommandArray [3] = API_ENROL_RAM; //0x85;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response[1]==API_ENROL_OK;  //0x06
}


/////////////////////////////////////////////////////////////////////
// ApiCaptureVerifyRAM
// Waits for finger present, captures an image,
// and verifies it against a template in RAM memory..
/////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiCaptureVerifyRAM()
{


	//CommandStructure: Startbyte 0x02, no index, command 0x82, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0;
	m_CommandArray [2] = 0;
	m_CommandArray [3] = API_CAPTURE_AND_VERIFY_RAM;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	//We have to do it the following way, if we want to be able to cancel the operation:
	while (!m_spi->ReadDataEx((char*)m_Response,4) && (m_CommandArray[3] != API_CANCEL)){ //Try to read 4 bytes, while the cancel button not has been pressed!
		//Enable the cancelling function:
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}

	//Print bytes on screen (if this response was not cancelled!)
	if (m_CommandArray[3] != API_CANCEL){
		gDlg->PrintData(m_Response, 4, false);
	}

	return m_Response[1]==0x04; //API_VERIFICATION_OK

}


////////////////////////////////////////////////////////////////////////////////////////
//ApiVerifyRAM
//Verifies image in RAM against template in RAM
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiVerifyRAM()
{

	//CommandStructure: Startbyte 0x02, idx =0 , command 0x86, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0;
	m_CommandArray [2] = 0;
	m_CommandArray [3] = API_VERIFY_RAM; //0x86;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);


	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response[1]==API_VERIFICATION_OK;  //0x04
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiCaptureVerifyFLASH
//Wait for finger present, capture image, and verify against template in FLASH slot
//No time-out, but it can be canceled with the API_CANCEL command
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiCaptureVerifyFLASH(int slotNumber)
{

	//CommandStructure: Startbyte 0x02, idx_lsb = slotNumber, idx_msb =0 , command 0x83, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slotNumber&0xFF;
	m_CommandArray [2] = slotNumber>>8;
	m_CommandArray [3] = API_CAPTURE_AND_VERIFY_FLASH; //0x83;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	//We have to do it the following way, if we want to be able to cancel the operation:
	while (!m_spi->ReadDataEx((char*)m_Response,4) && (m_CommandArray[3] != API_CANCEL)){ //Try to read 4 bytes, while the cancel button not has been pressed!
		//Enable the cancelling function:
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}

	//Print bytes on screen (unless cancelled)
	if (m_CommandArray[3] != API_CANCEL){
		gDlg->PrintData(m_Response, 4, false);
	}

	return m_Response[1]==0x04;  //API_VERIFICATION_OK
}



////////////////////////////////////////////////////////////////////////////////////////
//ApiVerifyFLASH
//Verify image in RAM against template in FLASH slot
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiVerifyFLASH(int slotNumber)
{

	//CommandStructure: Startbyte 0x02, idx_lsb = slotNumber, idx_msb =0 , command 0x87, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slotNumber&0xFF;
	m_CommandArray [2] = slotNumber>>8;
	m_CommandArray [3] = API_VERIFY_FLASH; //0x87;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);	

	return m_Response[1]==API_VERIFICATION_OK;  //0x04
}



////////////////////////////////////////////////////////////////////////////////////////
//ApiCaptureIdentify
//Wait for finger present, capture image, identify it against all FLASH slots..
//slotValue will contain the slot were identification succeeded, if any.
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiCaptureIdentify(int * slotValue)
{
	unsigned char localBuffer [10];
	int payload_size;

	//CommandStructure: Startbyte 0x02, no index, command 0x84, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CAPTURE_AND_IDENTIFY_FLASH; //0x84;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
//	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);
	//We have to do it the following way, if we want to be able to cancel the operation:
	while (!m_spi->ReadDataEx((char*)m_Response,4) && (m_CommandArray[3] != API_CANCEL)){ //Try to read 4 bytes, while the cancel button not has been pressed!
		//Enable the cancelling function:
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}

	payload_size = m_Response[2] + (m_Response[3]<<8);
	if (payload_size){ //This is where we will receive the index of the correctly matching finger slot!
		m_spi->ReadData(0,0,(char*)localBuffer,payload_size);
		m_spi->ReadData(0,0,(char*)m_CRC,4);

		*slotValue = localBuffer[0]+localBuffer[1]*256; //This contains which slot was found to match during identification
	}

	//Print bytes on screen (unless cancelled)
	if (m_CommandArray[3] != API_CANCEL){
		gDlg->PrintData(m_Response, 4, false);
		gDlg->PrintData(localBuffer, payload_size, false);
		if (payload_size) gDlg->PrintData(m_CRC, 4, false);
	}
	
	return m_Response[1]==API_IDENTIFY_OK; //0x0F
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiIdentify
//Identify image in RAM against all templates in FLASH
//slotValue will contain the slot were identification succeeded, if any.
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiIdentify(int * slotValue)
{
	unsigned char localBuffer [10];
	int payload_size;

	//CommandStructure: Startbyte 0x02, no index, command 0x88, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_IDENTIFY_FLASH; //0x88;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	payload_size = m_Response[2] + (m_Response[3]<<8);
	if (payload_size){ //This is where we might receive the Verification Score (in case of database mode!)
		m_spi->ReadData(0,0,(char*)localBuffer,payload_size);
		m_spi->ReadData(0,0,(char*)m_CRC,4);

		//FPC2020 compatible only:
		*slotValue = localBuffer[0]+localBuffer[1]*256; //This contains which slot was found to match during identification
	}

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);
	gDlg->PrintData(localBuffer, payload_size, false);
	if (payload_size) gDlg->PrintData(m_CRC, 4, false);
		
	return m_Response[1]==API_IDENTIFY_OK; //0x0F

}



/****** Template handling functions ****************************************************************/


/////////////////////////////////////////////////////////////////////
//ApiUploadTemplate
//Upload template from RAM to PC.. 
/////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiUploadTemplate( unsigned char *templ )
{

	int payload;

	//CommandBuffer: Startbyte 0x02, no index, command 0xA0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_UPLOAD_TEMPLATE;//0xA0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	gDlg->m_StatusCtrl.SetWindowText(_T("Status: Template is uploading, please wait."));

	//This command must be run a bit slower than the other spi signalling,
	//since the CRC is tough to compute.
	//Therefore, we turn down the speed of the transmission temporarily:
	m_spi->SetDelay1(CHEETAH_LOW_SPEED);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);
	
	if ( m_Response[1]==API_SUCCESS){  //0x01	
		payload = m_Response[2] + m_Response[3] * 256;//Read the image (payload data);
		m_spi->ReadData(0,0,(char*)templ,payload);
	}
	//Read CRC
	m_spi->ReadData(0,0,(char*)m_CRC,4);
	m_spi->SetDelay1(CHEETAH_DEFAULT_SPEED);
	
	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);
	gDlg->PrintData(templ, payload, false);
	gDlg->PrintData(m_CRC, 4, false);

	return m_Response [1] == API_SUCCESS; //0x01
}



////////////////////////////////////////////////////////////////////////////////////////
//ApiUploadTemplateFLASH
//Upload template from FLASH slot to PC.. 
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiUploadTemplateFLASH(unsigned char **templ, int *payload, int slot)
{
	//CommandBuffer: Startbyte 0x02, slot, 0, command 0xA3, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slot&255;
	m_CommandArray [2] = slot>>8;
	m_CommandArray [3] = API_UPLOAD_TEMPLATE_FROM_FLASH; //0xA3;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	gDlg->m_StatusCtrl.SetWindowText(_T("Status: Template is uploading, please wait."));

	m_spi->SetDelay1(CHEETAH_LOW_SPEED);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);
	
	if (m_Response[1] == API_SUCCESS){  //0x01	
		*payload	= m_Response[2] + m_Response[3] * 256;//Read the image (payload data);
		*templ		= (unsigned char *)malloc(*payload);
		m_spi->ReadData(0, 0, (char *)(*templ), *payload);
	}
	//Read CRC
	m_spi->ReadData(0, 0, (char *)m_CRC, 4);

	m_spi->SetDelay1(CHEETAH_DEFAULT_SPEED);

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);
	gDlg->PrintData(*templ, *payload, false);
	gDlg->PrintData(m_CRC, 4, false);

	return m_Response [1] == 0x01; //API_SUCCESS
}

/////////////////////////////////////////////////////////////////////
//ApiDownloadTemplate
//Download template from PC to RAM
/////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiDownloadTemplate( unsigned char *templ, int templSize)
{
	
	unsigned char payload_msb = templSize / 256;        
	unsigned char payload_lsb = templSize - templSize / 256 *256;  

	//CommandBuffer: Startbyte 0x02, no index, command 0xA1, templSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_DOWNLOAD_TEMPLATE; //0xA1; //0xA7 is for downloading to flash.
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb;
	
	gDlg->m_StatusCtrl.SetWindowText(_T("Status: Image is downloading, please wait."));

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);
	//Send Image
	m_spi->SendData((char*)templ,templSize,0,0);
	//Calculate and send CRC:
	unsigned int intCRC = crcCalculate(templ, templSize);
	m_CRC[0]= intCRC&0xFF; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	m_spi->SendData((char*)m_CRC,4,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Dump Communicated data to Screen:
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(templ, templSize, true);
	gDlg->PrintData(m_CRC, 4, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response [1] == API_SUCCESS; //0x01
}

bool CSPICommand::ApiUpgradeFirmwareBlock( unsigned char *data, int size, int flashSlot)
{
	//This command is not supported by the Main Program, only the Installer!
	
	unsigned char payload_msb = size >> 8;	//Should be 1.        
	unsigned char payload_lsb = size & 0xFF;  //Should be 0.

	//CommandBuffer: Startbyte 0x02, no index, command 0xA1, templSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = flashSlot&255;
	m_CommandArray [2] = flashSlot>>8;
	m_CommandArray [3] = API_FIRMWARE_UPDATE; //0xC1;
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb;
	
	m_spi->SetDelay1(CHEETAH_LOWER_SPEED);
	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);
	//Send Image
	m_spi->SendData((char*)data,size,0,0);
	//Calculate and send CRC:
	unsigned int intCRC = crcCalculate(data, size);
	m_CRC[0]= intCRC&0xFF; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	m_spi->SendData((char*)m_CRC,4,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4); //Buffer is a temporary debug variable hold!

	m_spi->SetDelay1(CHEETAH_DEFAULT_SPEED);

	//Dump Communicated data to Screen:
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(data, size, true);
	gDlg->PrintData(m_CRC, 4, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response [1] == API_SUCCESS; //0x01

}

bool CSPICommand::ApiDownloadProgram(unsigned char *buffer, int size){

	unsigned char payload_msb = size >> 8;	
	unsigned char payload_lsb = size & 0xFF; 

	//CommandBuffer: Startbyte 0x02, no index, command 0xA1, templSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0;
	m_CommandArray [2] = 0;
	m_CommandArray [3] = API_RUN_SUPPLIED_PRG; 
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb;
	
	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);
	gDlg->PrintData(m_CommandArray, 6, true);//Print bytes on screen
	
	//Send Program (Payload data)
	m_spi->SendData((char*)buffer,size,0,0);

	//Calculate and send the CRC		//This CRC is actually not checked by the boot code, so it could be any 4 bytes! (There is a CRC-16 built into any supplied program itself!)
	unsigned int intCRC = crcCalculate(buffer, size);
	m_CRC[0]= intCRC&0xFF; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	m_spi->SendData((char*)m_CRC,4,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	gDlg->PrintData(buffer, size, true);//Print sent data bytes to screen (This process takes too much time - so the watchdog will kill the program, if we insert this line in between the actual commandwrites!)
	gDlg->PrintData(m_CRC, 4, true);//Print sent CRC bytes to screen
	gDlg->PrintData(m_Response, 4, false);//Print reply on screen

	return m_Response [1] == API_SUCCESS; //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiCopyTemplateRamToFLASH
//Copy template from RAM to FLASH, using slot information on where to store it
//Be sure that there is a valid template in RAM before calling this function..
//A template is located in RAM after an ENROL call or after API_DOWNLOAD_TEMPLATE
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiCopyTemplateRamToFLASH(int slotNumber)
{

	//CommandStructure: Startbyte 0x02, idx_lsb = slotNumber, idx_msb =0 , command 0xA2, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slotNumber&0xFF;
	m_CommandArray [2] = slotNumber>>8;
	m_CommandArray [3] = API_COPY_TEMPLATE_RAM_TO_FLASH; //0xA2;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

// ============== TEST =============== 
/*	//Borrow this function for a test of the Flash Notepad Function:
	m_CommandArray [3] = 0xE9; //Notepad access command
	m_CommandArray [4] = 0x20;   //This must be 32, always.

	unsigned char data[32] = {1,2,3,4,5,5,4,3,2,1,1,1,1,1,1,1,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Some random test data to be written to the notepad
	
	//REQUEST:
	unsigned int intCRC = crcCalculate(data, 32);
	m_CRC[0]= intCRC; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	m_spi->SendData((char*)m_CommandArray,6,0,0);
	m_spi->SendData((char*)data,32,0,0);
	m_spi->SendData((char*)m_CRC,4,0,0);
	gDlg->PrintData(m_CommandArray, 6, true);//Print command data on screen
	gDlg->PrintData(data, 32, true);//Print command data on screen
	gDlg->PrintData(m_CRC, 4, true);//Print command data on screen
	
	//RESPONSE:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);
	gDlg->PrintData(m_Response, 4, false);//Print m_Response data on screen

	return m_Response[1]==API_SUCCESS;  //0x01  */
// ===================================
	
	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response[1]==API_SUCCESS;  //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiDeleteTemplateSlot
//Delete a template in a FLASH slot
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiDeleteTemplateSlot(int slot)
{
	
	//CommandBuffer: Startbyte 0x02, slot, 0, command 0xA5, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slot&0xFF;
	m_CommandArray [2] = slot>>8;
	m_CommandArray [3] = API_DELETE_SLOT_IN_FLASH; //0xA5;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response [1] == API_SUCCESS; //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiDeleteAllInFLASH
//Delete ALL templates in FLASH
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiDeleteAllInFLASH()
{
	
	//CommandBuffer: Startbyte 0x02, 0, 0, command 0xA6, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_DELETE_ALL_IN_FLASH; //0xA6;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response [1] == API_SUCCESS; //0x01
}


/************ Application commands **********************************************************/


////////////////////////////////////////////////////////////////////////
//ApiTestHardware
//Send this command to test module hardware and communications..
////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiTestHardware()
{
	unsigned char *Data;

	//Fill the command byte array
	m_CommandArray [0] = 0x02; //Start byte, always 0x02 
	m_CommandArray [1] = 0x00; //Index value, LSB
	m_CommandArray [2] = 0x00; //Index value, MSB
	m_CommandArray [3] = API_TEST_HARDWARE; //Command byte 0xD2;
	m_CommandArray [4] = 0x00; //Payload counter, LSB
	m_CommandArray [5] = 0x00; //Payload counter, MSB

	gDlg->PrintData(m_CommandArray, 6, true);//Print bytes on screen

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);
		
	//Under certain circumstances, dump the response to file
	int PayloadSize = m_Response[2] + (m_Response[3]<<8);
	if (m_Response[1] == API_HW_TEST_OK && PayloadSize){
		Data = new unsigned char [PayloadSize]; //Allocate memory
		m_spi->ReadData(0,0,(char*)Data,PayloadSize);
		//Read the CRC
		m_spi->ReadData(0,0,(char*)m_CRC,4);

		//Print communicated data on screen:
		gDlg->PrintData(m_Response, 4, false);
		gDlg->PrintData(Data, PayloadSize, false);
		gDlg->PrintData(m_CRC, 4, false); 

		//Dump reponse to file
		{
			FILE* f;
			fopen_s(&f, "HardwareTest.txt", "a");	
			for (int i=0; i<PayloadSize; i++){
				fprintf(f,"%d\n",Data[i]);
			}
			fclose(f);
		}
		delete [] Data;
	} else {
		
		gDlg->PrintData(m_Response, 4, false);//Print reply on screen
	}
	return m_Response[1] == API_HW_TEST_OK;  //0x08	

}


/////////////////////////////////////////////////////////////////////
//ApiFirmwareVersion
//Check the application's firmware version
/////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiFirmwareVersion()
{

	unsigned char localBuffer[100];
	int payload_size;
	
	//CommandBuffer: Startbyte 0x02, no index, command 0xC0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_FIRMWARE_VERSION; //0xC0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	payload_size = m_Response[2] + (m_Response[3]<<8);
	if (payload_size){ //This is where we might receive the Verification Score (in case of database mode!)
		m_spi->ReadData(0,0,(char*)localBuffer,payload_size);
		m_spi->ReadData(0,0,(char*)m_CRC,4);
	}

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);
	if (payload_size){
		gDlg->PrintData(localBuffer, payload_size, false);
		gDlg->PrintData(m_CRC, 4, false);
	}

	return m_Response [1] == API_SUCCESS; //0x01
}


//////////////////////////////////////////////////////////////////////////////////////////////
//ApiSetBaudRateRAM
//Set new baud rate. The unit will respond using the old baud rate, and then change baud rate 
//Index value (idx_lsb) is used to transmit the baud index value..
//The baudrate is set in RAM, and will be valid only until unit power-off or reset
//////////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiSetBaudRateRAM( int baudIndex)
{

	//CommandStructure: Startbyte 0x02, idx_lsb = baudIndex, idx_msb =0 , command 0xD0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = baudIndex;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_SET_BAUD_RATE_RAM;//0xD0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	
	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes:
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);
	gDlg->PrintData(m_Response, 4, false);

	return m_Response[1]==0x01;  //API_SUCCESS

}


////////////////////////////////////////////////////////////////////////////////////////
//ApiCancel
//Send cancel to abort the fingerpresent loop in the CaptureEnrol, CaptureVerify and 
//CaptureIdentify functions
////////////////////////////////////////////////////////////////////////////////////////
bool CSPICommand::ApiCancel()
{
	
	//CommandStructure: Startbyte 0x02, index =0 , command 0xE0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CANCEL; //0xE0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	//Print bytes on screen
	gDlg->PrintData(m_CommandArray, 6, true);

	//Send Command:
	m_spi->SendData((char*)m_CommandArray,6,0,0);

	Sleep(5); //A short pause between send and receive,

	//Read 4 response bytes: (We have a waiting time here)
	m_spi->ReadDataWhenNotBusy(0,0,(char*)m_Response,4);

	//Print bytes on screen
	gDlg->PrintData(m_Response, 4, false);

	return m_Response[1]==API_SUCCESS;  //0x01
}
