struct MakeyResponse {
  int statusCode;
  String rawBody;
  String body;
};

class Makey {
public:
  Makey(const char* url, int port, EthernetClient *client);
  virtual MakeyResponse request(const char* path, const char** headers, int headerCount, const char* body, long timeout);
  virtual void rotateKey();

  uint8_t key[16];
private:
  const char* url;
  int port;
  EthernetClient *client;
};
