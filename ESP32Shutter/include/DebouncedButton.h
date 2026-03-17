#pragma once

#include <Arduino.h>

#include "ITriggerInput.h"

class DebouncedButton : public ITriggerInput {
 public:
  DebouncedButton(uint8_t pin, uint32_t debounceMs);

  void begin() override;
  bool isHeld() override;

 private:
  uint8_t pin_;
  uint32_t debounceMs_;
  bool lastRawState_;
  bool stableState_;
  uint32_t lastChangeTimeMs_;
};
