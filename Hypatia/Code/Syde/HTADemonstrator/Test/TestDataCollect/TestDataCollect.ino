#include <HX711.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

// HX711 Pins
const uint8_t clockPin1 = 5;
const uint8_t dataPin1 = 4;
const uint8_t clockPin2 = 7;
const uint8_t dataPin2 = 6;

// DAC Pins
const int SDA_Pin = 41;
const int SCL_Pin = 42;

// DAC Eingangspin
const int dacInputPin1 = 3;
const int dacInputPin2 = 18;

Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;

// HX711 Instanzen
HX711 scale1;
HX711 scale2;

// Filterparameter
const int p = 10; // Tiefe des Filters
long buffer1[p], buffer2[p];
int bufIndex = 0;

// Kalibrierung
float C1 = 0; // Offset Sensor 1
float K1 = 1.0; // Kalibrierfaktor Sensor 1 (g/Einheit)
float C2 = 0;
float K2 = 1.0;

// PID Parameter
float Kp = 1.2;
float Kd = 0.4;
float prevFl1 = 0, prevFl2 = 0;

void calibrate(HX711 &scale, float &C, float &K, const char* label) {
  Serial.print("Starte Kalibrierung fuer ");
  Serial.println(label);

  Serial.println("Lege kein Gewicht auf die Waegezelle und druecke eine Taste...");
  while (!Serial.available());
  Serial.read();
  C = scale.read_average(10);
  Serial.print("Offset (C) = "); Serial.println(C);

  Serial.println("Lege ein bekanntes Gewicht auf die Waegezelle und gib das Gewicht in Gramm ein:");
  while (!Serial.available());
  float F = Serial.parseFloat();

  long raw = scale.read_average(10);
  long delta = raw - C;
  if (delta == 0) {
    Serial.println("Fehler: Ungueltige Kalibrierung (delta = 0). K wird auf 1.0 gesetzt.");
    K = 1.0;
  } else {
    K = F / delta;
  }
  Serial.print("Kalibrierfaktor (K) = "); Serial.println(K);
}

void setup() {
  Serial.begin(115200);
  delay(6000);
  Wire.begin(SDA_Pin, SCL_Pin);
  dac1.begin(0x60);
  dac2.begin(0x61);

  scale1.begin(dataPin1, clockPin1);
  scale2.begin(dataPin2, clockPin2);

  pinMode(dacInputPin1, INPUT);
  pinMode(dacInputPin2, INPUT);

  for (int i = 0; i < p; i++) {
    buffer1[i] = scale1.read();
    buffer2[i] = scale2.read();
  }

  calibrate(scale1, C1, K1, "Sensor 1");
  calibrate(scale2, C2, K2, "Sensor 2");
}

long filter(HX711 &scale, long *buffer) {
  long raw = scale.read();
  buffer[bufIndex] = raw;
  long sum = 0;
  for (int i = 0; i < p; i++) {
    sum += buffer[i];
  }
  return sum / p;
}

float pid(float Fl, float &prevFl) {
  float prop = Kp * Fl;
  float deriv = Kd * (Fl - prevFl);
  prevFl = Fl;
  float Vd = prop + deriv;
  return Vd;
}

void outputToDAC(Adafruit_MCP4725 &dac, float Vd) {
  float Vd_mV = constrain(Vd, -1500, 1500) + 1500;
  float scaled = (Vd_mV / 3000.0) * 4095.0;
  dac.setVoltage((uint16_t)scaled, false);
}

void loop() {
  long raw1 = filter(scale1, buffer1);
  long raw2 = filter(scale2, buffer2);

  float Fl1 = K1 * (raw1 - C1);
  float Fl2 = K2 * (raw2 - C2);

  float Vd1 = pid(Fl1, prevFl1);
  float Vd2 = pid(Fl2, prevFl2);

  outputToDAC(dac1, Vd1);
  outputToDAC(dac2, Vd2);

  int readDAC1 = analogRead(dacInputPin1);
  int readDAC2 = analogRead(dacInputPin2);
  float vDAC1 = readDAC1 * (3300.0 / 4095.0);
  float vDAC2 = readDAC2 * (3300.0 / 4095.0);

  Serial.print("Sensor1 raw:"); Serial.print(raw1);
  Serial.print(" Filtered:"); Serial.print(Fl1);
  Serial.print(" Vd:"); Serial.print(Vd1);
  Serial.print(" DAC_in:"); Serial.print(vDAC1); Serial.print(" mV");
  Serial.print("\t");

  Serial.print("Sensor2 raw:"); Serial.print(raw2);
  Serial.print(" Filtered:"); Serial.print(Fl2);
  Serial.print(" Vd:"); Serial.print(Vd2);
  Serial.print(" DAC_in:"); Serial.print(vDAC2); Serial.println(" mV");

  bufIndex = (bufIndex + 1) % p;
  delay(50);
}
