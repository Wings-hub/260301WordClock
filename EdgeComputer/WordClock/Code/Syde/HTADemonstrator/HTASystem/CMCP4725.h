#ifndef CMCP4725_H
#define CMCP4725_H

#include <Arduino.h>

class CMCP4725 {
  public:
    CMCP4725();

    // Initialisiert den DAC (I2C Output)
    void setupDout(uint8_t dataPin, uint8_t clockPin);

    // Merkt sich einen ADC-Pin für Messungen
    void setupAin(uint8_t dacInputPin);

    // Erzeugt eine einfache Testwelle (Sägezahn)
    void setTestWave();

    // Stellt eine Spannung ein (Eingang in mV, ±1500 mV um Neutralpunkt)
    void setDigitalData(float digitalData);

    // Liest am eingestellten oder angegebenen Pin die Spannung in mV
    float getAnalogData(uint8_t dacInputPin);

  private:
    int   dacInputPinSet;

    float digitalData_constrained;
    float scaled;
    float deadband;
    float halfDeadband;
    float vOut;

    // Testwave
    bool  useTestWave;
    float waveVal;
    float waveStep;
    float measured;
};

#endif
