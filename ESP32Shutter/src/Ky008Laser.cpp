#include "Ky008Laser.h"

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

Ky008LaserSensor::Ky008LaserSensor(uint8_t pin, uint8_t inputMode,
                                   bool beamDetectedState)
    : pin_(pin), inputMode_(inputMode), beamDetectedState_(beamDetectedState) {}

void Ky008LaserSensor::begin() { pinMode(pin_, inputMode_); }

bool Ky008LaserSensor::isBeamDetected() const {
  return readRaw() == (beamDetectedState_ ? HIGH : LOW);
}

bool Ky008LaserSensor::isBeamBroken() const { return !isBeamDetected(); }

int Ky008LaserSensor::readRaw() const { return digitalRead(pin_); }