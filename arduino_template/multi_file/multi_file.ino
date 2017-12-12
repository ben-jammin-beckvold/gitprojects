#include "support.h"

//const int led = 13;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  toggleDelay(HIGH, DELAY_TIME);   // turn the LED on (HIGH is the voltage level)
 
  toggleDelay(LOW, DELAY_TIME);  // turn the LED on (HIGH is the voltage level)
}


