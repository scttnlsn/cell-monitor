# cell-monitor

Battery cell monitor and balancer based on the ATTiny85 MCU.

* reads cell voltage and temperature (using built-in 10-bit ADC on the ATTiny)
* switches balancing FET
* communicates with host controller via isolated I2C (supporting many cell monitors on a single bus)
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

## Communication

The cell monitor implements a standard 100kHz I2C interface and acts as a slave device.
The I2C address is 7 bits and is configured when flashing the firmware (see config).

**Registers:**

* `STATUS`
  * address = `0x01`
  * description
  * bits = `RSVD | RSVD | RSVD | RSVD | RSVD | RSVD | RSVD | BALANCE`
  * `BALANCE`:
    * 1 when balancing FET is enabled
    * 0 when balancing FET is disabled
    * write a 1 to enable balancing FET
    * write a 0 to disable balancing FET
    * There's an internal watchdog that automatically turns off the balancing FET after 10 seconds.  In order to enable balancing for longer periods of time the host controller needs to repeatedly write a 1 to the `BALANCE` bit in this register.
* `VOLTAGE_HIGH`
  * address = `0x02`
  * description = upper 2 bits of 10-bit voltage reading
  * bits = `RSVD | RSVD | RSVD | RSVD | RSVD | RSVD | V10 | V9`
  * read-only
* `VOLTAGE_LOW`
  * address = `0x03`
  * description = lower 8 bits of 10-bit voltage reading
  * bits = `V8 | V7 | V6 | V5 | V4 | V3 | V2 | V1`
  * read-only
  * when 2 bytes are read from register `VOLTAGE_LOW`, the `VOLTAGE_HIGH` and `VOLTAGE_LOW` values are returned atomically
* `TEMP_HIGH`
  * address = `0x04`
  * description = upper 2 bits of 10-bit temperature reading
  * bits = `RSVD | RSVD | RSVD | RSVD | RSVD | RSVD | V10 | V9`
  * read-only
* `TEMP_LOW`
  * address = `0x05`
  * description = lower 8 bits of 10-bit temperature reading
  * bits = `T8 | T7 | T6 | T5 | T4 | T3 | T2 | T1`
  * read-only
  * when 2 bytes are read from register `TEMP_HIGH`, the `TEMP_HIGH` and `TEMP_LOW` values are returned atomically

## Config

Each cell monitor on a shared I2C bus needs a unique 7-bit address.  To program the address for a given monitor, pass the `CELL_ADDRESS` environment variable when building:

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
