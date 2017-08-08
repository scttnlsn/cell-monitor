import argparse
from serial import Serial
import struct

COMMAND = 0x1

parser = argparse.ArgumentParser(description='read cell monitor voltage')
parser.add_argument('address', type=int, help='cell monitor address')
parser.add_argument('--port', help='serial device name')

args = parser.parse_args()
serial = Serial(port = args.port, baudrate = 9600)
byte = struct.pack('!B', (args.address << 4) | COMMAND)

def read_voltage():
    serial.write(byte)
    values = map(ord, serial.read(2))
    return (values[0] << 8) | values[1]

if __name__ == '__main__':
    print(read_voltage())
