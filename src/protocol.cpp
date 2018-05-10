#include "protocol.h"

void encode(uint8_t *buffer, packet_t *packet);
void decode(uint8_t *buffer, packet_t *packet);

Protocol::Protocol(Communication *comm) : _comm(comm) {
  _address = -1;
}

// returns `true` if the caller needs to send a response
bool Protocol::handle(uint8_t *data, packet_t *packet) {
  decode(data, packet);

  if (packet->address != ADDRESS_BROADCAST && packet->address != _address) {
    // forward the packet unchanged
    _comm->send(data);
    return false;
  }

  if (!packet->request) {
    // forward responses unchanged
    _comm->send(data);
    return false;
  }

  if (packet->reg == REG_ADDRESS) {
    if (packet->write && packet->address == ADDRESS_BROADCAST) {
      // this is a special case of address assignment
      _address = packet->value;
      packet->value += 1;
      forward(packet);
    } else {
      respond(packet, _address);
    }

    // handle the address register internally here
    return false;
  }

  // all other registers should be handled externally
  return true;
}

void Protocol::forward(packet_t *packet) {
  uint8_t data[PACKET_LENGTH];
  encode(data, packet);
  _comm->send(data);
}

void Protocol::respond(packet_t *packet, uint16_t value) {
  packet->request = 0;
  packet->value = value;
  forward(packet);
}

// 4 byte packet structure (msb to lsb):
//
// address (7 bits)
// request=1/response=0 flag (1 bit)
// reg (7 bits)
// write=1/read=0 flag (1 bit)
// value (16 bits)

void decode(uint8_t *buffer, packet_t *packet) {
  packet->address = buffer[0] >> 1;
  packet->request = buffer[0] & 0x1;
  packet->reg = buffer[1] >> 1;
  packet->write = buffer[1] & 0x1;
  packet->value = (buffer[2] << 8) | buffer[3];
}

void encode(uint8_t *buffer, packet_t *packet) {
  buffer[0] = (packet->address << 1) | (packet->request & 0x1);
  buffer[1] = (packet->reg << 1) | (packet->write & 0x1);
  buffer[2] = packet->value >> 8;
  buffer[3] = packet->value & 0xFF;
}
