#include <WiFi.h>
#include <HTTPClient.h>

// Configura tus credenciales WiFi
const char* ssid = "NadiaVane";
const char* password = "galalola";

// Dirección del servidor (Flask)
const char* serverUrl = "http://192.168.100.52:5000/data";

void setup() {
  Serial.begin(115200);

  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;

    // Lee los valores de los sensores (simulados)
    int analogValue = 500;
    int ppmValue = 63; // Simulando valor PPM (corregido)
    int o2Value = 98;

    // Crea la URL con los datos
    String postData = "analog_value=" + String(analogValue) +
                      "&ppm_value=" + String(ppmValue) +
                      "&o2_value=" + String(o2Value);

    http.begin(serverUrl); // Conectar al servidor (corregido)
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Envía la solicitud POST
    int httpResponseCode = http.POST(postData);

    // Verifica el resultado de la solicitud
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Datos enviados correctamente");
      Serial.println(response);
    } else {
      Serial.print("Error enviando los datos: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Finaliza la conexión HTTP
  } else {
    Serial.println("WiFi desconectado");
  }

  delay(100); // Envía datos cada segundo
}
