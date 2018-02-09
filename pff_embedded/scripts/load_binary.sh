#!/bin/bash

rostopic pub /dfu_reset std_msgs/Bool true

ECHO_BACK_DFU=false
rostopic echo /dfu_reset > ECHO_BACK_DFU

echo $ECHO_BACK_DFU

