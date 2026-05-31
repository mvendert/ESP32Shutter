#pragma once

#include <Arduino.h>

#include "IShutterTrigger.h"
#include "ITriggerInput.h"

// Adapts a level-based ITriggerInput (e.g. a debounced push-button) into a
// one-shot IShutterTrigger. A fire request is generated on the released->held
// rising edge of the underlying input.
class ButtonShutterTrigger : public IShutterTrigger {
 public:
  ButtonShutterTrigger(ITriggerInput& input, uint32_t minimumIntervalMs);

  void begin() override;
  bool consumeFireRequest() override;
  uint32_t minimumIntervalMs() const override;

 private:
  ITriggerInput& input_;
  uint32_t minimumIntervalMs_;
  bool lastHeld_;
};
