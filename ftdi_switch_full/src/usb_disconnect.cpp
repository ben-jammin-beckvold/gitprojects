//g++ fp_scanner.cpp -lftd2xx -lMPSSE -o fp_scanner

#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
#include "pff_usb_disconnect/ftd2xx.h"
#include "pff_usb_disconnect/WinTypes.h"

#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include "pff_usb_disconnect/switchStatus.h"

#define BAUD_RATE 9600

ros::Duration retryWait(1);

FT_STATUS ftStatus;
FT_HANDLE ftHandle;

std_msgs::Bool usbSwitch1;
std_msgs::Bool usbSwitch2;
pff_usb_disconnect::switchStatus switchStatusMsg;

ros::Publisher usb_switch_status;

int setupFTDIDevice()
{
        int deviceIndex = -1;
        unsigned int numDevs;
        FT_DEVICE_LIST_INFO_NODE * devInfo;

        // create the device information list
        ftStatus = FT_CreateDeviceInfoList(&numDevs);
        if (ftStatus == FT_OK)
                ROS_DEBUG("Number of devices: %d\n", numDevs);

        devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs);
        ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);

        // display device info
        if (ftStatus == FT_OK) {
                for (int i = 0; i < numDevs; i++) {
                        ROS_DEBUG("Dev %d:\n",i);
                        ROS_DEBUG(" Flags=0x%x\n",devInfo[i].Flags);
                        ROS_DEBUG(" Type=0x%x\n",devInfo[i].Type);
                        ROS_DEBUG(" ID=0x%x\n",devInfo[i].ID);
                        ROS_DEBUG(" LocId=0x%x\n",devInfo[i].LocId);
                        ROS_DEBUG(" SerialNumber=%s\n",devInfo[i].SerialNumber);
                        ROS_DEBUG(" Description=%s\n",devInfo[i].Description);

                        // check if the serial number == 0, that means that the driver didn't unbind and it's not the correct FTDI
                        if (*devInfo[i].SerialNumber != 0)
                        {
                                ROS_DEBUG("devInfo[i].SerialNumber: %d\n", *devInfo[i].SerialNumber);
                                deviceIndex = i;
                                ROS_DEBUG("Device Index : %d\n", deviceIndex);
                        }
                }
        }

        // open the device
        printf("\nDevice Index: %d\n", deviceIndex);
        if (deviceIndex < 0)
        {
                ROS_ERROR("No valid FTDI device found");
                return 0;
        }

        ftStatus = FT_Open(deviceIndex, &ftHandle);


        if (ftStatus != FT_OK)
        {
                ROS_ERROR("Error opening device: %d\n", ftStatus);
                return ftStatus;
        }
        else ROS_INFO("Connected to FTDI device, serial number: %s\n\n", devInfo[deviceIndex].SerialNumber);

        // set baud rate
        ftStatus = FT_SetBaudRate(ftHandle, BAUD_RATE);
        if (ftStatus != FT_OK)
        {
                ROS_ERROR("Error setting baud rate: %d\n", ftStatus);
                return ftStatus;
        }
        else ROS_DEBUG("Baud rate set at %d\n", BAUD_RATE);

        // initialize RTS and DTR to `ON`
        ftStatus = FT_SetRts(ftHandle);
        if (ftStatus != FT_OK) ROS_ERROR("Error setting RTS: %d\n", ftStatus);
        else ROS_DEBUG("RTS set\n");

        ftStatus = FT_SetDtr(ftHandle);
        if (ftStatus != FT_OK) ROS_ERROR("Error setting DTR: %d\n", ftStatus);
        else ROS_DEBUG("DTR set\n");

        // set initial state
        switchStatusMsg.usbSwitch1 = true;
        switchStatusMsg.usbSwitch2 = true;
        usb_switch_status.publish(switchStatusMsg);

        free(devInfo);

        return ftStatus;
}

void deviceReconnect()
{

}

void usb_device1_cb(const std_msgs::Bool &usbSwitch1)
{
        if (usbSwitch1.data == true)
        {
                // set RTS ON
                ftStatus = FT_SetRts(ftHandle);
                if (ftStatus != FT_OK)
                        ROS_ERROR("Error setting RTS: %d\n", ftStatus);
                else
                {
                        ROS_DEBUG("RTS set\n");
                        switchStatusMsg.usbSwitch1 = true;
                }
        }
        else
        {
                // set RTS OFF
                ftStatus = FT_ClrRts(ftHandle);
                if (ftStatus != FT_OK)
                        ROS_ERROR("Error clearing RTS: %d\n", ftStatus);
                else
                {
                        ROS_DEBUG("RTS cleared\n");
                        switchStatusMsg.usbSwitch1 = false;
                }
        }

        usb_switch_status.publish(switchStatusMsg);


}

void usb_device2_cb(const std_msgs::Bool &usbSwitch2)
{
        if (usbSwitch2.data == true)
        {
                // set DTR ON
                ftStatus = FT_SetDtr(ftHandle);
                if (ftStatus != FT_OK)
                        ROS_ERROR("Error setting DTR: %d\n", ftStatus);
                else
                {
                        ROS_DEBUG("DTR set\n");
                        switchStatusMsg.usbSwitch2 = true;
                }
        }
        else
        {
                // set DTR OFF
                ftStatus = FT_ClrDtr(ftHandle);
                if (ftStatus != FT_OK)
                        ROS_ERROR("Error clearing DTR: %d\n", ftStatus);
                else
                {
                        ROS_DEBUG("DTR cleared\n");
                        switchStatusMsg.usbSwitch2 = false;
                }
        }

        usb_switch_status.publish(switchStatusMsg);


}

int main(int argc, char** argv)
{
        ros::init(argc, argv, "usb_disconnect_node");
        ros::NodeHandle nh;

        usb_switch_status = nh.advertise<pff_usb_disconnect::switchStatus>("usb_disconnect_switch_status", 1, true);

        ros::Subscriber usb_disconnect_device1 = nh.subscribe("usb_disconnect_device1", 10, usb_device1_cb);
        ros::Subscriber usb_disconnect_device2 = nh.subscribe("usb_disconnect_device2", 10, usb_device2_cb);

        while (setupFTDIDevice() != FT_OK)
        {
                ROS_ERROR("Could not setup FTDI device for USB_Disconnect. Retrying...");
                retryWait.sleep();
        }

        ROS_INFO("FTDI USB switch setup complete");


        ros::Rate loopRate(10);

        while(nh.ok())
        {
                ros::spinOnce();
                loopRate.sleep();

        }


        ftStatus = FT_Close(ftHandle);

        return 0;
}
