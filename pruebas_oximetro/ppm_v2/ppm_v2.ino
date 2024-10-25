#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Crear objeto ADS1115
Adafruit_ADS1115 ads;

// Variables globales
unsigned long previousMillis = 0;  // Para medir el tiempo entre picos
float bpm = 0;  // Pulsaciones por minuto
const float umbral = 0.4;  // Ajusta el umbral de detección de picos según tu señal
bool peakDetected = false;  // Para verificar si el pico ya fue detectado
int ppmValue = 0;  // Valor de BPM (Pulsaciones por minuto)
int o2Value = 0;   // Valor de SpO2

void setup() {
  Serial.begin(115200);

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
  float voltage = adc0 * 0.125 / 1000.0;  // 0.125mV por bit
  
  // Calcular PPM
  ppmValue = calcularBPM(voltage);

  Serial.print(voltage);
  Serial.print("\t");
  Serial.println(ppmValue);

  delay(30);  // Muestreo cada 30 ms
}


// Función para calcular el BPM
int calcularBPM(float signalValue) {
  unsigned long currentMillis = millis();  // Tiempo actual en milisegundos
  
  // Detectar un pico en la señal
  if (signalValue > umbral && !peakDetected) {
    peakDetected = true;  // Marca que el pico ha sido detectado
    unsigned long deltaTime = currentMillis - previousMillis;  // Diferencia de tiempo entre picos
    previousMillis = currentMillis;  // Actualiza el último tiempo de pico
    
    // Calcular BPM si el tiempo entre picos es válido
    if (deltaTime > 0) {
      float intervaloSegundos = deltaTime / 1000.0;  // Convertir tiempo a segundos
      bpm = 60.0 / intervaloSegundos;  // Calcular pulsaciones por minuto
    }
  }

  // Reiniciar la detección del pico una vez que la señal desciende por debajo del umbral
  if (signalValue < umbral) {
    peakDetected = false;
  }

  return bpm;  // Retornar BPM calculado
}