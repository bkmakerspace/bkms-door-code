#include <Arduino.h>
#include <Ethernet.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

/*
 * Required values
 * - SECRET_KEY: uint8_t[6] secret key to use
 * - mac: mac address to use
 */
#include "bkms-secrets.h"
#include "config.h"
#include "makey.h"
#include "ethernetMaintainer.h"
#include "doorDisplay.h"
#include "doorLock.h"
#include "cardReaderSystem.h"

EthernetClient client;
EthernetMaintainer maintainer;
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#ifdef DISPLAY_LCD
  LiquidCrystal_I2C lcd_peripherial(0x27,20,4);
  DoorLCDDisplay lcd_display(&lcd_peripherial);
  DoorDisplay *display = &lcd_display;
#else
  DoorSerialDisplay serial_display;
  DoorDisplay *display = &serial_display;
#endif

Makey makey(SERVER_URL, SERVER_PORT, &client);
DoorLock lock(DOOR_PIN);
CardReaderSystem readerSystem(display, &lock, &makey, &nfc, SECRET_KEY);

void setup(void) {
  delay(2000);
  Serial.begin(115200);

  lock.reset();
  Serial.println("Hello!");

  display->loadingMessage("INIT NFC...");
  readerSystem.initializeReader();

  display->loadingMessage("INIT NETWORK...");
  maintainer.initialize(mac, ip, myDns);

  display->loadingMessage("INIT KEYS...");
  makey.rotateKey();

  Serial.println("Waiting for an ISO14443A Card ...");
  display->readyMessage();
}

void loop(void) {
  readerSystem.attemptRead();
  while(!maintainer.maintain()) {
    display->message("NETWORK FAILURE", "DHCP ERROR");
    delay(500);
  }
}
