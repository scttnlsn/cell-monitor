#ifndef __MEASUREMENTS_H__
#define __MEASUREMENTS_H__

#include <stdint.h>

namespace Measurements {
  typedef enum {
    VOLTAGE = 0,
    TEMP = 1
  } measurement_t;

  void begin();
  uint16_t value(measurement_t measurement);
}

#endif
