//20.03.25  09:42

// Libraries
#include <WiFi.h>         //including library for Wifi
#include <WebServer.h>    //including library for WebServer Design
#include <Adafruit_NeoPixel.h>    //including library for LED
#include <Adafruit_MCP4725.h> // MCP4725 library from adafruit
#include <Wire.h>
#include "HX711.h"
#include "image.h"  

// Global Variables

//Loadcell
uint8_t clockPin1 = 4;
uint8_t dataPin1 = 5;
uint8_t clockPin2 = 6;
uint8_t dataPin2 = 7;

HX711 myScale1;
HX711 myScale2;

//DAC
Adafruit_MCP4725 MCP4725_1;
Adafruit_MCP4725 MCP4725_2;

int SDA_Pin = 41;
int SCL_Pin = 42;


//Wifi
const char *ssid = "Syde Technology";
const char *password = "nVvffsb#95B!";

IPAddress local_IP(192, 168, 10, 184);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 10, 200);
IPAddress secondaryDNS(8, 8, 4, 4);

WebServer server(80);

String serialData1 = "Waiting for data...";
String serialData2 = "Waiting for data...";
String ipAddress = "Not connected";
String webWeight1 = "Waiting for weight...";
String webWeight2 = "Waiting for weight...";
bool startMeasurement1 = false;
bool startMeasurement2 = false;

// LED
#define LED_PIN 21
Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);

//Functions
void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
    strip.setPixelColor(0, strip.Color(r, g, b));
    strip.show();
}

void handleResetAll() {
    startMeasurement1 = false;
    startMeasurement2 = false;
    webWeight1 = "Waiting for weight...";
    webWeight2 = "Waiting for weight...";
    setLEDColor(0, 255, 0); // Grün: bereit nach Reset
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
    setLEDColor(0, 0, 255); // Blau für aktive Messung
}

void handleStart2() {
    startMeasurement2 = true;
    server.send(200, "text/plain", "Measuring Weight Two...");
    setLEDColor(0, 0, 255); // Blau für aktive Messung
}

//Calibration
void zero1() {
    myScale1.tare(20);
}

void zero2() {
    myScale2.tare(20);
}

void enter1(){
    float calibrationFactor1 = myScale1.get_units(10)/1000;// 1000 mass
    myScale1.set_scale(calibrationFactor1);
}

void enter2(){
    float calibrationFactor2 = myScale2.get_units(10)/1000;
    myScale2.set_scale(calibrationFactor2);
}

float start1() {
    float raw = myScale1.get_units(10);
}

float start2() {
    float raw = myScale2.get_units(10);
}

//Webserver Design
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
        </div>
       
      <script>
            function Zero1() {
                fetch('/zero1')
                .then(response => response.text())
                .then(data => document.getElementById("status1").innerText = data);
            }
            function Zero2() {
                fetch('/zero2')
                .then(response => response.text())
                .then(data => document.getElementById("status2").innerText = data);
            }
            function Enter1() {
                fetch('/enter1')
                .then(response => response.text())
                .then(data => document.getElementById("status1").innerText = data);
            }
            function Enter2() {
                fetch('/enter2')
                .then(response => response.text())
                .then(data => document.getElementById("status2").innerText = data);
            }
            function Start1() {
                fetch('/start1')
                .then(response => response.text())
                .then(data => document.getElementById("status1").innerText = data);
                fetchWeightContinuously1();
            }
            function Start2() {
                fetch('/start2')
                .then(response => response.text())
                .then(data => document.getElementById("status2").innerText = data);
                fetchWeightContinuously2();
            }
            function fetchWeightContinuously1() {
                setInterval(() => {
                    fetch('/weight1')
                    .then(response => response.text())
                    .then(data => document.getElementById("weight1").innerText = "Weight: " + data + "g");
                }, 100);
            }
            function fetchWeightContinuously2() {
                setInterval(() => {
                    fetch('/weight2')
                    .then(response => response.text())
                    .then(data => document.getElementById("weight2").innerText = "Weight: " + data + "g");
                }, 100);
            }
            function ResetAll() {
                fetch('/resetall')
                .then(response => response.text())
                .then(data => alert(data));
            }



      </script>
    </body>
    </html>
    )rawliteral");
}

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_Pin,SCL_Pin);
    MCP4725_1.begin(0x60, &Wire); // Default I2C Address of MCP4725 breakout board (sparkfun)  
    MCP4725_2.begin(0x61, &Wire); // Default I2C Address of MCP4725 breakout board (sparkfun)  
    delay(6000);

    strip.begin();
    strip.show();
    setLEDColor(255, 165, 0); // Orange: startet Verbindung

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA Failed to configure");
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    setLEDColor(0, 255, 0); // Grün: verbunden und bereit

    ipAddress = WiFi.localIP().toString();
    Serial.println("Connected. IP: " + ipAddress);

    myScale1.begin(dataPin1, clockPin1);
    myScale2.begin(dataPin2, clockPin2);

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

    server.begin();
}

void loop() {
    server.handleClient();
    if (startMeasurement1) {
        int weight1 = start1();
        serialData1 = "Measured Weight 1: " + String(weight1);
        webWeight1 = String(weight1);

        // Normalize and send to DAC
        uint16_t dacOutput1 = map(constrain(weight1, 0, 10000), 0, 10000, 0, 4095);
        MCP4725_1.setVoltage(dacOutput1, false); // false: don't persist EEPROM
    }
    if (startMeasurement2) {
        int weight2 = start2();
        serialData2 = "Measured Weight 2: " + String(weight2);
        webWeight2 = String(weight2);
        // Normalize and send to DAC
        uint16_t dacOutput2 = map(constrain(weight2, 0, 10000), 0, 10000, 0, 4095);
        MCP4725_2.setVoltage(dacOutput2, false); // false: don't persist EEPROM
    }
    if (startMeasurement1 && startMeasurement2) {
        setLEDColor(128, 0, 128); // Lila wenn beide messen
    }
}
