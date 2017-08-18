#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <Arduino.h>
#include <Stream.h>

class CommandProcessor {
 public:
  CommandProcessor(uint8_t address, Stream &stream);
  uint8_t update();
  void reply(uint16_t value);

 private:
  uint8_t _address;
  Stream &_stream;
};

#endif
