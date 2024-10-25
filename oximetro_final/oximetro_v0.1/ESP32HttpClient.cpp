#include "ESP32HttpClient.h"

// Constructor de la clase
ESP32HttpClient::ESP32HttpClient(const char* ssid, const char* password, const char* serverUrl) {
    this->ssid = ssid;
    this->password = password;
    this->serverUrl = serverUrl;
}

void ESP32HttpClient::connect() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");
}

void ESP32HttpClient::sendData(int analogValue, int ppmValue, int o2Value) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String postData = "analog_value=" + String(analogValue) +
                          "&ppm_value=" + String(ppmValue) +
                          "&o2_value=" + String(o2Value);

        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int httpResponseCode = http.POST(postData);
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Datos enviados correctamente");
            Serial.println(response);
        } else {
            Serial.print("Error enviando los datos: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi desconectado");
    }
}
