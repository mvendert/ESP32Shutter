#include "Ky008LaserSensor.h"

Ky008LaserSensor::Ky008LaserSensor(uint8_t pin, uint8_t inputMode,
                                   bool beamDetectedState)
    : pin_(pin), inputMode_(inputMode), beamDetectedState_(beamDetectedState) {}

void Ky008LaserSensor::begin()
{
  pinMode(pin_, inputMode_);
}

bool Ky008LaserSensor::isBeamDetected() const
{
  return readRaw() == (beamDetectedState_ ? HIGH : LOW);
}

bool Ky008LaserSensor::isBeamBroken() const
{
  return !isBeamDetected();
}

int Ky008LaserSensor::readRaw() const
{
  int value = digitalRead(pin_);

  /*
  static int prevmillis = 0;
  if (millis() - prevmillis > 500)
  {
    Serial.print("Raw sensor value: ");
    Serial.println(value);
    prevmillis = millis();
  }
  */   
  return value;
}
