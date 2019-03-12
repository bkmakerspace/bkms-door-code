#include <Arduino.h>
#include "doorLock.h"

DoorLock::DoorLock(int pin) {
  this->pin = pin;
}

void DoorLock::reset() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void DoorLock::unlockFor(long milliseconds) {
  digitalWrite(pin,LOW);
  delay(milliseconds);
  digitalWrite(pin,HIGH);
}
