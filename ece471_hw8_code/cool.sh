#!/bin/bash

echo This is my something cool. Your device is named:

path_front="/sys/bus/w1/devices/"
path_back="/w1_slave"

device_name=$(ls /sys/bus/w1/devices/ | grep 28-)
path_full="$path_front$device_name$path_back"
echo Device name $device_name was found.
echo Beginning temperature scanning now
./temp_cool $path_full
