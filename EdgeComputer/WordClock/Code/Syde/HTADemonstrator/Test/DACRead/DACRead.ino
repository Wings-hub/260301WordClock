// File: dac_web_graph.ino
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <WiFi.h>
#include <WebServer.h>
#include "HX711.h"
#include "image.h" 

const char *ssid = "Syde Technology";
const char *password = "nVvffsb#95B!";

const int SDA_Pin = 41;
const int SCL_Pin = 42;
const int Read_Pin1 = 8;
const int Read_Pin2 = 3;

HX711 myScale1;
HX711 myScale2;
#define dataPin1 4
#define clockPin1 5
#define dataPin2 6
#define clockPin2 7

Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;
WebServer server(80);

#define BUFFER_SIZE 200
int signalBuffer1[BUFFER_SIZE];
int signalBuffer2[BUFFER_SIZE];
int bufferIndex = 0;

unsigned long lastUpdate = 0;
const int UPDATE_INTERVAL_MS = 20;

String serialData1 = "Waiting for data...";
String serialData2 = "Waiting for data...";
String ipAddress = "Not connected";
String webWeight1 = "Waiting for weight...";
String webWeight2 = "Waiting for weight...";
bool startMeasurement1 = false;
bool startMeasurement2 = false;

float filtered1 = 0.0;
float filtered2 = 0.0;
float alpha1 = 0.9; // Smoothing factor for Load Cell 1
float alpha2 = 0.9; // Smoothing factor for Load Cell 2

void handleResetAll() {
  startMeasurement1 = false;
  startMeasurement2 = false;
  webWeight1 = "Waiting for weight...";
  webWeight2 = "Waiting for weight...";
  server.send(200, "text/plain", "All measurements reset");
}

void handleImage() {
    server.send_P(200, "image/png", (const char*)ImageData, sizeof(ImageData));
}

void handleSerialData1() {
  server.send(200, "text/plain", serialData1);
}

void handleSerialData2() {
  server.send(200, "text/plain", serialData2);
}

void handleWeightData1() {
  server.send(200, "text/plain", webWeight1);
}

void handleWeightData2() {
  server.send(200, "text/plain", webWeight2);
}

void handleZero1() {
  zero1();
  server.send(200, "text/plain", "SetZero1");
}

void handleZero2() {
  zero2();
  server.send(200, "text/plain", "SetZero2");
}

void handleEnter1() {
  enter1();
  server.send(200, "text/plain", "SetEnter1");
}

void handleEnter2() {
  enter2();
  server.send(200, "text/plain", "SetEnter2");
}

void handleStart1() {
  startMeasurement1 = true;
  server.send(200, "text/plain", "Measuring Weight One...");
}

void handleStart2() {
  startMeasurement2 = true;
  server.send(200, "text/plain", "Measuring Weight Two...");
}

void zero1() {
  myScale1.tare(20);
  filtered1 = 0.0;
}

void zero2() {
  myScale2.tare(20);
  filtered2 = 0.0;
}

void enter1() {
  float calibrationFactor1 = myScale1.get_units(10) / 1003.0;
  myScale1.set_scale(calibrationFactor1);
}

void enter2() {
  float calibrationFactor2 = myScale2.get_units(10) / 1003.0;
  myScale2.set_scale(calibrationFactor2);
}

float start1() {
  float raw = myScale1.read();
  filtered1 = alpha1 * raw + (1.0 - alpha1) * filtered1;
  return filtered1;
}

