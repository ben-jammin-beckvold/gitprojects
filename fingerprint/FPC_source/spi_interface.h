#ifndef _SPI_INT_
#define _SPI_INT_


#define CHEETAH_DEFAULT_SPEED	17500	//These numbers define (in ns) the delay between sent/received bytes.
#define CHEETAH_LOW_SPEED		30000
#define CHEETAH_LOWER_SPEED		35000
#define CHEETAH_VERYLOW_SPEED	50000
#define CHEETAH_HIGH_SPEED		4000

#include "cheetah.h"   //This is the TotalPhase's Cheetah SPI Host Interface


class spi_interface{
	private : 
		int opened;
		unsigned long delay1;

		Cheetah m_hCheetah; //A handle to the open Cheetah device!
public :
		spi_interface();
		~spi_interface();
		int Open(int no);
		void Close();
		
		void SetSSNUp();
		void SetSSNDown();

		void ReadData(char * command,unsigned long cnt,char * data, unsigned long dcnt);
		void ReadDataWhenNotBusy(char * command,unsigned long cnt,char * data, unsigned long dcnt);
		bool ReadDataEx(char * data, unsigned long dcnt);

		
		void SendData(char * command,unsigned long cnt,char * data, unsigned long dcnt);
		void SetDelay1(unsigned long del);
		unsigned long GetDelay1();

};


#endif

