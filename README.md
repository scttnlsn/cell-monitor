# cell-monitor

Battery cell monitor and balancer based on the ATTiny85 MCU.

* reads cell voltage and temperature
* switches balancing FET
* communicates with host controller via isolated serial protocol
* low power consumption (<1mA when idle, ~2mA while active)

## Requirements

* [avr-libc](http://www.nongnu.org/avr-libc/)
* [avrdude](http://www.nongnu.org/avrdude/)
* [pio](http://platformio.org)
* [make](https://www.gnu.org/software/make/)

## Build

```
make
make flash
```

## Config

Each cell monitor on a shared serial bus needs a unique 4-bit address.  To program the address for a given monitor, pass the `CELL_ADDRESS` environment variable when building:

```
make CELL_ADDRESS=1
```

The cell monitor uses an internal bandgap reference to measure the cell voltage.  The internal reference has a nominal value of 1100mV but by calibrating this value you'll get more accurate voltage readings.  Use the included `command.py` script to read the voltage of a cell monitor (replacing the address and port, if needed):

```
python ./scripts/command.py --port /dev/ttyUSB0 <address> 1
```

You'll get back a cell voltage reading in mV.  Take an accurate reading of the actual cell voltage and plug both values into the following formula:

```
actual / reading * 1100
```

This value should be used as the internal reference voltage.  Pass the `REF_VOLTAGE` environment variable when building:

```
make REF_VOLTAGE=1100
```

## Serial protocol

Commands are sent from a host controller to the cell monitor via a 9600 baud isolated serial line.  Multiple monitors can share a single serial bus since each monitor has a unique 4-bit address.  Each command includes the cell address bits and only the corresponding cell monitor will respond.

Each command byte includes a 4 bit cell address (CA) and a 4 bit command code (CC):

```
| CA 4 | CA 3 | CA 2 | CA 1 | CC 4 | CC 3 | CC 2 | CC 1 |
```

The command code consists of the following bits:

```
| RSVD | BALANCE_ON | SEND_TEMP | SEND_VOLTAGE |
```

* `SEND_VOLTAGE` - when set to `1` the cell monitor will reply with a 2-byte voltage in mV (most-significant byte first)
* `SEND_TEMP` - when set to `1` the cell monitor will reply with a 2-byte temperature in degrees celcius * 100 (most-significant byte first)
* `BALANCE_ON` - when set to `1` the cell monitor will switch on the balancing FET, when set to `0` the cell monitor will switch off the balancing FET
* `RSVD` - reserved for future use

If you've set both the `SEND_TEMP` and `SEND_VOLTAGE` bits then you'll receive 4 bytes (voltage first and then temp).

The cell monitor has an internal watchdog that automatically turns off the balancing FET after 10 seconds.  If you'd like to balance for a longer period of time then you need to repeatedly send a command with the `BALANCE_ON` bit set to `1`.
