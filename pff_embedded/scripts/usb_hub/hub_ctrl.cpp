#include <stdio.h>
#include <string>
#include <unistd.h>
//#include <cstdlib>
#include <libusb.h>

//#include <ros/ros.h>
//#include <ros/init.h>

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

    printf("Dev (bus %d, device %d): %s\n",
           libusb_get_bus_number(dev), libusb_get_device_address(dev), description);

    if (handle) {
        if (desc.iSerialNumber) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string, sizeof(string));
            if (ret > 0)
                printf("- Serial Number: %s\n", string);
        }
    }

    printf("\tVendor ID: %.4x --- Product ID: %.4x\n\n", desc.idVendor, desc.idProduct);
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

    printf("Retrieved Device List...\n");
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
        printf("retVal: %d\n", retVal);
        return SETUP_I2C_FAILED;
    }

    // IO Expander setup
    uint8_t gpioData[3];
    // set all pins low except the reset pin (active low)
    gpioData[0] = OUTPUT_PORT_0;
    gpioData[1] = 0;
    gpioData[2] = RESET_MCU_PIN;
    libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS, 0, gpioData, 3, TIMEOUT);

    // make all pin direction -> output
    gpioData[0] = CONFIG_PORT_0;
    gpioData[1] = 0x00;
    gpioData[2] = 0x00;
    libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS, 0, gpioData, 3, TIMEOUT);


    return 0;
}

void printHelp()
{
    printf("usage:\n");
    printf("    ./setup_target_DFU <path to binary>\n");
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
    sleep(1);
}

void dfu_trigger(std::string binaryName)
{

        // kill the IO-board node
        printf("Killing tiva_C node if ros master is active\n");
        system("rosnode kill /tiva_C");

        std::string scriptString;
        printf("Triggering DFU...\n");

        uint8_t gpioData[2];
        gpioData[0] = OUTPUT_PORT_1;
        gpioData[1] = DFU_TRIGGER_PIN | RESET_MCU_PIN;

        libusb_control_transfer(handle, HOST_TO_DEV_TX, CMD_I2C_WRITE, I2C_CONTROL_FLAG | I2C_ADDRESS | I2C_WRITE_BIT, 0, gpioData, 2, TIMEOUT);

        printf("IO Board delay timer");
        sleep(1);

        printf("Resetting device and loading firmware\n");
        toggleReset();
        scriptString = "./dfu-wrap-load.sh " + binaryName;
        printf("%s\n", scriptString.c_str());
        system(scriptString.c_str());

        sleep(2);

        toggleReset();

        printf("-- DFU done. IO_Board rebooting.\n");
        printf("-- Renegotiating USB\n");
        for (int i =0; i<20; i++)
        {
            printf(".");
            sleep(1);
        }

        printf("Restarting tiva_C node...\n");
        system("rosrun rosserial_python serial_node.py __name:=tiva_C _port:=/dev/pff/io_board");

}

int main(int argc, char *argv[])
{
//    ros::init(argc, argv, "master_test");

    std::string binaryName;
    if (argc > 1)
    {
        if (argc > 2)
        {
            printf("Too Many Arguments");
            printHelp();
            return 0;
        }
        else
        {
            binaryName = argv[1];
        }
    }
    else
    {
        printf("Too few arguments\n");
        printHelp();
        return 0;
    }

    // get USB controller device and open handle
    int ret = getHubControllerDevice();
    if (ret < 0)
    {
        printf("USB device failure: %d\n\n", ret);
        return 0;
    }

    ret = setup_periph();
    if (ret < 0)
    {
        printf("Peripheral setup failure: %d\n\n", ret);
        return 0;
    }


    dfu_trigger(binaryName);


    printf("done--- \n");

    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(NULL);
    printf("\n");
    return 0;
}
