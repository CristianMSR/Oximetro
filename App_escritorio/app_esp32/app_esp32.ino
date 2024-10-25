#include <EEPROM.h>

const int uartBaudRate = 115200;
char ssid[32] = {0};     // Espacio suficiente para SSID
char password[64] = {0}; // Espacio suficiente para la contraseña

// Define el tamaño de la EEPROM (32 bytes para SSID y 64 bytes para la contraseña)
#define EEPROM_SIZE 96

void setup() {
  Serial.begin(uartBaudRate);
  while (!Serial) { }

  EEPROM.begin(EEPROM_SIZE);

  Serial.println("Esperando SSID y Password...");

  // Si ya hay credenciales guardadas en la EEPROM, las leemos
  if (EEPROM.read(0) != 0xFF) {  // Si la primera dirección no está vacía
    readCredentialsFromEEPROM();
    Serial.println("Credenciales cargadas desde EEPROM:");
    Serial.printf("SSID: %s\n", ssid);
    Serial.printf("Password: %s\n", password);
  } else {
    // Si no hay credenciales, las recibimos y las guardamos
    receiveCredentials();
    saveCredentialsToEEPROM();
  }
}

void loop() {
  Serial.println("Datos recibidos:");
  Serial.printf("SSID: %s\n", ssid);
  Serial.printf("Password: %s\n", password);

  delay(2000);
}

// Función para recibir las credenciales por serial
void receiveCredentials() {
  int indexSSID = 0;
  int indexPassword = 0;
  bool receivingSSID = true;

  while (true) {
    if (Serial.available()) {
      char receivedChar = Serial.read();

      if (receivedChar == '\n') {
        if (receivingSSID) {
          ssid[indexSSID] = '\0';  // Termina la cadena del SSID
          receivingSSID = false;    // Ahora se empieza a recibir la contraseña
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

// Función para guardar las credenciales en la EEPROM
void saveCredentialsToEEPROM() {
  // Guardar SSID
  for (int i = 0; i < sizeof(ssid); i++) {
    EEPROM.write(i, ssid[i]);
  }
  
  // Guardar contraseña
  for (int i = 0; i < sizeof(password); i++) {
    EEPROM.write(i + sizeof(ssid), password[i]);
  }

  EEPROM.commit();
  Serial.println("Credenciales guardadas en EEPROM.");
}

// Función para leer las credenciales desde la EEPROM
void readCredentialsFromEEPROM() {
  // Leer SSID
  for (int i = 0; i < sizeof(ssid); i++) {
    ssid[i] = EEPROM.read(i);
  }

  // Leer contraseña
  for (int i = 0; i < sizeof(password); i++) {
    password[i] = EEPROM.read(i + sizeof(ssid));
  }
}
