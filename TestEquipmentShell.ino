#include <SPI.h>
#include <SD.h>
#include "TimerOne.h"
#include "string.h"

#define SDSLAVESELECT 4
#define LEDG1 5
#define LEDG2 6
#define LEDR 7
#define CURRENTPIN 0
String analogValue = "";

#define ANALOGVALUES 1024
#define VMAX 20
int analogReading = 0;

byte ms = 0; // milisecond counter
byte cs = 0; // centisecond counter
byte ds = 0; // decasecond counter
byte s = 0; // second counter

char fileName[] = "XXXX0000.CSV";

File dataFile ;
void timerCounter(void){
  ms++;
}

void setup() {
  pinMode(LEDG1, OUTPUT);
  pinMode(LEDG2, OUTPUT);
  pinMode(LEDR, OUTPUT);
  Serial.begin(9600);
  if (!SD.begin(SDSLAVESELECT)){
    Serial.println("Card Read error");
    digitalWrite(LEDR, HIGH);
    while(1){}
  }
  dataFile = SD.open(fileName, FILE_WRITE);
  Timer1.initialize(10000); // 1kHz interrupt cycle
  Timer1.attachInterrupt(timerCounter);
}

void InitializeNewFile(char *fileName){
  while (SD.exists(fileName)){
    if (fileName[8] != '9') fileName[8]++;
    else {
      fileName[8] = '0';
      if (fileName[7] != '9') fileName[7]++;
      else {
        fileName[7] = '0';
        if (fileName[6] != '9') fileName[6]++;
        else {
          fileName[6] = '0';
          if (fileName[5] != '9') fileName[5]++;
          else {
            Serial.print("Clear the SD card, 9999 files is too many");
            digitalWrite(LEDR, HIGH);
            while (1){}
          }
        }
      }
    }
  }
  return;
}

int AnalogToVoltageDC(int analogReading){
  float smallestUnit = VMAX/ANALOGVALUES;
  return(analogReading*smallestUnit);
}

void loop() {
  //loop for thigns occuring @ 100Hz
  if (ms >9){
    analogReading = analogRead(CURRENTPIN);
    analogReading = AnalogToVoltageDC(analogReading);
    analogValue = String(analogReading);
    dataFile.print(analogValue);
    dataFile.print(',');
    cs ++;
    ms = 0 ;
  }
  //loop for 10HZ
  if (cs > 9){
    ds++;
    cs = 0;
  }
  // loop for 1 Hz 
  if (ds >10){
    s++;
    ds = 0;
  }
  
}
