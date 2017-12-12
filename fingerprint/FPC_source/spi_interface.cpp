//#include "stdafx.h"
//#include <windows.h>

#include "spi_interface.h"

#define BUSY 0x52


unsigned char m_tempBuffer[30400]; //Large enough to handle anything being sent.


spi_interface::spi_interface(){ //Constructor
	opened=0;
	delay1=95000; //Very large value.
}
spi_interface::~spi_interface(){
}
int spi_interface::Open(int no){

	int size = 5;
	unsigned short devices[5];

	if (!cheetah_find_devices(size, devices)){
		opened = 0;
		return 0; //No cheetah devices was found. Error.
	}
	//We know that the function returned a non-zero value.
	//We only have one cheetah attached, so let's pick that one.
	m_hCheetah = cheetah_open(devices[0]);
	if (m_hCheetah < 0){
		opened = 0;
		return 0; //Something went wrong. Error.
	}
	//Next, configure the device for proper polarity and speed
	cheetah_spi_configure(m_hCheetah, CHEETAH_SPI_POL_FALLING_RISING,
							CHEETAH_SPI_PHASE_SETUP_SAMPLE,
							CHEETAH_SPI_BITORDER_MSB, 0x00);
	cheetah_spi_bitrate(m_hCheetah, 2500); //800kHz is the lowest bitrate possible for the Cheetah.
	//Enable the outputs of the SPI device:
	cheetah_spi_queue_clear(m_hCheetah);
	cheetah_spi_queue_oe(m_hCheetah, 0x01);
	cheetah_spi_batch_shift(m_hCheetah, 0, NULL);
	//Use one of the delay member variables as our intrabyte delay (in ns)
	delay1 = CHEETAH_DEFAULT_SPEED; 

	//Set the ChipSelect signal inactive.
	SetSSNUp();

	opened=1;
	return 1;
}
void spi_interface::Close(){
	//Set the ChipSelect signal inactive to indicate that we now are not using the SPI interface (UART communication is now possible)!
	SetSSNUp();
	//Disable the outputs of the SPI device:
	cheetah_spi_queue_clear(m_hCheetah);
	cheetah_spi_queue_oe(m_hCheetah, 0x00);
	cheetah_spi_batch_shift(m_hCheetah, 0, NULL);
	//Close device connection
	cheetah_close(m_hCheetah);
	opened=0;
}



void spi_interface::SetSSNUp(){ //Disable
	//1. Clear the queue
	cheetah_spi_queue_clear(m_hCheetah);
	//2. Add the Chip Select signal to the queue.
	cheetah_spi_queue_ss(m_hCheetah, 0x00);
	//3. Send this command:
	cheetah_spi_batch_shift(m_hCheetah, 0, NULL);
}
void spi_interface::SetSSNDown(){ //Enable
	//1. Clear the queue
	cheetah_spi_queue_clear(m_hCheetah);
	//2. Add the Chip Select signal to the queue.
	cheetah_spi_queue_ss(m_hCheetah, 0x01);
	//3. Send this command:
	cheetah_spi_batch_shift(m_hCheetah, 0, NULL);
}

void spi_interface::ReadData(char * command,unsigned long cnt,char * _data, unsigned long dcnt){
	unsigned long i;
	unsigned int length;
	int result;
	//Send the command, consisting of cnt bytes:
	SetSSNDown();	//chipselect low
	//1. Clear the queue
	cheetah_spi_queue_clear(m_hCheetah);
	//2. Add our command-bytes to the queue, one byte at a time, with a delay between each byte
	for (i=0; i<dcnt; i++){
		cheetah_spi_queue_byte(m_hCheetah, 1, BUSY); //What we send over does not matter.
		cheetah_spi_queue_delay_ns(m_hCheetah, delay1); 
	}
	//3. Send the queued data (which also returns data to read)!
	length = cheetah_spi_batch_length(m_hCheetah);
	result = cheetah_spi_batch_shift(m_hCheetah, length, (unsigned char*)_data);
	SetSSNUp();	//chipselect high
}

