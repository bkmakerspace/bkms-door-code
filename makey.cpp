#include <Arduino.h>
#include <Ethernet.h>

#include "makey.h"

char webBuffer[256];
int webBytes = 0;

Makey::Makey(const char* url, int port, EthernetClient *client) {
  this->url = url;
  this->port = port;
  this->client = client;
}

MakeyResponse Makey::request(const char* path, const char** headers, int headerCount, const char* body, long timeout) {
  int retValue = client->connect(url, port);
  client->print("GET ");
  client->print(path);
  client->println(" HTTP/1.1");
  for(int i=0;i<headerCount;i++) {
    client->println(headers[i]);
  }
  client->println();
  client->println(body);
  MakeyResponse res;

  webBytes = 0;
  long startMillis = millis();
  while (client->connected()) {
    int len = client->available();
    if (len > 0) {
      if (webBytes + len > 255) len = 255 - webBytes;
      client->read((uint8_t*)&(webBuffer[webBytes]), len);
      webBytes = webBytes + len;
    }

    if(millis() > startMillis + timeout) {
      client->stop();
      res.statusCode = 0;
      return res;
    }
  }

  client->stop();
  webBuffer[webBytes] = 0;
  String response = String(webBuffer);

  res.rawBody = response;
  res.body = response.substring(response.indexOf("\r\n\r\n") + 4);
  res.statusCode = (int)response.substring(response.indexOf(" ") + 1, response.indexOf(" ") + 4).toInt();

  return res;
}

void Makey::rotateKey() {
  MakeyResponse res = request("/hubot/randombytes", {}, 0, "", 20000);

  while(res.statusCode != 200) {
    res = request("/hubot/randombytes", {}, 0, "", 20000);
  }
  memcpy(key, res.body.c_str(), 16);
}
