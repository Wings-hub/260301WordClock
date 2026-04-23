//19.03.25  10:05

// Libraries
#include <WiFi.h>         //including library for Wifi
#include <WebServer.h>    //including library for WebServer Design
#include "HX711.h"
#include "image.h"  

// Global Variables

//Loadcell
uint8_t dataPin = 5;
uint8_t clockPin = 4;

HX711 myScale;

//Wifi
const char *ssid = "Syde Technology";
const char *password = "nVvffsb#95B!";

WebServer server(80);

String serialData = "Waiting for data...";  // Stores received Serial data
String ipAddress = "Not connected";  // Stores ESP32's IP Address
String webWeight = "Waiting for weight...";
bool startMeasurement = false;

//Functions
void handleSerialData() {
    server.send(200, "text/plain", serialData);
}

void handleWeightData() {
    server.send(200, "text/plain", webWeight);
}

void handleImage() {
    server.send_P(200, "image/png", (const char*)ImageData, sizeof(ImageData));
}

void handleZero() {
    zero();
    server.send(200, "text/plain", "SetZero");
}

void handleEnter() {
    enter();
    server.send(200, "text/plain", "SetEnter");
}

void handleStart() {
    startMeasurement = true;
    server.send(200, "text/plain", "Measuring Weight...");
}

//Calibration
void zero() {
    myScale.tare();
}

void enter(){
    float claibrtionFactor = myScale.get_units(10)/1;
    myScale.set_scale(claibrtionFactor); // Set calibration factor (adjust as necessary)
}

float start() {
    return myScale.get_units(10);  // Read weight from the scale
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
            .container { max-width: 600px; margin: auto; background: white; padding: 20px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); border-radius: 10px; }
            button { background-color: #02aff3; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }
            button:hover { background-color: #6e6e6e; }
        </style>
    </head>
    <body>
        <div class="container">
            <img src="/image" alt="SYDE Logo"style="max-width: 100%; height: auto;">
            <h1>HTA Loadcell Communication Check</h1>
            <p>1. Click on "Zero" before adding weight</p>
            <p>2. Click "Enter" with 1000g beeing on the loadcell</p>
            <p>(Change in the code if you have a different known Weight)</p>
            <p>3. Click on "Start" to begin measurement.</p>
            <button onclick="Zero()">Zero</button>
            <button onclick="Enter()">Enter</button>
            <button onclick="Start()">Start</button>
            <h2 id="status">Status: Waiting...</h2>
            <h2 id="weight">Weight: Waiting...</h2>
        </div>
        <script>
            function Zero() {
                fetch('/zero')
                .then(response => response.text())
                .then(data => document.getElementById("status").innerText = data);
            }
            function Enter() {
                fetch('/enter')
                .then(response => response.text())
                .then(data => document.getElementById("status").innerText = data);
            }
            function Start() {
                fetch('/start')
                .then(response => response.text())
                .then(data => document.getElementById("status").innerText = data);
                fetchWeightContinuously();
            }
            function fetchWeightContinuously() {
                setInterval(() => {
                    fetch('/weight')
                    .then(response => response.text())
                    .then(data => document.getElementById("weight").innerText = "Weight: " + data + "g");
                }, 100);
            }
        </script>
    </body>
    </html>
    )rawliteral");
}

void setup() {
    Serial.begin(115200);
    delay(6000); // Added delay after Serial.begin
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    ipAddress = WiFi.localIP().toString();
    Serial.println("Connected. IP: " + ipAddress);


    myScale.begin(dataPin, clockPin);

    server.on("/", handleRoot);
    server.on("/image", handleImage);
    server.on("/serial", handleSerialData);
    server.on("/weight", handleWeightData);
    server.on("/zero", handleZero);
    server.on("/enter", handleEnter);
    server.on("/start", handleStart);
    server.begin();
}

void loop() {
    server.handleClient();
    if (Serial.available()) {
        serialData = Serial.readString();
    }
    if (startMeasurement) {
        int weight = start();
        Serial.println("Measured Weight: " + String(weight));
        serialData = "Measured Weight: " + String(weight);
        webWeight = String(weight);
    }
}

