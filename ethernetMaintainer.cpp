#include <Arduino.h>
#include <Ethernet.h>

#include "ethernetMaintainer.h"

void EthernetMaintainer::initialize(uint8_t *mac, IPAddress ip, IPAddress dnsIp)
{
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, dnsIp);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

bool EthernetMaintainer::maintain()
{
  switch (Ethernet.maintain()) {
    case 1:
      Serial.println("Error: renewed fail");
      return false;
    case 2:
      Serial.print("Renewed IP address: ");
      Serial.println(Ethernet.localIP());
      return true;
    case 3:
      Serial.println("Error: rebind fail");
      return false;
    case 4:
      Serial.print("Rebound IP address: ");
      Serial.println(Ethernet.localIP());
      return true;
    default:
      return true;
  }
}
