#ifndef OXIMETRO_H
#define OXIMETRO_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

class Oximetro {
public:
    Oximetro(int pinLedR, int pinLedIR, float umbralR = 1.8, float umbralIR = 2.0);
    
    void begin();
    void medir();
    int calcularSPO2();
    int calcularBPM();

private:
    // Pines
    int led_R;
    int led_IR;

    // Umbrales
    float umbral_R;
    float umbral_IR;

    // ADS1115
    Adafruit_ADS1115 ads;
    const float ads_Res = 0.0001875;

    // Variables de medición
    static const int n_Periodos = 10;
    float picos_R[n_Periodos];
    float picos_IR[n_Periodos];
    int BPM[18];
    int indice_R;
    int indice_IR;
    int indice_BPM;
    float previousMillis;
    float ultimo_R;
    float ultimo_IR;
    float vmax_R;
    float vmax_IR;
    bool creciente_R;
    bool creciente_IR;

    // Métodos internos
    void encenderRojo();
    void encenderInfrarrojo();
    void medirLedRojo();
    void medirLedInfrarrojo();
    void detectarPico(float valorActual, float *picos, int *indicePico, float *maxAmplitud, bool *tendenciaCreciente, float *ultimoValor, float umbral);
    void registrarPulso();
};

#endif
