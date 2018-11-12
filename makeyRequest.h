char webBuffer[256];
int webBytes = 0;

struct MakeyResponse {
  int statusCode;
  String rawBody;
  String body;
};

MakeyResponse makeyRequest(const char* path, const char** headers, int headerCount, const char* body, int timeout) {
  int retValue = client.connect(SERVER_URL, SERVER_PORT);
  client.print("GET ");
  client.print(path);
  client.println("HTTP/1.1");
  for(int i=0;i<headerCount;i++) {
    client.println(headers[i]);
  }
  client.println();
  client.println(body);
  MakeyResponse res;
  
  webBytes = 0;
  long startMillis = millis();
  while (client.connected()) {
    int len = client.available();
    if (len > 0) {
      if (webBytes + len > 255) len = 255 - webBytes;
      client.read(&(webBuffer[webBytes]), len);
      webBytes = webBytes + len;
    }

    if(millis() > startMillis + timeout) {
      res.statusCode = 0;
      return res;
    }
  }
  
  client.stop();
  webBuffer[webBytes] = 0;
  String response = String(webBuffer);

  res.rawBody = response;
  res.body = response.substring(response.indexOf("\r\n\r\n"));
  res.statusCode = (int)response.substring(response.indexOf(" ") + 1, 3).toInt();

  return res;
}
