import argparse
from serial import Serial
import struct
import time

ENABLE_BALANCE_CMD = 0b0010
DISABLE_BALANCE_CMD = 0b0000

parser = argparse.ArgumentParser(description='enable 5 second balancing for a cell')
parser.add_argument('address', type=int, help='cell monitor address')
parser.add_argument('--port', help='serial device name')

args = parser.parse_args()
serial = Serial(port = args.port, baudrate = 9600)

def read_voltage():
    serial.write(byte)
    values = map(ord, serial.read(2))
    return (values[0] << 8) | values[1]

def enable_balancing():
    byte = struct.pack('!B', (args.address << 4) | ENABLE_BALANCE_CMD)
    serial.write(byte)

def disable_balancing():
    byte = struct.pack('!B', (args.address << 4) | DISABLE_BALANCE_CMD)
    serial.write(byte)

if __name__ == '__main__':
    enable_balancing()
    print('on')
    time.sleep(5)
    disable_balancing()
    print('off')
