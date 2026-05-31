#include "ShutterApp.h"

ShutterApp::ShutterApp(ITriggerInput& focusButton,
                       IShutterTrigger* const* shutterTriggers,
                       size_t shutterTriggerCount,
                       IShutterController& shutterController,
                       uint32_t telemetryIntervalMs,
                       uint32_t shutterPulseMs)
    : focusButton_(focusButton),
      shutterTriggers_(shutterTriggers),
      shutterTriggerCount_(shutterTriggerCount),
      shutterController_(shutterController),
      telemetryIntervalMs_(telemetryIntervalMs),
      shutterPulseMs_(shutterPulseMs),
      triggerStates_(),
      lastTelemetryTimeMs_(0),
      pulseStartTimeMs_(0),
      shutterActive_(false) {}

void ShutterApp::begin() {
  focusButton_.begin();

  if (shutterTriggerCount_ > MAX_SHUTTER_TRIGGERS) {
    Serial.print("Too many shutter triggers configured; using first ");
    Serial.println(MAX_SHUTTER_TRIGGERS);
  }

  for (size_t i = 0; i < activeTriggerCount(); ++i) {
    if (shutterTriggers_[i] != nullptr) {
      shutterTriggers_[i]->begin();
    }
  }
  shutterController_.begin();
  Serial.println("Shutter app ready");
}

size_t ShutterApp::activeTriggerCount() const {
  return shutterTriggerCount_ < MAX_SHUTTER_TRIGGERS ? shutterTriggerCount_
                                                     : MAX_SHUTTER_TRIGGERS;
}

void ShutterApp::update() {
  const uint32_t now = millis();

  // --- Focus: level-based, follows the focus input directly.
  const bool focusHeld = focusButton_.isHeld();
  shutterController_.setFocus(focusHeld);

  // --- Shutter triggers: poll every source so internal edge state advances,
  // even if we won't act on the request because of cooldown/active pulse.
  bool fireAccepted = false;
  uint32_t acceptedCooldownMs = 0;
  for (size_t i = 0; i < activeTriggerCount(); ++i) {
    IShutterTrigger* trigger = shutterTriggers_[i];
    if (trigger == nullptr) continue;
    if (!trigger->consumeFireRequest()) continue;

    TriggerState& triggerState = triggerStates_[i];
    const uint32_t triggerCooldownMs = trigger->minimumIntervalMs();
    const bool cooldownElapsed =
        !triggerState.hasFiredOnce ||
        (now - triggerState.lastFireTimeMs) >= triggerCooldownMs;

    if (!fireAccepted && !shutterActive_ && cooldownElapsed) {
      shutterActive_ = true;
      pulseStartTimeMs_ = now;
      triggerState.lastFireTimeMs = now;
      triggerState.hasFiredOnce = true;
      acceptedCooldownMs = triggerCooldownMs;
      shutterController_.setShutter(true);
      Serial.print("Shutter: FIRE, trigger_cooldown_ms=");
      Serial.println(acceptedCooldownMs);
      fireAccepted = true;
    }
  }

  if (!fireAccepted && shutterActive_ &&
      (now - pulseStartTimeMs_) >= shutterPulseMs_) {
    shutterActive_ = false;
    shutterController_.setShutter(false);
  } else if (!fireAccepted && !shutterActive_) {
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
    Serial.print(", triggers=");
    Serial.println(activeTriggerCount());
  }
}
