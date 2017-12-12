
#include "CheckerboardTest.h"


//Global variables:

//SensorCommunication m_Sensor;
//CSPICommand m_SpiComm;



unsigned char AnalyzeImage(unsigned char *pImage,unsigned int* e_colErrors, unsigned int* e_rowErrors, unsigned int* e_pixelErrors, unsigned int* e_neighErrors)
{
	//Calculate Median for the different Sub-Areas (5 rows, all cols, black and white separate).
	//5*152/2 = 380
	/* Definition: When there is an even number of numbers, the median is the mean of the two middle numbers.
	Thus, the median of the numbers 2, 4, 7, 12 is (4+7)/2 = 5.5. 
	*/

	int Hist1 [256];
	int Hist2 [256];
	memset(Hist1,0,256*sizeof(int));
	memset(Hist2,0,256*sizeof(int));
	double Median1 [40];
	double Median2 [40];
	int SubNumber=0;
	int ValuesBelow1=0;
	int ValuesBelow2=0;
	int High1,Low1,High2,Low2;
	int row,col;
	bool Hist1Done=false,Hist2Done=false;
	int TotalErrors=0;


	//loop through all sub-areas
	for (SubNumber=0;SubNumber<40;SubNumber++){
		Hist1Done=false;
		Hist2Done=false;
		memset(Hist1,0,256*sizeof(int));
		memset(Hist2,0,256*sizeof(int));
		//Loop through 5 rows
		for ( row=0;row<5;row++){
			//Loop through all cols
			for ( col=0;col<152;col=col+2){
				if (pImage[(SubNumber*5+row)*152+col] < 0 || pImage[(SubNumber*5+row)*152+col] > 255){
					int stop2=2;
					return IMAGE_CAPTURE_ERROR; //This should not happen
				}
				if (pImage[(SubNumber*5+row)*152+col+1] < 0 || pImage[(SubNumber*5+row)*152+col] > 255){
					int stop2=2;
					return IMAGE_CAPTURE_ERROR; //This should not happen
				}
				//Create Histograms
				//Even rows start with white..
				if ((SubNumber*5+row)%2){
					Hist1[pImage[(SubNumber*5+row)*152+col]]++;
					Hist2[pImage[(SubNumber*5+row)*152+col+1]]++;
				}else{
					Hist2[pImage[(SubNumber*5+row)*152+col]]++; //Reversed pattern on every other row..
					Hist1[pImage[(SubNumber*5+row)*152+col+1]]++;
				}
			}
		}

		//Check that histogram is full, otherwise something is wrong
		//for (int k=0;k++;k<256)
		//Find median values
		ValuesBelow1=0;
		ValuesBelow2=0;
		for (int i=0;i<256;i++){
			ValuesBelow1+=Hist1[i];
			ValuesBelow2+=Hist2[i];
			//For first histogram
			if (ValuesBelow1 >= 190 && !Hist1Done){
				if (ValuesBelow1 == 190){
					Low1=i;
					//Find next value in histogram
					for (int j=1;j<256-i;j++){
						if (Hist1[i+j]){
							High1=i+j;
							break;
						}
					}
				}else{
					Low1=i;
					High1=i;
				}
				Hist1Done=true;
			}
			//For second histogram
			if (ValuesBelow2 >= 190 && !Hist2Done){
				if (ValuesBelow2 == 190){
					Low2=i;
					//Find next value in histogram
					for (int j=1;j<256-i;j++){
						if (Hist2[i+j]){
							High2=i+j;
							break;
						}
					}
				}else{
					Low2=i;
					High2=i;
				}
				Hist2Done=true;
			}
		}

	

		//Now we have the values for the sub-area
		Median1[SubNumber]=(Low1+High1)/2;
		Median2[SubNumber]=(Low2+High2)/2;
	}

	//OK

	//Ok, now we have the medians.. compare all pixels in each sub-area to those pixels..




	for (SubNumber=0;SubNumber<40;SubNumber++){
		if (((Median1[SubNumber] <= 128) && (Median2[SubNumber] <= 128)) || ((Median1[SubNumber] >= 128) && (Median2[SubNumber] >= 128))){
			//Something is very messed up in this SubArea. Constant image?
			//m_rowErrors = m_colErrors = 255;	//Just to signal an error value, if the results are retrieved later.
			return SENSOR_FAIL;
		}
	}

	


	//Make an image which is zero if no error, 1 if pixelerror is detected..
	unsigned char ErrorImage [152*200];
	memset(ErrorImage,0,152*200*sizeof(char));
	int pixel=0;
	const int medianThreshold = 20; // = 0x14 HEX!!!
	
	for (row=0;row<200;row++){
		SubNumber = row/5;
		for (col=0;col<152;col=col+2){
			
			if (row%2){
				pixel=pImage[row*152+col];//white pixels
				if ((pixel > Median1[SubNumber]+medianThreshold) || ( pixel < Median1[SubNumber]-medianThreshold)){
					ErrorImage[row*152+col]=1;
					TotalErrors++;
				}
				pixel=pImage[row*152+col+1];//black pixels
				if ((pixel > Median2[SubNumber]+medianThreshold) || ( pixel < Median2[SubNumber]-medianThreshold)){
					ErrorImage[row*152+col+1]=1;
					TotalErrors++;
				}
			}else{
				pixel=pImage[row*152+col];//black pixels
				if ((pixel > Median2[SubNumber]+medianThreshold) || ( pixel < Median2[SubNumber]-medianThreshold)){
					ErrorImage[row*152+col]=1;
					TotalErrors++;
				}
				pixel=pImage[row*152+col+1];//white pixels
				if ((pixel > Median1[SubNumber]+medianThreshold) || ( pixel < Median1[SubNumber]-medianThreshold)){
					ErrorImage[row*152+col+1]=1;
					TotalErrors++;
				}
			}
		}
	}


	//Now we have created an image where 1 is error and 0 is ok..
	//
	//1: Start with checking for row errors
	bool RowError[200];
	bool ColError[152];
	int Sum=0;
	int ColErrors=0;
	
	int RowErrors=0;
	for (row=0;row<200;row++){
		for (col=0;col<152;col++){
			Sum += ErrorImage[row*152+col];
		}
		//If Sum larger than 20% (0.2*152=30) we have a colError
		if (Sum > 30){
			RowErrors++;
			RowError[row]=true;
		}else{
			RowError[row]=false;
		}

		
		Sum=0;
	}


	//2: Check for col errors
	Sum=0;
	
	for (col=0;col<152;col++){
		for (row=0;row<200;row++){		
			Sum += ErrorImage[row*152+col];
		}
		//If Sum larger than 20% (0.2*200=40) we have a colError
		if (Sum > 40){
			ColErrors++;
			ColError[col]=true;
		}else{
			ColError[col]=false;
		}
		Sum=0;
	}

	
	//3: Check for neighborhood errors (more than 4 errors in a 12x12 neighborhood)
	//Also count total errorpixels, excluding row and col error pixels
	Sum=0;
	int NeighErrors=0;
	for (row=0;row<200-11;row++){
		for (col=0;col<152-11;col++){
			
			for (int i=0;i<12;i++){
				for (int j=0;j<12;j++){
					Sum += ErrorImage[(row+i)*152+col+j];
				}
			}
			//We'll accept 4 errors in a neighborhood
			if (Sum>4){
				NeighErrors++;
			}
		
			Sum=0;
		}
	}
	

	//Export errors:
	*e_colErrors = ColErrors;
	*e_rowErrors = RowErrors;
	*e_pixelErrors = TotalErrors;
	*e_neighErrors = NeighErrors;



	if (TotalErrors > 60 || RowErrors + ColErrors > 0 || NeighErrors > 0){
		return SENSOR_FAIL;
	}else{
		return SENSOR_OK;
	}

			
}


