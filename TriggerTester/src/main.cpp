#include <Arduino.h>

const int pinFocus = 25;
const int pinShutter = 26;
// put function declarations here:

void setup() {
  Serial.begin(115200);
  Serial.println("Camera Remote Control");
  pinMode(pinFocus, OUTPUT);
  pinMode(pinShutter, OUTPUT);
  digitalWrite(pinFocus, LOW);
  digitalWrite(pinShutter, LOW);  
}

void loop() {
  //Serial.println("Setting High");
  digitalWrite(pinFocus, HIGH);
  digitalWrite(pinShutter, LOW);
  delay(100);
  //Serial.println("Setting Low");
  digitalWrite(pinFocus, LOW);
  digitalWrite(pinShutter, HIGH);
  delay(100);  
}
