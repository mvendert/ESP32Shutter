#pragma once

#include "IShutterTrigger.h"
#include "ITriggerInput.h"

// Adapts a level-based ITriggerInput (e.g. a debounced push-button) into a
// one-shot IShutterTrigger. A fire request is generated on the released->held
// rising edge of the underlying input.
class ButtonShutterTrigger : public IShutterTrigger {
 public:
  explicit ButtonShutterTrigger(ITriggerInput& input);

  void begin() override;
  bool consumeFireRequest() override;

 private:
  ITriggerInput& input_;
  bool lastHeld_;
};
