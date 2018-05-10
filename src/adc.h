#ifndef __ADC_H__
#define __ADC_H__

namespace adc {
  void init();
  uint16_t ref_voltage();
  void set_ref_voltage(uint16_t ref_voltage);
  void calibrate_voltage(uint16_t voltage);
  uint16_t read_vcc();
  uint16_t read_temp();
}

#endif
