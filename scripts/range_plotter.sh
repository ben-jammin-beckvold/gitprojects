#!/bin/bash

roslaunch pff_embedded embedded_test.launch simple:=true && rqt_plot /ultrasonic_range
