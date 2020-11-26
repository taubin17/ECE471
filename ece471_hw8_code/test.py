#!/usr/bin/python
import os
from time import sleep

temp_sensor = '/sys/bus/w1/devices/28-000008d542d0/w1_slave'
def main():
    os.system('modprobe w1-gpio')
    os.system('modprobe w1-therm')
    
    while True:
        print(read_temp())
        sleep(1)



def temp_raw():
    fd = open(temp_sensor, 'r')
    lines = fd.readlines()
    fd.close()
    return lines


def read_temp():
    lines = temp_raw()
    while lines[0].strip()[-3:] != "Yes":
        sleep(0.2)
        lines = temp_raw()
    
    temp_output = lines[1].find("t=")

    if temp_output != -1:
        temp_string = lines[1].strip()[temp_output+2:]
        temp_c = float(temp_string)/1000
        return temp_c


if __name__ == '__main__':
    main()
