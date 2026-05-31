#pragma once

#include <Arduino.h>

#include "IShutterController.h"
#include "IShutterTrigger.h"
#include "ITriggerInput.h"

// Coordinates a focus input (level-based) with one or more shutter triggers
// (edge-based, one-shot). Each accepted fire request produces a short shutter
// pulse on the controller, subject to the requesting trigger's minimum interval.
class ShutterApp {
 public:
  ShutterApp(ITriggerInput& focusButton,
             IShutterTrigger* const* shutterTriggers,
             size_t shutterTriggerCount,
             IShutterController& shutterController,
             uint32_t telemetryIntervalMs,
             uint32_t shutterPulseMs);

  void begin();
  void update();

 private:
  struct TriggerState {
    uint32_t lastFireTimeMs;
    bool hasFiredOnce;
  };

  static constexpr size_t MAX_SHUTTER_TRIGGERS = 8;

  size_t activeTriggerCount() const;

  ITriggerInput& focusButton_;
  IShutterTrigger* const* shutterTriggers_;
  size_t shutterTriggerCount_;
  IShutterController& shutterController_;
  uint32_t telemetryIntervalMs_;
  uint32_t shutterPulseMs_;
  TriggerState triggerStates_[MAX_SHUTTER_TRIGGERS];

  uint32_t lastTelemetryTimeMs_;
  uint32_t pulseStartTimeMs_;
  bool shutterActive_;
};
