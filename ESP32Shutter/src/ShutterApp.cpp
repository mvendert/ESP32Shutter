#include "ShutterApp.h"

ShutterApp::ShutterApp(ITriggerInput& focusButton, ITriggerInput& shutterButton,
                       IShutterController& shutterController,
                       uint32_t telemetryIntervalMs)
    : focusButton_(focusButton),
      shutterButton_(shutterButton),
      shutterController_(shutterController),
      telemetryIntervalMs_(telemetryIntervalMs),
      lastTelemetryTimeMs_(0),
      lastFocusHeld_(false),
      lastShutterHeld_(false) {}

void ShutterApp::begin() {
  focusButton_.begin();
  shutterButton_.begin();
  shutterController_.begin();
  Serial.println("Shutter app ready");
}

void ShutterApp::update() {
  const bool focusHeld = focusButton_.isHeld();
  const bool shutterHeld = shutterButton_.isHeld();

  shutterController_.setFocus(focusHeld);
  shutterController_.setShutter(shutterHeld);

  const uint32_t now = millis();
  if (now - lastTelemetryTimeMs_ >= telemetryIntervalMs_) {
    lastTelemetryTimeMs_ = now;
    Serial.print("focus=");
    Serial.print(focusHeld ? "ON" : "OFF");
    Serial.print(", shutter=");
    Serial.println(shutterHeld ? "ON" : "OFF");
  }

  lastFocusHeld_ = focusHeld;
  lastShutterHeld_ = shutterHeld;
}
