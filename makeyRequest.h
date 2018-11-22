struct MakeyResponse {
  int statusCode;
  String rawBody;
  String body;
};

MakeyResponse makeyRequest(EthernetClient* client, const char* path, const char** headers, int headerCount, const char* body, long timeout);
