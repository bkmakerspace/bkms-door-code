#include "gtest/gtest.h"

#include <Arduino.h>
#include <Ethernet.h>
#include "../makeyRequest.h"

using testing::Return;
using testing::StrEq;
using testing::NotNull;
using testing::SetArrayArgument;
using testing::InSequence;

TEST(MakeyRequestTest, SendsRequestHeader) {
  EthernetClient clientMock;
  const char* path = "/path/to/something";

  EXPECT_CALL(clientMock, print(StrEq("GET ")));
  EXPECT_CALL(clientMock, print(StrEq(path)));
  EXPECT_CALL(clientMock, println(StrEq(" HTTP/1.1")));

  EXPECT_CALL(clientMock, println());
  EXPECT_CALL(clientMock, println(StrEq("")));

  EXPECT_CALL(clientMock, connected())
    .WillOnce(Return(false));

  makeyRequest(&clientMock, path, {}, 0, "", 0);
}

TEST(MakeyRequestTest, TimesOutAfterTimeout) {
  EthernetClient clientMock;
  const char* path = "/path/to/something";

  EXPECT_CALL(clientMock, connected())
    .WillRepeatedly(Return(true));

  MakeyResponse res = makeyRequest(&clientMock, path, {}, 0, "", 10);

  EXPECT_EQ(res.statusCode, 0);
}

TEST(MakeyRequestTest, SucceedsIfDataRecieved) {
  EthernetClient clientMock;
  const char* path = "/path/to/something";
  const char* response = "HTTP/1.1 200 OK\r\nHeader: value\r\n\r\nBODY";
  int len = strlen(response);

  {
    InSequence seq;
    EXPECT_CALL(clientMock, connected())
      .WillRepeatedly(Return(true));
    EXPECT_CALL(clientMock, available())
      .WillOnce(Return(len));
    EXPECT_CALL(clientMock, read(NotNull(), len))
      .WillOnce(SetArrayArgument<0>(response, response + len));
    EXPECT_CALL(clientMock, connected())
      .WillRepeatedly(Return(false));
  }

  MakeyResponse res = makeyRequest(&clientMock, path, {}, 0, "", 200000);

  EXPECT_EQ(res.statusCode, 200);
  EXPECT_STREQ(res.body.c_str(), "BODY");
}
