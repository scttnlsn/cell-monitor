import serial
import struct

ser = serial.Serial(
    port = '/dev/cu.usbserial-A9GBVLD5',
    baudrate = 9600)

address = 0x1
command = 0x1

byte = (address << 4) | command

ser.write(struct.pack('!B', byte))

values = map(ord, ser.read(2))
voltage = (values[0] << 8) | values[1]

print(voltage)
