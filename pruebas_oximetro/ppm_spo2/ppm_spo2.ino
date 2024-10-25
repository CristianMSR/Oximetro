#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Crear objeto ADS1115
Adafruit_ADS1115 ads;

// Variables globales
unsigned long previousMillis = 0;  // Para medir el tiempo entre picos
float bpm = 0;  // Pulsaciones por minuto
float umbral = 0.2;  // Umbral de detección de picos
const float umbralMinimo = 0.05;  // Umbral mínimo para evitar que el umbral caiga demasiado bajo
bool peakDetected = false;  // Para verificar si el pico ya fue detectado
int ppmValue = 0;  // Valor de BPM (Pulsaciones por minuto)

// Definir el tamaño del buffer de picos
const int numPicos = 5;
float picos[numPicos] = {0};  // Array para almacenar los últimos 5 picos
int indicePico = 0;  // Índice para el array de picos

// Tamaño del filtro de suavizado
const int tamanoFiltro = 10;
float bufferFiltro[tamanoFiltro] = {0};
int indiceFiltro = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  beginADS();
}

void loop() {
  // Leer valor analógico del ADS1115 en el canal 0
  int16_t adc0 = ads.readADC_SingleEnded(0);
  // Convertir el valor ADC a voltaje
  float voltage = adc0 * 0.125 / 1000.0;  // 125uV por bit
  
  // Aplicar un filtro de suavizado
  float signalSuavizada = filtrarSenal(voltage);

  // Calcular PPM
  ppmValue = calcularPPM(signalSuavizada);

  // Enviar los datos al Plotter Serie
  // Imprimir el voltaje (señal) y las PPM en la misma línea, separados por una coma
  Serial.print(signalSuavizada, 3);  // Voltaje con 3 decimales
  Serial.print(",");         // Separador de datos
  Serial.println(ppmValue);  // PPM en la misma línea

  delay(10);  // Muestreo cada 10 ms
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void beginADS(){
  if (!ads.begin()) {
    Serial.println("¡No se encontró el ADS1115!");
    while (1);
  }

  ads.setGain(GAIN_ONE);  // Configura el PGA a +/- 4.096V (1 bit = 0.125mV)
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Función para calcular el BPM y ajustar el umbral dinámicamente
int calcularPPM(float signalValue) {
  unsigned long currentMillis = millis();  // Tiempo actual en milisegundos

  // Detectar un pico en la señal
  if (signalValue > umbral && !peakDetected) {
    peakDetected = true;  // Marca que el pico ha sido detectado
    unsigned long deltaTime = currentMillis - previousMillis;  // Diferencia de tiempo entre picos
    previousMillis = currentMillis;  // Actualiza el último tiempo de pico
    
    // Guardar el valor de este pico
    picos[indicePico] = signalValue;
    indicePico = (indicePico + 1) % numPicos;  // Avanzar el índice circularmente

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

  // Ajustar el umbral basado en los últimos picos detectados
  ajustarUmbral();

  return bpm;  // Retornar BPM calculado
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Función para ajustar el umbral dinámicamente
void ajustarUmbral() {
  float sumaPicos = 0;
  for (int i = 0; i < numPicos; i++) {
    sumaPicos += picos[i];
  }
  float promedioPicos = sumaPicos / numPicos;

  // Ajusta el umbral a un porcentaje del promedio de los picos (por ejemplo, 80%)
  umbral = promedioPicos * 0.8;

  // Asegurar que el umbral no caiga por debajo de un valor mínimo
  if (umbral < umbralMinimo) {
    umbral = umbralMinimo;
  }

  // Imprime el nuevo umbral para monitoreo
  Serial.println("Nuevo umbral: " + String(umbral));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Función para aplicar un filtro de suavizado (media móvil)
float filtrarSenal(float nuevoValor) {
  bufferFiltro[indiceFiltro] = nuevoValor;
  indiceFiltro = (indiceFiltro + 1) % tamanoFiltro;  // Avanzar el índice circularmente

  // Calcular la media de los valores en el buffer
  float suma = 0;
  for (int i = 0; i < tamanoFiltro; i++) {
    suma += bufferFiltro[i];
  }

  return suma / tamanoFiltro;  // Retornar el valor suavizado
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
