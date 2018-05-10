#include <Arduino.h>

#include "balance.h"

Balancer::Balancer(uint8_t pin) : _pin(pin) {
  _enabled_time = 0;
}

void Balancer::enable() {
  _enabled_time = millis();
  digitalWrite(_pin, HIGH);
  _enabled = 1;
}

void Balancer::disable() {
  digitalWrite(_pin, LOW);
  _enabled = 0;
}

bool Balancer::enabled() {
  return _enabled;
}

bool Balancer::update() {
  uint32_t now = millis();
  uint32_t balancing_time = 0;

  if (now < _enabled_time) {
    balancing_time = _enabled_time - now;
  } else {
    balancing_time = now - _enabled_time;
  }

  if (balancing_time >= BALANCE_TIMEOUT) {
    disable();
  }

  return enabled();
}
