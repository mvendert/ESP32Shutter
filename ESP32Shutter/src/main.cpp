#include <Arduino.h>

#include "ButtonShutterTrigger.h"
#include "Config.h"
#include "DebouncedButton.h"
#include "IShutterTrigger.h"
#include "Ky008LaserSensor.h"
#include "LaserBreakShutterTrigger.h"
#include "OptocouplerShutterController.h"
#include "ShutterApp.h"

namespace {
// Inputs
DebouncedButton focusButton(Config::FOCUS_BUTTON_PIN, Config::BUTTON_DEBOUNCE_MS);
DebouncedButton shutterButton(Config::SHUTTER_BUTTON_PIN, Config::BUTTON_DEBOUNCE_MS);
Ky008LaserSensor laserSensor(Config::LASER_SENSOR_PIN, INPUT,
                             Config::LASER_SENSOR_DETECTED_STATE);

// Shutter trigger sources
ButtonShutterTrigger buttonTrigger(shutterButton);
LaserBreakShutterTrigger laserTrigger(laserSensor);

IShutterTrigger* shutterTriggers[] = {&buttonTrigger, &laserTrigger};
constexpr size_t SHUTTER_TRIGGER_COUNT =
    sizeof(shutterTriggers) / sizeof(shutterTriggers[0]);

// Output
OptocouplerShutterController shutterController(Config::FOCUS_PIN, Config::SHUTTER_PIN,
                                                Config::FOCUS_LED_PIN, Config::SHUTTER_LED_PIN);

// Application coordinator
ShutterApp app(focusButton, shutterTriggers, SHUTTER_TRIGGER_COUNT,
               shutterController, Config::TELEMETRY_INTERVAL_MS,
               Config::SHUTTER_PULSE_MS,
               Config::SHUTTER_MIN_INTERVAL_S * 1000UL);
}  // namespace

void setup() {
  Serial.begin(115200);
  // The button inputs and shutter controller are initialized by ShutterApp.
  // The laser sensor is wrapped by LaserBreakShutterTrigger, but the trigger's
  // begin() relies on the sensor already being configured.
  shutterButton.begin();
  laserSensor.begin();
  app.begin();
}

void loop() { app.update(); }