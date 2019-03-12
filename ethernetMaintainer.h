#ifndef ethernet_maintainer_h_
#define ethernet_maintainer_h_

class EthernetMaintainer {
  public:
    void initialize(uint8_t *mac, IPAddress ip, IPAddress dnsIp);
    bool maintain();
};

#endif