unsigned char CaptureAndAnalyzeCBImage(CSPICommand* spi,unsigned char* pImage,unsigned char usePIF,unsigned int* colErrors, unsigned int* rowErrors, unsigned int* pixelErrors, unsigned int* neighErrors){
	//This piece of code should capture, and analyze a checkerboard image from a FPC1011C sensor,
	//using the Cheetah SPI interface.
	//The image will be placed in the (already allocated) pImage memory pointer, A NULL ptr is allowed.
	//The return value indicates the result of the overall test.
	//Note that it requires the connection to the Cheetah module already to exist!
	//The parameters ADCRef and DrivC aretaken from the Params.txt file. (which has to follow a special format)

	
	int NrOfPix=152*200;
	int IsImage=0;
	int result;
	unsigned int i;

	//Read the data from the file parameters,txt
	//It must contain the entries 'ADCRef=XXX', 'DrivC=XXX', and "SenseMode=XXX" where XXX are the desired values.
	int ADCRef = 3; //Default value.
	int DrivC = 70; //Default value.
	int SenseMode = 1; //Default value

	//The SensorCommunication Class reserves the memory for the memory! Just catch the pointer to the image!
	//unsigned char* tmpImage;
	unsigned char tmpImage[152*200];

		
	bool ok;
		
	ok = spi->ApiSetADCREF_RAM((unsigned char) ADCRef);
		
	if(ok)
		ok = spi->ApiSetDRIVC_RAM((unsigned char) DrivC);
		
	if(ok)
		ok = spi->ApiSetSenseMode_RAM((unsigned char) SenseMode);

	if(!ok)
		return FPC2020_CONNECTION_FAILURE;


	if(usePIF)
	{
		
		if(ok){
			ok = spi->ApiCaptureImage_PIF();
		}
	
	}else{

		if(ok){
			ok = spi->ApiCaptureImage();
		}
	}

	if(!ok)
		return SENSOR_CONNECTION_FAILURE;

	if(ok)
		ok = spi->ApiUploadImage(tmpImage);
		

	if(!ok)
		return IMAGE_UPLOAD_ERROR;

				
	for (i=0;i<152*200;i++){
		IsImage+=tmpImage[i]; 
	}

	if (IsImage> 0 && IsImage < 200*152*255){
	
		result = AnalyzeImage(tmpImage,colErrors,rowErrors,pixelErrors,neighErrors);
		
		memcpy(pImage, tmpImage, NrOfPix);


	}else{
		result =  8;//SENSOR_CONNECTION_FAILURE;
	}


	return result;
	
}

