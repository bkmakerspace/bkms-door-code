#ifndef card_reader_system_h
#define card_reader_system_h

class CardReaderSystem {
public:
  CardReaderSystem(DoorDisplay *display, DoorLock *lock, Makey *makey, Adafruit_PN532 *nfc, uint8_t *secret_key);
  void initializeReader();
  void attemptRead();

private:
  DoorDisplay *display;
  DoorLock *lock;
  Makey *makey;
  Adafruit_PN532 *nfc;
  uint8_t *secret_key;
};

#endif
