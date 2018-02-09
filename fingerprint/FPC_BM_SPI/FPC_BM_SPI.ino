#include <SPI.h>

const int slaveSelectPin = 18 ;

// build command: advanced settings (enable UART and Voltage control)
byte command[6] = {0x02, 0xd0, 0, 0xe8, 0, 0};
#define CHECK_READY 0x52
#define DEVICE_BUSY 0x52
#define DEVICE_READY 0x02

void setup() {
  Serial.begin(19200);
  Serial.println("Starting..\n");
  
  SPI.setModule(2);
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  pinMode (slaveSelectPin, OUTPUT);
  SPI.begin();

  byte returnByte;
  digitalWrite(slaveSelectPin,LOW);
  for (int i=0; i< 6;i++)
  {
    SPI.transfer(command[i]);
    Serial.println(command[i], HEX);
  }

  do
  {
  delay(20);
  returnByte = SPI.transfer(CHECK_READY);
  } while (returnByte != DEVICE_READY && false);
  
  for (int i = 0; i < 4; i++)
  {
    Serial.println(SPI.transfer(0), HEX);
  }
  digitalWrite(slaveSelectPin,HIGH);
  SPI.end();
}

void loop() {
  delay(2000);
  Serial.println("Hanging out...");

  digitalWrite(slaveSelectPin,HIGH);

}

