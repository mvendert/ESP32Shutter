#pragma once

#include <Arduino.h>

#include "IShutterController.h"
#include "ITriggerInput.h"

class ShutterApp {
 public:
  ShutterApp(ITriggerInput& focusButton, ITriggerInput& shutterButton,
             IShutterController& shutterController, uint32_t telemetryIntervalMs);

  void begin();
  void update();

 private:
  ITriggerInput& focusButton_;
  ITriggerInput& shutterButton_;
  IShutterController& shutterController_;
  uint32_t telemetryIntervalMs_;
  uint32_t lastTelemetryTimeMs_;
  bool lastFocusHeld_;
  bool lastShutterHeld_;
};
