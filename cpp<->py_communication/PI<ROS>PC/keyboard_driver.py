#!/usr/bin/python -u
# -*- coding: utf-8 -*-

import smbus
import time
import threading
import csv

address = 0x68
address2 = 0x0c
channel = 1
bus     = smbus.SMBus(channel)
f = open('data.csv', 'w')

#unsigned??signed???(16??????
def u2s(unsigneddata):
    if unsigneddata & (0x01 << 15) : 
        return -1 * ((unsigneddata ^ 0xffff) + 1)
    return unsigneddata

# ???????????
#bus.write_i2c_block_data(address, 0x6B, [0x80])
time.sleep(0.1)     

# PWR_MGMT_1????
bus.write_i2c_block_data(address, 0x6B, [0x00])
time.sleep(0.1)
bus.write_i2c_block_data(address, 0x37, [0x02])
time.sleep(0.1)
bus.write_i2c_block_data(address2, 0x0a, [0x12])
time.sleep(0.1)

# ?????????

count = 0
print"start"

while (count < 1000):
    data    = bus.read_i2c_block_data(address2, 0x03 ,7)
    rawX    = (u2s(data[1] << 8 | data[0]) / 32768.0 * 4921.0 * 10.0 - 72.0) * 1.54
    rawY    = (u2s(data[3] << 8 | data[2]) / 32768.0 * 4921.0 * 10.0 + 32.0 ) * 1.54
    rawZ    = (u2s(data[5] << 8 | data[4]) / 32768.0 * 4921.0 * 10.0 + 247.5) * 1.54
    #bus.read_i2c_block_data(0x0c, 0x09 ,1)

    writer = csv.writer(f, lineterminator = ',')
    writer.writerow([rawX])
    writer = csv.writer(f, lineterminator = ',')
    writer.writerow([rawY])
    writer = csv.writer(f, lineterminator = '\n')
    writer.writerow([rawZ])

    #print "%+8.7f" % rawX + "   ",
    #print "%+8.7f" % rawY + "   ",
    #print "%+8.7f" % rawZ

    count = count + 1
    
    time.sleep(0.1)

print"finished"
