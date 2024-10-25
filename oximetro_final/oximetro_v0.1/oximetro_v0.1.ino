#include "WiFiCredentials.h"
#include "ESP32HttpClient.h"

#define WifiConectado 2

WiFiCredentials wifiCreds;
ESP32HttpClient httpClient("", "", "http://192.168.100.52:5000/data");

void setup() {
    Serial.begin(115200);
    pinMode(WifiConectado, OUTPUT);
    wifiCreds.begin();

    // Si hay credenciales almacenadas, leerlas
    if (EEPROM.read(0) != 0xFF) {
        wifiCreds.readCredentialsFromEEPROM();
    } else {
        Serial.println("No se encontraron credenciales guardadas.");
    }

    // Intentar conectar a WiFi
    connectToWiFiWithTimeout(6); // Timeout de 6 segundos
}

void loop() {
    // Verificar si hay nuevas credenciales por serial
    wifiCreds.checkForNewCredentials();
    
    // Actualizar las credenciales en el cliente HTTP si han cambiado
    static String previousSSID = wifiCreds.getSSID();
    static String previousPassword = wifiCreds.getPassword();

    if (strcmp(previousSSID.c_str(), wifiCreds.getSSID()) != 0 ||
        strcmp(previousPassword.c_str(), wifiCreds.getPassword()) != 0) {
        Serial.println("Credenciales WiFi cambiadas. Re-conectando...");
        
        // Actualizar las credenciales del cliente HTTP
        httpClient = ESP32HttpClient(wifiCreds.getSSID(), wifiCreds.getPassword(), "http://192.168.100.52:5000/data");
        connectToWiFiWithTimeout(6);  // Timeout de 6 segundos para reconectar
        
        // Actualizar las credenciales anteriores
        previousSSID = wifiCreds.getSSID();
        previousPassword = wifiCreds.getPassword();
    }

    // Aquí puedes leer y enviar los datos de los sensores
    int analogValue = 500; // Simulando valor
    int ppmValue = 63;     // Simulando valor PPM
    int o2Value = 98;      // Simulando valor

    httpClient.sendData(analogValue, ppmValue, o2Value);

    delay(1000); // Espera 1 segundo entre envíos
}

// Función para conectar a WiFi con un timeout
void connectToWiFiWithTimeout(int timeout) {
    WiFi.begin(wifiCreds.getSSID(), wifiCreds.getPassword());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < timeout) {
        delay(1000);
        Serial.print("Conectando a WiFi...");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado a WiFi");
        digitalWrite(WifiConectado, HIGH);
    } else {
        Serial.println("No se pudo conectar a WiFi. Esperando credenciales...");
        // Aquí podrías agregar lógica para manejar la espera de credenciales
    }
}
