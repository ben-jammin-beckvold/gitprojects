#ifndef hub_ctrl
#define hub_ctrl


#include <cstring>
#include <vector>
#include <stdlib.h>

/*
   typedef int bool;
#define true 1
#define false 0
*/

// register access
#define CMD_REG_READ 	0x04
#define CMD_REG_WRITE 	0x03

#define DEV_TO_HOST_TX 	0xc1
#define HOST_TO_DEV_TX 	0x41

// I2C
#define CMD_I2C_READ 	0x72
#define CMD_I2C_WRITE 	0x71
#define I2C_PASSTHRU	0x70

#define SEND_NACK (1 << 2)
#define SEND_START (1 << 1)
#define SEND_STOP (1 << 0)
#define I2C_CONTROL_FLAG ((SEND_START | SEND_STOP | SEND_NACK) << 8)
//#define I2C_ADDRESS 	0x75 << 1 // 7 bit address
#define I2C_ADDRESS 	0x77 << 1 // 7 bit address   // default address for IO expander -- testing only
#define I2C_READ_BIT 	1
#define I2C_WRITE_BIT   0

#define I2C_BUS_FREQ 	0x0080	// 223.2kHz

/**
 *    * Registers used for configuration, setting output levels, and reading input levels
 *       */
constexpr uint8_t  INPUT_PORT_0    = 0x00; // read inputs
constexpr uint8_t  INPUT_PORT_1    = 0x01;
constexpr uint8_t OUTPUT_PORT_0    = 0x02; // control outputs, read current output levels
constexpr uint8_t OUTPUT_PORT_1    = 0x03;
constexpr uint8_t    INV_PORT_0    = 0x04; // invert the configured polarity on a pin
constexpr uint8_t    INV_PORT_1    = 0x05;
constexpr uint8_t CONFIG_PORT_0    = 0x06; // configure a pin as an input or output
constexpr uint8_t CONFIG_PORT_1    = 0x07;

// pins on the IO Expander, not the hub native GPIO
constexpr uint8_t DFU_TRIGGER_PIN   = (1 << 7);  // port 1, pin 7. DFU pin on the MCU
constexpr uint8_t RESET_MCU_PIN     = (1 << 6);  // port 1, pin 6. reset pin on the MCU

// constexpr int32_t DFU_ACK_CODE      = 112233;

// GPIO
#define GPIO_PIN_69	(1 << 4)	// DFU erase
#define GPIO_PIN_77	(1 << 5)	// M4 reset

#define GPIO_64_71_PU	0x097F
#define GPIO_64_71_DIR	0x0973
#define GPIO_64_71_OUT	0x0977

#define GPIO_PIN_0 0x1
#define GPIO_PIN_1 0x2
#define GPIO_PIN_2 0x4
#define GPIO_PIN_3 0x8
#define GPIO_PIN_4 0x10
#define GPIO_PIN_5 0x20
#define GPIO_PIN_6 0x40
#define GPIO_PIN_7 0x80

// error codes
#define DEVICE_NOT_FOUND 		-1
#define FAILED_TO_OPEN 			-2
#define CLAIM_FAILED			-3
#define DEVICE_LIST_NOT_FOUND	-4
#define FAILED_TO_GET_DESCRIPTOR -5
#define FAILED_TO_INIT			-6
#define SETUP_I2C_FAILED		-7


#endif
