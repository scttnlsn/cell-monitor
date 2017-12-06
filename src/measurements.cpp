#include <Arduino.h>

#include "adc.h"
#include "config.h"
#include "measurements.h"

#define NUM_MEASUREMENTS 2

namespace Measurements {
  uint16_t measurements[NUM_MEASUREMENTS];
  measurement_t current_measurement = VOLTAGE;

  uint8_t adc_muxes[NUM_MEASUREMENTS] = {
    _BV(MUX3) | _BV(MUX2), // internal bandgap reference
    _BV(MUX1) // PB4
  };

  uint16_t calculate_voltage(uint16_t adc_value);
  uint16_t calculate_temp(uint16_t adc_value);

  uint16_t (*calculations[NUM_MEASUREMENTS])(uint16_t adc_value) = {
    &calculate_voltage,
    &calculate_temp
  };

  void adc_callback(uint16_t value) {
    uint16_t (*calculate)(uint16_t) = calculations[current_measurement];
    measurements[current_measurement] = calculate(value);
    current_measurement = (measurement_t) ((current_measurement + 1) % NUM_MEASUREMENTS);
    begin();
  }

  void begin() {
    uint8_t mux = adc_muxes[current_measurement];
    Adc::read(mux, &adc_callback);
  }

  uint16_t value(measurement_t measurement) {
    return measurements[measurement];
  }

  // calculations

  uint16_t calculate_voltage(uint16_t adc_value) {
    return (REF_VOLTAGE * 1023L) / adc_value;
  }

  uint16_t calculate_temp(uint16_t adc_value) {
    float resistance = THERMISTOR_NOMINAL / (1024 / adc_value - 1);

    // 1/T = 1/T0 + 1/B * ln(R/R0)
    float steinhart;

    steinhart = log(resistance / THERMISTOR_NOMINAL) / THERMISTOR_COEF; // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMP_NOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart; // inverse
    steinhart -= 273.15; // celcius

    return steinhart * 100;
  }
}
