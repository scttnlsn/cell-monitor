import argparse
from serial import Serial
import struct
import time

CMD_READ_VOLTAGE = 0b0001
CMD_BALANCE = 0b0010

parser = argparse.ArgumentParser(description='enable 5 second balancing for a cell')
parser.add_argument('address', type=int, help='cell monitor address')
parser.add_argument('command', type=int, help='cell monitor command')
parser.add_argument('--port', help='serial device name')

args = parser.parse_args()
serial = Serial(port = args.port, baudrate = 9600)

def send_command():
    byte = struct.pack('!B', (args.address << 4) | args.command)
    serial.write(byte)

def read_voltage():
    values = map(ord, serial.read(2))
    return (values[0] << 8) | values[1]

if __name__ == '__main__':
    send_command();

    if args.command & CMD_READ_VOLTAGE:
        print(read_voltage())
