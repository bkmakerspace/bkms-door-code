#include <Arduino.h>
#include <Ethernet.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal_I2C.h>

#include "doorDisplay.h"
#include "doorLock.h"
#include "makey.h"
#include "cardReaderSystem.h"

CardReaderSystem::CardReaderSystem(DoorDisplay *display, DoorLock *lock, Makey *makey, Adafruit_PN532 *nfc, uint8_t *secret_key)
{
  this->display = display;
  this->lock = lock;
  this->makey = makey;
  this->nfc = nfc;
  this->secret_key = secret_key;
}

void CardReaderSystem::initializeReader() {
  while(1) {
    nfc->begin();
    uint32_t versiondata = nfc->getFirmwareVersion();
    if (versiondata) {
      // Got ok data, print it out!
      Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
      Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
      Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
      break;
    }
  }
  // configure board to read RFID tags
  nfc->SAMConfig();
}

void CardReaderSystem::attemptRead() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t data[16];
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  display->readyMessage();

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc->PrintHex(uid, uidLength);
    Serial.println("");

    //try to read the badge data from page 4 using the default code
    Serial.println("Trying to authenticate block 4 with default KEYA value");
    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t success = nfc->mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

    if(success) {
      //new card! we need to generate a random key for use in the future.
      Serial.println("New card");
      display->verifyMessage();
      if(success) {
        Serial.println("Reading data...");
        success = nfc->mifareclassic_ReadDataBlock(7, data);

        if(success) {
          //now we have the keys, overwrite keya
          data[0] = secret_key[0];
          data[1] = secret_key[1];
          data[2] = secret_key[2];
          data[3] = secret_key[3];
          data[4] = secret_key[4];
          data[5] = secret_key[5];

          success = nfc->mifareclassic_WriteDataBlock(7, data);
          Serial.println("Wrote key");
          success = nfc->mifareclassic_WriteDataBlock(4, makey->key);
          Serial.println("Wrote secret");
          makey->rotateKey();
        }
      }
    } else {
      Serial.println("Not factory, trying our key");
      success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    }

    if (success) {
      // SHHH IT'S A SECRET - this is subobtimal, but we'll go with it for now
      success = nfc->mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, secret_key);

      if(success) {
        success = nfc->mifareclassic_ReadDataBlock(4, data);

        if(success) {
          Serial.println("Valid card!");
          //state = STATE_VERIFYING;
          display->verifyMessage2();

          // Make a HTTP request:
          char buffer[14];
          char buffer2[33];
          for(int xx=0;xx<7;xx++) {
            if(xx>=uidLength) {
              uid[xx]=0;
            }
          }
          sprintf(buffer, "/door/%02X%02X%02X%02X%02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6], uidLength);
          sprintf(buffer2, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X"
                , data[0]
                , data[1]
                , data[2]
                , data[3]
                , data[4]
                , data[5]
                , data[6]
                , data[7]
                , data[8]
                , data[9]
                , data[10]
                , data[11]
                , data[12]
                , data[13]
                , data[14]
                , data[15]);
          const char* headers[] = {"Host: makey->localdomain", "Connection: close", "Content-Type: text/plain;charset=UTF-8", "Content-Length: 32"};
          MakeyResponse res = makey->request(buffer, headers, 4, buffer2, 60000);

          if(res.statusCode == 200) {
            Serial.println("Accepted!");
            int idx2 = res.body.indexOf("\n");
            display->message(res.body.substring(0, idx2).c_str(), res.body.substring(idx2+1).c_str());
            lock->unlockFor(5000);

          } else if (res.statusCode == 0) {
            display->serverFailMessage();
            delay(5000);
          } else {
            Serial.println("Failed!");
            int idx2 = res.body.indexOf("\n");
            display->message(res.body.substring(0, idx2).c_str(), res.body.substring(idx2+1).c_str());
            delay(5000);
          }
        }
      }

      if(!success) {
        Serial.println("Invalid card!");
        display->serverFailMessage();
        delay(5000);
        return;
      }
    }
    return;
  }
}
