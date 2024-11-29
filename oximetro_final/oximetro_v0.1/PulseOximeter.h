#ifndef PULSE_OXIMETER_H
#define PULSE_OXIMETER_H

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

class PulseOximeter {
public:
    PulseOximeter(int pinLedR, int pinLedIR, float umbralR, float umbralIR, int nPeriodos);
    void begin();
    void medir();
    int getSPO2() const;
    int getBPM() const;

private:
    void encenderRojo();
    void medirLedRojo();
    void encenderInfrarrojo();
    void medirLedInfrarrojo();
    void detectarPico(float valorActual, float *picos, int *indicePico, float *maxAmplitud, bool *tendenciaCreciente, float *ultimoValor, float umbral);
    void registrarPulso();
    int calcularSPO2();
    int calcularBPM();

    Adafruit_ADS1115 ads;
    int ledR, ledIR;
    float umbralR, umbralIR;
    int nPeriodos;

    float adsRes = 0.0001875;
    bool medirR = true;

    int SPO2;
    float previousMillis = 0;
    int BPM[18];
    int BPM_total;

    float voltR, voltIR;
    float vmaxR, vmaxIR;

    float *picosR;
    float *picosIR;

    int indiceR, indiceIR, indiceBPM;

    bool crecienteR, crecienteIR;
    float ultimoR, ultimoIR;
};

#endif
