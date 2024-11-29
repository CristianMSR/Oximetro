#include "PulseOximeter.h"

PulseOximeter::PulseOximeter(int pinLedR, int pinLedIR, float umbralR, float umbralIR, int nPeriodos)
    : ledR(pinLedR), ledIR(pinLedIR), umbralR(umbralR), umbralIR(umbralIR), nPeriodos(nPeriodos) {
    picosR = new float[nPeriodos]{0};
    picosIR = new float[nPeriodos]{0};
    indiceR = indiceIR = indiceBPM = 0;
    crecienteR = crecienteIR = true;
    ultimoR = ultimoIR = vmaxR = vmaxIR = 0;
}

void PulseOximeter::begin() {
    pinMode(ledR, OUTPUT);
    pinMode(ledIR, OUTPUT);
    digitalWrite(ledR, LOW);
    digitalWrite(ledIR, LOW);

    if (!ads.begin()) {
        Serial.println("No se encontro el ADS1115!");
        while (1);
    }
    ads.setGain(GAIN_TWOTHIRDS);
}

void PulseOximeter::medir() {
    if (medirR) {
        encenderRojo();
        delay(5000);
        medirLedRojo();
        medirR = false;
    } else {
        encenderInfrarrojo();
        delay(7000);
        medirLedInfrarrojo();
        medirR = true;
    }

    if ((indiceR == (nPeriodos - 1)) && (indiceIR == (nPeriodos - 1))) {
        digitalWrite(ledIR, LOW);
        SPO2 = calcularSPO2();
        BPM_total = calcularBPM();
        indiceR = 0;
        indiceIR = 0;
        indiceBPM = 0;
    }
}

void PulseOximeter::encenderRojo() {
    digitalWrite(ledR, HIGH);
    digitalWrite(ledIR, LOW);
}

void PulseOximeter::medirLedRojo() {
    while (indiceR < (nPeriodos - 1)) {
        int16_t lectR = ads.readADC_SingleEnded(0);
        voltR = lectR * adsRes;
        detectarPico(voltR, picosR, &indiceR, &vmaxR, &crecienteR, &ultimoR, umbralR);
        delay(10);
    }
}

void PulseOximeter::encenderInfrarrojo() {
    digitalWrite(ledIR, HIGH);
    digitalWrite(ledR, LOW);
}

void PulseOximeter::medirLedInfrarrojo() {
    while (indiceIR < (nPeriodos - 1)) {
        int16_t lectIR = ads.readADC_SingleEnded(0);
        voltIR = lectIR * adsRes;
        detectarPico(voltIR, picosIR, &indiceIR, &vmaxIR, &crecienteIR, &ultimoIR, umbralIR);
        delay(10);
    }
}

void PulseOximeter::detectarPico(float valorActual, float *picos, int *indicePico, float *maxAmplitud, bool *tendenciaCreciente, float *ultimoValor, float umbral) {
    if (valorActual > *ultimoValor) {
        *tendenciaCreciente = true;
        if (valorActual > *maxAmplitud) {
            *maxAmplitud = valorActual;
        }
    }
    if (*tendenciaCreciente && (valorActual < *ultimoValor) && (valorActual > umbral)) {
        picos[*indicePico] = *maxAmplitud;
        if (*indicePico == 0) {
            previousMillis = millis();
        } else {
            registrarPulso();
        }
        *indicePico += 1;
        *maxAmplitud = 0;
        *tendenciaCreciente = false;
    }
    *ultimoValor = valorActual;
}

void PulseOximeter::registrarPulso() {
    float currentMillis = millis();
    float deltaTime = (currentMillis - previousMillis) / 1000;
    previousMillis = currentMillis;
    BPM[indiceBPM] = (int)(60.0 / deltaTime);
    indiceBPM += 1;
}

int PulseOximeter::calcularSPO2() {
    float sumapicosR = 0, sumapicosIR = 0;
    for (int i = 0; i < nPeriodos; i++) {
        sumapicosR += picosR[i];
        sumapicosIR += picosIR[i];
    }
    float promR = (sumapicosR / nPeriodos) - 1.64;
    float promIR = (sumapicosIR / nPeriodos) - 1.64;

    return promIR != 0 ? (-25 * (promR / promIR) + 110) * 0.93 : 0;
}

int PulseOximeter::calcularBPM() {
    float sumaBPM = 0;
    for (int i = 0; i < 8; i++) {
        sumaBPM += BPM[i];
    }
    return (sumaBPM / 8);
}

int PulseOximeter::getSPO2() const {
    return SPO2;
}

int PulseOximeter::getBPM() const {
    return BPM_total;
}
