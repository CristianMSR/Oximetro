#include <PulseOximeter.h>

PulseOximeter oximeter(19, 18, 1.8, 2.0, 10);

void setup() {
    Serial.begin(115200);
    oximeter.begin();
}

void loop() {
    oximeter.medir();
    Serial.print("SPO2: ");
    Serial.println(oximeter.getSPO2());
    Serial.print("BPM: ");
    Serial.println(oximeter.getBPM());
}
