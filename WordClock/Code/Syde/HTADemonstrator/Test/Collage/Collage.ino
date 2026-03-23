#include <HX711.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "Malena";
const char *password = "8 Zeichen!";

IPAddress local_IP(192, 168, 10, 184);      // Freie IP im gleichen Subnetz
IPAddress gateway(192, 168, 10, 1);         // Standardgateway (Router)
IPAddress subnet(255, 255, 255, 0);         // Subnetzmaske
IPAddress primaryDNS(192, 168, 10, 200);    // Lokaler DNS-Server
IPAddress secondaryDNS(8, 8, 8, 8);         // Fallback DNS (Google)

WebServer server(80);

const uint8_t clockPin1 = 5;
const uint8_t dataPin1 = 4;
const uint8_t clockPin2 = 7;
const uint8_t dataPin2 = 6;

const int SDA_Pin = 41;
const int SCL_Pin = 42;
const int dacInputPin1 = 35;
const int dacInputPin2 = 36;

Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;
HX711 scale1;
HX711 scale2;

const int p = 10;
long buffer1[p], buffer2[p];
int bufIndex = 0;

float C1 = 0, K1 = 1.0, C2 = 0, K2 = 1.0;
float Kp = 1.2, Kd = 0.4;
float prevFl1 = 0, prevFl2 = 0;

long raw1 = 0, raw2 = 0;
float Fl1 = 0, Fl2 = 0;
float Vd1 = 0, Vd2 = 0;
float vDAC1 = 0, vDAC2 = 0;

const int maxSamples = 50;
long raw1Buf[maxSamples] = {0};
long raw2Buf[maxSamples] = {0};
float Fl1Buf[maxSamples] = {0};
float Fl2Buf[maxSamples] = {0};
float Vd1Buf[maxSamples] = {0};
float Vd2Buf[maxSamples] = {0};
float vDAC1Buf[maxSamples] = {0};
float vDAC2Buf[maxSamples] = {0};
int sampleIndex = 0;

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Live Sensor Charts</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <script>
    let labels = Array.from({length: 50}, (_, i) => i);
    let raw1Data = [], Fl1Data = [], raw2Data = [], Fl2Data = [], vdac1Data = [], vdac2Data = [];
    let chartHX, chartDAC;

    window.onload = function () {
      const ctxHX = document.getElementById('hxChart').getContext('2d');
      chartHX = new Chart(ctxHX, {
        type: 'line',
        data: {
          labels: labels,
          datasets: [
            { label: 'raw1', borderWidth: 1, data: [] },
            { label: 'Fl1', borderWidth: 1, data: [] },
            { label: 'raw2', borderWidth: 1, data: [] },
            { label: 'Fl2', borderWidth: 1, data: [] }
          ]
        },
        options: {
          animation: false,
          responsive: true,
          scales: {
            y: {}  // ← Auto-Skalierung aktiviert
          }
        }
      });

      const ctxDAC = document.getElementById('dacChart').getContext('2d');
      chartDAC = new Chart(ctxDAC, {
        type: 'line',
        data: {
          labels: labels,
          datasets: [
            { label: 'vDAC1', borderWidth: 1, data: [] },
            { label: 'vDAC2', borderWidth: 1, data: [] }
          ]
        },
        options: {
          animation: false,
          responsive: true,
          scales: {
            y: {}  // ← Auto-Skalierung aktiviert
          }
        }
      });

      fetchData();
      setInterval(fetchData, 200);
    };

    function fetchData() {
      fetch("/data").then(r => r.json()).then(d => {
        if (raw1Data.length >= 50) {
          raw1Data.shift(); Fl1Data.shift(); raw2Data.shift(); Fl2Data.shift(); vdac1Data.shift(); vdac2Data.shift();
        }
        raw1Data.push(d.raw1); Fl1Data.push(d.Fl1);
        raw2Data.push(d.raw2); Fl2Data.push(d.Fl2);
        vdac1Data.push(d.vDAC1); vdac2Data.push(d.vDAC2);

        chartHX.data.datasets[0].data = raw1Data;
        chartHX.data.datasets[1].data = Fl1Data;
        chartHX.data.datasets[2].data = raw2Data;
        chartHX.data.datasets[3].data = Fl2Data;
        chartHX.update();

        chartDAC.data.datasets[0].data = vdac1Data;
        chartDAC.data.datasets[1].data = vdac2Data;
        chartDAC.update();
      });
    }
  </script>
</head>
<body>
  <h2>HX711 Roh- und Filterwerte</h2>
  <canvas id="hxChart" width="600" height="200"></canvas>
  <h2>DAC Ausgang (mV)</h2>
  <canvas id="dacChart" width="600" height="200"></canvas>
</body>
</html>


  )rawliteral";
  server.send(200, "text/html", html);
}

void handleData() {
  int i = (sampleIndex - 1 + maxSamples) % maxSamples;
  String json = "{";
  json += "\"raw1\":" + String(raw1Buf[i]) + ",";
  json += "\"Fl1\":" + String(Fl1Buf[i]) + ",";
  json += "\"Vd1\":" + String(Vd1Buf[i]) + ",";
  json += "\"vDAC1\":" + String(vDAC1Buf[i]) + ",";
  json += "\"raw2\":" + String(raw2Buf[i]) + ",";
  json += "\"Fl2\":" + String(Fl2Buf[i]) + ",";
  json += "\"Vd2\":" + String(Vd2Buf[i]) + ",";
  json += "\"vDAC2\":" + String(vDAC2Buf[i]);
  json += "}";
  server.send(200, "application/json", json);
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

long filter(HX711 &scale, long *buffer) {
  long raw = scale.read();
  buffer[bufIndex] = raw;
  long sum = 0;
  for (int i = 0; i < p; i++) {
    sum += buffer[i];
  }
  return sum / p;
}

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

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected. IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  raw1 = filter(scale1, buffer1);
  raw2 = filter(scale2, buffer2);
  Fl1 = K1 * (raw1 - C1);
  Fl2 = K2 * (raw2 - C2);
  Vd1 = pid(Fl1, prevFl1);
  Vd2 = pid(Fl2, prevFl2);
  outputToDAC(dac1, Vd1);
  outputToDAC(dac2, Vd2);
  int read1 = analogRead(dacInputPin1);
  int read2 = analogRead(dacInputPin2);
  vDAC1 = read1 * (3300.0 / 4095.0);
  vDAC2 = read2 * (3300.0 / 4095.0);

  raw1Buf[sampleIndex] = raw1;
  raw2Buf[sampleIndex] = raw2;
  Fl1Buf[sampleIndex] = Fl1;
  Fl2Buf[sampleIndex] = Fl2;
  Vd1Buf[sampleIndex] = Vd1;
  Vd2Buf[sampleIndex] = Vd2;
  vDAC1Buf[sampleIndex] = vDAC1;
  vDAC2Buf[sampleIndex] = vDAC2;

  sampleIndex = (sampleIndex + 1) % maxSamples;

  bufIndex = (bufIndex + 1) % p;
  server.handleClient();
  delay(10);
}
