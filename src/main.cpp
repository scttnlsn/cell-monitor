#include <Arduino.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>

// this address should be unique for each cell monitor on the serial bus
#define CELL_ADDRESS 0b0001

#define CMD_SEND_VOLTAGE 0b0001
#define CMD_BALANCE 0b0010

// serial data is inverted since the optocoupler also inverts it
SoftwareSerial serial(3, 4, true); // rx, tx, inverse logic

uint16_t read_vcc(void) {
  // use VCC as reference
  // measure internal bandgap reference voltage
  ADMUX = _BV(MUX3) | _BV(MUX2);

  // wait for ADC to settle
  delay(2);

  // start conversion
  ADCSRA |= _BV(ADSC);

  // wait for conversion to complete
  while (bit_is_set(ADCSRA, ADSC));

  uint8_t low  = ADCL; // must read ADCL first
  uint8_t high = ADCH;
  uint16_t result = (high << 8) | low;

  // return Vcc in mV (1125300 = 1.1 * 1023 * 1000)
  // TODO: calibrate this value
  return 1125300L / result;
}

uint16_t read_vcc_avg(void) {
  uint16_t sum = 0;

  for (int i = 0; i < 8; i++) {
    sum += read_vcc();
    delay(i);
  }

  return sum / 8;
}

void send_voltage(void) {
  uint16_t vcc = read_vcc_avg();
  serial.write(vcc >> 8); // MSB
  serial.write(vcc & 0xFF); // LSB
}

// TODO: put a watchdog timer on balancing
// balancing should be auto-disabled after N seconds
// receiving a balance command resets this timer

void enable_balancing(void) {
  digitalWrite(0, HIGH);
}

void disable_balancing(void) {
  digitalWrite(0, LOW);
}

void process_command(uint8_t command) {
  if (command & CMD_SEND_VOLTAGE) {
    send_voltage();
  }

  if (command & CMD_BALANCE) {
    enable_balancing();
  } else {
    disable_balancing();
  }
}

void process_input() {
  if (serial.available()) {
    uint8_t value = serial.read();

    if (value > 0) {
      uint8_t address = value >> 4;

      if (address == CELL_ADDRESS) {
        uint8_t command = value & 0xF;

        process_command(command);
      }
    }
  }
}

void setup() {
  // 1 second watchdog
  wdt_enable(WDTO_1S);

  // serial pins
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);

  // balancing pin
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  serial.begin(9600);
}

void loop() {
  process_input();
  wdt_reset();
}
