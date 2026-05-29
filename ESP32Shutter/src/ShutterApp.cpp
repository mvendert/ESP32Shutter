#include "ShutterApp.h"

ShutterApp::ShutterApp(ITriggerInput& focusButton,
                       IShutterTrigger* const* shutterTriggers,
                       size_t shutterTriggerCount,
                       IShutterController& shutterController,
                       uint32_t telemetryIntervalMs,
                       uint32_t shutterPulseMs,
                       uint32_t minIntervalMs)
    : focusButton_(focusButton),
      shutterTriggers_(shutterTriggers),
      shutterTriggerCount_(shutterTriggerCount),
      shutterController_(shutterController),
      telemetryIntervalMs_(telemetryIntervalMs),
      shutterPulseMs_(shutterPulseMs),
      minIntervalMs_(minIntervalMs),
      lastTelemetryTimeMs_(0),
      lastFireTimeMs_(0),
      pulseStartTimeMs_(0),
      hasFiredOnce_(false),
      shutterActive_(false),
      lastFocusHeld_(false) {}

void ShutterApp::begin() {
  focusButton_.begin();
  for (size_t i = 0; i < shutterTriggerCount_; ++i) {
    if (shutterTriggers_[i] != nullptr) {
      shutterTriggers_[i]->begin();
    }
  }
  shutterController_.begin();
  Serial.println("Shutter app ready");
}

void ShutterApp::setMinimumIntervalMs(uint32_t intervalMs) {
  minIntervalMs_ = intervalMs;
}

void ShutterApp::setMinimumIntervalSeconds(uint32_t seconds) {
  minIntervalMs_ = seconds * 1000UL;
}

void ShutterApp::update() {
  const uint32_t now = millis();

  // --- Focus: level-based, follows the focus input directly.
  const bool focusHeld = focusButton_.isHeld();
  shutterController_.setFocus(focusHeld);

  // --- Shutter triggers: poll every source so internal edge state advances,
  // even if we won't act on the request because of cooldown/active pulse.
  bool fireRequested = false;
  for (size_t i = 0; i < shutterTriggerCount_; ++i) {
    if (shutterTriggers_[i] == nullptr) continue;
    if (shutterTriggers_[i]->consumeFireRequest()) {
      fireRequested = true;
    }
  }

  const bool cooldownElapsed =
      !hasFiredOnce_ || (now - lastFireTimeMs_) >= minIntervalMs_;

  if (fireRequested && !shutterActive_ && cooldownElapsed) {
    shutterActive_ = true;
    pulseStartTimeMs_ = now;
    lastFireTimeMs_ = now;
    hasFiredOnce_ = true;
    shutterController_.setShutter(true);
    Serial.println("Shutter: FIRE");
  } else if (shutterActive_ && (now - pulseStartTimeMs_) >= shutterPulseMs_) {
    shutterActive_ = false;
    shutterController_.setShutter(false);
  } else if (!shutterActive_) {
    // Keep the controller line low between pulses.
    shutterController_.setShutter(false);
  }

  // --- Telemetry
  if (now - lastTelemetryTimeMs_ >= telemetryIntervalMs_) {
    lastTelemetryTimeMs_ = now;
    Serial.print("focus=");
    Serial.print(focusHeld ? "ON" : "OFF");
    Serial.print(", shutter=");
    Serial.print(shutterActive_ ? "ON" : "OFF");
    Serial.print(", cooldown_ms=");
    Serial.println(minIntervalMs_);
  }

  lastFocusHeld_ = focusHeld;
}
