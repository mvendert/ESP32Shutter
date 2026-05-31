#pragma once

#include <Arduino.h>

// Edge-based, one-shot fire source for the shutter.
//
// Unlike ITriggerInput (which exposes a level signal via isHeld()), an
// IShutterTrigger reports discrete "fire" events. Each call to
// consumeFireRequest() returns true at most once per logical event and then
// clears the request until the next event occurs. Each trigger also owns the
// minimum interval between its accepted fire requests.
class IShutterTrigger {
 public:
  virtual ~IShutterTrigger() = default;
  virtual void begin() = 0;

  // Returns true exactly once for each fire event that has occurred since the
  // previous call. Implementations must internally clear the request after
  // returning true.
  virtual bool consumeFireRequest() = 0;

  // Minimum time between accepted fire requests from this trigger.
  virtual uint32_t minimumIntervalMs() const = 0;
};
