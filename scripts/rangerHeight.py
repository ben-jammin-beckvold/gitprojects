#!/usr/bin/python3

import math

degree_sign= u'\N{DEGREE SIGN}'

fov_deg = 12.5
fov_rad = math.radians(fov_deg)

upward_angle_deg = 0
maxRange = 1000 # mm
upward_angle_rad = math.radians(upward_angle_deg)

parallelHeight = maxRange * math.sin(fov_rad+upward_angle_rad)

for upward_angle_deg in range(0,8,1):
    upward_angle_rad = math.radians(upward_angle_deg)
    minimumHeight = (maxRange * math.sin(fov_rad - upward_angle_rad))
    print ("Angle from parallel: {}{}".format(upward_angle_deg, degree_sign))
    print ("minimum height: {0:.0f}mm".format(minimumHeight))
    print (" ")

