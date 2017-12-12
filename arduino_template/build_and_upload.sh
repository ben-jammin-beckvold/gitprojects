#!/bin/bash
# This file is covered by the LICENSE file in the root of this project.
# Copyright (C) 2017 Piaggio Fast Forward.

#---------------------------------------------------#
#  Install dependencies
#---------------------------------------------------#
ino_installed=$(pip list | grep -F ino)
if [ "${#ino_installed}" -eq "0" ]; then
  pip install ino
fi

#---------------------------------------------------#
#  ARDUINO CONFIG
#---------------------------------------------------#
BOARD_MODEL=mega2560

PORT=/dev/pff/mega2560

if [ $# -eq 2 ]; then
  BOARD_MODEL=$1
  PORT=$1
fi

# check if serial ids set
#if [ ! -e $PORT ]; then
#  echo "Port [$PORT] is not connected."
#  exit 0
#fi

#---------------------------------------------------#
#  PATHS
#---------------------------------------------------#
PKG_DIR='~/Documents/arduino_test/multi_file'
FW_DIR=$PKG_DIR

# paths - source
PROJECT_NAME=multi_file
PROJECT_DIR=$FW_DIR

# paths - build
#SCRIPTS_DIR=$PKG_DIR/scripts
INO_DIR=$FW_DIR/.ino-build
DST=$INO_DIR/$PROJECT_NAME
#ROS_LIB_DIR=$DST/lib/ros_lib
PWD=`pwd`

if [ ! -d $INO_DIR ]; then
  mkdir -v $INO_DIR;
fi

printf "\nPROJECT: [$PROJECT_NAME]\n\n"
printf "Hardware:\n"
printf "\t- PORT: [$PORT]\n"
printf "\t- BOARD: [$BOARD_MODEL]\n"
printf "\n"
sleep 1

#---------------------------------------------------#
#  SETUP DIRECTORIES
#---------------------------------------------------#
# if not empty, then remove all files
if [ "$(ls -A $INO_DIR)" ]; then
  printf "\n\t[$INO_DIR] is not empty, remove all files.\n"
  rm -r $INO_DIR/*
fi

# setup build directory structure
if [ ! -d $DST  ]; then
  mkdir -v -p $DST
  mkdir -v $DST/src
  mkdir -v $DST/lib
fi

# copy source files
cp -v $PROJECT_DIR/*.ino $DST/src

#---------------------------------------------------#
#  BUILD & UPLOAD
#---------------------------------------------------#
cd $DST;
#ino list-models
ino clean
ino build -m $BOARD_MODEL
#ino upload -m $BOARD_MODEL -p $PORT
# ino serial -p $PORT
printf "\n\tuploaded [$PROJECT_DIR].\n"
cd $PWD
