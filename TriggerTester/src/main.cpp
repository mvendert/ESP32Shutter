#include <Arduino.h>
#include "DrukknopT.h"

const int pinFocus = 25;
const int pinShutter = 26;
const int pinButFocus = 18;
const int pinButRelease = 19;

Drukknop butFocus(pinButFocus, INPUT);
Drukknop butRelease(pinButRelease, INPUT);

unsigned long startFocusMillis;
unsigned long startReleaseMillis;
bool isFocus;
bool isRelease;

void setup() {
  Serial.begin(115200);
  Serial.println("Camera Remote Control");
  pinMode(pinFocus, OUTPUT);
  pinMode(pinShutter, OUTPUT);
  digitalWrite(pinFocus, LOW);
  digitalWrite(pinShutter, LOW);  
  isFocus = isRelease = false;
  butFocus.leesKnop();
  butRelease.leesKnop();
}

void loop() {
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();

  //Mimic the E3 remote shutter.
  // Focus is pressed prior to release

  butRelease.leesKnop();
  //React to the shutter release button as soon as possible  
  if (butRelease.toestandIs(Drukknop::DrukknopToestand::Indrukken)) {
    digitalWrite(pinFocus, HIGH);
    digitalWrite(pinShutter, HIGH);
    startReleaseMillis = currentMillis;
    startFocusMillis = currentMillis;
    isFocus = true;
    isRelease = true;
  }
  butFocus.leesKnop();
  if (!isRelease) {
    if (butFocus.toestandIs(Drukknop::DrukknopToestand::Indrukken)) {
      digitalWrite(pinFocus, HIGH);
      startFocusMillis = currentMillis;
      isFocus = true;
    }
  }
  /*
  delay(500);
  Serial.print("Toestand Focus: ");
  Serial.println(butFocus.toString(butFocus.getToestand()));
  Serial.print("Toestand Release: ");
  Serial.println(butRelease.toString(butRelease.getToestand()));
  */

  if (butRelease.toestandIs(Drukknop::DrukknopToestand::Loslaten)) {
    isFocus = false;
    isRelease = false;
  }
  if (butFocus.toestandIs(Drukknop::DrukknopToestand::Loslaten)) {    
    isFocus = false;
  }
  if (!isRelease) {
    digitalWrite(pinShutter, LOW);
  }
  if (!isFocus) {
    digitalWrite(pinFocus, LOW);
  }

  /*
    //Serial.println("Setting High");
    digitalWrite(pinFocus, HIGH);
    digitalWrite(pinShutter, LOW);
    delay(100);
    //Serial.println("Setting Low");
    digitalWrite(pinFocus, LOW);
    digitalWrite(pinShutter, HIGH);
    delay(100);  
  */
}
