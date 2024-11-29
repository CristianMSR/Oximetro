#include <Oximetro.h>

// Pines para los LEDs
const int pinLedR = 18;
const int pinLedIR = 19;

// Crear una instancia de la clase Oximetro
Oximetro oximetro(pinLedR, pinLedIR);

void setup() {
  Serial.begin(115200); // Inicializar comunicación serie
  oximetro.begin();     // Inicializar el oximetro
}

void loop() {
  oximetro.medir(); // Realizar mediciones

  int spo2 = oximetro.calcularSPO2();
  int bpm = oximetro.calcularBPM();
    
  Serial.print("SpO2: ");
  Serial.print(spo2);
  Serial.print("%  BPM: ");
  Serial.println(bpm);

  delay(1000); // Esperar un segundo antes de la siguiente medición
}
