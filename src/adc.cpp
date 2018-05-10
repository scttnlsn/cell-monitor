#include <Arduino.h>

#include "adc.h"
#include "config.h"

#define BANDGAP_NOMINAL_VOLTAGE 1100L

namespace adc {
  uint32_t _ref_voltage = BANDGAP_NOMINAL_VOLTAGE;

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
    eeprom_write_word((uint16_t *)0x0, _ref_voltage);
  }

  void calibrate_voltage(uint16_t voltage) {
    uint16_t ref_voltage = (uint32_t)(voltage * _ref_voltage) / read_vcc();
    set_ref_voltage(ref_voltage);
  }

  uint16_t read_vcc() {
    uint16_t result;

    for (int i = 0; i < ADC_NUM_SAMPLES; i++) {
      result += read_bandgap();
    }

    result /= ADC_NUM_SAMPLES;

    return (_ref_voltage * 1023L) / result;
  }

  uint16_t read_temp() {
    float result;

    for (int i = 0; i < ADC_NUM_SAMPLES; i++) {
      result += analogRead(THERMISTOR_PIN);
    }

    result /= ADC_NUM_SAMPLES;

    float resistance = THERMISTOR_NOMINAL / (1024 / result - 1);

    // 1/T = 1/T0 + 1/B * ln(R/R0)
    float steinhart;

    steinhart = log(resistance / THERMISTOR_NOMINAL) / THERMISTOR_COEF; // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMP_NOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart; // inverse
    steinhart -= 273.15; // celcius

    return steinhart * 100;
  }
}
