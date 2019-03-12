#include "gmock/gmock.h"
#include "WString.h"
#include <chrono>

#ifndef arduino_h_
#define arduino_h_

class IPAddress {
public:
  IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
  {
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
  }
  uint8_t a,b,c,d;
  bool operator ==(const IPAddress &other) const {
    return a == other.a && b == other.b && c == other.c && d == other.d;
  }
};

#define AMOCK_CAT(prefix, suffix)            prefix ## suffix
#define AMOCK__UNIQUE_LABEL(prefix, suffix)  AMOCK_CAT(prefix, suffix)
#define AMOCK_UNIQUE_LABEL(prefix)           AMOCK__UNIQUE_LABEL(prefix, __LINE__)

using namespace std::chrono;

long millis();
void delay(long amt);
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);

#define HIGH (1)
#define LOW (0)

#define INPUT (1)
#define OUTPUT (0)

class ArduinoDefaults {
public:
  MOCK_METHOD0(millis, long());
  MOCK_METHOD1(delay, void(long));
  MOCK_METHOD2(pinMode, void(int, int));
  MOCK_METHOD2(digitalWrite, void(int,int));

  static long defaultMillisFunction() {
    duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
  }

  void standardMillis() { EXPECT_CALL(*this, millis).WillRepeatedly(testing::Invoke(defaultMillisFunction)); }
};

class SerialClass {
public:
  MOCK_METHOD1(print, void(const char*));
  MOCK_METHOD1(println, void(const char*));
  MOCK_METHOD1(print, void(long));
  MOCK_METHOD1(println, void(long));
  MOCK_METHOD1(print, void(IPAddress));
  MOCK_METHOD1(println, void(IPAddress));
};

#define Serial (*getSerialMock())

SerialClass *getSerialMock();
void setSerialMock(SerialClass *mock);

#define Defaults (*getDefaultsMock())

ArduinoDefaults *getDefaultsMock();
void setDefaultsMock(ArduinoDefaults* mock);

#define WITH_SERIAL_MOCK()  SerialClass serialMock; \
                            setSerialMock(&serialMock); \
                            goto AMOCK_UNIQUE_LABEL(enter); \
                            while(1) if (1) { \
                              setSerialMock(NULL); \
                              break; \
                            } \
                            else AMOCK_UNIQUE_LABEL(enter):

#define WITH_DEFAULT_MOCK()  ArduinoDefaults defaultMock; \
                            setDefaultsMock(&defaultMock); \
                            defaultMock.standardMillis(); \
                            goto AMOCK_UNIQUE_LABEL(enter); \
                            while(1) if (1) { \
                              setDefaultsMock(NULL); \
                              break; \
                            } \
                            else AMOCK_UNIQUE_LABEL(enter):

#endif
