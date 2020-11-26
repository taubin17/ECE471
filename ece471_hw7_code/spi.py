#!/usr/bin/python

import spidev
import time

spi = spidev.SpiDev()
spi.open(0, 0)
to_send = (0x01, 0x80, 0x0)

while True:
    resp = spi.xfer2(to_send)
    print (resp)
    value = (resp[1] & 0x3) << 8 + resp[2];
    print(value)
    print((value * 3.3)/1024)
    time.sleep(1)
