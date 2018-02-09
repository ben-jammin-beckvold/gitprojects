#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libusb.h>

#include <ros/ros.h>
#include <ros/console.h>

#include <std_msgs/Bool.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include "pff_embedded/dfu.h"
#include "hub_ctrl.h"

#define USB_PORT_DISABLE 0x300b
#define ENABLE 1

#define TIMEOUT 500    // .5 sec

libusb_device_handle *handle = NULL;
libusb_device * i2cBridge;

static int printDeviceInfo()
{
    libusb_device* dev = i2cBridge;
    struct libusb_device_descriptor desc;
    char description[256];
    unsigned char string[256];
    int ret, i;

    ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
        return FAILED_TO_GET_DESCRIPTOR;
    }

    if (LIBUSB_SUCCESS == ret) {
        if (desc.iManufacturer) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string, sizeof(string));
            if (ret > 0)
                snprintf(description, sizeof(description), "%s - ", string);
            else
                snprintf(description, sizeof(description), "%04X - ",
                        desc.idVendor);
        }
        else
            snprintf(description, sizeof(description), "%04X - ",
                    desc.idVendor);

        if (desc.iProduct) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
            if (ret > 0)
                snprintf(description + strlen(description), sizeof(description) -
                        strlen(description), "%s", string);
            else
                snprintf(description + strlen(description), sizeof(description) -
                        strlen(description), "%04X", desc.idProduct);
        }
        else
            snprintf(description + strlen(description), sizeof(description) -
                    strlen(description), "%04X", desc.idProduct);
    }
    else {
        snprintf(description, sizeof(description), "%04X - %04X",
                desc.idVendor, desc.idProduct);
    }

    ROS_INFO("Dev (bus %d, device %d): %s\n",
            libusb_get_bus_number(dev), libusb_get_device_address(dev), description);
    //printf("Dev (bus %d, device %d): %s\n",
    //        libusb_get_bus_number(dev), libusb_get_device_address(dev), description);

    if (handle) {
        if (desc.iSerialNumber) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string, sizeof(string));
            if (ret > 0)
            {
                ROS_DEBUG("- Serial Number: %s\n", string);
                //printf("- Serial Number: %s\n", string);
            }
        }
    }

    ROS_DEBUG("\tVendor ID: %.4x --- Product ID: %.4x\n", desc.idVendor, desc.idProduct);
    //printf("\tVendor ID: %.4x --- Product ID: %.4x\n", desc.idVendor, desc.idProduct);
}

int getHubControllerDevice()
{
    libusb_device **devs;
    struct libusb_device_descriptor i2cDevice;

    bool bHubFound = false;

    if (libusb_init(NULL) < 0)
        return FAILED_TO_INIT;

    if (libusb_get_device_list(NULL, &devs) < 0)
    {
        return DEVICE_LIST_NOT_FOUND;
    }

    ROS_DEBUG("Retrieved Device List...\n");
    //printf("Retrieved Device List...\n");
    for (int i = 0; devs[i]; ++i)
    {
        // get the device descriptor for each device and check if it's the hub controller
        libusb_get_device_descriptor(devs[i], &i2cDevice);
        if (i2cDevice.idVendor == 0x0424 && i2cDevice.idProduct == 0x2840)
        {
            i2cBridge = devs[i];
            bHubFound = true;
            break;
        }

    }
    // release full list, only keeping the controller device
    libusb_free_device_list(devs, 1);
    if (!bHubFound)
    {
        return DEVICE_NOT_FOUND;
    }

    // open device and get handle
    int ret = libusb_open(i2cBridge, &handle);
    if (ret != LIBUSB_SUCCESS)
    {
        return FAILED_TO_OPEN;
    }

    printDeviceInfo();

    libusb_set_auto_detach_kernel_driver(handle, ENABLE);
    ret = libusb_claim_interface(handle, 0);
    if (ret != LIBUSB_SUCCESS)
    {
        return CLAIM_FAILED;
    }
    return 0;
}

int setup_periph()
{
    // config I2C Bus - do once
    int retVal = libusb_control_transfer(handle, HOST_TO_DEV_TX, I2C_PASSTHRU, 0, I2C_BUS_FREQ, NULL, 0, TIMEOUT);
    if (retVal < 0)
    {
        ROS_DEBUG("I2C configure fail: %d\n", retVal);
        //printf("I2C configure fail: %d\n", retVal);
        return SETUP_I2C_FAILED;
    }

    // IO Expander setup
    uint8_t gpioData[3];
    // set all pins low except the reset pin (active low)
    gpioData[0] = OUTPUT_PORT_0;
    gpioData[1] = 0x00;
    gpioData[2] = 0x00 | RESET_MCU_PIN;

    // set direction to output
    libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS | I2C_WRITE_BIT, 0, gpioData, 3, TIMEOUT);

    // set direction
    gpioData[0] = CONFIG_PORT_0;
    gpioData[1] = 0;
    gpioData[2] = 0;
    libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS | I2C_WRITE_BIT, 0, gpioData, 3, TIMEOUT);

    return 0;
}

void toggleReset()
{
    uint8_t gpioData[2];
    gpioData[0] = OUTPUT_PORT_1;
    gpioData[1] = ~RESET_MCU_PIN;
    libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS | I2C_WRITE_BIT, 0, gpioData, 2, TIMEOUT);
    gpioData[0] = OUTPUT_PORT_1;
    gpioData[1] = RESET_MCU_PIN;
    libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS | I2C_WRITE_BIT, 0, gpioData, 2, TIMEOUT);
}

/*
void dfu_reset_cb(const pff_embedded::dfu::ConstPtr &dfu_msg)
{

        uint8_t gpioData[2];
        gpioData[0] = OUTPUT_PORT_1;
        gpioData[1] = DFU_TRIGGER_PIN | RESET_MCU_PIN;

        libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS | I2C_WRITE_BIT, 0, gpioData, 2, TIMEOUT);

        ROS_WARN("IO Board delay timer");

        ROS_WARN("Resetting device and loading firmware");
        //printf("Resetting device and loading firmware");
        toggleReset();

        dfuScript.data = "cd $(rospack find pff_embedded)/scripts/ && ./dfu-wrap-load.sh ";
        dfuScript.data = dfuScript.data + dfu_msg->binary_name;
        cmd_runner.publish(dfuScript);

        toggleReset();

        ROS_WARN("DFU done. boxed checked");
}
*/

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "pff_USB_hub_controller_node");
    ros::NodeHandle nh;

    ros::Rate loop_rate(20);

    // get USB controller device and open handle
    int ret = getHubControllerDevice();
    if (ret < 0)
    {
        ROS_ERROR("USB device failure: %d\n", ret);
        ROS_FATAL("---- USB HUB likely not connected or powered ----");
        return -1;
    }

    ret = setup_periph();
    if (ret < 0)
    {
        ROS_DEBUG("Peripheral setup failure: %d\n", ret);
        return -1;
    }

    if(nh.ok())
    {
        ros::spinOnce();
    }

    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(NULL);
    return 0;
}
