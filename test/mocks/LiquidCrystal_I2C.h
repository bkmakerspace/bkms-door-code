#include "gmock/gmock.h"

#ifndef lcd_mock_h_
#define lcd_mock_h_

class LiquidCrystal_I2C {
public:
  MOCK_METHOD0(init, void());
  MOCK_METHOD0(backlight, void());
  MOCK_METHOD0(clear, void());
  MOCK_METHOD2(setCursor, void(int x, int y));
  MOCK_METHOD1(print, void(const char*));
};

#endif
