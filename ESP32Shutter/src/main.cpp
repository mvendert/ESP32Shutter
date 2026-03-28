#include <Arduino.h>

#include "Config.h"
#include "DebouncedButton.h"
#include "OptocouplerShutterController.h"
#include "ShutterApp.h"

namespace {
DebouncedButton focusButton(Config::FOCUS_BUTTON_PIN, Config::BUTTON_DEBOUNCE_MS);
DebouncedButton shutterButton(Config::SHUTTER_BUTTON_PIN, Config::BUTTON_DEBOUNCE_MS);
OptocouplerShutterController shutterController(Config::FOCUS_PIN, Config::SHUTTER_PIN,
                                                Config::FOCUS_LED_PIN, Config::SHUTTER_LED_PIN);
ShutterApp app(focusButton, shutterButton, shutterController,
               Config::TELEMETRY_INTERVAL_MS);
}  // namespace

void setup() {
  Serial.begin(115200);
  app.begin();
}

void loop() {
  app.update();
}