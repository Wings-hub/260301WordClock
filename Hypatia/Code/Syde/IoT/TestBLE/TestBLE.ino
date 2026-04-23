/*
  ============================================================================
  Project:    ESP32 BLE ⇄ WS2812B (NeoPixel) Single-LED Controller
  Board:      ESP32
  Summary:    Exposes a BLE characteristic you can write "on" or "off" to.
              - "on"  → LED turns BLUE
              - "off" → LED turns OFF
              Includes a helper to set the LED to GREEN (not called by default).

  How to test:
    1) Flash the sketch to your ESP32.
    2) Use a BLE app (e.g., nRF Connect / LightBlue).
    3) Connect to device name "MyESP32".
    4) Find the Service UUID and Characteristic UUID below.
    5) Write ASCII "on" or "off" to the characteristic and observe the LED.

  Hardware:
    - One WS2812B (NeoPixel) LED on GPIO 21 (5V, GND, and data on LED_PIN).
    - Level shifting recommended for long runs / multiple LEDs.

  Dependencies (Arduino Library Manager):
    - Adafruit NeoPixel by Adafruit
    - ESP32 BLE Arduino (or NimBLE-Arduino; this sketch uses the classic API)

  UUIDs (customize if needed):
    - SERVICE_UUID:        b0cee666-6f5c-48fa-9b6b-ee42fec51a13
    - CHARACTERISTIC_UUID: 4b15cd64-97b9-4ff2-9599-88ccd01f07f1
  ============================================================================
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <Adafruit_NeoPixel.h>  // WS2812B (NeoPixel) control

// -----------------------------
// NeoPixel configuration
// -----------------------------
#define LED_PIN   21   // GPIO used for NeoPixel data line
#define NUM_LEDS  1    // Single LED for this example

// -----------------------------
// BLE UUIDs (customizable)
// -----------------------------
#define SERVICE_UUID        "b0cee666-6f5c-48fa-9b6b-ee42fec51a13"
#define CHARACTERISTIC_UUID "4b15cd64-97b9-4ff2-9599-88ccd01f07f1"

// Global NeoPixel instance
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// -----------------------------------------------------------------------------
// Helper: Set LED to GREEN (demonstrates a reusable LED action; not called here)
// -----------------------------------------------------------------------------
void handleGreen() {
  strip.setPixelColor(0, strip.Color(0, 255, 0));  // Green
  strip.show();
}

// -----------------------------------------------------------------------------
// BLE Characteristic Write Callback
// Receives ASCII strings. Recognized commands:
//   "on"  → set LED BLUE
//   "off" → turn LED OFF
// -----------------------------------------------------------------------------
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    // getValue() returns std::string in the ESP32 BLE library
    std::string value = pCharacteristic->getValue();

    if (!value.empty()) {
      Serial.println("*********");
      Serial.print("New value: ");

      // Print raw characters for clarity
      for (size_t i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }
      Serial.println();

      // Compare the incoming text (prefix match is sufficient for this demo)
      if (strncmp(value.c_str(), "on", 2) == 0) {
        // LED → BLUE
        strip.setPixelColor(0, strip.Color(0, 0, 255));
        strip.show();
        Serial.println("Action: LED set to BLUE (\"on\")");
      } else if (strncmp(value.c_str(), "off", 3) == 0) {
        // LED → OFF
        strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.show();
        Serial.println("Action: LED turned OFF (\"off\")");
      } else {
        Serial.println("Action: Unrecognized command (use \"on\" or \"off\")");
      }

      Serial.println("*********");
    }
  }
};

// -----------------------------------------------------------------------------
// Arduino Setup: Initialize Serial, NeoPixel, BLE server/service/characteristic,
//                set callbacks, start service and advertising.
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(2000);  // Allow time for Serial to attach

  // --- NeoPixel init ---
  strip.begin();
  strip.show();           // Ensure LED starts OFF
  // strip.setBrightness(50); // Optional: limit brightness (0–255)

  Serial.println("Starting BLE initialization...");

  // --- BLE device init & name ---
  BLEDevice::init("MyESP32");
  Serial.println("BLE initialized!");

  // --- BLE server ---
  BLEServer *pServer = BLEDevice::createServer();
  Serial.println("BLE server created!");

  // --- BLE service ---
  BLEService *pService = pServer->createService(SERVICE_UUID);
  Serial.println("BLE service created!");

  // --- BLE characteristic (READ/WRITE) ---
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  Serial.println("BLE characteristic created!");

  // --- Hook write callback for command handling ---
  pCharacteristic->setCallbacks(new MyCallbacks());
  Serial.println("Callback for characteristic set!");

  // --- Initial characteristic value (visible to clients) ---
  pCharacteristic->setValue("Hello World");

  // --- Start service & begin advertising ---
  pService->start();
  Serial.println("BLE service started!");

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);  // Advertise our service
  pAdvertising->start();
  Serial.println("BLE advertising started! Ready for connections.");
}

// -----------------------------------------------------------------------------
// Arduino Loop: Nothing to do here; BLE stack handles connections & writes.
// A small delay keeps the loop calm.
// -----------------------------------------------------------------------------
void loop() {
  delay(2000);
}
