//g++ fp_scanner.cpp -lftd2xx -lMPSSE -o fp_scanner

#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "ftd2xx.h"
#include "WinTypes.h"

#define BAUD_RATE 9600

FT_STATUS ftStatus;
FT_HANDLE ftHandle;

std::fstream stateFile ("switchState.txt", std::fstream::in | std::fstream::out);

int setupFTDIDevice()
{
        int deviceIndex = -1;
        unsigned int numDevs;
        FT_DEVICE_LIST_INFO_NODE * devInfo;

        // create the device information list
        ftStatus = FT_CreateDeviceInfoList(&numDevs);
        if (ftStatus == FT_OK)
        {
#ifdef DEBUG
                printf("Number of devices: %d\n", numDevs);
#endif
        }

        devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs);
        ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);

        // display device info
        if (ftStatus == FT_OK) {
                for (int i = 0; i < numDevs; i++) {
#ifdef DEBUG
                        printf("Dev %d:\n",i);
                        printf(" Flags=0x%x\n",devInfo[i].Flags);
                        printf(" Type=0x%x\n",devInfo[i].Type);
                        printf(" ID=0x%x\n",devInfo[i].ID);
                        printf(" LocId=0x%x\n",devInfo[i].LocId);
                        printf(" SerialNumber=%s\n",devInfo[i].SerialNumber);
                        printf(" Description=%s\n",devInfo[i].Description);
#endif
                        // check if the serial number == 0, that means that the driver didn't unbind and it's not the correct FTDI
                        if (*devInfo[i].SerialNumber != 0)
                        {
#ifdef DEBUG
                                printf("devInfo[i].SerialNumber: %s\n", devInfo[i].SerialNumber);
                                printf("Device Index: %d\n", deviceIndex);
#endif
                                deviceIndex = i;
                        }
                }
        }

        // open the device
#ifdef DEBUG
        printf("\nDevice Index: %d\n", deviceIndex);
#endif
        if (deviceIndex < 0)
        {
                printf("No valid FTDI device found\n");
                return -1;
        }

        ftStatus = FT_Open(deviceIndex, &ftHandle);


        if (ftStatus != FT_OK)
        {
                printf("Error opening device: %d\n", ftStatus);
                return ftStatus;
        }
#ifdef DEBUG
        else printf("Connected to FTDI device, serial number: %s\n\n", devInfo[deviceIndex].SerialNumber);
#endif

        // set baud rate
        ftStatus = FT_SetBaudRate(ftHandle, BAUD_RATE);
        if (ftStatus != FT_OK)
        {
                printf("Error setting baud rate: %d\n", ftStatus);
                return ftStatus;
        }
#ifdef DEBUG
        else printf("Baud rate set at %d\n", BAUD_RATE);

        // initialize RTS and DTR to `ON`
        ftStatus = FT_SetRts(ftHandle);
        if (ftStatus != FT_OK) printf("Error setting RTS: %d\n", ftStatus);
        else printf("RTS set\n");

        ftStatus = FT_SetDtr(ftHandle);
        if (ftStatus != FT_OK) printf("Error setting DTR: %d\n", ftStatus);
        else printf("DTR set\n");
#endif

        free(devInfo);

        return ftStatus;
}

void usb_device1_cb(bool switchState)
{
        // SetRts is OFF, ClrRts is ON
        if (switchState)
        {
                // set RTS ON
                ftStatus = FT_ClrRts(ftHandle);
                if (ftStatus != FT_OK)
                        printf("Error setting RTS: %d\n", ftStatus);
                else
                {
                        printf("Port 1 ON\n");
                        stateFile << switchState;
                }
        }
        else
        {
                // set RTS OFF
                ftStatus = FT_SetRts(ftHandle);
                if (ftStatus != FT_OK)
                        printf("Error clearing RTS: %d\n", ftStatus);
                else
                {
                        printf("Port 1 OFF\n");
                        stateFile << switchState;
                }
        }
}

void usb_device2_cb(bool switchState)
{
    // I don't know why this operate backwards
        if (switchState)
        {
                // set DTR ON
                ftStatus = FT_ClrDtr(ftHandle);
                if (ftStatus != FT_OK)
                        printf("Error setting DTR: %d\n", ftStatus);
                else
                {
                        printf("Port 2 ON\n");
                }
        }
        else
        {
                // set DTR OFF
                ftStatus = FT_SetDtr(ftHandle);
                if (ftStatus != FT_OK)
                        printf("Error clearing DTR: %d\n", ftStatus);
                else
                {
                        printf("Port 2 OFF\n");
                }
        }

}

int usage()
{
        printf("Usage: usbSwitch [port] [state]\n");
        printf("  -port:\t1/2\n");
        printf("  -state:\t0/1, OFF/ON\n\n");
        return 0;
}

int main(int argc, char** argv)
{
        int port, state;
        bool b_state;

        // Get input args
        if (argc > 3 || argc < 3)
                return usage();
        else if (argc == 3)
        {
                port = *argv[1] - '0';
                if (!(port == 1 || port == 2))
                {
#ifdef DEBUG
                        printf("Wrong port #\n");
#endif
                        return usage();
                }

                state = *argv[2] - '0';
                if (state == 0) b_state = false;
                else if (state == 1) b_state = true;
                else
                {
#ifdef DEBUG
                        printf("Wrong state\n");
#endif
                        usage();
                        return 0;
                }
        }

        // setup FTDI Device
        if (setupFTDIDevice() != FT_OK)
        {
                return 0;
        }
#ifdef DEBUG
        printf("FTDI USB switch setup complete\n");
#endif

        // get switch state from file
        int port1State, port2State;
        stateFile >> port1State >> port2State;

        if (port == 1)
                usb_device1_cb(b_state);
        else
                usb_device2_cb(b_state);

        ftStatus = FT_Close(ftHandle);
        stateFile.close();

        return 0;
}
