#include "Oximetro.h"

Oximetro::Oximetro(int pinLedR, int pinLedIR, float umbralR, float umbralIR)
    : led_R(pinLedR), led_IR(pinLedIR), umbral_R(umbralR), umbral_IR(umbralIR), 
      indice_R(0), indice_IR(0), indice_BPM(0), previousMillis(0),
      ultimo_R(0), ultimo_IR(0), vmax_R(0), vmax_IR(0), 
      creciente_R(true), creciente_IR(true) {
    memset(picos_R, 0, sizeof(picos_R));
    memset(picos_IR, 0, sizeof(picos_IR));
    memset(BPM, 0, sizeof(BPM));
}

void Oximetro::begin() {
    pinMode(led_R, OUTPUT);
    pinMode(led_IR, OUTPUT);
    digitalWrite(led_R, LOW);
    digitalWrite(led_IR, LOW);

    if (!ads.begin()) {
        Serial.println("No se encontro el ADS1115!");
        while (1);
    }
    ads.setGain(GAIN_TWOTHIRDS);
}

void Oximetro::medir() {
    if (indice_R < n_Periodos) {
        encenderRojo();
        delay(5000);
        medirLedRojo();
    } else if (indice_IR < n_Periodos) {
        encenderInfrarrojo();
        delay(5000);
        medirLedInfrarrojo();
    }
}

void Oximetro::encenderRojo() {
    digitalWrite(led_R, HIGH);
    digitalWrite(led_IR, LOW);
}

void Oximetro::medirLedRojo() {
    int16_t lect_R = ads.readADC_SingleEnded(0);
    float volt_R = lect_R * ads_Res;
    detectarPico(volt_R, picos_R, &indice_R, &vmax_R, &creciente_R, &ultimo_R, umbral_R);
}

void Oximetro::encenderInfrarrojo() {
    digitalWrite(led_IR, HIGH);
    digitalWrite(led_R, LOW);
}

void Oximetro::medirLedInfrarrojo() {
    int16_t lect_IR = ads.readADC_SingleEnded(0);
    float volt_IR = lect_IR * ads_Res;
    detectarPico(volt_IR, picos_IR, &indice_IR, &vmax_IR, &creciente_IR, &ultimo_IR, umbral_IR);
}

void Oximetro::detectarPico(float valorActual, float *picos, int *indicePico, float *maxAmplitud, bool *tendenciaCreciente, float *ultimoValor, float umbral) {
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

void Oximetro::registrarPulso() {
    float currentMillis = millis();
    float deltaTime = (currentMillis - previousMillis) / 1000;
    previousMillis = currentMillis;
    BPM[indice_BPM] = (int)(60.0 / deltaTime);
    indice_BPM++;
}

int Oximetro::calcularSPO2() {
    float suma_R = 0;
    float suma_IR = 0;
    for (int i = 0; i < n_Periodos; i++) {
        suma_R += picos_R[i];
        suma_IR += picos_IR[i];
    }
    float prom_R = suma_R / n_Periodos - 1.64;
    float prom_IR = suma_IR / n_Periodos - 1.64;

    if (prom_IR != 0) {
        return (-25 * (prom_R / prom_IR) + 110) * 0.93;
    }
    return 0;
}

int Oximetro::calcularBPM() {
    float suma = 0;
    for (int i = 0; i < 8; i++) {
        suma += BPM[i];
    }
    return suma / 8;
}
