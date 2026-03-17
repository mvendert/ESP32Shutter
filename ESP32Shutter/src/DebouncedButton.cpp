#include "DebouncedButton.h"

DebouncedButton::DebouncedButton(uint8_t pin, uint32_t debounceMs)
    : pin_(pin),
      debounceMs_(debounceMs),
      lastRawState_(false),
      stableState_(false),
      lastChangeTimeMs_(0) {}

void DebouncedButton::begin() { pinMode(pin_, INPUT_PULLUP); }

bool DebouncedButton::isHeld() {
  const bool rawPressed = (digitalRead(pin_) == LOW);

  if (rawPressed != lastRawState_) {
    lastRawState_ = rawPressed;
    lastChangeTimeMs_ = millis();
  }

  if ((millis() - lastChangeTimeMs_) >= debounceMs_) {
    stableState_ = rawPressed;
  }

  return stableState_;
}
