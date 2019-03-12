#include "gtest/gtest.h"

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "../doorDisplay.h"

using testing::Return;
using testing::StrEq;
using testing::NotNull;
using testing::SetArrayArgument;
using testing::InSequence;
using testing::Matcher;

TEST(DoorLCDDisplayTests, InitializesLCD) {
  LiquidCrystal_I2C mockLCD;

  EXPECT_CALL(mockLCD, init());
  EXPECT_CALL(mockLCD, backlight());

  DoorLCDDisplay disp(&mockLCD);
}

TEST(DoorLCDDisplayTests, PrintsMessagesWhenMessageIsCalled) {
  LiquidCrystal_I2C mockLCD;
  const char* line1 = "TEST LINE 1";
  const char* line2 = "TEST LINE 2";

  {
    InSequence seq;
    EXPECT_CALL(mockLCD, init());
    EXPECT_CALL(mockLCD, backlight());
    EXPECT_CALL(mockLCD, init());
    EXPECT_CALL(mockLCD, clear());
    EXPECT_CALL(mockLCD, setCursor(0, 0));
    EXPECT_CALL(mockLCD, print(line1));
    EXPECT_CALL(mockLCD, setCursor(0, 1));
    EXPECT_CALL(mockLCD, print(line2));
  }

  DoorLCDDisplay disp(&mockLCD);
  disp.message(line1,line2);
}

TEST(DoorSerialDisplayTests, PrintsMessagesWhenMessageIsCalled) {
  const char* line1 = "TEST LINE 1";
  const char* line2 = "TEST LINE 2";

  WITH_SERIAL_MOCK() {
    {
      InSequence seq;
      EXPECT_CALL(Serial, print(Matcher<const char*>(StrEq("LCD: "))));
      EXPECT_CALL(Serial, println(Matcher<const char*>(StrEq(line1))));
      EXPECT_CALL(Serial, print(Matcher<const char*>(StrEq("LCD: "))));
      EXPECT_CALL(Serial, println(Matcher<const char*>(StrEq(line2))));
    }

    DoorSerialDisplay disp;
    disp.message(line1,line2);
  }
}