unsigned char CaptureAndAnalyzeInvCBImage(CSPICommand* spi,unsigned char* pImage,unsigned char usePIF,unsigned int* colErrors, unsigned int* rowErrors, unsigned int* pixelErrors, unsigned int* neighErrors){
	//This piece of code should capture, and analyze an inverted checkerboard image from a FPC1011C sensor,
	//using the Cheetah SPI interface.
	//The code is virtually identical to the regular CB image.
	//The image will be placed in the (already allocated) pImage memory pointer, A NULL ptr is allowed.
	//The return value indicates the result of the overall test.
	//Note that it requires the connection to the Cheetah module already to exist!
	//The parameters ADCRef and DrivC aretaken from the Params.txt file. (which has to follow a special format)

	
	int NrOfPix=152*200;
	int IsImage=0;
	int result;
	unsigned int i;

	//Read the data from the file parameters,txt
	//It must contain the entries 'ADCRef=XXX', 'DrivC=XXX', and "SenseMode=XXX" where XXX are the desired values.
	int ADCRef = 3; //Default value.
	int DrivC = 70; //Default value.
	int SenseMode = 2; //Default value

	//The SensorCommunication Class reserves the memory for the memory! Just catch the pointer to the image!
	//unsigned char* tmpImage;
	unsigned char tmpImage[152*200];

	
	bool ok;
		
	ok = spi->ApiSetADCREF_RAM((unsigned char) ADCRef);
		
	if(ok)
		ok = spi->ApiSetDRIVC_RAM((unsigned char) DrivC);
		
	if(ok)
		ok = spi->ApiSetSenseMode_RAM((unsigned char) SenseMode);

	if(!ok)
		return FPC2020_CONNECTION_FAILURE;


	if(usePIF)
	{
		if(ok){
			ok = spi->ApiCaptureImage_PIF();
		}
	
	}else{

		if(ok){
			ok = spi->ApiCaptureImage();
		}
	}

	if(!ok)
		return SENSOR_CONNECTION_FAILURE;

	if(ok)
		ok = spi->ApiUploadImage(tmpImage);
		

	if(!ok)
		return IMAGE_UPLOAD_ERROR;

	
	for (i=0;i<152*200;i++){
		IsImage+=tmpImage[i]; 
	}

	if (IsImage> 0 && IsImage < 200*152*255){
		
		result =  AnalyzeImage(tmpImage,colErrors,rowErrors,pixelErrors,neighErrors);	
		memcpy(pImage, tmpImage, NrOfPix); 
	
	}else{
		result = 8;//SENSOR_CONNECTION_FAILURE;
	}



	return result;
	
}