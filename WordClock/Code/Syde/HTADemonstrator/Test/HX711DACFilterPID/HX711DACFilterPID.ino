//20.03.25  09:42
// Libraries
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MCP4725.h>
#include <Wire.h>
#include <deque>
#include "HX711.h"
#include "image.h"  
#include "webpage.h"

// Global Variables

//Loadcell
uint8_t clockPin1 = 5;
uint8_t dataPin1 = 4;
uint8_t clockPin2 = 6;
uint8_t dataPin2 = 7;

HX711 myScale1;
HX711 myScale2;

//DAC
Adafruit_MCP4725 MCP4725_1;
Adafruit_MCP4725 MCP4725_2;

int dacAnalogRead1 = 0;
int dacAnalogRead2 = 0;

int SDA_Pin = 41;
int SCL_Pin = 42;

//Filter
int p = 10;
std::deque<float> buffer1;
std::deque<float> buffer2;
float sum1 = 0.0f, sum2 = 0.0f;

//PID
float Kp1 = 1.0f, Kd1 = 0.1f;
float Kp2 = 1.0f, Kd2 = 0.1f;
float lastFl1 = 0.0f;
float lastFl2 = 0.0f;
float Fl1 = 0.0f, Fd1 = 0.0f, Vd1 = 0.0f;
float Fl2 = 0.0f, Fd2 = 0.0f, Vd2 = 0.0f;

//Wifi
const char *ssid = "Syde Technology";
const char *password = "nVvffsb#95B!";

IPAddress local_IP(192, 168, 10, 184);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 10, 200);
IPAddress secondaryDNS(8, 8, 8, 8);

WebServer server(80);

String serialData1 = "Waiting for data...";
String serialData2 = "Waiting for data...";
String ipAddress = "Not connected";
String webWeight1 = "Waiting for weight...";
String webWeight2 = "Waiting for weight...";
bool startMeasurement1 = false;
bool startMeasurement2 = false;
bool graphRunning = true;

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
    setLEDColor(0, 255, 0);
    server.send(200, "text/plain", "All measurements reset");
}

void handleImage() {
    server.send_P(200, "image/png", (const char*)ImageData, sizeof(ImageData));
}

void handleSerialData1() { server.send(200, "text/plain", serialData1); }
void handleSerialData2() { server.send(200, "text/plain", serialData2); }
void handleWeightData1() { server.send(200, "text/plain", webWeight1); }
void handleWeightData2() { server.send(200, "text/plain", webWeight2); }

void handleZero1() { zero1(); server.send(200, "text/plain", "SetZero1"); }
void handleZero2() { zero2(); server.send(200, "text/plain", "SetZero2"); }
void handleEnter1() { enter1(); server.send(200, "text/plain", "SetEnter1"); }
void handleEnter2() { enter2(); server.send(200, "text/plain", "SetEnter2"); }
void handleStart1() { startMeasurement1 = true; server.send(200, "text/plain", "Measuring Weight One..."); setLEDColor(0, 0, 255); }
void handleStart2() { startMeasurement2 = true; server.send(200, "text/plain", "Measuring Weight Two..."); setLEDColor(0, 0, 255); }
void handleStop1() { startMeasurement1 = false; server.send(200, "text/plain", "Stopped 1"); }
void handleStop2() { startMeasurement2 = false; server.send(200, "text/plain", "Stopped 2"); }

// Filter
float updateMovingAverage(std::deque<float>& buffer, float& sum, float newVal) {
    buffer.push_back(newVal);
    sum += newVal;
    if (buffer.size() > p) {
        sum -= buffer.front();
        buffer.pop_front();
    }
    return sum / buffer.size();
}

float start1() {
    float raw = myScale1.get_units(1);
    return updateMovingAverage(buffer1, sum1, raw);
}

float start2() {
    float raw = myScale2.get_units(1);
    return updateMovingAverage(buffer2, sum2, raw);
}

void handleSetP() {
    if (server.hasArg("value")) {
        int newP = server.arg("value").toInt();
        if (newP > 0 && newP <= 100) {
            p = newP;
            buffer1.clear(); buffer2.clear();
            sum1 = sum2 = 0.0f;
            server.send(200, "text/plain", "Set filter depth to: " + String(p));
        } else {
            server.send(400, "text/plain", "Value must be between 1 and 100");
        }
    } else {
        server.send(400, "text/plain", "Missing value");
    }
}

void handleGetP() { server.send(200, "text/plain", String(p)); }
void handleSetKp1() { if (server.hasArg("value")) Kp1 = server.arg("value").toFloat(); server.send(200, "text/plain", "Kp1 set to " + String(Kp1)); }
void handleSetKd1() { if (server.hasArg("value")) Kd1 = server.arg("value").toFloat(); server.send(200, "text/plain", "Kd1 set to " + String(Kd1)); }
void handleSetKp2() { if (server.hasArg("value")) Kp2 = server.arg("value").toFloat(); server.send(200, "text/plain", "Kp2 set to " + String(Kp2)); }
void handleSetKd2() { if (server.hasArg("value")) Kd2 = server.arg("value").toFloat(); server.send(200, "text/plain", "Kd2 set to " + String(Kd2)); }

