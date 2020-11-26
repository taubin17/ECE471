#!/usr/bin/python

import board
import busio
import adafruit_bme280
from adafruit_ht16k33 import segments
from string import ascii_lowercase
import time

        
def display_initialize(i2c):
    display = segments.Seg7x4(i2c)
    display.fill(0)
    return display


def to_print(value, display):
    display.fill(0)
    if value >= 1000 or value < -999:
        print("Error, cannot print value for it does not fit on display!")
        display.print('Bad')
    else:
        if value < 0:
            value = round(value, 2)
        elif value < -99.9:
            value = int(value)
        # Convert the value to a string so we can index said number by each digit
        value = str(value)
        # For each digit (and the decimal point if there is one present) write them to the screen in order!
        for each in value:
            display.print(each)

if __name__ == '__main__':
    main()
