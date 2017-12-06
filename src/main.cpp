#include <Arduino.h>
#include <USIWire.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

#include "adc.h"
#include "balance.h"
#include "config.h"
#include "measurements.h"

void received(int bytes);
void requested();

static volatile uint8_t reg;
static volatile uint8_t value;
static volatile uint8_t status;
static volatile uint16_t voltage;
static volatile uint16_t temp;

#define REGISTER_STATUS 1
#define REGISTER_VOLTAGE_HIGH 2
#define REGISTER_VOLTAGE_LOW 3
#define REGISTER_TEMP_HIGH 4
#define REGISTER_TEMP_LOW 5

Balancer balancer(BALANCE_PIN);

void sleep() {
  adc_disable();
  wdt_reset();
  wdt_disable();
  sleep_enable();
  sleep_cpu();

  // wake up
  adc_enable();
  wdt_enable(WDTO_1S);
  wdt_reset();
}

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // 1 second watchdog
  wdt_enable(WDTO_1S);

  Wire.begin(CELL_ADDRESS);
  Wire.onRequest(requested);
  Wire.onReceive(received);

  Adc::setup();
  Measurements::begin();
}

void loop() {
  Adc::update();
  wdt_reset();

  voltage = Measurements::value(Measurements::VOLTAGE);
  temp = Measurements::value(Measurements::TEMP);

  uint8_t balancing = balancer.update();
  if (balancing) {
    status |= (1 << 0);
  }
  // if (!balancing) {
  //   sleep();
  // }
}

// I2C

void requested() {
  switch (reg) {
  case REGISTER_STATUS:
    Wire.write(status);
    break;
  case REGISTER_VOLTAGE_HIGH:
    Wire.write((voltage >> 8) & 0xFF);
    Wire.write(voltage & 0xFF);
    break;
  case REGISTER_VOLTAGE_LOW:
    Wire.write(voltage & 0xFF);
    break;
  case REGISTER_TEMP_HIGH:
    Wire.write((temp >> 8) & 0xFF);
    Wire.write(temp & 0xFF);
    break;
  case REGISTER_TEMP_LOW:
    Wire.write(temp & 0xFF);
    break;
  }

  reg = 0;
}

void received(int bytes) {
  if (bytes < 1) {
    reg = 0;
    return;
  }

  reg = Wire.read();
  bytes--;

  while (bytes > 0) {
    // FIXME: handle multiple written bytes
    // TODO: handle updates to writable registers
    value = Wire.read();
    bytes--;
  }

  // clear the Wire RX buffer
  while (Wire.available()) {
    Wire.read();
  }
}
