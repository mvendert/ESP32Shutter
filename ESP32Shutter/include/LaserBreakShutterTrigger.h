#pragma once

#include <Arduino.h>

#include "IShutterTrigger.h"
#include "Ky008LaserSensor.h"

// Generates a one-shot fire request when a laser beam transitions from
// "detected" to "broken". After firing the trigger is disarmed and will only
// re-arm once the beam is detected again, preventing repeated firings while
// the beam remains broken.
class LaserBreakShutterTrigger : public IShutterTrigger {
 public:
  LaserBreakShutterTrigger(Ky008LaserSensor& sensor, uint32_t minimumIntervalMs);

  void begin() override;
  bool consumeFireRequest() override;
  uint32_t minimumIntervalMs() const override;

 private:
  Ky008LaserSensor& sensor_;
  uint32_t minimumIntervalMs_;
  bool armed_;
  bool lastDetected_;
};
