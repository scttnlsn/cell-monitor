# You may need to disable hangup-on-close for the serial port:
#   stty -F <device> -hupcl

import argparse
import os
import struct

from serial import Serial

ADDRESS_BROADCAST = 0x0

REG_ADDRESS = 0x1
REG_VOLTAGE = 0x2
REG_BALANCE = 0x3

PACKET_LENGTH = 5

DEBUG = os.getenv("DEBUG") == "1"


def debug(label, value):
    if DEBUG:
        print(label, vars(value))


def crc8(values):
    crc = 0

    for value in values:
        data = crc ^ value

        for i in range(0, 8):
            if data & 0x80:
                data <<= 1
                data ^= 0x07
            else:
                data <<= 1

            data &= 0xFF

        crc = data

    return crc


class Packet(object):

    @classmethod
    def receive(cls, serial):
        values = list(serial.read(PACKET_LENGTH + 1))
        if len(values) != PACKET_LENGTH + 1:
            raise ValueError("timeout")
        data = values[:-1]
        if crc8(data) != values[-1]:
            raise ValueError("CRC mismatch")
        packet = Packet(data)
        debug("recv", packet)
        return packet

    def __init__(self, bytes=None):
        if bytes is not None:
            self.decode(bytes)

    def decode(self, bytes):
        self.id = bytes[0]
        self.address = bytes[1] >> 1
        self.request = bool(bytes[1] & 0x1)
        self.reg = bytes[2] >> 1
        self.write = bool(bytes[2] & 0x1)
        self.value = (bytes[3] << 8) | bytes[4]

    def encode(self):
        return [
            self.id,
            (self.address << 1) | int(self.request),
            (self.reg << 1) | int(self.write),
            self.value >> 8,
            self.value & 0xFF,
        ]

    def send(self, serial):
        debug("send", self)
        values = self.encode()
        for value in values:
            self._write(serial, value)
        self._write(serial, crc8(values))
        serial.flush()

    def _write(self, serial, value):
        byte = struct.pack("<B", value)
        serial.write(byte)


class CellMonitor(object):

    def __init__(self, serial):
        self.serial = serial
        self.packet_id = 0

    def start(self):
        req = self._request(ADDRESS_BROADCAST, REG_ADDRESS)
        req.write = True
        req.value = 1
        req.send(self.serial)

        res = Packet.receive(self.serial)
        assert res.address == ADDRESS_BROADCAST
        assert res.request
        assert res.reg == REG_ADDRESS
        assert res.write

        self.num_cells = res.value - 1

    def write(self, address, reg, value):
        req = self._request(address, reg)
        req.write = True
        req.value = value
        req.send(self.serial)

        res = self._receive(address, reg)
        assert res.write

        return res.value

    def read(self, address, reg):
        req = self._request(address, reg)
        req.write = False
        req.send(self.serial)

        res = self._receive(address, reg)
        assert not res.write

        return res.value

    def _request(self, address, reg):
        self.packet_id = (self.packet_id + 1) % 256  # 8 bit ID
        req = Packet()
        req.id = self.packet_id
        req.address = address
        req.request = True
        req.reg = reg
        req.value = 0
        return req

    def _receive(self, address, reg):
        res = Packet.receive(self.serial)
        assert res.id == self.packet_id
        assert res.address == address
        assert not res.request
        assert res.reg == reg
        return res


def connect(port):
    serial = Serial(port=port, baudrate=9600, timeout=0.1)

    cell_monitor = CellMonitor(serial)
    cell_monitor.start()

    return cell_monitor


def main():
    parser = argparse.ArgumentParser(description="communicate with cell monitor")
    parser.add_argument("address", type=int, help="cell monitor address")
    parser.add_argument("reg", type=int, help="cell monitor register")
    parser.add_argument("--port", help="serial device name")
    parser.add_argument("--read", action="store_true", help="read value from register")
    parser.add_argument("--write", type=int, help="write the given value to register")
    args = parser.parse_args()

    cell_monitor = connect(args.port)

    value = None
    if args.write is not None:
        value = cell_monitor.write(args.address, args.reg, args.write)
    elif args.read:
        value = cell_monitor.read(args.address, args.reg)

    print(value)


if __name__ == "__main__":
    main()
    # cell_monitor = connect('/dev/ttyUSB0')
    # for i in range(0, cell_monitor.num_cells):
    #     cell = i + 1
    #     print(cell_monitor.read(cell, 3))
