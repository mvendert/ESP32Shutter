#pragma once

#include "IShutterTrigger.h"
#include "Ky008LaserSensor.h"

// Generates a one-shot fire request when a laser beam transitions from
// "detected" to "broken". After firing the trigger is disarmed and will only
// re-arm once the beam is detected again, preventing repeated firings while
// the beam remains broken.
class LaserBreakShutterTrigger : public IShutterTrigger {
 public:
  explicit LaserBreakShutterTrigger(Ky008LaserSensor& sensor);

  void begin() override;
  bool consumeFireRequest() override;

 private:
  Ky008LaserSensor& sensor_;
  bool armed_;
  bool lastDetected_;
};
