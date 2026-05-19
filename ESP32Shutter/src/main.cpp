#include <Arduino.h>

#include "Config.h"
#include "DebouncedButton.h"
#include "OptocouplerShutterController.h"
#include "ShutterApp.h"
#include "Ky008LaserSensor.h"

namespace {
DebouncedButton focusButton(Config::FOCUS_BUTTON_PIN, Config::BUTTON_DEBOUNCE_MS);
DebouncedButton shutterButton(Config::SHUTTER_BUTTON_PIN, Config::BUTTON_DEBOUNCE_MS);
OptocouplerShutterController shutterController(Config::FOCUS_PIN, Config::SHUTTER_PIN,
                                                Config::FOCUS_LED_PIN, Config::SHUTTER_LED_PIN);
ShutterApp app(focusButton, shutterButton, shutterController,
               Config::TELEMETRY_INTERVAL_MS);
Ky008LaserSensor LaserSensor(34);
}  // namespace

static bool prevStateBeam = false;
bool laserBeamDetected = false;
void setup() {
  Serial.begin(115200);
  app.begin();
  LaserSensor.begin();
  laserBeamDetected = LaserSensor.isBeamDetected();
  prevStateBeam = laserBeamDetected;
}

void loop() {  
  app.update();  
  laserBeamDetected = LaserSensor.isBeamDetected();
     
  if (laserBeamDetected != prevStateBeam) {
    if (laserBeamDetected) {
      Serial.println("Laser beam is detected!");
    } else {
      Serial.println("Laser beam is broken!");
    }
    prevStateBeam = laserBeamDetected;
  }  
}