#!/bin/bash

RULES=false
LIBS=false

PACKAGE_DIR=$PWD

if [ $# -gt 0 ]; then
    LIBS=true
    RULES=true
elif [[ $1 -eq "-rules" ]]; then
    RULES=true
elif [[ $1 -eq "-libs" ]]; then
    LIBS=true
fi

if [[ LIBS -eq "true" ]]; then
# copy the ftdi library
    echo -e "Installing libraries ..."
    sudo cp $PACKAGE_DIR/libraries/libftd2xx* /usr/lib
    sudo chmod 0755 /usr/lib/libftd2xx.so.1.3.6
    sudo ln -sf /usr/lib/libftd2xx.so.1.3.6 /usr/lib/libftd2xx.so
fi

if [[ RULES -eq "true" ]]; then
# install rules
    echo -e "Installing rules ..."
    sudo cp hub_disconnect.rules /etc/udev/rules.d/
    sudo udevadm control --reload
    sudo udevadm trigger --action=add
fi

# install unbind kernel driver script, this is needed with the rules
sudo cp $PACKAGE_DIR/scripts/unbind_ftdi_sio.sh /usr/local/bin/


