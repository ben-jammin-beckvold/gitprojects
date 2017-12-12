//g++ fp_scanner.cpp -lftd2xx -lMPSSE -o fp_scanner

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ftd2xx.h"
#include "WinTypes.h"

#define false 0
#define true 1

#define DELAY_LOOP 40000

#define ON 1
#define OFF 0

#define BAUD_RATE 9600
#define ASYNC_BIT_BANG 0x01
#define RESET 0

FT_STATUS ftStatus;
FT_HANDLE ftHandle;

int main()
{
    int deviceIndex = -1;  
    unsigned int numDevs;
    FT_DEVICE_LIST_INFO_NODE * devInfo;

    // create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus == FT_OK)
        printf("Number of FTDI devices: %d\n", numDevs);

    devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs);
    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);

    // display device info
    if (ftStatus == FT_OK) {
        for (unsigned int i = 0; i < numDevs; ++i) {
            printf("Dev %d:\n",i);
            printf(" Flags=0x%x\n",devInfo[i].Flags);
            printf(" Type=0x%x\n",devInfo[i].Type);
            printf(" ID=0x%x\n",devInfo[i].ID);
            printf(" LocId=0x%x\n",devInfo[i].LocId);
            printf(" SerialNumber=%s\n",devInfo[i].SerialNumber);
            printf(" Description=%s\n",devInfo[i].Description);

            if (*devInfo[i].SerialNumber != 0)
            {
                printf("devInfo[i].SerialNumber: %d\n", *devInfo[i].SerialNumber);
                deviceIndex = i;
                printf("Device Index : %d\n", deviceIndex);
            }
        }
    }

#ifndef TOGGLE_TEST
    // open the device
    printf("\nDevice Index: %d\n", deviceIndex);
    if (deviceIndex < 0)
    {
        printf("No valid FTDI device found\n");
        return 0;
    }
    
    ftStatus = FT_Open(deviceIndex, &ftHandle);  
    if (ftStatus != FT_OK) printf("Error opening device: %d\n", ftStatus);
    else printf("Connected to FTDI device, serial number: %s\n\n", devInfo[deviceIndex].SerialNumber);

    // set baud rate
    ftStatus = FT_SetBaudRate(ftHandle, BAUD_RATE);
    if (ftStatus != FT_OK) printf("Error setting baud rate: %d\n", ftStatus);
    else printf("Baud rate set at %d\n", BAUD_RATE);


    // set RTS ON
    ftStatus = FT_SetRts(ftHandle);
    if (ftStatus != FT_OK) printf("Error setting RTS: %d\n", ftStatus);
    else printf("RTS set\n");

    // busy loop for delay
    for (int i =0 ; i < DELAY_LOOP; i++)
    {
        for (int i =0 ; i < DELAY_LOOP; i++)
            int temp = i;
    }

    // set RTS OFF
    ftStatus = FT_ClrRts(ftHandle);
    if (ftStatus != FT_OK) printf("Error clearing RTS: %d\n", ftStatus);
    else printf("RTS cleared\n");

    // --------- //

    // set CTS ON
    ftStatus = FT_SetDtr(ftHandle);
    if (ftStatus != FT_OK) printf("Error setting CTS: %d\n", ftStatus);
    else printf("CTS set\n");

    // busy loop for delay
    for (int i =0 ; i < DELAY_LOOP; i++)
    {
        for (int i =0 ; i < DELAY_LOOP; i++)
            int temp = i;
    }

    // set CTS OFF
    ftStatus = FT_ClrDtr(ftHandle);
    if (ftStatus != FT_OK) printf("Error clearing CTS: %d\n", ftStatus);
    else printf("CTS cleared\n");
    
    // close the device
    printf("END\n");

    ftStatus = FT_Close(ftHandle);
#endif
    
    free(devInfo);
    printf("BREAKING..?\n");
    return 0;
}



/*
 * The FTDI cable has 6 conductors broken out as follows
 * 
 * Red - 5V
 * Black - Ground
 * Green - RTS
 * Brown - CTS
 * Orange - Tx
 * Yellow - Rx
 *
 */


