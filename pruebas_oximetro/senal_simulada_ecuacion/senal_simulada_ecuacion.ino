#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "NadiaVane";
const char* password = "galalola";
const char* serverUrl = "http://192.168.100.52:5000/data";

float x = 0.0;
float analogValue=0.0;


void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
}


void loop() {
  analogValue = simularSenal(x);
  enviarDatos(analogValue);
  

  delay(100); // Ajusta el delay según la frecuencia que quieras
}


float simularSenal(float x){
  // Simulación de la señal pletismográfica
  float signalValue = 1.2 + sin(2 * PI * x) + 0.4 * sin(4 * PI * x) + 0.1 * sin(8 * PI * x);

  // Incrementa el valor de 'x' para avanzar en el tiempo (algo así como un muestreo)
  x += 0.1;

  return signalValue;
}

void enviarDatos(float analogValue){
  // Si WiFi está conectado, envía los datos al servidor
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Crea los datos para enviar
    String postData = "analog_value=" + String(analogValue) +
                      "&ppm_value=" + String(60) +
                      "&o2_value=" + String(100);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Envía la solicitud POST
    int httpResponseCode = http.POST(postData);

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
}