#include "command.h"

CommandProcessor::CommandProcessor(uint8_t address, Stream &stream) : _address(address), _stream(stream) {
}

uint8_t CommandProcessor::update() {
  if (_stream.available()) {
    uint8_t value = _stream.read();

    if (value > 0) {
      uint8_t address = value >> 4;

      if (address == _address) {
        uint8_t command = value & 0xF;
        return command;
      }
    }
  }

  return 0;
}

void CommandProcessor::reply(uint16_t value) {
  _stream.write(value >> 8); // MSB
  _stream.write(value & 0xFF); // LSB
}
