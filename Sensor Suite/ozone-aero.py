#!/usr/bin/env python3
import struct  
import time
import serial                           # import the module 
import rospy

rospy.init_node('aero_sensor')
rospy.loginfo("Staretd aero_sensor node")
ComPort = serial.Serial('/dev/ttyUSB0') # open ttyUSB0 

ComPort.baudrate = 4800                 # set Baud rate 
ComPort.bytesize = 8                    # Number of data bits = 8
ComPort.parity = 'N'                    # No parity
ComPort.stopbits = 1    # Number of Stop bits = 1

x=list()

command=b"\x55\x1a\x00\x91"

ComPort.write(command)
y=(ComPort.read(15))
print(y[:2])



if y[:2] ==b'\xaa\x10':
    print(struct.unpack('<f',y[2:6]))
    vals = struct.unpack('<f',y[2:6])
    rospy.set_param('/aero/O3',vals)
