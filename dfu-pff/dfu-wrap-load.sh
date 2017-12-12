#!/bin/bash

if [ "$1" == "" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ] ; then
    echo
    echo "This script wraps the DFU suffix and prefix for the Tiva TM4C123"
    echo "used with the pff embedded IO board"
    echo
    echo "dfu-wrap-load.sh [ file ]" #[ start address ] "
    echo
    echo "      -binary file: name of the binary in bin/ directory"
    echo
#    echo "      -start address: if not specified it's assumed to be 0"
#    echo
    exit 0
else
    FILE="$1"
fi

sudo chmod 666 $FILE

./dfu-suffix -p 0x00ff -v 0x1bce -d 0x0000 -s 0x0000 -a $FILE

./dfu-util -D $FILE -R
