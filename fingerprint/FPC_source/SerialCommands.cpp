// SerialFpcConnection.cpp: implementation of the CSerialCommand class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"


#include "crc.h"
#include "ComDlg.h"

#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "SerialCommands.h"
#include "Definitions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CComDlg *gDlg; 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialCommand::CSerialCommand(LPCSTR szPort)
{
	m_bIsOpen     = FALSE;
	m_hComm      = NULL;
	m_szPortName = (LPSTR)szPort;

}

CSerialCommand::~CSerialCommand()
{

}


////////////////////////////////////////////////////////////////////////
//Open the serial port
////////////////////////////////////////////////////////////////////////
bool CSerialCommand::Open(CString Port, int Baud, int BitsValue, CString Parity, int StopValue, HANDLE dlg)
{
	gDlg = (CComDlg*)dlg;
	assert(!m_bIsOpen);
	CString wAround = "\\\\.\\"; //Use this to be able to open port numbers above 9.. According to MS Article ID: Q115831 
	wAround += Port;
	//Open the port as we would open a file..
	m_hComm = ::CreateFile(wAround,//"COM1:",
						GENERIC_READ|GENERIC_WRITE,
						0,  // No sharing                         
						0,                    
						OPEN_EXISTING,
						0,
						0);

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//Get and set the serial settings
	DCB dcb; 
	if (!GetCommState(m_hComm, &dcb))
	{
		CloseHandle(m_hComm);
		return FALSE;
	}

	dcb.BaudRate = Baud;
//	dcb.fParity  = 0; //Do not actually check the parity bit!
	dcb.fParity  = TRUE; //Enforce the parity bit (mode set below)
	dcb.fAbortOnError = TRUE; //To see the parity errors!
	dcb.StopBits = int(StopValue*2-2);//ONESTOPBIT=0,ONE5=1,TWO=2;
	dcb.ByteSize = BitsValue;//8;
	dcb.fBinary  = 1;
	dcb.fDsrSensitivity = 0;
	dcb.Parity   = (Parity=="Odd")+(Parity=="Even")*2;//NOPARITY=0,ODD=1,EVEN=2;

	if (!SetCommState(m_hComm, &dcb))
	{
		DWORD ErrorCode = GetLastError();
		CloseHandle(m_hComm);
		return FALSE;
	}

	//Set the time-outs
	COMMTIMEOUTS cto;
	GetCommTimeouts(m_hComm, &cto);
	cto.ReadIntervalTimeout        = 50;//Max interval between bytes
	cto.ReadTotalTimeoutConstant   = 200;//Add this constant time to total time-out
	cto.ReadTotalTimeoutMultiplier = 50;//Multiply this time with the number of bytes to read

	if (!SetCommTimeouts(m_hComm, &cto))
	{
		CloseHandle(m_hComm);
		return FALSE;
	}

	//This is done to empty the pipe, which sometimes contains garbage from power off..
	unsigned long nRead;
	ReadFile(m_hComm, &m_CommandArray, 6, &nRead, NULL);


	GetCommTimeouts(m_hComm, &cto);
	cto.ReadIntervalTimeout        = 0;//Max interval between bytes
	cto.ReadTotalTimeoutConstant   = 2000;//Add this constant time to total time-out
	cto.ReadTotalTimeoutMultiplier = 100;//Multiply this time with the number of bytes to read

	if (!SetCommTimeouts(m_hComm, &cto))
	{
		CloseHandle(m_hComm);
		return FALSE;
	}
	m_bIsOpen = TRUE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//Close the serial port
////////////////////////////////////////////////////////////////////////
void CSerialCommand::Close()
{
	assert(m_bIsOpen);

	m_bIsOpen = FALSE;

	CloseHandle(m_hComm);
}

////////////////////////////////////////////////////////////////////////
//Check if the port is open
////////////////////////////////////////////////////////////////////////
bool CSerialCommand::IsOpen()
{
	return m_bIsOpen;
}

//Cancels all Serial Communication:
//Note that this function is not used in this implementation anymore. Its functionality is removed.
void CSerialCommand::KillCurrentCommunication(){

	//Start by setting all timeouts to zero!
/*	COMMTIMEOUTS cto;
	GetCommTimeouts(m_hComm, &cto);
	cto.ReadIntervalTimeout        = 1;//Max interval between bytes
	cto.ReadTotalTimeoutConstant   = 1;//Add this constant time to total time-out
	cto.ReadTotalTimeoutMultiplier = 1;//Multiply this time with the number of bytes to send
	cto.WriteTotalTimeoutConstant = 1;
	cto.WriteTotalTimeoutMultiplier = 1;

	SetCommTimeouts(m_hComm, &cto);

	PurgeComm(m_hComm, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);*/


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
bool CSerialCommand::ApiCaptureImage(bool checkFingerPresent)
{
	DWORD nWritten;
	DWORD nRead=0;
	DWORD nPartRead;

	//CommandStructure: Startbyte 0x02, index 0 , command 0x80, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CAPTURE_IMAGE; //0x80;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	if (checkFingerPresent) m_CommandArray [3] = API_CAPTURE_IMAGE_FINGERPRESENT; 

	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print command data on screen

	while ((nRead<4) && (m_CommandArray[3] != API_CANCEL)) {
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
		{
			return FALSE;
		}
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue

	}

	gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

	return m_Response[1]==API_FINGER_PRESENT;  //0x03

}


/////////////////////////////////////////////////////////////////////
//ApiUploadImage
//Upload image from the image memory to PC..
/////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiUploadImage(unsigned char **image, bool delay, DWORD *imgSize)
{
	DWORD nWritten;
	DWORD nRead;
	
	//CommandBuffer: Startbyte 0x02, no index, command 0x90, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x01; //Will skip the CRC generation if set to 1.
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_UPLOAD_IMAGE; //0x90;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	memset(m_Response, 0x0, 4);

	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);

	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);
	gDlg->m_StatusCtrl.SetWindowText(_T("Status: Image is uploading, please wait."));
	if (m_Response[1] == API_SUCCESS){  //0x01	
		*imgSize = m_Response[2] + m_Response[3] * 256;//Read the image (payload data);
		*image = (unsigned char *)malloc(*imgSize);
		if (ReadFile(m_hComm, *image, *imgSize, &nRead, NULL) == 0)
		{
			return FALSE;
		}
		gDlg->PrintData(*image, nRead, false);
	}
	//Read CRC
	ReadFile(m_hComm, &m_CRC, 4, &nRead, NULL);
	gDlg->PrintData(m_CRC,nRead,false);

	return m_Response [1] == API_SUCCESS; //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiDownloadImage
//Download image from PC to module.. Only relevant for FPC6610 module and FPC2020
//Special restriction for FPC6610: We have to delay between the 6 byte command and the 30400 byte payload.
//This is due to the limited RAM memory in the module..
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiDownloadImage( unsigned char *img, int imgSize, int indexValue, bool delay)
{

	DWORD nWritten;
	DWORD nRead;

	unsigned char payload_msb = imgSize >> 8;        
	unsigned char payload_lsb = imgSize & 0xFF;

	//CommandBuffer: Startbyte 0x02, no index, command 0x91, imgSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0;
	m_CommandArray [2] = 0;
	m_CommandArray [3] = API_DOWNLOAD_IMAGE; //0x91;
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb;
	
	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	Sleep(20);

	if (!WriteFile(m_hComm, img, imgSize, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(img, nWritten, true);//Print sent data bytes to screen

	//Calculate and send the CRC
	unsigned int intCRC = crcCalculate(img, imgSize);
	m_CRC[0]= intCRC; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	if (!WriteFile(m_hComm, m_CRC, 4, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CRC, nWritten, true);//Print sent CRC bytes to screen


	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen

	return m_Response [1] == API_SUCCESS; //0x01
}


/****** Enrol, verify & identify functions ****************************************************************/

/////////////////////////////////////////////////////////////////////
//ApiCaptureEnrolRAM
//function waits for finger present, captures an image, 
//and enrols it into RAM
/////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiCaptureEnrolRAM() 
{
	DWORD nWritten;
	DWORD nRead=0, nPartRead=0;

	//CommandStructure: Startbyte 0x02, no index, command 0x81, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00; 
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CAPTURE_AND_ENROL_RAM; //0x81;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	gDlg->PrintData(m_CommandArray, 6, true);//Print bytes on screen
	//Send command bytes
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}	

	//Read until response is received or the command is canceled.. 
	while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)){ 
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
		{
			return FALSE;
		}
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	gDlg->PrintData(m_Response, nRead, false);

	return m_Response[1]==0x06; //API_ENROL_OK
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiEnrolRAM
//uses the image in RAM, enrols it, and places template in RAM
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiEnrolRAM() 
{
	DWORD nWritten;
	DWORD nRead=0, nPartRead=0;

	//CommandStructure: Startbyte 0x02, index 0 , command 0x85, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_ENROL_RAM; //0x85;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;


	gDlg->PrintData(m_CommandArray, 6, true);//Print command data on screen
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	
	while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)){ //Loop until response is received or the command is canceled
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
		{
			return FALSE;
		}
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

	return m_Response[1]==API_ENROL_OK;  //0x06
}


/////////////////////////////////////////////////////////////////////
// ApiCaptureVerifyRAM
// Waits for finger present, captures an image,
// and verifies it against a template in RAM memory..
/////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiCaptureVerifyRAM()
{

	DWORD nWritten;
	DWORD nRead=0, nPartRead=0;

	//CommandStructure: Startbyte 0x02, no index, command 0x82, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CAPTURE_AND_VERIFY_RAM;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	gDlg->PrintData(m_CommandArray, 6, true);
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	
	while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)){ //Loop until response is received or the command is canceled
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
		{
			return FALSE;
		}
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	gDlg->PrintData(m_Response, nRead, false);

	return m_Response[1]==0x04; //API_VERIFICATION_OK

}


////////////////////////////////////////////////////////////////////////////////////////
//ApiVerifyRAM
//Verifies image in RAM against template in RAM
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiVerifyRAM()
{

	DWORD nWritten;
	DWORD nRead=0;

	//CommandStructure: Startbyte 0x02, idx =0 , command 0x86, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_VERIFY_RAM; //0x86;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print command data on screen

	while ((nRead < 4)){ //Loop until response is received 
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
		{
			return FALSE;
		}
	}
	gDlg->PrintData(m_Response, nRead, false);

	return m_Response[1]==API_VERIFICATION_OK;  //0x04
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiCaptureVerifyFLASH
//Wait for finger present, capture image, and verify against template in FLASH slot
//No time-out, but it can be canceled with the API_CANCEL command
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiCaptureVerifyFLASH(int slotNumber)
{

	DWORD nWritten;
	DWORD nRead=0, nPartRead=0;

	//CommandStructure: Startbyte 0x02, idx_lsb = slotNumber, idx_msb =0 , command 0x83, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slotNumber&0xFF;
	m_CommandArray [2] = slotNumber>>8;
	m_CommandArray [3] = API_CAPTURE_AND_VERIFY_FLASH; //0x83;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	gDlg->PrintData(m_CommandArray, 6, true);//Print command data on screen
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}

	//Loop until response is received or the command is canceled
	while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)){ 		
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
		{
			return FALSE;
		}
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

	return m_Response[1]==0x04;  //API_VERIFICATION_OK
}



