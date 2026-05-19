#include "Ky008LaserEmitter.h"

Ky008LaserEmitter::Ky008LaserEmitter(uint8_t pin) : pin_(pin), enabled_(false) {}

void Ky008LaserEmitter::begin(bool initialEnabled) {
  pinMode(pin_, OUTPUT);
  setEnabled(initialEnabled);
}

void Ky008LaserEmitter::setEnabled(bool enabled) {
  enabled_ = enabled;
  digitalWrite(pin_, enabled ? HIGH : LOW);
}

void Ky008LaserEmitter::turnOn() { setEnabled(true); }

void Ky008LaserEmitter::turnOff() { setEnabled(false); }

bool Ky008LaserEmitter::isEnabled() const { return enabled_; }
