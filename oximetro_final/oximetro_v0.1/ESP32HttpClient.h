#ifndef ESP32HttpClient_h
#define ESP32HttpClient_h

#include <WiFi.h>
#include <HTTPClient.h>

class ESP32HttpClient {
public:
    ESP32HttpClient(const char* ssid, const char* password, const char* serverUrl);
    void connect();
    void sendData(int analogValue, int ppmValue, int o2Value);

private:
    const char* ssid;
    const char* password;
    const char* serverUrl;
};

#endif
