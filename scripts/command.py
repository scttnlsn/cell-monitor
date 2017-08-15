import argparse
from serial import Serial
import struct
import time

parser = argparse.ArgumentParser(description='enable 5 second balancing for a cell')
parser.add_argument('address', type=int, help='cell monitor address')
parser.add_argument('command', type=int, help='cell monitor command')
parser.add_argument('--port', help='serial device name')
parser.add_argument('--read', type=int, help='read given number of bytes after sending command')
parser.add_argument('--write', type=int, help='write 2-byte value after sending command')

class CellMonitor(object):

    def __init__(self, port):
        self.serial = Serial(port = port, baudrate = 9600)

    def send_command(self, address, command):
        byte = struct.pack('!B', (address << 4) | command)
        self.serial.write(byte)

    def read_value(self, n):
        values = map(ord, self.serial.read(n))
        result = 0

        for value in values:
            result = (result << 8) | value

        return result

    def write_value(self, value):
        self.serial.write(value >> 8)
        self.serial.write(value & 0xFF)

if __name__ == '__main__':
    args = parser.parse_args()

    cell_monitor = CellMonitor(args.port)
    cell_monitor.send_command(args.address, args.command)

    if args.write:
        cell_monitor.write_value(args.write)

    if args.read:
        print(cell_monitor.read_value(args.read))
