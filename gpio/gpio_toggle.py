#!/usr/bin/env python3

import RPi.GPIO as GPIO
import time


GPIO.setmode(GPIO.BCM)
LED = 17
GPIO.setup(LED, GPIO.OUT)

for i in range(0, 5):
  GPIO.output(LED, True)
  time.sleep(.5)
  GPIO.output(LED, False)
  time.sleep(.5)

GPIO.cleanup(LED)

