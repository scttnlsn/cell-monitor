#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <Arduino.h>
#include <Stream.h>

#define COMMAND_SEND_VOLTAGE (1 << 0)
#define COMMAND_BALANCE_ON (1 << 1)

class CommandProcessor {
 public:
  CommandProcessor(uint8_t address, Stream &stream);
  uint8_t update();

 private:
  uint8_t _address;
  Stream &_stream;
};

#endif
