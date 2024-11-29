#include "WiFiCredentials.h"
#include "ESP32HttpClient.h"
#include "PulseOximeter.h"

#define WifiConectado 2

WiFiCredentials wifiCreds;
ESP32HttpClient httpClient("", "", "http://192.168.100.52:5000/data");
PulseOximeter oximeter(19, 18, 1.8, 2.0, 10);
int BPM;
int SPO2;

void setup() {
    Serial.begin(115200);
    pinMode(WifiConectado, OUTPUT);

    wifiCreds.begin();

    // Verificar si hay credenciales válidas en EEPROM
    if (EEPROM.read(0) != 0xFF) {
        wifiCreds.readCredentialsFromEEPROM();
        Serial.println("Credenciales encontradas en EEPROM:");
        Serial.printf("SSID: %s\n", wifiCreds.getSSID());
        Serial.printf("Password: %s\n", wifiCreds.getPassword());
    } else {
        Serial.println("No se encontraron credenciales. Esperando nuevas credenciales...");
        while (WiFi.status() != WL_CONNECTED) {
            checkCredentials();  // Esperar credenciales por serial
            delay(1000);
        }
    }

    oximeter.begin();
}




void loop() {
    checkCredentials();
    
    oximeter.medir();
    oximeter.medir();

    SPO2 = oximeter.getSPO2();
    BPM = oximeter.getBPM();

    Serial.print("SPO2: ");
    Serial.println(SPO2);
    Serial.print("BPM: ");
    Serial.println(BPM);
    
    connectToWiFiWithTimeout(10);
    delay(10);

    httpClient.sendData(0, BPM, SPO2);

    delay(100);

    WiFi.disconnect();
    digitalWrite(WifiConectado, LOW);
}





void checkCredentials() {
    wifiCreds.checkForNewCredentials();

    static String previousSSID = wifiCreds.getSSID();
    static String previousPassword = wifiCreds.getPassword();

    if (strcmp(previousSSID.c_str(), wifiCreds.getSSID()) != 0 ||
        strcmp(previousPassword.c_str(), wifiCreds.getPassword()) != 0) {
        Serial.println("Credenciales WiFi actualizadas. Guardando en EEPROM...");
        wifiCreds.saveCredentialsToEEPROM();

        // Guardar marcador de datos válidos
        EEPROM.write(0, 0xAA);
        EEPROM.commit();

        connectToWiFiWithTimeout(10);

        previousSSID = wifiCreds.getSSID();
        previousPassword = wifiCreds.getPassword();
    }
}



// Función para conectar a WiFi con un timeout
void connectToWiFiWithTimeout(int timeout) {
    WiFi.begin(wifiCreds.getSSID(), wifiCreds.getPassword());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < timeout) {
        delay(1000);
        Serial.printf("Intentando conectar a WiFi (Intento %d)...\n", attempts + 1);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado a WiFi.");
        digitalWrite(WifiConectado, HIGH);
    } else {
        Serial.println("No se pudo conectar a WiFi. Credenciales actuales:");
        Serial.printf("SSID: %s\n", wifiCreds.getSSID());
        Serial.printf("Password: %s\n", wifiCreds.getPassword());
    }
}

