/*
  =============================================================================
  Project:    ESP32 Wi-Fi WebServer  WS2812B (NeoPixel) Single-LED Controller
  Board:      ESP32
  Summary:    Hosts a simple web page to toggle a single NeoPixel LED color
              via HTTP endpoints:
                - GET /H   LED RED
                - GET /L   LED GREEN
                - GET /K   LED BLUE
                - GET /    Main page (buttons + color preview)
                - GET /image  Serves SYDE logo from image.h

  How to use:
    1) Flash to your ESP32.
    2) Update Wi-Fi SSID and password below.
    3) Open Serial Monitor @115200 (baudrate) to read the assigned IP address.
    4) Visit http://<ESP32-IP>/ in your browser and use the buttons.

  Hardware:
    - Hypatia Board (WS2812B LED Pin is 21)

  Dependencies (Arduino Library Manager):
    - WiFi (ESP32 core)
    - WebServer (ESP32 core)
    - Adafruit NeoPixel by Adafruit

  Notes:
    - Credentials are hardcoded for demo purposes only.
    - The logo image bytes are expected in image.h as `ImageData`.
  =============================================================================
*/

#include <WiFi.h>               // Wi-Fi connectivity (ESP32)
#include <WebServer.h>          // Minimal HTTP server
#include <Adafruit_NeoPixel.h>  // WS2812B (NeoPixel) control

#include "image.h"              // SYDE logo image data (uint8_t array)

// =============================
// NeoPixel configuration
// =============================
#define LED_PIN   21   // GPIO used for the NeoPixel data line
#define NUM_LEDS  1    // Single LED in this example

// Global NeoPixel instance
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// =============================
// Wi-Fi credentials (edit these)
// =============================
const char *ssid = "*****";
const char *password = "*****";  // Demo placeholder; replace with your password

// =============================
// Web server on TCP port 80
// =============================
WebServer server(80);

// -----------------------------------------------------------------------------
// LED color handlers (mapped to HTTP routes)
// -----------------------------------------------------------------------------
void handleRed() {
  strip.setPixelColor(0, strip.Color(255, 0, 0));  // RED
  strip.show();
}

void handleGreen() {
  strip.setPixelColor(0, strip.Color(0, 255, 0));  // GREEN
  strip.show();
}

void handleBlue() {
  strip.setPixelColor(0, strip.Color(0, 0, 255));  // BLUE
  strip.show();
}

// -----------------------------------------------------------------------------
// Root page handler: serves a minimal UI with three buttons and a color preview
// -----------------------------------------------------------------------------
void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SYDE Hypatia Wi-Fi Connection Check</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        background-color: #f4f4f9;
        color: #333;
        text-align: center;
        margin: 0;
        padding: 0;
      }
      .container {
        max-width: 600px;
        margin: 20px auto;
        padding: 20px;
        background-color: white;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
        border-radius: 10px;
      }
      h1 {
        color: #6e6e6e;
        margin-top: 10px;
      }
      button {
        background-color: #02aff3;
        color: white;
        padding: 10px 20px;
        margin: 10px;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        font-size: 16px;
        display: inline-block;
      }
      button:hover {
        background-color: #6e6e6e;
      }
      img {
        margin-top: 10px;
        margin-bottom: 20px;
        max-width: 80%;
        border-radius: 10px;
      }
      #color-circle {
        width: 100px;
        height: 100px;
        border-radius: 50%;
        background-color: lightgray;
        margin: 20px auto;
        box-shadow: 0 0 15px 5px rgba(0, 0, 0, 0.2);
      }
    </style>
  </head>
  <body>
    <div class="container">
      <img src="/image" alt="SYDE Logo">
      <h1>SYDE Hypatia Wi-Fi connection check</h1>
      <p>Control the LED on the Hypatia Board:</p>

      <div id="color-circle"></div>

      <div style="display: flex; justify-content: center; gap: 10px; margin-top: 20px;">
        <button onclick="fetch('/H'); changeCircleColor('red');">Turn LED RED</button>
        <button onclick="fetch('/L'); changeCircleColor('green');">Turn LED GREEN</button>
        <button onclick="fetch('/K'); changeCircleColor('blue');">Turn LED BLUE</button>
      </div>

      <script>
        function changeCircleColor(color) {
          const el = document.getElementById('color-circle');
          el.style.backgroundColor = color;
          el.style.boxShadow = '0 0 15px 5px ' + color;
        }
      </script>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// -----------------------------------------------------------------------------
// Image handler: serves the SYDE logo as PNG from PROGMEM (image.h)
// -----------------------------------------------------------------------------
void handleImage() {
  server.send_P(200, "image/png", (const char*)ImageData, sizeof(ImageData));
}

// -----------------------------------------------------------------------------
// Arduino Setup: initialize Serial, NeoPixel, connect to Wi-Fi, and start server
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(5000);           // Give Serial time to attach (optional)

  // --- NeoPixel init ---
  strip.begin();
  strip.show();          // Ensure LED starts OFF

  // --- Wi-Fi connect ---
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // --- Connection feedback ---
  Serial.println("\nWi-Fi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- HTTP route mapping ---
  server.on("/",      handleRoot);   // Main page
  server.on("/image", handleImage);  // SYDE logo
  server.on("/H",     handleRed);    // LED  RED
  server.on("/L",     handleGreen);  // LED  GREEN
  server.on("/K",     handleBlue);   // LED  BLUE

  // --- Start web server ---
  server.begin();
  Serial.println("WebServer started!");
}

// -----------------------------------------------------------------------------
// Arduino Loop: process incoming HTTP requests
// -----------------------------------------------------------------------------
void loop() {
  server.handleClient();
}
