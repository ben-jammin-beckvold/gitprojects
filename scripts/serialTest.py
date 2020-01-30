#!/usr/bin/python3
# -*- coding: utf-8 -*-
from serial import *
import time

dev_path = '/dev/ttyUSB0'
baud = 115200

time.sleep(1)

ser = Serial(dev_path, baud, timeout=300)

#for i in range(10):
while 1:
    byte1 = ser.read().hex()
    
    print(byte1)
#    print hex(int(byte1.encode('hex'), 16))
    time.sleep(.01)

ser.close()