float start2() {
  float raw = myScale2.read();
  filtered2 = alpha2 * raw + (1.0 - alpha2) * filtered2;
  return filtered2;
}

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>SYDE HTA Loadcell Response</title>
        <style>
            body { font-family: Arial, sans-serif; background-color: #f4f4f9; text-align: center; }
            .container { max-width: 800px; margin: auto; background: white; padding: 20px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); border-radius: 10px; }
            .frame-container { display: flex; justify-content: space-around; }
            .frame { border: 2px solid #ccc; padding: 20px; border-radius: 10px; background-color: #fff; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); width: 45%; }
            button { background-color: #02aff3; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }
            button:hover { background-color: #6e6e6e; }
            #graph { border: 2px solid white; background-color: #f0f0f0; display: block; margin-bottom: 10px; }
            #legend { font-family: sans-serif; }
            .legend-item { display: inline-block; margin-right: 20px; }
            .color-box { width: 12px; height: 12px; display: inline-block; margin-right: 5px; vertical-align: middle; }
        </style>
    </head>
    <body>
        <div class="container">
            <img src="/image" alt="SYDE Logo"style="max-width: 100%; height: auto;">
            <h1>HTA Loadcell Communication Check</h1>
            <div class="reset-container">
            <button onclick="ResetAll()">Reset All</button>
            <h2> </h2>
            </div>
            <div class="frame-container">
                <div class="frame">
                    <h2>Loadcell 1</h2>
                    <button onclick="Zero1()">Zero</button>
                    <button onclick="Enter1()">Enter</button>
                    <button onclick="Start1()">Start</button>
                    <h2 id="status1">Status: Waiting...</h2>
                    <h2 id="weight1">Weight: Waiting...</h2>
                </div>
                <div class="frame">
                    <h2>Loadcell 2</h2>
                    <button onclick="Zero2()">Zero</button>
                    <button onclick="Enter2()">Enter</button>
                    <button onclick="Start2()">Start</button>
                    <h2 id="status2">Status: Waiting...</h2>
                    <h2 id="weight2">Weight: Waiting...</h2>
                </div>
            </div>
            
            <div class="frame-container">   
            <div class="frame">
            <h2>Filter option 2</h2>        
            <p>
                Smoothing Load Cell 1:
                <input type="range" id="alpha1" min="0" max="100" value="10" oninput="updateAlpha1(this.value)">
                <span id="alpha1val">0.90</span> <!-- ← hier kommt die .xx Anzeige -->
            </p>
            <p id="alpha1percent">Current: 10%</p>

            <p>
                Smoothing Load Cell 2:
                <input type="range" id="alpha2" min="0" max="100" value="10" oninput="updateAlpha2(this.value)">
                <span id="alpha2val">0.90</span>
            </p>
            <p id="alpha2percent">Current: 10%</p>
            

            
            <h3>Filter Behavior Guide</h3>
                <table style="margin:auto; border-collapse:collapse;">
                <tr>
                    <th style="border-bottom:1px solid #ccc; padding:5px;">Smoothing %</th>
                    <th style="border-bottom:1px solid #ccc; padding:5px;">Behavior</th>
                </tr>
                <tr>
                    <td style="padding:5px;">0–20%</td>
                    <td style="padding:5px;">Very fast, no smoothing</td>
                </tr>
                <tr>
                    <td style="padding:5px;">20–50%</td>
                    <td style="padding:5px;">Fast with minor noise reduction</td>
                </tr>
                <tr>
                    <td style="padding:5px;">50–80%</td>
                    <td style="padding:5px;">Balanced: smooth but responsive</td>
                </tr>
                <tr>
                    <td style="padding:5px;">80–100%</td>
                    <td style="padding:5px;">Very smooth, slow reaction</td>
                </tr>
            </table>
            </div>
            </div>
   
            <h2>Live Signal</h2>
            <canvas id="graph" width="800" height="300"></canvas>
            <div id="legend">
                <div class="legend-item"><span class="color-box" style="background-color: blue;"></span>Signal von Pin 8</div>
                <div class="legend-item"><span class="color-box" style="background-color: red;"></span>Signal von Pin 3</div>
            </div>
        </div>
        <script>
            const ctx = document.getElementById('graph').getContext('2d');
            let data1 = new Array(200).fill(0);
            let data2 = new Array(200).fill(0);

            function drawGraph() {
                ctx.clearRect(0, 0, 800, 300);
                ctx.beginPath();
                ctx.strokeStyle = 'blue';
                ctx.moveTo(0, 300 - data1[0] / 4);
                for (let i = 1; i < data1.length; i++) {
                    ctx.lineTo(i * 4, 300 - data1[i] / 4);
                }
                ctx.stroke();

                ctx.beginPath();
                ctx.strokeStyle = 'red';
                ctx.moveTo(0, 300 - data2[0] / 4);
                for (let i = 1; i < data2.length; i++) {
                    ctx.lineTo(i * 4, 300 - data2[i] / 4);
                }
                ctx.stroke();
            }

            async function updateData() {
                const res = await fetch('/data');
                const json = await res.json();
                data1 = json.signal1;
                data2 = json.signal2;
                drawGraph();
            }

            setInterval(updateData, 100);

            function Zero1() { fetch('/zero1').then(r => r.text()).then(t => document.getElementById("status1").innerText = t); }
            function Zero2() { fetch('/zero2').then(r => r.text()).then(t => document.getElementById("status2").innerText = t); }
            function Enter1() { fetch('/enter1').then(r => r.text()).then(t => document.getElementById("status1").innerText = t); }
            function Enter2() { fetch('/enter2').then(r => r.text()).then(t => document.getElementById("status2").innerText = t); }
            function Start1() {
                fetch('/start1').then(r => r.text()).then(t => document.getElementById("status1").innerText = t);
                setInterval(() => {
                    fetch('/weight1').then(r => r.text()).then(w => document.getElementById("weight1").innerText = "Weight: " + w + "g");
                }, 100);
            }
            function Start2() {
                fetch('/start2').then(r => r.text()).then(t => document.getElementById("status2").innerText = t);
                setInterval(() => {
                    fetch('/weight2').then(r => r.text()).then(w => document.getElementById("weight2").innerText = "Weight: " + w + "g");
                }, 100);
            }
            function ResetAll() {
                fetch('/resetall').then(r => r.text()).then(alert);
            }
        </script>
    </body>
    </html>
  )rawliteral");
}


void setup() {
  Serial.begin(115200);
  delay(6000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  ipAddress = WiFi.localIP().toString();
  Serial.println("Connected. IP: " + ipAddress);

  myScale1.begin(dataPin1, clockPin1);
  myScale2.begin(dataPin2, clockPin2);
  myScale1.set_gain(128); // Standard Gain bei HX711 – 80 Hz auf Channel A
  myScale2.set_gain(128);

  Wire.begin(SDA_Pin, SCL_Pin);
  dac1.begin(0x60);
  dac2.begin(0x61);

  pinMode(Read_Pin1, INPUT);
  pinMode(Read_Pin2, INPUT);

  server.on("/", handleRoot);
  server.on("/image", handleImage);
  server.on("/resetall", handleResetAll);
  server.on("/serial1", handleSerialData1);
  server.on("/serial2", handleSerialData2);
  server.on("/weight1", handleWeightData1);
  server.on("/weight2", handleWeightData2);
  server.on("/zero1", handleZero1);
  server.on("/enter1", handleEnter1);
  server.on("/start1", handleStart1);
  server.on("/zero2", handleZero2);
  server.on("/enter2", handleEnter2);
  server.on("/start2", handleStart2);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (startMeasurement1) {
    int weight1 = start1();
    serialData1 = "Measured Weight 1: " + String(weight1);
    webWeight1 = String(weight1);
  }
  if (startMeasurement2) {
    int weight2 = start2();
    serialData2 = "Measured Weight 2: " + String(weight2);
    webWeight2 = String(weight2);
  }

  unsigned long now = millis();
  if (now - lastUpdate > UPDATE_INTERVAL_MS) {
    lastUpdate = now;

    long raw1 = myScale1.read();
    long raw2 = myScale2.read();

    int value1 = map(raw1, -8388608, 8388607, 0, 4095);
    int value2 = map(raw2, -8388608, 8388607, 0, 4095);

    value1 = constrain(value1, 0, 4095);
    value2 = constrain(value2, 0, 4095);

    dac1.setVoltage(value1, false);
    dac2.setVoltage(value2, false);

    signalBuffer1[bufferIndex] = analogRead(Read_Pin1);
    signalBuffer2[bufferIndex] = analogRead(Read_Pin2);
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  }
}

void handleData() {
  String json = "{\"signal1\": [";
  for (int i = 0; i < BUFFER_SIZE; i++) {
    json += String(signalBuffer1[(bufferIndex + i) % BUFFER_SIZE]);
    if (i < BUFFER_SIZE - 1) json += ",";
  }
  json += "], \"signal2\": [";
  for (int i = 0; i < BUFFER_SIZE; i++) {
    json += String(signalBuffer2[(bufferIndex + i) % BUFFER_SIZE]);
    if (i < BUFFER_SIZE - 1) json += ",";
  }
  json += "]}";

  server.send(200, "application/json", json);
}
