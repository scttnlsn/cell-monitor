#include <Arduino.h>

#include "adc.h"
#include "config.h"

static uint16_t adc_read_bandgap() {
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

uint16_t adc_read_vcc() {
  uint16_t result;

  for (int i = 0; i < ADC_NUM_SAMPLES; i++) {
    result += adc_read_bandgap();
  }

  result /= ADC_NUM_SAMPLES;

  return (REF_VOLTAGE * 1023L) / result;
}

uint16_t adc_read_temp() {
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
