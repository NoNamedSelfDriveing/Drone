from ctypes import *
from PyQt4 import QtTest
import serial

ser = []
packet = []
baud = 57600

class uint8_arr(Array):
    _type_ = c_uint8
    _length_ = 4

class union(Union):
    _fields_ = ("data", c_float), ("buff", uint8_arr)

# find enable COM PORT
def findEnableCOMPort():
    for i in range(0, 100):
        try:
            serObj = serial.Serial(
                port="COM{num}".format(num = str(i)),\
                baudrate=baud,\
                parity=serial.PARITY_NONE,\
                stopbits=serial.STOPBITS_ONE,\
                bytesize=serial.EIGHTBITS,\
                timeout = 0)
            ser.append(serObj.port)
            serObj.close()
        except:
            pass
    return ser

# Make Packet for Transmit
def makePacket(gainType, gainValue):
    checksum = 0
    gainUnion = union()
    gainUnion.data = gainValue


    packet.append(83)
    packet.append(77)
    packet.append(gainType)
    packet.append(gainUnion.buff[0])
    packet.append(gainUnion.buff[1])
    packet.append(gainUnion.buff[2])
    packet.append(gainUnion.buff[3])
    packet.append(80)

    # calculate checksum
    for data in packet:
        checksum += data

    checksum = 0x0100 - (checksum & 0x00FF)
    packet.append(checksum)

# Transmit Packet
def transmitPacket(portNum, baudRate, gainType, gainValue):
    makePacket(gainType, gainValue)

    try:
        ser = serial.Serial(
            port=portNum,\
            baudrate=baudRate,\
            parity=serial.PARITY_NONE,\
            stopbits=serial.STOPBITS_ONE,\
            bytesize=serial.EIGHTBITS,\
            timeout = 0)
    except:
        pass

    for data in packet:
        print(format(data, '#04x'), end=' ')
        ser.write(c_uint8(data))
        QtTest.QTest.qWait(10)

    QtTest.QTest.qWait(50)

    print('')
    packet.clear()

