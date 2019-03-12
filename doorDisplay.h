#ifndef door_display_h
#define door_display_h

class DoorDisplay {
public:
  virtual void message(const char* line1, const char* line2) = 0;
  void readyMessage();
  void verifyMessage();
  void verifyMessage2();
  void serverFailMessage();
  void loadingMessage(const char* msg);
};

class DoorLCDDisplay : public DoorDisplay {
public:
  DoorLCDDisplay(LiquidCrystal_I2C* lcdPtr);
  virtual void message(const char *line1, const char* line2);
private:
  LiquidCrystal_I2C* lcd;
};

class DoorSerialDisplay : public DoorDisplay {
public:
  virtual void message(const char *line1, const char* line2);
};

#endif
