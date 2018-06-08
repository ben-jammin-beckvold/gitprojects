#!/usr/bin/python3

from serial import *
import time

dev_path = '/dev/ttyUSB'
baud = 115200

time.sleep(1)

for i in range(10):

    ser = Serial(dev_path, baud, timeout=300)
    byte1 = ser.readline()
    
    print(byte1.decode('utf-8'))
    time.sleep(.5)

ser.close()



