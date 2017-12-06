#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#define adc_disable() (ADCSRA &= ~(1 << ADEN))
#define adc_enable() (ADCSRA |=  (1 << ADEN))

namespace Adc {
  typedef void (*callback_t)(uint16_t);

  void setup();
  void update();
  void read(uint8_t mux, callback_t callback);
}

#endif
