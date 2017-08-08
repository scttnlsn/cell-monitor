import argparse
from serial import Serial
import struct
import time

COMMAND = 0b0010

parser = argparse.ArgumentParser(description='enable 5 second balancing for a cell')
parser.add_argument('address', type=int, help='cell monitor address')
parser.add_argument('--port', help='serial device name')

args = parser.parse_args()
serial = Serial(port = args.port, baudrate = 9600)
byte = struct.pack('!B', (args.address << 4) | COMMAND)

def enable_balancing():
    serial.write(byte)

if __name__ == '__main__':
    enable_balancing()
