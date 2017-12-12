//g++ fp_scanner.cpp -lftd2xx -lMPSSE -o fp_scanner

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/time.h>
#include "ftd2xx.h"
//#include "pff_mpsse.h"
#include "libMPSSE_spi.h"

#define BUF_SIZE 8

#define false 0
#define true 1

#define SPI_XFER_OPTIONS (SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE)

uint8  bufWrite[BUF_SIZE];
uint  bytesWritten;
uint8  bufRead[BUF_SIZE];
uint  bytesRead;

FT_STATUS ftStatus;
FT_HANDLE ftHandle;

void print_return(uint numBytes, uint8 *buff)
{
  for (uint8 i = 1; i < numBytes; ++i) {

    printf("%02X", buff[i]);
  }
  printf("\n");

}

uint oscillator_calibration()
{
  // calibrate oscillator
  int writeSize = 16;
  bufWrite[0] = 0x3c;
  bufWrite[1] = 0;
  bufWrite[2] = 0;
  bufWrite[3] = 0;
  bufWrite[4] = 0;
  bufWrite[5] = 0;
  bufWrite[6] = 0;
  bufWrite[7] = 0;
  bufWrite[8] = 0;
  bufWrite[9] = 0;
  bufWrite[10] = 0;
  bufWrite[11] = 0;
  bufWrite[12] = 0;
  bufWrite[13] = 0;
  bufWrite[14] = 0;
  bufWrite[15] = 0;
  printf("Calibrating Oscillator\n");
  SPI_Write(ftHandle, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);

  // read calibration register
  writeSize = 5;
  bufWrite[0] = 0x7c;
  bufWrite[1] = 0;
  bufWrite[2] = 0;
  bufWrite[3] = 0;
  bufWrite[4] = 0;
  printf("Oscillator calibration results: ");
  SPI_ReadWrite(ftHandle, bufRead, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
  print_return(bytesWritten, bufRead);
  return bytesWritten;
}

void config_reader()
{
  printf("------Config-------\n");
  // Finger Drive (0x8c 0x02)
  int writeSize = 2;
  bufWrite[0] = 0x8c;
  bufWrite[1] = 0x30;
  SPI_Write(ftHandle, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);

  // Finger detection threshold (0xd8)
  writeSize = 2;
  bufWrite[0] = 0xd8;
  bufWrite[1] = 0xff;
  SPI_Write(ftHandle, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);

//  oscillator_calibration();

  printf("\\------Config-------\n\n");
}

uint read_INT_CLEAR()
{
  // Read Int w/ clear (0x1c)
  int writeSize = 2;
  bufWrite[0] = 0x1c;
  bufWrite[1] = 0;
  printf("Read Int w/ clear: ");
  SPI_ReadWrite(ftHandle, bufRead, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
  print_return(bytesWritten, bufRead);
  return bytesWritten;
}

uint read_INT()
{
  // Read Int w/o clear (0x18)
  int writeSize = 2;
  bufWrite[0] = 0x18;
  bufWrite[1] = 0;
  printf("Read Int w/o clear: ");
  SPI_ReadWrite(ftHandle, bufRead, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
  print_return(bytesWritten, bufRead);
  return bytesWritten;
}

uint finger_present_status()
{
  // finger present status (0xd4 0x00)
  int writeSize = 3;
  bufWrite[0] = 0xd4;
  bufWrite[1] = 0;
  bufWrite[2] = 0;
  printf("Finger present status: ");
  SPI_ReadWrite(ftHandle, bufRead, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
  print_return(bytesWritten, bufRead);
  return bytesWritten;
}

void soft_reset()
{
  // soft reset (0xf8)
  int writeSize = 1;
  bufWrite[0] = 0xf8;
  printf("\n--Soft Reset--\n");
  SPI_Write(ftHandle, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
}

uint get_HW_ID()
{
  // HW ID
  int writeSize = 3;
  bufWrite[0] = 0xfc;
  bufWrite[1] = 0;
  bufWrite[2] = 0;
  printf("hardware ID (020A): ");
  SPI_ReadWrite(ftHandle, bufRead, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
  print_return(bytesWritten, bufRead);
  return bytesWritten;
}

void query_finger_present()
{
  // Finger present query (0x20)
  int writeSize = 1;
  bufWrite[0] = 0x20;
  printf("Query finger present\n");
  SPI_Write(ftHandle, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
}

void wait_for_finger()
{
  // wait for finger present (0x24)
  int writeSize = 1;
  bufWrite[0] = 0x24;
  printf("wait for finger...\n");
  SPI_Write(ftHandle, bufWrite, writeSize, &bytesWritten, SPI_XFER_OPTIONS);
}


int main()
{

  FT_DEVICE_LIST_INFO_NODE * devInfo;
  devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE));

  ChannelConfig channelConf;
  Init_libMPSSE();

  channelConf.ClockRate = 8000000;
  channelConf.LatencyTimer = 100;
  channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
  channelConf.Pin = 0x00000804;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

  ftStatus = SPI_GetChannelInfo(0, &devInfo[0]);
  printf(" SerialNumber=%s\n", devInfo[0].SerialNumber);
  printf(" Description=%s\n\n", devInfo[0].Description);

  ftStatus = SPI_OpenChannel(0, &ftHandle);
  if (ftStatus != FT_OK) printf("Error SPI Open: %d\n", ftStatus);


  ftStatus = SPI_InitChannel(ftHandle, &channelConf);
  if (ftStatus != FT_OK) printf("Error SPI Init: %d\n", ftStatus);

  get_HW_ID();

  soft_reset();
  printf("\n");

  read_INT_CLEAR();
  config_reader();

  read_INT();
  sleep(1);

  read_INT_CLEAR();

  finger_present_status();
  read_INT();

//  wait_for_finger();
  query_finger_present();

  finger_present_status();
  sleep(3);
  soft_reset();
  read_INT();
  finger_present_status();

//  printf("\n---Loop----\n");
  for (int i = 0 ; i < 3; i++)
  {

  }

  printf("\n");
  ftStatus = FT_Close(ftHandle);

  return 0;
}

