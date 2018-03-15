#!/usr/bin/python

import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 4800, timeout = 5)
time.sleep(1)

while True:
    line = ser.readline()
    splitline = line.split(',')
    
    if splitline[0] == '$GPGGA':
        latitude = splitline[2]
        latDirec = splitline[3]
        longitude = splitline[4]
        longDirec = splitline[5]
        print 'lat: ' + latitude
        print 'latDir: ' + latDirec
        print 'long: ' + longitude
        print 'longDirec: ' + longDirec + '\n'
#        break

