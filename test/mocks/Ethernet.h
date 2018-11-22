#include "gmock/gmock.h"

#ifndef ethernet_mock_h_
#define ethernet_mock_h_
#define ethernet_h_

class EthernetClient {
public:
  MOCK_METHOD2(connect, int(const char* url, int port));
  MOCK_METHOD1(print, void(const char* data));
  MOCK_METHOD1(println, void(const char* data));
  MOCK_METHOD0(println, void());
  MOCK_METHOD0(connected, bool());
  MOCK_METHOD0(available, int());
  MOCK_METHOD0(stop, void());
  MOCK_METHOD2(read, void(uint8_t* ptr, int len));
};

#endif
