
USB disconnect switch controller -> this is designed for toggling the power lines on a USB device. The 2 TOF cameras (RealSense and PMD) are not recognized when the computer boots. The will need to be power cycled to be recognized.

The PCB has 2 USB devices passing through and an FTDI chip uses it's RTS and DTR lines to toggle the load switch input.

#### RTS is switch#1

#### DTR is switch#2


## Topics

### Publisher

usb_switch_status: 2 boolean fields that store the status of each of the switches. This is a custom message type.
```
bool usbSwitch1 # RTS
bool usbSwitch2 # DTR
```

### Subscriber

usb_disconnect_device1 -- boolean sets the USB device power ON or OFF

usb_disconnect_device2 -- boolean sets the USB device power ON or OFF

## Installation

There is an installation script in `pff_install/install` that
- copies the ftd2xx library to `/usr/local/lib/`, with permissions and simlink
- copies a script to `/usr/local/bin` that runs with the udev rule

There is also a udev rule that is with the other FTDI rules.
This is a unique rule that runs the script that unbinds the kernel driver `ftdi_sio`.
There is a conflict when using the ftd2xx library for custom FTDI applications.
It is important that the FTDI controller is plugged into the same USB port each time or the kernel driver won't unbind properly.
