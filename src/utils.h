#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stddef.h>

namespace utils {
  uint8_t crc8(const uint8_t *buffer, size_t length);
}

#endif
