
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "serial/serial.h"

//using enums from the serial library
#define BAUD 115200
#define PORT "/dev/launchpadEncoder"
#define STOPBITS stopbits_one
#define PARITY parity_none
#define BYTESIZE eightbits

#define TWOBYTEPACKET 2

using namespace serial;

Serial encSerial;

void initLaunchpad()
{
	Timeout to(serial::Timeout::simpleTimeout(500));
	encSerial.setTimeout(to);
	encSerial.setBaudrate(BAUD);
	encSerial.setPort(PORT);
	encSerial.setStopbits(STOPBITS);
	encSerial.setParity(PARITY);
	encSerial.setBytesize(BYTESIZE);
}

void launchpadConnect()
{

	try
	{
		encSerial.open();

		if (encSerial.isOpen())
		{
			encSerial.flush();
			printf("Launchpad connected: %s", PORT);
			return;
		}
		else
		{
			printf("Bad Connection with serial port. error: %s", PORT);
			return;
		}
	}
	catch (IOException) { }
	catch (SerialException) { }

	printf("Serial IO Exception caught. Exiting 'false'.");
}

int main ()
{
	int rtnSize;
	uint8_t rtnBuffer[2] = {0, 0};
	uint16_t angle = 0;

	initLaunchpad();
	launchpadConnect();

	while (1)
	{

		rtnSize = encSerial.read(rtnBuffer, TWOBYTEPACKET);

		if (rtnSize != TWOBYTEPACKET)
		{
			printf("not enough bytes read\n");
		}

		if (true)
		{
			angle = rtnBuffer[1];
			angle = (angle << 8) | rtnBuffer[0];
			printf("angle: %d\n", angle);
			// printf("rtnBuffer: %X : %X\n", rtnBuffer[1], rtnBuffer[0]);
			usleep(1000);
//				encSerial.flush();
		}

	}


	return 0;
}

