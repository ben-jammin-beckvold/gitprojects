#!/bin/bash

if [ "$1" == "" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ] ; then
#    echo
#    echo "This script wraps the DFU suffix and prefix for the Tiva TM4C123"
#    echo "used with the pff embedded IO board. If a file has the DFU wrap it"
#    echo "is appended with .dfu. If the file selected ends in .dfu only the"
#    echo "loader is called. Ensure the binary has user write permissions."
#    echo
#    echo "dfu-wrap-load.sh [ /path/to/binary/file ]"
#    echo
#    echo "      -binary file: name of the binary to load. *.dfu files will not wrap"

    echo "improper input arguments"
    echo
    exit 0
else
    FULLFILENAME="$1"
fi

FILE=$(basename "$FULLFILENAME")
EXT="${FILE##*.}"

if [ "$EXT" != "dfu" ] ; then
    ./dfu-suffix -p 0x00ff -v 0x1bce -d 0x0000 -s 0x0000 -a $FILE
    WRAPPEDFILE=$FILE".dfu"
    mv $FILE $WRAPPEDFILE
else
    WRAPPEDFILE=$FILE
fi

echo $FILE" >>>>> " $WRAPPEDFILE
./dfu-util -D $WRAPPEDFILE -R
