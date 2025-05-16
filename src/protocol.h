#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "communication.h"

#define ADDRESS_BROADCAST 0x0

#define REG_ADDRESS 0x1
#define REG_VOLTAGE 0x2
#define REG_BALANCE 0x3

typedef struct {
  uint8_t id;
  uint8_t address;
  uint8_t request;
  uint8_t reg;
  uint8_t write;
  uint16_t value;
} packet_t;

class Protocol {
 public:
  Protocol(Communication *comm);
  bool handle(uint8_t *data, packet_t *packet);
  void respond(packet_t *packet, uint16_t value);

 private:
   Communication *_comm;
   uint8_t _address;
   void forward(packet_t *packet);
};

#endif