void handleStatus() {
    if (!graphRunning) {
        server.send(200, "application/json", "{}");
        return;
    }
    dacAnalogRead1 = analogRead(8);
    dacAnalogRead2 = analogRead(3);
    String json = "{";
    json += "\"raw1\":" + String(Fl1 + Fd1 - Fd1) + ",";
    json += "\"raw2\":" + String(Fl2 + Fd2 - Fd2) + ",";
    json += "\"filtered1\":" + String(Fl1) + ",";
    json += "\"filtered2\":" + String(Fl2) + ",";
    json += "\"dac1\":" + String(dacAnalogRead1) + ",";
    json += "\"dac2\":" + String(dacAnalogRead2);
    json += "}";
    server.send(200, "application/json", json);
}

void handleGraphStart() { graphRunning = true; server.send(200, "text/plain", "Graph started"); }
void handleGraphStop() { graphRunning = false; server.send(200, "text/plain", "Graph stopped"); }

//Calibration
void zero1() { myScale1.tare(20); }
void zero2() { myScale2.tare(20); }
void enter1() { float calibrationFactor1 = myScale1.get_units(10)/1000; myScale1.set_scale(calibrationFactor1); }
void enter2() { float calibrationFactor2 = myScale2.get_units(10)/1000; myScale2.set_scale(calibrationFactor2); }

//Webserver
void handleRoot() { server.send_P(200, "text/html", MAIN_page); }


void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_Pin, SCL_Pin);
    MCP4725_1.begin(0x60, &Wire);
    // MCP4725_2.begin(0x61, &Wire); // falls aktiviert

    strip.begin();
    strip.show();
    setLEDColor(255, 165, 0); // orange = connecting

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA Failed to configure");
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    setLEDColor(0, 255, 0); // green = ready

    ipAddress = WiFi.localIP().toString();
    Serial.println("Connected. IP: " + ipAddress);

    myScale1.begin(dataPin1, clockPin1);
    myScale2.begin(dataPin2, clockPin2);

    server.on("/", handleRoot);
    server.on("/image", handleImage);
    server.on("/resetall", handleResetAll);
    server.on("/status", handleStatus);
    server.on("/serial1", handleSerialData1);
    server.on("/serial2", handleSerialData2);
    server.on("/weight1", handleWeightData1);
    server.on("/weight2", handleWeightData2);
    server.on("/setp", handleSetP);
    server.on("/getp", handleGetP);
    server.on("/setkp1", handleSetKp1);
    server.on("/setkd1", handleSetKd1);
    server.on("/setkp2", handleSetKp2);
    server.on("/setkd2", handleSetKd2);
    server.on("/zero1", handleZero1);
    server.on("/enter1", handleEnter1);
    server.on("/start1", handleStart1);
    server.on("/stop1", handleStop1);
    server.on("/zero2", handleZero2);
    server.on("/enter2", handleEnter2);
    server.on("/start2", handleStart2);
    server.on("/stop2", handleStop2);
    server.on("/graphstart", handleGraphStart);
    server.on("/graphstop", handleGraphStop);

    server.begin();
}

void loop() {
    server.handleClient();

    if (startMeasurement1) {
        Fl1 = start1();
        Fd1 = Fl1 - lastFl1;
        Vd1 = Fl1 * Kp1 + Fd1 * Kd1;
        lastFl1 = Fl1;

        serialData1 = "F: " + String(Fl1) + " | dF: " + String(Fd1) + " | Vd: " + String(Vd1);
        webWeight1 = String(Fl1);

        uint16_t dacOutput1 = map(constrain(Vd1, 0, 10000), 0, 10000, 0, 4095);
        MCP4725_1.setVoltage(dacOutput1, false);
    }

    if (startMeasurement2) {
        Fl2 = start2();
        Fd2 = Fl2 - lastFl2;
        Vd2 = Fl2 * Kp2 + Fd2 * Kd2;
        lastFl2 = Fl2;

        serialData2 = "F: " + String(Fl2) + " | dF: " + String(Fd2) + " | Vd: " + String(Vd2);
        webWeight2 = String(Fl2);

        // DAC2 optional aktivieren
        // uint16_t dacOutput2 = map(constrain(Vd2, 0, 10000), 0, 10000, 0, 4095);
        // MCP4725_2.setVoltage(dacOutput2, false);
    }

    if (startMeasurement1 && startMeasurement2) {
        setLEDColor(128, 0, 128); // violett bei gleichzeitiger Messung
    }
}
