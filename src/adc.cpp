#include <Arduino.h>
#include <avr/eeprom.h>

#include "adc.h"
#include "config.h"

// ATTiny internal voltage reference
#define BANDGAP_NOMINAL_VOLTAGE 1100L

// 4^(12bit - 10bit) = 16 samples
#define NUM_SAMPLES 16

// 10bit ADC w/ 2bit oversampling = 12bit
#define ADC_MAX 4095L

namespace adc {
  uint32_t _ref_voltage = BANDGAP_NOMINAL_VOLTAGE;

  // returns a single 10-bit reading of the bandgap voltage
  uint16_t read_bandgap() {
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
    return (high << 8) | low;
  }

  // returns an oversampled 12-bit reading of the bandgap voltage
  uint16_t sample_bandgap() {
    uint32_t sum = 0;
    for (uint16_t i = 0; i < NUM_SAMPLES; i++) {
      sum += read_bandgap();
    }

    // shift by 12bit - 10bit = 2bits
    uint16_t bandgap = sum >> 2;
    return bandgap;
  }

  void init() {
    _ref_voltage = eeprom_read_word((uint16_t *)0x0);
    if ((int) _ref_voltage == -1) {
      _ref_voltage = BANDGAP_NOMINAL_VOLTAGE;
    }
  }

  uint16_t ref_voltage() {
    return _ref_voltage;
  }

  void set_ref_voltage(uint16_t ref_voltage) {
    _ref_voltage = ref_voltage;
    eeprom_write_word((uint16_t *) 0x0, _ref_voltage);
  }

  void calibrate_voltage(uint16_t vcc_voltage) {
    uint32_t numerator = (uint32_t) vcc_voltage * (uint32_t) sample_bandgap();
    uint32_t ref_voltage = numerator / (uint32_t) ADC_MAX;
    set_ref_voltage((uint16_t) ref_voltage);
  }

  uint16_t read_vcc() {
    uint16_t bandgap = sample_bandgap();
    return ((uint32_t) _ref_voltage * ADC_MAX) / bandgap;
  }
}
