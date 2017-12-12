#!/bin/bash

if [[ ! -d $(rospack find pff_usb_disconnect) ]]; then
    printf "\nt\tCould not find package: pff_usb_disconnect.\n"
    exit 0
else
    PACKAGE_DIR=$(rospack find pff_usb_disconnect)
fi

# copy the ftdi library
sudo cp $PACKAGE_DIR/libraries/libftd2xx* /usr/local/lib
sudo chmod 0755 /usr/local/lib/libftd2xx.so.1.3.6
sudo ln -sf /usr/local/lib/libftd2xx.so.1.3.6 /usr/local/lib/libftd2xx.so

# install rules
sudo cp hub_disconnect.rules /etc/udev/rules.d/
sudo udevadm control --reload
sudo udevadm trigger --action=add

# install unbind kernel driver script, this is needed with the rules
sudo cp $PACKAGE_DIR/scripts/unbind_ftdi_sio.sh /usr/local/bin/


