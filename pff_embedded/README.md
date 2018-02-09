## Embedded IO board -- host side


### Building Firmware Binaries
See the pff-firmware repo for information about firmware development and creating firmware binaries.

Binaries will be versioned and stored in a central location. **(TODO)**

### Remote DFU

```
./setup_target_dfu <binary_name.bin>
```
This program toggles the pins on the MCU to trigger the DFU and reset the board, kills and restarts the ROS node running the rosserial communication to the MCU, and run the DFU upload script.

Flow diagram of the DFU: 
![alt text][dfu_flow]

[dfu_flow]: ./doc/dfu_flow.png "DFU Flow Diagram"

#### Uploading Firmware

The DFU is part of the MCU ROM. If the first 2 addresses in flash are erased then it will run the bootloader from ROM when reset. The flash is erased from inside the firmware. Once erased the MCU will halt and need to be reset to trigger the ROM bootloader.

```
scripts/dfu-wrap-load.sh /path/to/binary.bin
```
-w will only provide the DFU wrap
-l will load a previously wrapped binary
ensure that the binary does not need root permissions to write.
This will wrap the binary with the proper DFU info and load it to the MCU. The MCU will need to be reset to start executing.

#### USB HUB Controller
The Microchip USB5806 USB hub includes a bridge to I2C and GPIO pins that can be accessed through the hub's USB port.

I2C breakout bus are setup using libusb. This was developed for remote DFU but that implementation was ported outside of ROS. This node runs once to set the initial state of the IO Expander pins.


---
#### Source info for dfu scripts

dependencies
```
sudo apt install autoconf libusb-1.0-0-dev -y
```
dfu-suffix v0.7
```
git clone https://github.com/cjheath/dfu-util.git
cd dfu-util
./autogen.sh
./configure
make
```
dfu-util v0.9
```
git clone git://git.code.sf.net/p/dfu-util/dfu-util
cd dfu-util
./autogen.sh
./configure
make
```
both repos have the dfu-suffix and dfu-util use the v0.7 suffix and v0.9 util. it won't work otherwise. prebuilt executables arein the /scripts directory.

