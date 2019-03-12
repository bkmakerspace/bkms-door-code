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

class EthernetClass {
public:
  MOCK_METHOD1(begin, int(uint8_t* mac));
  MOCK_METHOD3(begin, int(uint8_t* mac, IPAddress ip, IPAddress dnsIp));
  MOCK_METHOD0(maintain, int());
  MOCK_METHOD0(localIP, IPAddress());
};

#define Ethernet (*getEthernetMock())

EthernetClass *getEthernetMock();
void setEthernetMock(EthernetClass *mock);

#define WITH_ETHERNET_MOCK()  EthernetClass ethernetMock; \
                              setEthernetMock(&ethernetMock); \
                              goto AMOCK_UNIQUE_LABEL(enter); \
                              while(1) if (1) { \
                                setEthernetMock(NULL); \
                                break; \
                              } \
                              else AMOCK_UNIQUE_LABEL(enter):

#endif
