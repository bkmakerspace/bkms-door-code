#include "gtest/gtest.h"

#include <Arduino.h>
#include <Ethernet.h>
#include "../makey.h"

using testing::Return;
using testing::StrEq;
using testing::Eq;
using testing::NotNull;
using testing::SetArrayArgument;
using testing::ElementsAreArray;
using testing::InSequence;

TEST(MakeyTest, SendsRequestHeader) {
  EthernetClient clientMock;
  const char* path = "/path/to/something";

  WITH_DEFAULT_MOCK() {
    EXPECT_CALL(clientMock, connect(StrEq("http://test.url"), Eq(80)))
      .WillRepeatedly(Return(0));
    EXPECT_CALL(clientMock, print(StrEq("GET ")));
    EXPECT_CALL(clientMock, print(StrEq(path)));
    EXPECT_CALL(clientMock, println(StrEq(" HTTP/1.1")));

    EXPECT_CALL(clientMock, println());
    EXPECT_CALL(clientMock, println(StrEq("")));

    EXPECT_CALL(clientMock, connected())
      .WillOnce(Return(false));
    EXPECT_CALL(clientMock, stop());

    Makey makey("http://test.url", 80, &clientMock);
    makey.request(path, {}, 0, "", 0);
  }
}

TEST(MakeyTest, TimesOutAfterTimeout) {
  EthernetClient clientMock;
  const char* path = "/path/to/something";

  WITH_DEFAULT_MOCK() {
    EXPECT_CALL(clientMock, connect(StrEq("http://test.url"), Eq(80)))
      .WillRepeatedly(Return(0));
    EXPECT_CALL(clientMock, print(StrEq("GET ")));
    EXPECT_CALL(clientMock, print(StrEq("/path/to/something")));
    EXPECT_CALL(clientMock, println(StrEq(" HTTP/1.1")));
    EXPECT_CALL(clientMock, println());
    EXPECT_CALL(clientMock, println(StrEq("")));
    EXPECT_CALL(clientMock, connected())
      .WillRepeatedly(Return(true));
    EXPECT_CALL(clientMock, available())
      .WillRepeatedly(Return(0));
    EXPECT_CALL(clientMock, stop());
    Makey makey("http://test.url", 80, &clientMock);

    MakeyResponse res = makey.request(path, {}, 0, "", 10);

    EXPECT_EQ(res.statusCode, 0);
  }
}

TEST(MakeyTest, SucceedsIfDataRecieved) {
  EthernetClient clientMock;
  const char* path = "/path/to/something";
  const char* response = "HTTP/1.1 200 OK\r\nHeader: value\r\n\r\nBODY";
  WITH_DEFAULT_MOCK() {
    int len = strlen(response);

    {
      InSequence seq;
      EXPECT_CALL(clientMock, connect(StrEq("http://test.url"), Eq(80)))
        .WillRepeatedly(Return(0));
      EXPECT_CALL(clientMock, print(StrEq("GET ")));
      EXPECT_CALL(clientMock, print(StrEq("/path/to/something")));
      EXPECT_CALL(clientMock, println(StrEq(" HTTP/1.1")));
      EXPECT_CALL(clientMock, println());
      EXPECT_CALL(clientMock, println(StrEq("")));
      EXPECT_CALL(clientMock, connected())
        .WillRepeatedly(Return(true));
      EXPECT_CALL(clientMock, available())
        .WillOnce(Return(len));
      EXPECT_CALL(clientMock, read(NotNull(), len))
        .WillOnce(SetArrayArgument<0>(response, response + len));
      EXPECT_CALL(clientMock, connected())
        .WillRepeatedly(Return(false));
      EXPECT_CALL(clientMock, stop());
    }

    Makey makey("http://test.url", 80, &clientMock);

    MakeyResponse res = makey.request(path, {}, 0, "", 200000);

    EXPECT_EQ(res.statusCode, 200);
    EXPECT_STREQ(res.body.c_str(), "BODY");
  }
}

TEST(MakeyTest, SetsKeyWhenSuccessful) {
  EthernetClient clientMock;
  uint8_t testKey[16] = {'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  const char* path = "/hubot/randombytes";
  const char* response = "HTTP/1.1 200 OK\r\nHeader: value\r\n\r\n123456789ABCDEF";
  WITH_DEFAULT_MOCK() {
    int len = strlen(response);

    {
      InSequence seq;
      EXPECT_CALL(clientMock, connect(StrEq("http://test.url"), Eq(80)))
        .WillRepeatedly(Return(0));
      EXPECT_CALL(clientMock, print(StrEq("GET ")));
      EXPECT_CALL(clientMock, print(StrEq(path)));
      EXPECT_CALL(clientMock, println(StrEq(" HTTP/1.1")));
      EXPECT_CALL(clientMock, println());
      EXPECT_CALL(clientMock, println(StrEq("")));
      EXPECT_CALL(clientMock, connected())
        .WillRepeatedly(Return(true));
      EXPECT_CALL(clientMock, available())
        .WillOnce(Return(len));
      EXPECT_CALL(clientMock, read(NotNull(), len))
        .WillOnce(SetArrayArgument<0>(response, response + len));
      EXPECT_CALL(clientMock, connected())
        .WillRepeatedly(Return(false));
      EXPECT_CALL(clientMock, stop());
    }

      Makey makey("http://test.url", 80, &clientMock);

      makey.rotateKey();

      EXPECT_THAT(makey.key, ElementsAreArray(testKey));
  }
}
