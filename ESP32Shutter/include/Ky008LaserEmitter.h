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
