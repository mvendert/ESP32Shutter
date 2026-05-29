#pragma once

#include <Arduino.h>

#include "IShutterController.h"
#include "IShutterTrigger.h"
#include "ITriggerInput.h"

// Coordinates a focus input (level-based) with one or more shutter triggers
// (edge-based, one-shot). Each fire request from any trigger produces a short
// shutter pulse on the controller, subject to a configurable minimum interval
// between consecutive firings.
class ShutterApp {
 public:
  ShutterApp(ITriggerInput& focusButton,
             IShutterTrigger* const* shutterTriggers,
             size_t shutterTriggerCount,
             IShutterController& shutterController,
             uint32_t telemetryIntervalMs,
             uint32_t shutterPulseMs,
             uint32_t minIntervalMs);

  void begin();
  void update();

  // Cooldown between consecutive shutter firings.
  void setMinimumIntervalMs(uint32_t intervalMs);
  void setMinimumIntervalSeconds(uint32_t seconds);
  uint32_t minimumIntervalMs() const { return minIntervalMs_; }

 private:
  ITriggerInput& focusButton_;
  IShutterTrigger* const* shutterTriggers_;
  size_t shutterTriggerCount_;
  IShutterController& shutterController_;
  uint32_t telemetryIntervalMs_;
  uint32_t shutterPulseMs_;
  uint32_t minIntervalMs_;

  uint32_t lastTelemetryTimeMs_;
  uint32_t lastFireTimeMs_;
  uint32_t pulseStartTimeMs_;
  bool hasFiredOnce_;
  bool shutterActive_;
  bool lastFocusHeld_;
};
