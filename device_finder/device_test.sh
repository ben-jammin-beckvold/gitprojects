#!/usr/bin/env bash

# ROS device file check before launching

LOG_FILE="device_test.log"

#######################################
# debug printing to screen and file
# Globals:
#   LOG_FILE
# Arguments:
#   msg
# Returns:
#   None
#######################################

log_to_file() {
    file="$1"
    msg="$2"
    # create file if does not exist
    if [ ! -e ${file} ]; then
        printf "Start Log [$DATE_YMD] [$DATE_MS].\n\n" > $file
    else
        d1=`date +%s`
        d2=`stat -c %Y ${file}`
        dt=$(($d1-$d2))
        if [ $dt -gt 10 ]; then
            printf "Start Log [$DATE_YMD] [$DATE_MS].\n\n" > $file
        fi
    fi
    printf "$msg"
    printf "$msg" >> $file
}


log_debug() {
    log_to_file "${LOG_FILE}" "$1"
}

#devices=( "/dev/pff/bmotor" "/dev/pff/lmotor" "/dev/pff/rmotor" )
devices=( "/dev/pff/adc" "/dev/pff/hub_disconnect" "/dev/pff/arduino_A104JTAB" )
if [[ $USE_LATCH = true ]]; then
    devices+=( "/dev/pff/tm4c123_prog" )
fi

log_debug $devices

log_debug "Checking devices before launch ...\n"
for d in "${devices[@]}"
do
    log_debug "\t- ${d}\n"
done

for i in {1..10}
do
    found_all=true
    for d in "${devices[@]}"
    do
        if [ ! -e ${d} ]; then
            log_debug "\tAttempt [$i].  Device ${d} not found.\n"
            found_all=false
            sleep 1
            break
        fi
    done
    if [ $found_all = true ]; then
        log_debug "\tAttempt [$i].  Found all devices.\n"
        break;
    else
        log_debug "Unable to find devices.\n"
    fi
done

