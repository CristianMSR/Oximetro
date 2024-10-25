#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Configuración del ADS1115
Adafruit_ADS1115 ads;  // Para ADS1115 usar 16-bit

// Configuración de WiFi
const char* ssid = "NadiaVane";         // Reemplaza con tu SSID
const char* password = "galalola"; // Reemplaza con tu contraseña

// Dirección IP del servidor Flask
const char* serverName = "http://192.168.100.52:5000/data";

// Variable para almacenar el valor leído
int16_t adc0;
float voltage;

void setup() {
  Serial.begin(115200);  // Inicializamos el puerto serie

  // Conectar a la red WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConectado a la red WiFi");

  ads.setGain(GAIN_ONE); // Configuramos la ganancia a 1 (±4.096V)
  ads.begin();           // Inicializamos el ADS1115
}

void loop() {
  // Leer el valor del canal A0
  adc0 = ads.readADC_SingleEnded(0);
  
  // Convertir a tensión (0.125mV por bit cuando usamos GAIN_ONE)
  voltage = adc0 * 0.125 / 1000.0;  // Conversión para obtener voltios
  
  Serial.print("Tensión (V): ");
  Serial.println(voltage);

  // Enviar los datos al servidor Flask
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Iniciar la solicitud HTTP
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Crear el cuerpo de la solicitud (valores enviados)
    String httpRequestData = "analog_value=" + String(adc0) + "&ppm_value=0&o2_value=0";

    // Enviar la solicitud POST
    int httpResponseCode = http.POST(httpRequestData);

    // Verificar la respuesta
    if (httpResponseCode > 0) {
      String response = http.getString();  // Obtenemos la respuesta del servidor
      Serial.println("Código de respuesta: " + String(httpResponseCode));
      Serial.println("Respuesta del servidor: " + response);
    } else {
      Serial.println("Error al enviar los datos: " + String(httpResponseCode));
    }

    // Finalizar la conexión HTTP
    http.end();
  } else {
    Serial.println("Error en la conexión WiFi");
  }

  // Retardo de 1 segundo antes de la siguiente lectura y envío
  delay(100);
}
