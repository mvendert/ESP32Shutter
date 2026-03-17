#include "OptocouplerShutterController.h"

OptocouplerShutterController::OptocouplerShutterController(
    uint8_t focusPin, uint8_t shutterPin)
    : focusPin_(focusPin), shutterPin_(shutterPin),
      focusActive_(false), shutterActive_(false) {}

void OptocouplerShutterController::begin() {
  pinMode(focusPin_, OUTPUT);
  pinMode(shutterPin_, OUTPUT);

  digitalWrite(focusPin_, LOW);
  digitalWrite(shutterPin_, LOW);

  Serial.println("Shutter controller initialized: focus=LOW, shutter=LOW");
}

void OptocouplerShutterController::setFocus(bool active) {
  if (active != focusActive_) {
    focusActive_ = active;
    digitalWrite(focusPin_, active ? HIGH : LOW);
    Serial.print("Focus ");
    Serial.println(active ? "asserted (HIGH)" : "released (LOW)");
  }
}

void OptocouplerShutterController::setShutter(bool active) {
  if (active != shutterActive_) {
    shutterActive_ = active;
    digitalWrite(shutterPin_, active ? HIGH : LOW);
    Serial.print("Shutter ");
    Serial.println(active ? "asserted (HIGH)" : "released (LOW)");
  }
}
