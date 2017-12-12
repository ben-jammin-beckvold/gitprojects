#!/bin/bash

cd ~
#git clone git@github.com:piaggiofastforward/pff-ros-ws.git && cd pff-ros-ws
#cd ~/pff-ros-ws/src/pff_common/core/pff_install/install && ./install

source ~/.bashrc


catkin_make -C ~/pff-ros-ws;
#echo "source ~/pff-ros-ws/devel/setup.bash" >> ~/.bashrc
source ~/.bashrc
 
LIBUVC_DIR=~/pff-ros-ws/src/pff_third_party/drivers/libuvc_ros
touch ${LIBUVC_DIR}/libuvc_ros/CATKIN_IGNORE
touch ${LIBUVC_DIR}/libuvc_camera/CATKIN_IGNORE

cd ~/pff-ros-ws/src/pff_common/core/pff_install/install && ./install-libuvc

#chmod +x install-fan-control
#sudo ./install-fan-control

rm ${LIBUVC_DIR}/libuvc_ros/CATKIN_IGNORE
rm ${LIBUVC_DIR}/libuvc_camera/CATKIN_IGNORE

catkin_make -C ~/pff-ros-ws;

source ~/.bashrc


