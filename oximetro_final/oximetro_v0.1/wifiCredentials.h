#ifndef WIFICREDENTIALS_H
#define WIFICREDENTIALS_H

#include <EEPROM.h>

class WiFiCredentials {
public:
    WiFiCredentials();                      // Constructor
    void begin();                           // Inicializa EEPROM
    void checkForNewCredentials();          // Verifica si hay datos en el puerto serial
    void receiveCredentials();              // Recibe SSID y contraseña
    void saveCredentialsToEEPROM();         // Guarda credenciales en EEPROM
    void readCredentialsFromEEPROM();       // Lee credenciales desde EEPROM
    const char* getSSID();                  // Obtiene el SSID
    const char* getPassword();              // Obtiene la contraseña

private:
    char ssid[32];
    char password[64];
};

#endif
