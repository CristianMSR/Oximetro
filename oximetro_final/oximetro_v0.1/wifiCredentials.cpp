#include "WiFiCredentials.h"

// Constructor: inicializa los buffers de SSID y password
WiFiCredentials::WiFiCredentials() {
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
}

// Inicializa la EEPROM
void WiFiCredentials::begin() {
    EEPROM.begin(96);  // 32 bytes para SSID y 64 para contraseña
}

// Verifica si hay nuevos datos en el puerto serial y los actualiza si es necesario
void WiFiCredentials::checkForNewCredentials() {
    if (Serial.available()) {
        Serial.println("Nuevos datos recibidos. Actualizando credenciales...");
        receiveCredentials();
        saveCredentialsToEEPROM();
        
        Serial.println("Nuevas credenciales guardadas:");
        Serial.printf("SSID: %s\n", getSSID());
        Serial.printf("Password: %s\n", getPassword());
    }
}

// Recibe los datos de SSID y contraseña desde el puerto serial
void WiFiCredentials::receiveCredentials() {
    int indexSSID = 0;
    int indexPassword = 0;
    bool receivingSSID = true;

    while (true) {
        if (Serial.available()) {
            char receivedChar = Serial.read();

            if (receivedChar == '\n') {
                if (receivingSSID) {
                    ssid[indexSSID] = '\0';  // Termina la cadena del SSID
                    receivingSSID = false;    // Cambia a la contraseña
                } else {
                    password[indexPassword] = '\0'; // Termina la cadena de la contraseña
                    break;
                }
            } else {
                if (receivingSSID) {
                    if (indexSSID < sizeof(ssid) - 1) {
                        ssid[indexSSID++] = receivedChar;
                    }
                } else {
                    if (indexPassword < sizeof(password) - 1) {
                        password[indexPassword++] = receivedChar;
                    }
                }
            }
        }
    }
}

// Guarda el SSID y la contraseña en la EEPROM
void WiFiCredentials::saveCredentialsToEEPROM() {
    for (int i = 0; i < sizeof(ssid); i++) {
        EEPROM.write(i, ssid[i]);
    }
    for (int i = 0; i < sizeof(password); i++) {
        EEPROM.write(i + sizeof(ssid), password[i]);
    }
    EEPROM.commit();
}

// Lee el SSID y la contraseña desde la EEPROM
void WiFiCredentials::readCredentialsFromEEPROM() {
    for (int i = 0; i < sizeof(ssid); i++) {
        ssid[i] = EEPROM.read(i);
    }
    for (int i = 0; i < sizeof(password); i++) {
        password[i] = EEPROM.read(i + sizeof(ssid));
    }
}

// Devuelve el SSID actual
const char* WiFiCredentials::getSSID() {
    return ssid;
}

// Devuelve la contraseña actual
const char* WiFiCredentials::getPassword() {
    return password;
}
