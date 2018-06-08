/*
 * Simple I2C example
 *
 * Copyright 2017 Joel Stanley <joel@jms.id.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// io expander definitions
const uint8_t IOEXP_ADDR[8] = { 
    0x20, 0x21, 0x22, 0x23,     // front
    0x24, 0x25, 0x26, 0x27 };   // rear

const uint8_t IOEXP_CONFIG_REG = 0x03;
const uint8_t IOEXP_OUTPUT_REG = 0x01;
const uint8_t ALL_OUTPUT = 0x00;
const uint8_t ALL_LOW = 0x00;
const uint8_t ALL_HIGH = 0xFF;

// VL53L0X definitions
const uint8_t RANGER_ADDR[8] = { 
    0x30, 0x31, 0x32, 0x33,     // front
    0x34, 0x35, 0x36, 0x37 };   // rear

const uint8_t DFLT_RANGER_ADDR = 0x52;
const uint8_t TEST_REG[3] = {0xC0, 0xC1, 0xC2};
const uint8_t TEST_REG_VALUE[3] = {0xEE, 0xAA, 0x10};

static inline __s32 i2c_smbus_access(int i2cBus, char read_write, __u8 command,
        int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(i2cBus,I2C_SMBUS,&args);
}

static inline __s32 i2c_smbus_write_byte_data(int i2cBus, __u8 command, 
        __u8 value)
{
    union i2c_smbus_data data;
    data.byte = value;
    return i2c_smbus_access(i2cBus,I2C_SMBUS_WRITE,command,
            I2C_SMBUS_BYTE_DATA, &data);
}

static inline __s32 i2c_smbus_read_byte_data(int i2cBus, __u8 command)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(i2cBus,I2C_SMBUS_READ,command,
                I2C_SMBUS_BYTE_DATA,&data))
        return -1;
    else
        return 0x0FF & data.byte;
}

int main(int argc, char **argv)
{
//    VL53L0X rangerDev;
    const char *path = "/dev/i2c-1";
    int i2cBus, rc;

    i2cBus = open(path, O_RDWR);
    if (i2cBus < 0)
        err(errno, "Tried to open '%s'", path); 


    // set all IO expander pins to output
    // set all IO expander pins to 0 (XSHUT off)
    for (int i = 0; i < NUMRANGERS; i++)
    { 
        rc = ioctl(i2cBus, I2C_SLAVE, IOEXP_ADDR[i]);
        if (rc < 0)
            err(errno, "Tried to access device address '0x%02x'", IOEXP_ADDR[i]);

        i2c_smbus_write_byte_data(i2cBus, IOEXP_CONFIG_REG, ALL_OUTPUT);
        i2c_smbus_write_byte_data(i2cBus, IOEXP_OUTPUT_REG, ALL_LOW);

        // NOTE::: may need to separate front and rear ranger loops if 2 busses are used
    }

    // sequence each ranger for initialization
    for (int i = 0; i < NUMRANGERS; i++)
    {
        // open connection to IO expander
        rc = ioctl(i2cBus, I2C_SLAVE, IOEXP_ADDR[i]);
        if (rc < 0)
            err(errno, "Tried to access device address '0x%02x'", IOEXP_ADDR[i]);

        // turn on XSHUT
        i2c_smbus_write_byte_data(i2cBus, IOEXP_OUTPUT_REG, ALL_HIGH);
        
        // open connection to default ranger. there should only be one on with the default address at this point.
        rc = ioctl(i2cBus, I2C_SLAVE, DFLT_RANGER_ADDR);
        if (rc < 0)
            err(errno, "Tried to access device address '0x%02x'", DFLT_RANGER_ADDR);
        
        // test registers on VL53 to ensure connection
        for (int k = 0; k < 3; k++)
        {
            uint8_t data = i2c_smbus_read_byte_data(i2cBus, TEST_REG[k]);
            if (data != TEST_REG_VALUE[k])
            {
                printf("Test register:[%x] value does not match: Returned:%0x/Expected:%0x\n", TEST_REG[k], data, TEST_REG_VALUE[k]);
                return -1;
            }

            printf("Test register:[%x]/Returned:%0x/Expected:%0x\n", TEST_REG[k], data, TEST_REG_VALUE[k]);
        }
        

        // initialize the VL53
//        VL53L0X_DataInit();
    }

    //data = i2c_smbus_read_byte_data(i2cBus, reg);

    //printf("%s: device 0x%02x at address 0x%02x: 0x%02x\n", path, addr, reg, regValue);

}

