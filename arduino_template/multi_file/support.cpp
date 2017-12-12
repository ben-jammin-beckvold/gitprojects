// source file for arduino sketches
//
#include "support.h"
#include "Arduino.h"

void toggleDelay(int state, int time_ms)
{
    digitalWrite(led, state);
    delay(time_ms);
}