////////////////////////////////////////////////////////////////////////////////////////
//ApiVerifyFLASH
//Verify image in RAM against template in FLASH slot
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiVerifyFLASH(int slotNumber)
{

	DWORD nWritten;
	DWORD nRead=0, nPartRead=0;

	//CommandStructure: Startbyte 0x02, idx_lsb = slotNumber, idx_msb =0 , command 0x87, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slotNumber&0xFF;
	m_CommandArray [2] = slotNumber>>8;
	m_CommandArray [3] = API_VERIFY_FLASH; //0x87;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	gDlg->PrintData(m_CommandArray, 6, true);//Print command data on screen
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
		
	//Loop until response is received or the command is canceled
	while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)){ 		
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
		{
			return FALSE;
		}
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

	return m_Response[1]==0x04;  //API_VERIFICATION_OK
}



////////////////////////////////////////////////////////////////////////////////////////
//ApiCaptureIdentify
//Wait for finger present, capture image, identify it against all FLASH slots..
//slotValue will contain the slot were identification succeeded, if any.
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiCaptureIdentify(int * slotValue)
{

	DWORD nWritten;
	DWORD nRead=0, nPartRead=0;	
	unsigned char localBuffer [10];	

	//CommandStructure: Startbyte 0x02, no index, command 0x84, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CAPTURE_AND_IDENTIFY_FLASH; //0x84;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);

	while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)){ //Loop until response is received or the command is canceled
		memset(m_Response, 0x0, 4);
		ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL);
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	gDlg->PrintData(m_Response, nRead, false);

	//Read the slotvalue if any.. (payload data);
	int payload = m_Response[2] + m_Response[3] * 256;
	if (payload > 0 && nRead > 0){
		if (ReadFile(m_hComm, &localBuffer, payload, &nRead, NULL) == 0)
		{
			return FALSE;
		}
		gDlg->PrintData(localBuffer, nRead, false);//print data on screen
		
		//FPC2020 compatible only:
		*slotValue = localBuffer[0]+localBuffer[1]*256; //This contains which slot was found to match during identification
	 
		//Read CRC
		ReadFile(m_hComm, &m_CRC, 4, &nRead, NULL);
		gDlg->PrintData(m_CRC,nRead,false);
	}
	
	return m_Response[1]==API_IDENTIFY_OK; //0x0F
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiIdentify
//Identify image in RAM against all templates in FLASH
//slotValue will contain the slot were identification succeeded, if any.
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiIdentify(int * slotValue)
{
	unsigned char localBuffer [20]; //Too large for normal operation - this is just for FPC debugging.
	DWORD nWritten;
	DWORD nRead=0, nPartRead=0;

	//CommandStructure: Startbyte 0x02, no index, command 0x88, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_IDENTIFY_FLASH; //0x88;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);

	
	while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)){ //Loop until response is received or the command is canceled
		memset(m_Response, 0x0, 4);
		ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL);
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	gDlg->PrintData(m_Response, nRead, false);

	//Read the slot info (payload data);
	int payload = m_Response[2] + m_Response[3] * 256;
	if (payload > 0 && nRead > 0){
		if (ReadFile(m_hComm, &localBuffer, payload, &nRead, NULL) == 0)
		{
			return FALSE;
		}
		gDlg->PrintData(localBuffer, nRead, false);//print data on screen
		
		//FPC2020 compatible only:
		*slotValue = localBuffer[0]+localBuffer[1]*256; //This contains which slot was found to match during identification
		
		//Read CRC
		ReadFile(m_hComm, &m_CRC, 4, &nRead, NULL);
		gDlg->PrintData(m_CRC,nRead,false);

	}
		
	return m_Response[1]==API_IDENTIFY_OK; //0x0F

}



