#include "gtest/gtest.h"

#include <Arduino.h>
#include <Ethernet.h>
#include "../ethernetMaintainer.h"

using testing::Return;
using testing::StrEq;
using testing::NotNull;
using testing::SetArrayArgument;
using testing::InSequence;
using testing::Matcher;

TEST(EthernetMaintainerTest, SerialPrintsIpOnRenew) {
  IPAddress fakeIp = IPAddress(127,0,0,1);

  EthernetClass eth;
  SerialClass ser;

  WITH_ETHERNET_MOCK() {
    WITH_SERIAL_MOCK() {
      EXPECT_CALL(Ethernet, maintain)
        .WillOnce(Return(2));
      EXPECT_CALL(Ethernet, localIP())
        .WillOnce(Return(fakeIp));

      EXPECT_CALL(Serial, print(Matcher<const char*>(StrEq("Renewed IP address: "))));
      EXPECT_CALL(Serial, println(fakeIp));

      EthernetMaintainer maint;
      bool res = maint.maintain();
      EXPECT_EQ(res, true);
    }
  }
}

TEST(EthernetMaintainerTest, SerialPrintsIpOnRebind) {
  IPAddress fakeIp = IPAddress(127,0,0,1);

  EthernetClass eth;
  SerialClass ser;

  WITH_ETHERNET_MOCK() {
    WITH_SERIAL_MOCK() {
      EXPECT_CALL(Ethernet, maintain)
        .WillOnce(Return(4));
      EXPECT_CALL(Ethernet, localIP())
        .WillOnce(Return(fakeIp));

      EXPECT_CALL(Serial, print(Matcher<const char*>(StrEq("Rebound IP address: "))));
      EXPECT_CALL(Serial, println(fakeIp));

      EthernetMaintainer maint;
      bool res = maint.maintain();
      EXPECT_EQ(res, true);
    }
  }
}
