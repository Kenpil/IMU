#!/usr/bin/python -u
# -*- coding: utf-8 -*-

import smbus
import time
from fractions import Fraction

address = 0x68
bus     = smbus.SMBus(1)

PI = 3.1415
gyro_coefficient = float(Fraction(500 , 32768))

def u2s(unsigneddata):
    if unsigneddata & (0x01 << 15) : 
        return -1 * ((unsigneddata ^ 0xffff) + 1)
    return unsigneddata


# ???????????
bus.write_i2c_block_data(address, 0x6B, [0x80])
time.sleep(0.1)     

# PWR_MGMT_1????
bus.write_i2c_block_data(address, 0x6B, [0x00])
time.sleep(0.1)
bus.write_i2c_block_data(address, 0x1B, [0x08])
time.sleep(0.1)
bus.write_i2c_block_data(address, 0x37, [0x02])
time.sleep(0.1)
bus.write_i2c_block_data(0x0C, 0x0A, [0x12])
time.sleep(0.1)

#gyro_offset
gy_os_sum_x = 0.0
gy_os_sum_y = 0.0
gy_os_sum_z = 0.0
for gy_os_cnt in range(1000):
    gdata = bus.read_i2c_block_data(address, 0x43, 6)
    gy_os_sum_x = gy_os_sum_x + gyro_coefficient * u2s(gdata[0] << 8 | gdata[1])
    gy_os_sum_y = gy_os_sum_y + gyro_coefficient * u2s(gdata[0] << 8 | gdata[1])
    gy_os_sum_z = gy_os_sum_z + gyro_coefficient * u2s(gdata[0] << 8 | gdata[1])
    if  gy_os_cnt == 999:
        gyro_offset_x = gy_os_sum_x / 1000
        gyro_offset_y = gy_os_sum_y / 1000
        gyro_offset_z = gy_os_sum_z / 1000
        #print " gyro_coefficient:%f" % gyro_coefficient + "  ",
        #print " gyro_offset_x:%f" % gyro_offset_x + "  ",
        #print " gyro_offset_y:%f" % gyro_offset_y + "  ",
        #print " gyro_offset_z:%f" % gyro_offset_z + "\n",
        break

# ?????????
while True:
    adata = bus.read_i2c_block_data(address, 0x3B ,6)
    ax    = adata[0] << 8 | adata[1]
    ay    = adata[2] << 8 | adata[3]
    az    = adata[4] << 8 | adata[5]
    ax    = (-2.0 / float(0x8000)) * u2s(ax)
    ay    = (-2.0 / float(0x8000)) * u2s(ay)
    az    = (-2.0 / float(0x8000)) * u2s(az)

    gdata = bus.read_i2c_block_data(address, 0x43, 6)
    gx    = gdata[0] << 8 | gdata[1]
    gy    = gdata[2] << 8 | gdata[3]
    gz    = gdata[4] << 8 | gdata[5]
    gx    = (gyro_coefficient * u2s(gx) - gyro_offset_x) / 180 * PI
    gy    = (gyro_coefficient * u2s(gy) - gyro_offset_y) / 180 * PI
    gz    = (gyro_coefficient * u2s(gz) - gyro_offset_z) / 180 * PI

    mdata = bus.read_i2c_block_data(0x0c, 0x03, 7)
    mx    = mdata[1] << 8 | mdata[0]
    my    = mdata[3] << 8 | mdata[2]
    mz    = mdata[5] << 8 | mdata[4]
    mx    = (float(Fraction(u2s(mx) , 32768)) * 49210 - 72) * 1.54
    my    = (float(Fraction(u2s(my) , 32768)) * 49210 + 32) * 1.54
    mz    = (float(Fraction(u2s(mz) , 32768)) * 49210  + 247.5) * 1.54 
    print "%+8.7f" % mx + "   ",
    print "%+8.7f" % my + "   ",
    print "%+8.7f" % mz + "\n",
    time.sleep(1)
