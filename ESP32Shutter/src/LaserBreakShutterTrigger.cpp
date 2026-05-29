#include "LaserBreakShutterTrigger.h"

LaserBreakShutterTrigger::LaserBreakShutterTrigger(Ky008LaserSensor& sensor)
    : sensor_(sensor), armed_(false), lastDetected_(false) {}

void LaserBreakShutterTrigger::begin() {
  // The sensor itself is initialized by its owner. We only arm the trigger
  // if the beam is currently detected; otherwise the user must restore the
  // beam at least once before a break will fire the shutter.
  lastDetected_ = sensor_.isBeamDetected();
  armed_ = lastDetected_;
}

bool LaserBreakShutterTrigger::consumeFireRequest() {
  const bool detected = sensor_.isBeamDetected();
  bool fire = false;

  //Serial.print("LaserBreakShutterTrigger: detected=");  Serial.println(detected);

  if (detected) {
    // Beam present: re-arm so the next break can fire.
    armed_ = true;
  } else if (lastDetected_ && armed_) {
    // detected -> broken transition while armed: fire once and disarm.
    fire = true;
    armed_ = false;
  }

  lastDetected_ = detected;
  return fire;
}
