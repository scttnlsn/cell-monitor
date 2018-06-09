#include <Arduino.h>
#include <SoftwareSerial.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

#include "config.h"
#include "adc.h"
#include "balance.h"
#include "communication.h"
#include "protocol.h"

#define adc_disable() (ADCSRA &= ~(1 << ADEN))
#define adc_enable() (ADCSRA |=  (1 << ADEN))

SoftwareSerial serial(RX_PIN, TX_PIN);
Balancer balancer(BALANCE_PIN);
Communication comm(serial);
Protocol protocol(&comm);

void sleep() {
  adc_disable();
  wdt_reset();
  wdt_disable();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();
  sleep_cpu();

  // wake up
  sleep_disable();
  adc_enable();
  wdt_enable(WDTO_1S);
  wdt_reset();
}

void setup() {
  pinMode(THERMISTOR_PIN, INPUT);

  // 1 second watchdog
  wdt_enable(WDTO_1S);

  serial.begin(9600);

  adc::init();
}

void loop() {
  uint32_t now = millis();

  if (comm.update(now)) {
    uint8_t *data = comm.data();

    packet_t packet;
    if (protocol.handle(data, &packet)) {
      switch (packet.reg) {
        case REG_VOLTAGE_REF:
          if (packet.write) {
            adc::set_ref_voltage(packet.value);
          }
          protocol.respond(&packet, adc::ref_voltage());
          break;

        case REG_VOLTAGE:
          if (packet.write) {
            adc::calibrate_voltage(packet.value);
          }
          protocol.respond(&packet, adc::read_vcc());
          break;

        case REG_TEMP:
          if (packet.write) {
            // TODO: calibrate temp
          }
          protocol.respond(&packet, adc::read_temp());
          break;

        case REG_BALANCE:
          if (packet.write) {
            if (packet.value) {
              balancer.enable();
            } else {
              balancer.disable();
            }
          }
          protocol.respond(&packet, balancer.enabled());
          break;
      }
    }
  }

  wdt_reset();

  uint8_t balancing = balancer.update();
  if (!balancing && !serial.available() && comm.empty()) {
    sleep();
  }
}
