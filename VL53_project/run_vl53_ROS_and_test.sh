#!/bin/bash

roslaunch pff_rangefinders test_disconnect.launch&

./vl53_data_to_csv.py
