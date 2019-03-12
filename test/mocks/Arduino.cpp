#include "Arduino.h"
#include "Ethernet.h"

ArduinoDefaults *defaults;

long millis() {
  return defaults->millis();
}

void delay(long amt) {
  defaults->delay(amt);
}

void pinMode(uint8_t pin, uint8_t mode){
  defaults->pinMode(pin, mode);
}
void digitalWrite(uint8_t pin, uint8_t val){
  defaults->digitalWrite(pin,val);
}

ArduinoDefaults *getDefaultsMock() {
  return defaults;
}

void setDefaultsMock(ArduinoDefaults* mock) {
  defaults = mock;
}

EthernetClass *ethernetMockObj;

EthernetClass *getEthernetMock()
{
  return ethernetMockObj;
}

void setEthernetMock(EthernetClass *mock)
{
  ethernetMockObj = mock;
}

SerialClass *serialMockObj;

SerialClass *getSerialMock()
{
  return serialMockObj;
}

void setSerialMock(SerialClass *mock)
{
  serialMockObj = mock;
}
