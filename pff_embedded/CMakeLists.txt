cmake_minimum_required(VERSION 2.8.3)
project(pff_embedded)

find_package(catkin REQUIRED COMPONENTS
    message_generation
    rosserial_python
    std_msgs
    roscpp
    )

## Generate messages in the 'msg' folder
add_message_files(
    FILES
    tempSensors.msg
    devGpio.msg
    ADCInputs.msg
    dfu.msg
    PowerBoardStatus.msg
    IOBoardStatus.msg
    EmbeddedStatus.msg
    rangefinder.msg
    EmbeddedTimestamp.msg
    )

## Generate added messages and services with any dependencies listed here
generate_messages(
    DEPENDENCIES
    std_msgs
    )

catkin_package(
    INCLUDE_DIRS include
    LIBRARIES
    CATKIN_DEPENDS message_runtime std_msgs
    DEPENDS
    roscpp
    std_msgs
    )

include_directories(
    include
    /usr/include/libusb-1.0
    ${catkin_INCLUDE_DIRS}
    )

add_definitions(-std=c++11)

add_executable(usb_hub_control src/hub_ctrl.cpp)
target_link_libraries(usb_hub_control ${catkin_LIBRARIES} -lusb-1.0)
add_dependencies(usb_hub_control ${catkin_EXPORTED_TARGETS})

add_executable(embedded_timestamp src/timestamp.cpp)
target_link_libraries(embedded_timestamp ${catkin_LIBRARIES})
add_dependencies(embedded_timestamp ${catkin_EXPORTED_TARGETS})
