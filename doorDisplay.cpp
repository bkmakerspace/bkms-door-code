#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "doorDisplay.h"

void DoorDisplay::readyMessage() {
  this->message(" ^ ^ ^ ^ ^ ^ ^"," Ready to Scan");
}

void DoorDisplay::verifyMessage() {
  this->message(" Verifying Card", "");
}

void DoorDisplay::verifyMessage2() {
  this->message(" Verifying Card", " AUTHORIZING...");
}
void DoorDisplay::serverFailMessage() {
  this->message("", "BADGE ERROR");
}
void DoorDisplay::loadingMessage(const char* msg) {
  this->message("LOADING...", msg);
}

DoorLCDDisplay::DoorLCDDisplay(LiquidCrystal_I2C* lcdPtr) {
  lcd = lcdPtr;
  lcd->init();
  lcd->backlight();
}

void DoorLCDDisplay::message(const char *line1, const char* line2) {
  lcd->init();
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(line1);
  lcd->setCursor(0,1);
  lcd->print(line2);
}

void DoorSerialDisplay::message(const char *line1, const char* line2) {
  Serial.print("LCD: ");
  Serial.println(line1);
  Serial.print("LCD: ");
  Serial.println(line2);
}
