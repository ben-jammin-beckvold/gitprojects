#include <SPI.h>

#define DEBUG

const int slaveSelectPin = 18 ;

/*
TM4C123 Launchpad
SSI Module 2 (SPI.setModule(2))
PB6 - MISO
PB7 - MOSI
PB4 - CLK
PE0 - CS
*/


#define NOP 				0x0000
#define CLEAR_ERROR_FLAG 	0x0001
#define PROGRAMMING_CONTROL 0x0003
//#define OTP_ZERO_HI 		0x0016
//#define OTP_ZERO_LOW 		0x0017
#define DIAG_AGC			0x3FFD
#define MAGNITUDE			0x3FFE
#define ANGLE				0x3FFF

#define READ_BIT			(1 << 14)
#define WRITE_BIT     		0

#define ERROR_BIT_MASK		0x40
#define PARITY_BIT_MASK		0x8000

byte compute_parity(uint16_t packet)
{
	byte count = 0;

	for (int i = 0; i < 15; i++)
	{
		count ^= packet & 1;
		// Serial.print("count: ");
		// Serial.println(packet & 1, BIN);
		packet >>= 1;
	}

	if (count % 2 == 0)
		return 0;
	else
		return 1;
}

uint16_t attach_parity(uint16_t packet)
// if the # of 1s in the lower 15 bits is even, the parity bit is 0
{
	byte parity = compute_parity(packet);

	return packet | (parity << 15);

}

void errorHandler()
// used for debugging
{
	byte returnByte_low, returnByte_high;

	// clear error flags
	uint16_t packet = (READ_BIT | CLEAR_ERROR_FLAG);
	packet = attach_parity(packet);
	byte cmd_high_byte = 0xFF & packet;
	byte cmd_low_byte = 0xFF & (packet >> 8);

	// write command
	digitalWrite(slaveSelectPin, LOW);
	SPI.transfer(cmd_low_byte);
	SPI.transfer(cmd_high_byte);
	digitalWrite(slaveSelectPin, HIGH);

	// READ_ANGLE command
	packet = (READ_BIT | ANGLE);
	packet = attach_parity(packet);
	cmd_high_byte = 0xFF & packet;
	cmd_low_byte = 0xFF & (packet >> 8);

	// shift in READ_ANGLE cmd to shift out error register
	digitalWrite(slaveSelectPin, LOW);
	returnByte_high = SPI.transfer(cmd_low_byte);
	returnByte_low = SPI.transfer(cmd_high_byte);
	digitalWrite(slaveSelectPin, HIGH);

	// error codes are only the bottom 3 bits
	uint16_t rtnData = returnByte_high;
	rtnData = ((rtnData << 8) | returnByte_low);

	if ((rtnData & 0x1) > 0)
		Serial.println("Framing Error");
	if ((rtnData & 0x2) > 0)
		Serial.println("Command Invalid");
	if ((rtnData & 0x4) > 0)
		Serial.println("Parity Error");
}

void setup() {
	Serial.begin(115200);

	SPI.setModule(2);
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE1);				// CPOL = 0, CPHA = 1
	SPI.setClockDivider(SPI_CLOCK_DIV4);	// 2 MHz
	pinMode(slaveSelectPin, OUTPUT);

	digitalWrite(slaveSelectPin, HIGH);

	// clear error flags
	uint16_t packet = (READ_BIT | CLEAR_ERROR_FLAG);
	packet = attach_parity(packet);
	byte cmd_low_byte = 0xFF & packet;
	byte cmd_high_byte = 0xFF & (packet >> 8);

	// write command
	digitalWrite(slaveSelectPin, LOW);
	SPI.transfer(cmd_high_byte);
	SPI.transfer(cmd_low_byte);
	digitalWrite(slaveSelectPin, HIGH);

}


unsigned long last_time = millis();

void loop() {

	byte returnByte_low, returnByte_high;
	uint16_t rtnData_raw, rtnData;

	// setup 16 bit command
	uint16_t packet = (READ_BIT | ANGLE);

	packet = attach_parity(packet);
	byte cmd_low_byte = 0xFF & packet;
	byte cmd_high_byte = 0xFF & (packet >> 8);

	// write command
	digitalWrite(slaveSelectPin, LOW);
	returnByte_high = SPI.transfer(cmd_high_byte);
	returnByte_low = SPI.transfer(cmd_low_byte);
	digitalWrite(slaveSelectPin, HIGH);

	rtnData_raw = returnByte_high;
	rtnData_raw = (rtnData_raw << 8) | returnByte_low;

#ifndef DEBUG
	Serial.write(angle_buf, 2);
#endif

delay(24);	// throttle the output for ~200 samples/s

#ifdef DEBUG
	static int eCount = 0;
	static int dataCount = 0;

	// if (compute_parity(rtnData_raw) == (rtnData_raw & PARITY_BIT_MASK))
	if (true)
	{
		// check the error bit
		// if(false)
		if ((returnByte_high & ERROR_BIT_MASK) == ERROR_BIT_MASK)
		{
			eCount++;
			errorHandler();		// for debugging
		}
		else
		{
			dataCount++;
			// take the lower 14 bits
			rtnData = rtnData_raw & 0x3FFF;
			Serial.println(rtnData, DEC);
		}
	}
	else
		Serial.println("++");
#endif

// debug counting of errors vs good data
#ifdef DEBUG
	if ((millis() - last_time) > 5000)
	{
		last_time = millis();
		Serial.print("DEBUG: ");
		Serial.print(eCount);
		Serial.print(" : ");
		Serial.print(dataCount);
		Serial.print("---\n");
		eCount = 0;
		dataCount = 0;
	}
#endif

}

