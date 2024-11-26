#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Crear objeto ADS1115
Adafruit_ADS1115 ads;

const char* ssid = "NadiaVane";
const char* password = "galalola";
const char* serverUrl = "http://192.168.100.52:5000/data";

float analogValues[100]; // Array para almacenar los datos
int dataIndex = 0; // Índice para el arreglo

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Iniciar ADS1115
  if (!ads.begin()) {
    Serial.println("¡No se encontró el ADS1115!");
    while (1);
  }
  
  ads.setGain(GAIN_ONE);  // Configura el PGA a +/- 4.096V (1 bit = 0.125mV)
}


void loop() {
  // Leer valor analógico del ADS1115 en el canal 0
  int16_t adc0 = ads.readADC_SingleEnded(0);
  // Convertir el valor ADC a voltaje
  float analogValue = adc0 * 0.125 / 1000.0;  // 0.125mV por bit
  
  Serial.println(analogValue);

  // Almacena el valor en el arreglo
  analogValues[dataIndex++] = analogValue;

  // Cuando el arreglo está lleno, envía los datos
  if (dataIndex >= 100) {
    enviarDatos();
    dataIndex = 0; // Reinicia el índice para la próxima ronda de datos
  }

  delay(10);
}


void enviarDatos() {
  // Si WiFi está conectado, envía los datos al servidor
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Crea los datos para enviar en formato JSON
    String postData = "{\"analog_values\": [";
    for (int i = 0; i < 100; i++) {
      postData += String(analogValues[i]);
      if (i < 99) postData += ",";
    }
    postData += "], \"ppm_value\": 60, \"o2_value\": 100}";

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

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
