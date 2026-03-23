#include "CMCP4725.h"
#include <Adafruit_MCP4725.h>
#include <Wire.h>

// Eigener I2C-Bus (ESP32: frei wählbare SDA/SCL-Pins)
TwoWire I2C = TwoWire(0);
Adafruit_MCP4725 DAC;

CMCP4725::CMCP4725() {
  digitalData_constrained = 0.0f;
  scaled       = 0.0f;
  deadband     = 0.0f;
  halfDeadband = 0.0f;
  vOut         = 0.0f;

  // Testwave
  useTestWave = false;
  waveVal     = 4095.0f;   // Start am oberen Ende
  waveStep    = 105.0f;    // Schrittweite pro Aufruf
  measured    = 0.0f;

  dacInputPinSet = -1;
}

void CMCP4725::setupDout(uint8_t dataPin, uint8_t clockPin) {
  // I2C starten (400 kHz ist für MCP4725 ok)
  I2C.begin(dataPin, clockPin, 400000);
  // MCP4725 an Standardadresse 0x60 initialisieren (A0 = GND).
  // Falls dein Modul 0x61 hat, hier anpassen.
  DAC.begin(0x60, &I2C);
}

void CMCP4725::setupAin(uint8_t dacInputPin) {
  dacInputPinSet = dacInputPin;
  pinMode(dacInputPin, INPUT);
  // WICHTIG: Kein DAC.begin() hier! (Nur Output-Setup initialisiert den DAC)
}

void CMCP4725::setTestWave() {
  // Sägezahn (4095 -> 0 -> 4095 ...)
  waveVal -= waveStep;
  if (waveVal < 0.0f) waveVal = 4095.0f;

  if (waveVal > 4095.0f) waveVal = 4095.0f;  // harte Klammer
  DAC.setVoltage((uint16_t)waveVal, false);
}

void CMCP4725::setDigitalData(float digitalData) {
  // Eingangsbereich auf +-1500 mV beschränken
  digitalData_constrained = constrain(digitalData, -1500.0f, 1500.0f);

  // Deadband um 0 herum
  halfDeadband = deadband * 0.5f;
  if (fabsf(digitalData_constrained) <= halfDeadband) {
    // Neutral: 1.5 V
    scaled = 1500.0f;
  } else {
    // Offset auf 0..3000 mV verschieben
    scaled = digitalData_constrained + 1500.0f;
  }

  // Endgültig auf 0..3000 mV begrenzen (bewusst < 3.3 V Vollaussteuerung)
  scaled = constrain(scaled, 0.0f, 3000.0f);

  // mV -> 12-bit DAC Code (bei 3.3 V Versorgung)
  // Hinweis: 3000 mV entspricht ~3723 LSB; wenn du volle 3.3 V willst,
  // skaliere oben bis 3300 mV.
  uint16_t dacValue = (uint16_t)((scaled / 3300.0f) * 4095.0f);
  DAC.setVoltage(dacValue, false);
}

float CMCP4725::getAnalogData(uint8_t dacInputPin) {
  // Falls vorher setupAin() genutzt wurde, den gemerkten Pin bevorzugen
  int pin = (dacInputPinSet >= 0) ? dacInputPinSet : (int)dacInputPin;
  return analogRead(pin) * (3300.0f / 4095.0f);  // Rückgabe in mV
}
