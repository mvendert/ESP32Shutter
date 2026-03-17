#pragma once

#include <Arduino.h>

#include "IShutterController.h"

class OptocouplerShutterController : public IShutterController {
 public:
  OptocouplerShutterController(uint8_t focusPin, uint8_t shutterPin);

  void begin() override;
  void setFocus(bool active) override;
  void setShutter(bool active) override;

 private:
  uint8_t focusPin_;
  uint8_t shutterPin_;
  bool focusActive_;
  bool shutterActive_;
};