/****** Template handling functions ****************************************************************/


/////////////////////////////////////////////////////////////////////
//ApiUploadTemplate
//Upload template from RAM to PC.. 
/////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiUploadTemplate(unsigned char **templ, int *payload)
{

	DWORD nWritten;
	DWORD nRead;

	//CommandBuffer: Startbyte 0x02, no index, command 0xA0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_UPLOAD_TEMPLATE;//0xA0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	//Read m_Response bytes
	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen

	if ( m_Response[1]==API_SUCCESS){  //0x01
		//Read the template (payload data);
		(*payload)	= m_Response[2] + m_Response[3] * 256;
		(*templ) = (unsigned char *)malloc((*payload));
		if ((*payload) > 0 && nRead > 0 && (*templ) != 0) {
			if (ReadFile(m_hComm, (*templ), (*payload), &nRead, NULL) == 0) {
				return FALSE;
			}
			gDlg->PrintData((*templ), nRead, false);//print template data on screen
			
			//Read CRC
			ReadFile(m_hComm, &m_CRC, 4, &nRead, NULL);
			gDlg->PrintData(m_CRC,nRead,false);
		} else {
			free((*templ));
			return FALSE;
		}
	}	
	return m_Response [1] == API_SUCCESS; //0x01
}



////////////////////////////////////////////////////////////////////////////////////////
//ApiUploadTemplateFLASH
//Upload template from FLASH slot to PC.. 
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiUploadTemplateFLASH(unsigned char **templ, int *payload, int slot)
{
	DWORD nWritten;
	DWORD nRead;

	//CommandBuffer: Startbyte 0x02, slot, 0, command 0xA3, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slot & 0xFF;
	m_CommandArray [2] = slot >> 8;
	m_CommandArray [3] = API_UPLOAD_TEMPLATE_FROM_FLASH; //0xA3;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	//Read m_Response bytes
	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen

	if (m_Response[1] == API_SUCCESS) {  //0x01
		//Read the template (payload data);
		*payload	= m_Response[2] + m_Response[3] * 256;
		*templ		= (unsigned char *)malloc(*payload);
		if (*payload > 0 && nRead > 0 && *templ != 0) {
			if (ReadFile(m_hComm, *templ, *payload, &nRead, NULL) == 0) {
				return FALSE;
			}
			gDlg->PrintData(*templ, nRead, false);//print template data on screen
	
			//Read CRC		
			ReadFile(m_hComm, &m_CRC, 4, &nRead, NULL);
			gDlg->PrintData(m_CRC,nRead,false); 
		} else {
			return FALSE;
		}
	}	
	return m_Response [1] == 0x01; //API_SUCCESS
}

