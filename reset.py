#!/usr/bin/env python
import serial, sys
port = sys.argv[1]
ser = serial.Serial(
    port=port,
    baudrate=1200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)
ser.setRTS(True)
ser.setDTR(False)
ser.isOpen()
ser.close()
