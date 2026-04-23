#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLE2902.h>

// UUIDs must match the server code
#define SERVICE_UUID         "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define CHAR_LEFT_UUID       "cba1d466-344c-4be3-ab3f-189f80dd7518"
#define CHAR_RIGHT_UUID      "f78ebbff-c8b7-4107-93de-889a6a06d408"

BLEClient* pClient;
BLERemoteCharacteristic* pCharLeft = nullptr;
BLERemoteCharacteristic* pCharRight = nullptr;

// Static test values as placeholder for throttle ENA/ENB
float throttleENA = 100.0;
float throttleENB = 80.0;

bool newLeft = false;
bool newRight = false;
std::string leftValue;
std::string rightValue;

static void notifyCallbackLeft(
BLERemoteCharacteristic* pBLERemoteCharacteristic,
uint8_t* pData,
size_t length,
bool isNotify) {
leftValue = std::string((char*)pData, length);
newLeft = true;
}

static void notifyCallbackRight(
BLERemoteCharacteristic* pBLERemoteCharacteristic,
uint8_t* pData,
size_t length,
bool isNotify) {
rightValue = std::string((char*)pData, length);
newRight = true;
}

void connectToServer(BLEAdvertisedDevice advertisedDevice) {
pClient = BLEDevice::createClient();
pClient->connect(&advertisedDevice);

BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
if (pRemoteService == nullptr) {
Serial.println("Failed to find service UUID");
return;
}

pCharLeft = pRemoteService->getCharacteristic(CHAR_LEFT_UUID);
pCharRight = pRemoteService->getCharacteristic(CHAR_RIGHT_UUID);

if (pCharLeft && pCharLeft->canNotify()) {
pCharLeft->registerForNotify(notifyCallbackLeft);
}

if (pCharRight && pCharRight->canNotify()) {
pCharRight->registerForNotify(notifyCallbackRight);
}

Serial.println("Connected and notification registered.");
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
void onResult(BLEAdvertisedDevice advertisedDevice) {
if (advertisedDevice.getName() == "ESP32_Encoder_Server") {
Serial.println("Found target device. Connecting...");
BLEDevice::getScan()->stop();
connectToServer(advertisedDevice);
}
}
};

void setup() {
Serial.begin(115200);
BLEDevice::init("ESP32_Client");

BLEScan* pBLEScan = BLEDevice::getScan();
pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
pBLEScan->setInterval(100);
pBLEScan->setWindow(99);
pBLEScan->setActiveScan(true);
pBLEScan->start(5, false);  // Scan for 5 seconds
}

void loop() {
if (newLeft || newRight) {
if (newLeft) {
Serial.print("Velocity Left: ");
Serial.println(leftValue.c_str());
newLeft = false;
}
if (newRight) {
Serial.print("Velocity Right: ");
Serial.println(rightValue.c_str());
newRight = false;
}

// Example: send PWM values as strings
if (pCharLeft && pCharLeft->canWrite()) {
char buffer[8];
dtostrf(throttleENA, 6, 2, buffer);
pCharLeft->writeValue((uint8_t*)buffer, strlen(buffer));
}

if (pCharRight && pCharRight->canWrite()) {
char buffer[8];
dtostrf(throttleENB, 6, 2, buffer);
pCharRight->writeValue((uint8_t*)buffer, strlen(buffer));
}
}

delay(100);  // optional: reduce unnecessary CPU load
}