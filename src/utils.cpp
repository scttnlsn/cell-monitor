#include "utils.h"

namespace utils {
  uint8_t crc8(const uint8_t *buffer, size_t length) {
    uint8_t crc = 0;

    for (uint8_t i = 0; i < length; i++) {
      uint8_t data = crc ^ buffer[i];

      for (uint8_t j = 0; j < 8; j++ ) {
        if (data & 0x80) {
          data <<= 1;
          data ^= 0x07;
        } else {
          data <<= 1;
        }
      }

      crc = data;
    }

    return crc;
  }
}
