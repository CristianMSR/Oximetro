#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

//Pines
const int led_R = 19;
const int led_IR = 18;

bool medir_R = 1;
int SPO2 = 0;
float previousMillis = 0;
const float ads_Res = 0.0001875;

const int n_Periodos = 10;
int BPM[18] = { 0 };
int BPM_total = 0;

float umbral_R = 1.8;
float umbral_IR = 2;

float volt_R = 0;
float volt_IR = 0;

float ppm_R = 0;
float ppm_IR = 0;

float vmax_R = 0;
float vmax_IR = 0;

float picos_R[n_Periodos] = { 0 };   // Almacenar picos de n periodos rojo
float picos_IR[n_Periodos] = { 0 };  // Almacenar picos de n periodos infrarrojo

int indice_R = 0;
int indice_IR = 0;
int indice_BPM = 0;

bool creciente_R = true;
bool creciente_IR = true;

float ultimo_R = 0;
float ultimo_IR = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(led_R, OUTPUT);   // Configura el pin como salida
  pinMode(led_IR, OUTPUT);  // Configura el pin como salida
  digitalWrite(led_R, LOW);
  digitalWrite(led_IR, LOW);

  // Iniciar ADS1115
  beginADS();
}

void loop() {

    if (medir_R) {
      encenderRojo();
      //aca un delay
      medirLedRojo();
      medir_R = 0;
    } else {
      encenderInfrarrojo();
      //aca un delay
      medirLedInfrarrojo();
      medir_R = 1;
    }

    if ((indice_R == (n_Periodos - 1)) && (indice_IR == (n_Periodos - 1))) {
      SPO2 = calcularSPO2();
      BPM_total = calcularBPM();
      indice_R = 0;
      indice_IR = 0;
      indice_BPM = 0;
    }
}

void beginADS(){
  Serial.begin(115200);
  if (!ads.begin()) {
    Serial.println("¡No se encontro el ADS1115!");
    while (1)
      ;
  }
  ads.setGain(GAIN_TWOTHIRDS);  // Configura el PGA a +/- 6.114V (1 bit = 0.1875mV)
}

void encenderRojo(){
  digitalWrite(led_R, HIGH);
  digitalWrite(led_IR, LOW);
}

void medirLedRojo() {
  while (indice_R < (n_Periodos - 1)) {
    int16_t lect_R = ads.readADC_SingleEnded(0);
    volt_R = lect_R * ads_Res;  // 0.125mV por bit
    detectarPico(volt_R, picos_R, &indice_R, &vmax_R, &creciente_R, &ultimo_R, umbral_R);
    delay(10);
  }
}

void encenderInfrarrojo(){
  digitalWrite(led_IR, HIGH);
  digitalWrite(led_R, LOW);
}

void medirLedInfrarrojo() {
  while (indice_IR < (n_Periodos - 1)) {
    int16_t lect_IR = ads.readADC_SingleEnded(0);
    volt_IR = lect_IR * ads_Res;
    detectarPico(volt_IR, picos_IR, &indice_IR, &vmax_IR, &creciente_IR, &ultimo_IR, umbral_IR);
    delay(10);
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**************Funcion para detectar picos**************/
void detectarPico(float valorActual, float *picos, int *indicePico, float *maxAmplitud, bool *tendenciaCreciente, float *ultimoValor, float umbral) {
  if (valorActual > *ultimoValor) {
    *tendenciaCreciente = true;  // La senal esta creciendo
    if (valorActual > *maxAmplitud) {
      *maxAmplitud = valorActual;  // Actualizar el valor maximo
    }
  }
  if (*tendenciaCreciente && (valorActual < *ultimoValor) && (valorActual > umbral)) {
    // Si cambia la tendencia hemos detectado un pico

    picos[*indicePico] = *maxAmplitud;
    if (*indicePico == 0) {
      previousMillis = millis();
    } else {
      registrarPulso();
    }
    *indicePico += 1;
    *maxAmplitud = 0;  // Reiniciar la amplitud maxima para el siguiente ciclo
    *tendenciaCreciente = false;
  }
  *ultimoValor = valorActual;  // Guardar el valor actual para la proxima comparacion
}

/**************Funcion para registrar un pulso**************/
void registrarPulso() {
  float currentMillis = millis();                             // Tiempo actual en milisegundos
  float deltaTime = (currentMillis - previousMillis) / 1000;  // Diferencia de tiempo entre picos
  previousMillis = currentMillis;
  BPM[indice_BPM] = (int)(60.0 / deltaTime);  // Calcular pulsaciones por minuto
  indice_BPM += 1;
}

/**************Funcion para calcular SPO2**************/
int calcularSPO2(void) {
  float sumapicos_R = 0;
  float sumapicos_IR = 0;
  for (int i = 0; i < n_Periodos; i++) {
    sumapicos_R += picos_R[i];
  }
  for (int i = 0; i < n_Periodos; i++) {
    sumapicos_IR += picos_IR[i];
  }
  float prom_R = (sumapicos_R / n_Periodos) - 1.64;
  float prom_IR = (sumapicos_IR / n_Periodos) - 1.64;

  if (prom_IR != 0) {
    return (-25 * (prom_R / prom_IR) + 110)*0.93;
  } else {
    return 0;
  }
}

/**************Funcion para calcular BPM**************/
int calcularBPM(void) {
  float sumaBPM = 0;
  for (int i = 0; i < 8; i++) {
    sumaBPM += BPM[i];
  }
  return (sumaBPM / 8);
}