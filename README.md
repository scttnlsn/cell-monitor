# cell-monitor

Battery cell monitor and balancer based on the AVR ATTiny85 MCU.

* reads cell voltage and temperature
* switches balancing FET
* communicates with a host controller via isolated serial
* low power consumption (<1mA when idle, ~2mA while active)

A cell monitor is intended to monitor a single cell as part of a larger series pack.
The cell monitor is powered directly from the cell it monitors and works with any chemistry
where the voltage remains within 1.8V-5.5V (i.e. lithium-ion, LiFePO4).

## Build

Dependencies:

* [pio](http://platformio.org)
* [make](https://www.gnu.org/software/make/)

```
make
make fuse
make flash
```

## Wiring

Cell monitors are wired in a daisy chain with the TX from the host controller connected to
the RX of cell monitor 1, the TX of cell monitor 1 connected to the RX of cell monitor 2, etc.
The TX of cell monitor N is connected to the RX of the host controller.

```
+-----------+
| HOST   RX | <------------------------------------------+
|        TX | >---+    +--->---+    +--->  ...  >---+    |
+-----------+     |    |       |    |               |    |
                  |    |       |    |               |    |
                +--------+   +--------+           +--------+
                | RX  TX |   | RX  TX |    ...    | RX  TX |
                |    1   |   |    2   |    ...    |    N   |
                +--------+   +--------+    ...    +--------+
```

Cell monitors are addressed by their position in the daisy chain.

## Serial protocol

The host controller initiates all communication with the network of cell monitors and does so at 9600 baud.
The host controller sends a request packet which is forwarded through the daisy chain to an addressed cell
monitor.  The addressed cell monitor will reply with a similar response packet which will be forwarded through
the remaining chain and eventually read by the host controller.

#### Packet structure

Communication consists of 5 byte packets with the following structure:

| Byte  | 7     | 6     | 5     |    4  |    3  |    2  |    1  |     0 |
| ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
| **1** | ADDR6 | ADDR5 | ADDR4 | ADDR3 | ADDR2 | ADDR1 | ADDR0 | REQ   |
| **2** | REG6  | REG5  | REG4  | REG3  | REG2  | REG1  | REG0  | WRITE |
| **3** | VAL15 | VAL14 | VAL13 | VAL12 | VAL11 | VAL10 | VAL9  | VAL8  |
| **4** | VAL7  | VAL6  | VAL5  | VAL4  | VAL3  | VAL2  | VAL1  | VAL0  |
| **5** | CRC7  | CRC6  | CRC5  | CRC4  | CRC3  | CRC2  | CRC1  | CRC0  |

* **ADDR6-0** - cell monitor address
* **REQ** - 1 if packet is a request, 0 if packet is a response
* **REG6-0** - register (described below)
* **WRITE** - 1 if packet is a write command, 0 if packet is a read command
* **VAL15-0** - value
* **CRC7-0** - CRC8 of prior 4 bytes

#### Registers

* `0x1` - cell address (1 byte, unsigned)
* `0x2` - reference voltage (2 bytes, unsigned)
* `0x3` - cell voltage (2 bytes, unsigned)
* `0x4` - celsius temperature (2 bytes, signed)
* `0x5` - balance (1 if balancing, 0 otherwise)

#### Example

*Reading the voltage from cell 1*

Request:

| Byte | Value    |
| ---- | -------- |
| 1    | 00000011 |
| 2    | 00000110 |
| 3    | 00000000 |
| 4    | 00000000 |
| 5    | 11010110 |

* `ADDR` = 1
* `REQ` bit set to 1
* `REG` = 3
* `WRITE` bit set to 0
* `VAL` is not applicable for read request
* `CRC` = crc8(50724864)

The following response will be received:

| Byte | Value    |
| ---- | -------- |
| 1    | 00000010 |
| 2    | 00000110 |
| 3    | 00001100 |
| 4    | 11100100 |
| 5    | 01110111 |

* `ADDR` = 1
* `REQ` bit set to 0
* `REG` = 3
* `WRITE` bit set to 0
* `VAL` is the returned voltage (3300mV in this example)
* `CRC` = crc8(33950948)

#### Address assignment

A cell monitor has an undefined address when it is initially powered on.  The host controller
needs to initiate an address assignment request (which has slightly different semantics than
other value requests):

| Byte | Value    |
| ---- | -------- |
| 1    | 00000001 |
| 2    | 00000011 |
| 3    | 00000000 |
| 4    | 00000001 |
| 5    | 00110011 |

* `ADDR` = 0 (broadcast to all cell monitors)
* `REQ` bit set to 1
* `REG` = 1
* `WRITE` bit set to 1
* `VAL` = first cell address
* `CRC` = crc8(16973825)

The first cell monitor will receive the request and set its address to the value in the `VAL` field.
Instead of sending a response it will increment the `VAL` field and forward the request onward.
Eventually the host controller will receive the same request packet (with an incremented `VAL`).
The host controller should assert that `VAL` equals 1 more than the number of cell monitors in the
network.
