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
#include "makeyRequest.h"

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 1, 1);

EthernetClient client;

#define PN532_IRQ   (2)
#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

LiquidCrystal_I2C lcd(0x27,20,4);

#include "messages.h"

uint8_t nextKey[16];

void getNewKey() {
  MakeyResponse res = makeyRequest(&client, "/hubot/randombytes", {}, 0, "", 20000);

  while(res.statusCode != 200) {
    res = makeyRequest(&client, "/hubot/randombytes", {}, 0, "", 20000);
  }
  memcpy(nextKey, res.body.c_str(), 16);
}

void setup(void) {
  delay(2000);
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.println("Hello!");
  randomSeed(analogRead(0));

  lcd.init();
  lcd.backlight();

  printLoading("INIT NFC...");

  while(1) {
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (versiondata) {
      // Got ok data, print it out!
      Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
      Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
      Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
      break;
    }
  }
  // configure board to read RFID tags
  nfc.SAMConfig();

  printLoading("INIT NETWORK...");
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

  printLoading("INIT KEYS...");
  getNewKey();

  Serial.println("Waiting for an ISO14443A Card ...");

  printReadyMessage();
}

void readCard(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t data[16];
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  printReadyMessage();

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    //try to read the badge data from page 4 using the default code
    Serial.println("Trying to authenticate block 4 with default KEYA value");
    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

    if(success) {
      //new card! we need to generate a random key for use in the future.
      Serial.println("New card");
      printVerifyMessage();
      if(success) {
        Serial.println("Reading data...");
        success = nfc.mifareclassic_ReadDataBlock(7, data);

        if(success) {
          //now we have the keys, overwrite keya
          data[0] = SECRET_KEY[0];
          data[1] = SECRET_KEY[1];
          data[2] = SECRET_KEY[2];
          data[3] = SECRET_KEY[3];
          data[4] = SECRET_KEY[4];
          data[5] = SECRET_KEY[5];

          success = nfc.mifareclassic_WriteDataBlock(7, data);
          Serial.println("Wrote key");
          success = nfc.mifareclassic_WriteDataBlock(4, nextKey);
          Serial.println("Wrote secret");
          getNewKey();
        }
      }
    } else {
      Serial.println("Not factory, trying our key");
      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    }

    if (success) {
      // SHHH IT'S A SECRET - this is subobtimal, but we'll go with it for now
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, SECRET_KEY);

      if(success) {
        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if(success) {
          Serial.println("Valid card!");
          //state = STATE_VERIFYING;
          printVerify2Message();
          //startRequest();

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
          const char* headers[] = {"Host: makey.localdomain", "Connection: close", "Content-Type: text/plain;charset=UTF-8", "Content-Length: 32"};
          MakeyResponse res = makeyRequest(&client, buffer, headers, 4, buffer2, 60000);

          if(res.statusCode == 200) {
            Serial.println("Accepted!");
            int idx2 = res.body.indexOf("\n");
            printWelcomeMessage(res.body.substring(0, idx2).c_str(), res.body.substring(idx2+1).c_str());
            digitalWrite(4, LOW);
            delay(5000);
            digitalWrite(4, HIGH);
          } else if (res.statusCode == 0) {
            printServerFailMessage();
            delay(5000);
          } else {
            Serial.println("Failed!");
            int idx2 = res.body.indexOf("\n");
            printFailMessage(res.body.substring(0, idx2).c_str(), res.body.substring(idx2+1).c_str());
            delay(5000);
          }
        }
      }

      if(!success) {
        Serial.println("Invalid card!");
        printServerFailMessage();
        delay(5000);
        return;
      }
    }
    return;
  }
}

void loop(void) {
  readCard();
}
