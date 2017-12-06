#ifndef __BALANCE_H__
#define __BALANCE_H__

#ifndef BALANCE_TIMEOUT
#define BALANCE_TIMEOUT 10000
#endif

class Balancer {
 public:
  Balancer(uint8_t pin);
  void enable();
  void disable();
  uint8_t update();
  uint8_t enabled();

 private:
  uint8_t _pin;
  uint8_t _enabled;
  uint32_t _enabled_time;
};

#endif
