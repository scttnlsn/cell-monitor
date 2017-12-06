#include <avr/io.h>
#include <avr/interrupt.h>
#include <Timer.h>

#include "adc.h"

#define NO_VALUE ((uint16_t) -1)

namespace Adc {
  Timer _timer;

  volatile callback_t _callback;
  volatile uint16_t _value = NO_VALUE;

  void start_conversion() {
    ADCSRA |= _BV(ADSC);
  }

  void setup() {
    adc_enable();

    // enable interrupt
    ADCSRA |= _BV(ADIE);

    sei();
  }

  void update() {
    _timer.update();

    if (_callback && _value != NO_VALUE) {
      _callback(_value);
      _value = NO_VALUE;
    }
  }

  void read(uint8_t mux, callback_t callback) {
    _callback = callback;
    ADMUX = mux;

    // wait for MUX to settle
    _timer.after(2, &start_conversion);
  }
}

ISR(ADC_vect) {
  cli();

  uint8_t low  = ADCL; // must read ADCL first
  uint8_t high = ADCH;
  Adc::_value =  (high << 8) | low;

  sei();
}