//This function is similar to ReadDataEx,
//in that it waits for a non-busy 0x02 response, before it 
//starts collecting data into the recieve buffer.
//The difference from ReadDataEx is that this function waits in a
//neverending loop, until the response byte is received.
void spi_interface::ReadDataWhenNotBusy(char * command,unsigned long cnt,char * _data, unsigned long dcnt){
	unsigned long i=0;
	unsigned int length;
	int result;
	SetSSNDown();	//chipselect low
	//1. Clear the queue
	cheetah_spi_queue_clear(m_hCheetah);
	//2. Add a single busy signal (+delay) to the queue:
	cheetah_spi_queue_byte(m_hCheetah, 1, BUSY); //send our busy-signal byte.
	cheetah_spi_queue_delay_ns(m_hCheetah, delay1); 
	//3. OK, now we have prepared a single byte read.
	//Let's repeat this dcnt times, and if we get the response we want,
	//we continue to read the complete response (of length dcnt).
	length = cheetah_spi_batch_length(m_hCheetah);
	while (1){	//Wait in this loop for the actual transmission to start!
		//No problems to repeat the batch-shift, since the queue is intact.	
		result = cheetah_spi_batch_shift(m_hCheetah, length, (unsigned char*)_data);
//		i++;
//		if (command) { 
//			command[i] = _data[0]; //DEBUG ONLY: Store the received busy bytes for debugging purposes!
//		}
		if (_data[0] == (char)0x02){ //OK, we just received the first part of the Response!
//			if (command) command[0] = i; //DEBUG ONLY: Store away the number of bytes actually collected, until the real anwer appeared.
			break;
		}
	}
	//Now, take home the last dcnt-1 bytes! (by reading one byte at a time - we are in no hurry (otherwise, we could change the queue, and read all bytes at once...)
	for (i=1;i<dcnt;i++){	//Now, read the rest of the bytes!
		result = cheetah_spi_batch_shift(m_hCheetah, length, (unsigned char*)(_data+i));
	} 
	SetSSNUp();	//chipselect high
}

//This function reads the given number of bytes, and checks if any of the received
//bytes is 0x02 (our start signal).
//If not, then return false (and the buffer is not interesting)
//If yes, the response buffer is filled, with the first value being 0x02!
//It is to be used repeadedly in cases where we are waiting for a response!
bool spi_interface::ReadDataEx(char * _data, unsigned long dcnt){
	unsigned long i;
	unsigned int length;
	unsigned long counter = dcnt;
	int result;
	SetSSNDown();	//chipselect low
	//1. Clear the queue
	cheetah_spi_queue_clear(m_hCheetah);
	//2. Add a single busy signal (+delay) to the queue:
	cheetah_spi_queue_byte(m_hCheetah, 1, BUSY); //Send our busy-signal byte.
	cheetah_spi_queue_delay_ns(m_hCheetah, delay1); //Defines the overall bitrate.
	//3. OK, now we have prepared a single byte read.
	//Let's repeat this dcnt times, and if we get the response we want,
	//we continue to read the complete response (of length dcnt).
	length = cheetah_spi_batch_length(m_hCheetah);
	while (counter--){	//Wait in this loop for the actual transmission to start!
		//No problems to repeat the batch-shift, since the queue is intact.	
		result = cheetah_spi_batch_shift(m_hCheetah, length, (unsigned char*)_data);
		if (_data[0] == (char)0x02){ //OK, we just received the first part of the Response!
			goto readRealResponse;
		}
	}
	//If we got down here, nothing interesting happened. Shut down
	SetSSNUp();	//chipselect high
	return false;

readRealResponse:
	//Take home the last dcnt-1 bytes! (by reading one byte at a time - we are in no hurry (otherwise, we could change the queue, and read all bytes at once...)
	for (i=1;i<dcnt;i++){	//Now, read the rest of the bytes!
		result = cheetah_spi_batch_shift(m_hCheetah, length, (unsigned char*)(_data+i));
	} 
	SetSSNUp();	//chipselect high
	return true;
}


void spi_interface::SendData(char * command,unsigned long cnt,char * _data, unsigned long dcnt){
	unsigned long i;
	unsigned int length;
	int result;
	//Send the command, consisting of cnt bytes:
	SetSSNDown();	//chipselect low
	//1. Clear the queue
	cheetah_spi_queue_clear(m_hCheetah);
	//2. Add our command-bytes to the queue, one byte at a time, with a delay between each byte
	for (i=0; i<cnt; i++){
		cheetah_spi_queue_byte(m_hCheetah, 1, command[i]);
		cheetah_spi_queue_delay_ns(m_hCheetah, delay1); //Corresponds to a rate of 100kbps.
	}
	//3. Send the queued data!
	length = cheetah_spi_batch_length(m_hCheetah);
	result = cheetah_spi_batch_shift(m_hCheetah, length, m_tempBuffer);
	SetSSNUp();	//chipselect low
}


void spi_interface::SetDelay1(unsigned long del){
	delay1=del;
}
unsigned long spi_interface::GetDelay1(){
	return delay1;
}



