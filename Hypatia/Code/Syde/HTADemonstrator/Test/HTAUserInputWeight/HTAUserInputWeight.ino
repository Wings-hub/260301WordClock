//19.03.25  10:05

// Libraries
#include <WiFi.h>         //including library for Wifi
#include <WebServer.h>    //including library for WebServer Design
#include <Adafruit_NeoPixel.h>    //including library for LED
#include "HX711.h"
#include "image.h"  

// Global Variables
//LED
#define LED_PIN 21
#define NUM_LEDS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

//Loadcell
uint8_t dataPin = 6;
uint8_t clockPin = 5;

HX711 myScale;

//Wifi
const char *ssid = "Syde Technology";
const char *password = "nVvffsb#95B!";

WebServer server(80);

String serialData = "Waiting for data...";  // Stores received Serial data
String ipAddress = "Not connected";  // Stores ESP32's IP Address
String webWeight = "Waiting for weight...";
bool startMeasurement = false;
float userCalibrationWeight = 1000.0; // Default calibration weight

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

void handleCalibrationInput() {
    if (server.hasArg("weight")) {
        userCalibrationWeight = server.arg("weight").toFloat();
    }
    server.send(200, "text/plain", "Calibration weight set to " + String(userCalibrationWeight) + " g");
}

//Calibration
void zero() {
    myScale.tare(100);  // Reset to zero properly
    myScale.set_scale();  // Reset scale factor to ensure accurate readings
    Serial.println("Loadcell Tared!");
    //myScale.set_scale();  // Reset scale factor to ensure correct tare
   // myScale.tare(20);  // Properly reset load cell to zero

}

void enter(){
    if (!myScale.is_ready()) {
        Serial.println("HX711 not ready! Check wiring.");
        return;
    }
    float calibrationFactor = myScale.get_units(10) / userCalibrationWeight;
    //myScale.tare();  // Ensure tare after setting the scale factor
    myScale.set_scale(calibrationFactor);
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
            <img src="/image" alt="SYDE Logo" style="max-width: 100%; height: auto;">
            <h1>HTA Loadcell Communication Check</h1>
            <h3>How to use:</h3>
            <p>1. Click on "Zero" to reset the Loadcell to zero before adding weight.</p>
            <p>2. Enter the known weight and click "Set Calibration" for calibration.</p>
            <p>3. Click on "Start" to begin measurement.</p>
            <button onclick="Zero()">Zero</button>
            <button onclick="sendCalibrationWeight()">Set Calibration</button>
            <button onclick="Start()">Start</button>
            <button onclick="Reset()">Reset</button>

            <p>Known Weight: </p>
            <input type="number" id="calibrationInput" placeholder="Enter known weight (g)">
            
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
            function sendCalibrationWeight() {
                let weight = document.getElementById("calibrationInput").value;
                fetch('/calibrate?weight=' + weight)
                .then(response => response.text())
                .then(data => document.getElementById("status").innerText = data);
            }
            function fetchWeightContinuously() {
                setInterval(() => {
                    fetch('/weight')
                    .then(response => response.text())
                    .then(data => document.getElementById("weight").innerText = "Weight: " + data);
                }, 100);
            }
                    function Reset() {
                location.reload();
            }
        </script>
    </body>
    </html>
    )rawliteral");
}

void setup() {
    Serial.begin(115200);
    delay(6000); // Added delay after Serial.begin
    myScale.begin(dataPin, clockPin);
    myScale.set_scale();  // Ensure scale factor is initially reset
    myScale.tare();  // Properly zero the scale on startup
    myScale.set_scale();  // Reset scale factor before use
    myScale.tare();  // Automatically zero the scale on startup
    Serial.begin(115200);
    delay(6000); // Added delay after Serial.begin
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    ipAddress = WiFi.localIP().toString();
    Serial.println("Connected. IP: " + ipAddress);

    strip.begin();
    strip.show();
    myScale.begin(dataPin, clockPin);

    server.on("/", handleRoot);
    server.on("/image", handleImage);
    server.on("/serial", handleSerialData);
    server.on("/weight", handleWeightData);
    server.on("/zero", handleZero);
    server.on("/enter", handleEnter);
    server.on("/start", handleStart);
    server.on("/calibrate", handleCalibrationInput);
    server.begin();
}

void loop() {
    server.handleClient();
    if (Serial.available()) {
        serialData = Serial.readString();
    }
    if (startMeasurement) {
        float weight = start();
        Serial.println("Measured Weight: " + String(weight));
        serialData = "Measured Weight: " + String(weight);
        webWeight = String(weight);
    }
}