/////////////////////////////////////////////////////////////////////
//ApiDownloadTemplate
//Download template from PC to RAM
/////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiDownloadTemplate( unsigned char *templ, int templSize)
{
	
	DWORD nWritten;
	DWORD nRead;
	unsigned char payload_msb = templSize / 256;        
	unsigned char payload_lsb = templSize - templSize / 256 *256;  

	//CommandBuffer: Startbyte 0x02, no index, command 0xA1, templSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_DOWNLOAD_TEMPLATE; //0xA1; 
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb;
	
	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	//Send the template (payload data);
	if (!WriteFile(m_hComm, templ, templSize, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(templ, nWritten, true);//Print sent data bytes to screen

	//Calculate and send the CRC		
	unsigned int intCRC = crcCalculate(templ, templSize);
	m_CRC[0]= intCRC; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	if (!WriteFile(m_hComm, m_CRC, 4, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CRC, nWritten, true);//Print sent CRC bytes to screen

	//Read m_Response bytes
	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen 

	return m_Response [1] == API_SUCCESS; //0x01
}

bool CSerialCommand::ApiInitiateUpgrade()
{
    DWORD nWritten = 0;
    DWORD nRead = 0, nPartRead = 0;

    m_CommandArray[0] = 0x02;
    m_CommandArray[1] = 0;
    m_CommandArray[2] = 0;
    m_CommandArray[3] = API_FIRMWARE_INITIATE_UPGRADE; //0xC4;
    m_CommandArray[4] = 0x00;
    m_CommandArray[5] = 0x00;

    gDlg->PrintData(m_CommandArray, 6, true);//Print command data on screen
    if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
    {
        return FALSE;
    }

    //Loop until response is received or the command is canceled
    while ((nRead < 4) && (m_CommandArray[3] != API_CANCEL)) {
		memset(m_Response, 0x0, 4);
        if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
        {
            return FALSE;
        }
        nRead += nPartRead;
        { MSG msg; while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) { AfxGetApp()->PumpMessage(); }}//Pump message queue
    }
    gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

    return m_Response[1] == API_SUCCESS;
}

bool CSerialCommand::ApiUpgradeFirmwareBlock( unsigned char *data, int size, int flashSlot)
{
	
	DWORD nWritten;
	DWORD nRead;
	unsigned char payload_msb = size >> 8;	//Should be 1.        
	unsigned char payload_lsb = size & 0xFF;  //Should be 0.

	//CommandBuffer: Startbyte 0x02, no index, command 0xA1, templSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = flashSlot&255;
	m_CommandArray [2] = flashSlot>>8;
	m_CommandArray [3] = API_FIRMWARE_UPDATE;  
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb; 
	

	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	Sleep(20);

	//Send the template (payload data);
	if (!WriteFile(m_hComm, data, size, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(data, nWritten, true);//Print sent data bytes to screen

	//Calculate and send the CRC		
	unsigned int intCRC = crcCalculate(data, size);
	m_CRC[0]= intCRC; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	if (!WriteFile(m_hComm, m_CRC, 4, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CRC, nWritten, true);//Print sent CRC bytes to screen

	//Read m_Response bytes
	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen

	//This is extra added code, in case the upgrade firmware command starts returning information, which is does not do by default...
	int payload = m_Response[2] + m_Response[3] * 256;
	unsigned char buffer[256];
	if (payload > 0 && nRead > 0){
		if (ReadFile(m_hComm, buffer, payload, &nRead, NULL) == 0)
		{
			return FALSE;
		}
		gDlg->PrintData(buffer, nRead, false);//print template data on screen

		//Read CRC		
		ReadFile(m_hComm, &m_CRC, 4, &nRead, NULL);
		gDlg->PrintData(m_CRC,nRead,false); 
	}	//Ok, no payload - that is ok!
	
	return m_Response [1] == API_SUCCESS; //0x01
}

bool CSerialCommand::ApiDownloadProgram(unsigned char *buffer, int size){
	DWORD nWritten;
	DWORD nRead;
	DWORD nPartRead;
	unsigned char payload_msb = size >> 8;	
	unsigned char payload_lsb = size & 0xFF; 
	int totalWritten;

	//CommandBuffer: Startbyte 0x02, no index, command 0xA1, templSize bytes payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0;
	m_CommandArray [2] = 0;
	m_CommandArray [3] = API_RUN_SUPPLIED_PRG; //0xA1;
	m_CommandArray [4] = payload_lsb;
	m_CommandArray [5] = payload_msb;
	

	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	{
		totalWritten = 0;
		while (totalWritten < size){
			if (!WriteFile(m_hComm, buffer+totalWritten, size-totalWritten, &nWritten, NULL))
			{
				return FALSE;
			}
			totalWritten += nWritten;
		}
	}


	//Calculate and send the CRC		//This CRC is actually not checked by the boot code, so it could be any 4 bytes! (There is a CRC-16 built into any supplied program itself!)
	unsigned int intCRC = crcCalculate(buffer, size);
	m_CRC[0]= intCRC; m_CRC[1]=intCRC>>8; m_CRC[2]=intCRC>>16; m_CRC[3]=intCRC>>24;
	if (!WriteFile(m_hComm, m_CRC, 4, &nWritten, NULL))
	{
		return FALSE;
	}
	

	//Read m_Response bytes
	//The response is not immediate - make a couple of attempts until the response is received.
	nRead = 0;
	while (nRead < 4){ 
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nPartRead, NULL) == 0)
		{
			return FALSE;
		}
		nRead += nPartRead;
		{ MSG msg; while(::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {AfxGetApp()->PumpMessage(); }}//Pump message queue
	}
	//No payload possible here - skip handling of that!

	gDlg->PrintData(buffer, totalWritten, true);//Print sent data bytes to screen. (This process takes too much time - so the watchdog will kill the program, if we insert this line in between the actual commandwrites!)
	gDlg->PrintData(m_CRC, nWritten, true);//Print sent CRC bytes to screen	
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen

	return m_Response [1] == API_SUCCESS; //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiCopyTemplateRamToFLASH
//Copy template from RAM to FLASH, using slot information on where to store it
//Be sure that there is a valid template in RAM before calling this function..
//A template is located in RAM after an ENROL call or after API_DOWNLOAD_TEMPLATE
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiCopyTemplateRamToFLASH(int slotNumber)
{
	DWORD nWritten;
	DWORD nRead;

	//CommandStructure: Startbyte 0x02, idx_lsb = slotNumber, idx_msb =0 , command 0xA2, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slotNumber&0xFF;
	m_CommandArray [2] = slotNumber>>8;
	m_CommandArray [3] = API_COPY_TEMPLATE_RAM_TO_FLASH; //0xA2;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print command data on screen

	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

	return m_Response[1]==API_SUCCESS;  //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiDeleteTemplateSlot
//Delete a template in a FLASH slot
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiDeleteTemplateSlot(int slot)
{

	DWORD nWritten;
	DWORD nRead;
	
	//CommandBuffer: Startbyte 0x02, slot, 0, command 0xA5, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = slot&0xFF;
	m_CommandArray [2] = slot>>8;
	m_CommandArray [3] = API_DELETE_SLOT_IN_FLASH; //0xA5;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	//Read m_Response bytes
	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen

	return m_Response [1] == API_SUCCESS; //0x01
}


////////////////////////////////////////////////////////////////////////////////////////
//ApiDeleteAllInFLASH
//Delete ALL templates in FLASH
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiDeleteAllInFLASH()
{

	DWORD nWritten;
	DWORD nRead = 0;	
	
	//CommandBuffer: Startbyte 0x02, 0, 0, command 0xA6, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_DELETE_ALL_IN_FLASH; //0xA6;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	//Send the command data
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print sent bytes to screen

	//Read m_Response bytes. Note that this might take a while.
	while ((nRead < 4)){ //Loop until response is received 
		memset(m_Response, 0x0, 4);
		if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
		{
			return FALSE;
		}
	}	

	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes to screen

	return m_Response [1] == API_SUCCESS; //0x01
}


/************ Application commands **********************************************************/


////////////////////////////////////////////////////////////////////////
//ApiTestHardware
//Send this command to test module hardware and communications..
////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiTestHardware()
{
	DWORD nRead;
	DWORD nWritten;

	//Fill the command byte array
	m_CommandArray [0] = 0x02; //Start byte, always 0x02 
	m_CommandArray [1] = 0x00; //Index value, LSB
	m_CommandArray [2] = 0x00; //Index value, MSB
	m_CommandArray [3] = API_TEST_HARDWARE; //Command byte 0xD2;
	m_CommandArray [4] = 0x00; //Payload counter, LSB
	m_CommandArray [5] = 0x00; //Payload counter, MSB

	
	//Send command bytes
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print bytes on screen

	//Read reply bytes, always at least four..
	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print reply on screen

	return m_Response[1] == API_HW_TEST_OK;  //0x08	

}


/////////////////////////////////////////////////////////////////////
//ApiFirmwareVersion
//Check the application's firmware version
/////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiFirmwareVersion()
{

	unsigned char *Data;
	DWORD nWritten;
	DWORD nRead;
	
	//CommandBuffer: Startbyte 0x02, no index, command 0xC0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_FIRMWARE_VERSION; //0xC0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;

	//Send command buffer
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);  //Print sent bytes on screen

	//Read m_Response buffer
	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response bytes on screen

	if ( m_Response[1]==API_SUCCESS){  //0x01
	
		int payload = m_Response[2] + m_Response[3] * 256; //Check for incomming data
		Data = new unsigned char [payload]; //Allocate memory
		if (ReadFile(m_hComm, Data, payload, &nRead, NULL) == 0) //Read payload data
		{
			return FALSE;
		}
		if (payload) gDlg->PrintData(Data, nRead, false); //Print payload data on screen
		delete [] Data;
	}
	//Read CRC
	ReadFile(m_hComm, &m_CRC, 4, &nRead, NULL);
	gDlg->PrintData(m_CRC,nRead,false);
	return m_Response [1] == API_SUCCESS; //0x01
}


//////////////////////////////////////////////////////////////////////////////////////////////
//ApiSetBaudRateRAM
//Set new baud rate. The unit will respond using the old baud rate, and then change baud rate 
//Index value (idx_lsb) is used to transmit the baud index value..
//The baudrate is set in RAM, and will be valid only until unit power-off or reset
//////////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiSetBaudRateRAM( int baudIndex)
{

	DWORD nWritten;
	DWORD nRead;

	//CommandStructure: Startbyte 0x02, idx_lsb = baudIndex, idx_msb =0 , command 0xD0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = baudIndex;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_SET_BAUD_RATE_RAM;//0xD0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	
	
	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print command data on screen

	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

	return m_Response[1]==0x01;  //API_SUCCESS

}


////////////////////////////////////////////////////////////////////////////////////////
//ApiCancel
//Send cancel to abort the fingerpresent loop in the CaptureEnrol, CaptureVerify and 
//CaptureIdentify functions
////////////////////////////////////////////////////////////////////////////////////////
bool CSerialCommand::ApiCancel()
{

	DWORD nWritten;
	DWORD nRead;	
	
	//CommandStructure: Startbyte 0x02, index =0 , command 0xE0, no payload
	m_CommandArray [0] = 0x02;
	m_CommandArray [1] = 0x00;
	m_CommandArray [2] = 0x00;
	m_CommandArray [3] = API_CANCEL; //0xE0;
	m_CommandArray [4] = 0x00;
	m_CommandArray [5] = 0x00;
	

	if (!WriteFile(m_hComm, m_CommandArray, 6, &nWritten, NULL))
	{
		return FALSE;
	}
	gDlg->PrintData(m_CommandArray, nWritten, true);//Print command data on screen

	memset(m_Response, 0x0, 4);
	if (ReadFile(m_hComm, &m_Response, 4, &nRead, NULL) == 0)
	{
		return FALSE;
	}
	gDlg->PrintData(m_Response, nRead, false);//Print m_Response data on screen

	return m_Response[1]==API_SUCCESS;  //0x01
}
