#include <Arduino.h>
#include <SoftwareSerial.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

#include "config.h"
#include "adc.h"
#include "balance.h"
#include "command.h"

#define adc_disable() (ADCSRA &= ~(1 << ADEN))
#define adc_enable() (ADCSRA |=  (1 << ADEN))

SoftwareSerial serial(RX_PIN, TX_PIN);
CommandProcessor commands(CELL_ADDRESS, serial);
Balancer balancer(BALANCE_PIN);

void process_input() {
  uint8_t command = commands.update();

  if (command) {
    if (command & COMMAND_SEND_VOLTAGE) {
      commands.reply(adc_read_vcc());
    }

    if (command & COMMAND_SEND_TEMP) {
      commands.reply(adc_read_temp());
    }

    if (command & COMMAND_BALANCE_ON) {
      balancer.enable();
    } else {
      balancer.disable();
    }
  }
}

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
  pinMode(THERMISTOR_PIN, INPUT);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // 1 second watchdog
  wdt_enable(WDTO_1S);

  serial.begin(9600);
}

void loop() {
  process_input();
  wdt_reset();

  uint8_t balancing = balancer.update();
  if (!balancing) {
    sleep();
  }
}
