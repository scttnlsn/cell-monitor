#include "communication.h"
#include "utils.h"

Communication::Communication(Stream &stream) : _stream(stream) {
  _index = 0;
}

bool Communication::update(uint32_t tick) {
  while (_stream.available()) {
    uint8_t byte = _stream.read();

    _buffer[_index] = byte;
    _index++;
    _tick = tick;
  }

  if (_index == (PACKET_LENGTH + 1)) {
    _index = 0;

    uint8_t crc = utils::crc8(_buffer, PACKET_LENGTH);
    if (crc == _buffer[PACKET_LENGTH]) {
      return true;
    } else {
      // CRC failed, what do we do?
    }
  }

  if (tick - _tick > TIMEOUT) {
    // we've timed out waiting for the rest of the packet
    // next byte received will be interpreted as the first byte of a new packet
    _index = 0;
  }

  return false;
}

bool Communication::empty() {
  return _index == 0;
}

uint8_t *Communication::data() {
  return _buffer;
}

void Communication::send(uint8_t *buffer) {
  for (uint8_t i = 0; i < PACKET_LENGTH; i++) {
    _stream.write(buffer[i]);
  }

  uint8_t crc = utils::crc8(buffer, PACKET_LENGTH);
  _stream.write(crc);

  _stream.flush();
}
