#pragma once

#include <Arduino.h>

class Ky008LaserEmitter {
 public:
  explicit Ky008LaserEmitter(uint8_t pin);

  void begin(bool initialEnabled = false);
  void setEnabled(bool enabled);
  void turnOn();
  void turnOff();
  bool isEnabled() const;

 private:
  uint8_t pin_;
  bool enabled_;
};

class Ky008LaserSensor {
 public:
  explicit Ky008LaserSensor(uint8_t pin, uint8_t inputMode = INPUT,
                            bool beamDetectedState = HIGH);

  void begin();
  bool isBeamDetected() const;
  bool isBeamBroken() const;
  int readRaw() const;

 private:
  uint8_t pin_;
  uint8_t inputMode_;
  bool beamDetectedState_;
};