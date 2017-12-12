

#ifndef _CB_TEST_
#define _CB_TEST_


//Constants
#define SENSOR_OK					0
#define SENSOR_CONNECTION_FAILURE   1
#define SENSOR_FAIL					2
#define USB_ERROR					3
#define IMAGE_CAPTURE_ERROR			4
#define CHEETAH_OPEN_ERROR			5
#define FPC2020_CONNECTION_FAILURE	6
#define IMAGE_UPLOAD_ERROR			7

//Switches
#define DUMP_STATISTICS				0

//Includes
//#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SPICommands.h"

//Functions
unsigned char AnalyzeImage(unsigned char *pImage,unsigned int* e_colErrors, unsigned int* e_rowErrors, unsigned int* e_pixelErrors,unsigned int* e_neighErrors);

unsigned char CaptureAndAnalyzeCBImage(CSPICommand* spi, unsigned char* pImage,unsigned char usePIF,unsigned int* colErrors, unsigned int* rowErrors, unsigned int* pixelErrors,unsigned int* neighErrors);

unsigned char CaptureAndAnalyzeInvCBImage(CSPICommand* spi,unsigned char* pImage,unsigned char usePIF,unsigned int* colErrors, unsigned int* rowErrors, unsigned int* pixelErrors,unsigned int* neighErrors);

#endif