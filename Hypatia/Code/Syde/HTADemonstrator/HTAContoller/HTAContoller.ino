/**
 * @file HTAController.ino
 * @brief BLE Joystick Controller for Remote Motor System (Client)
 *
 * This program runs on an ESP32 and acts as a BLE client.
 * It reads user input from two analog joysticks:
 *  - One joystick provides direction (X and Y axes)
 *  - The other provides throttle (speed control)
 *
 * The system calculates left/right motor PWM values based on joystick input,
 * then sends them via BLE to a server device.
 * It also receives encoder velocity feedback from the server via BLE notifications.
 *
 * Dependencies:
 *  - CBLEClientModule.h/cpp
 *  - CJoystick.h/cpp
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#include "CESPNowControler.h"
#include "CJoystick.h"
#include "CDisplay.h"

#define BUTTON_PIN 8

// Define pins for the joysticks
uint8_t pinJoystickX = 17; // X-axis: Left/Right
uint8_t pinJoystickY = 18; // Y-axis: Forward/Backward


// Create joystick instances:
CJoystick JoystickLeft;
CJoystick JoystickRight;

//variables to work with Joystick data
int rawX = 0;
int rawY = 0;
int throttleENA = 0;
int throttleENB = 0;

//variables to get Encoder Values
float encoderLeft = 0;
float encoderRight = 0;

// Stores the most recently received encoder velocity from the server
CESPNowControler ESPNowControler;

CDisplay HTAdisplay;
bool connection = false; //check if ESP NOW Connection works

void setup() {
  Serial.begin(115200);
  delay(6000);
  ESPNowControler.setup();
  HTAdisplay.setup(&ESPNowControler);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // internen Pullup aktivieren
}

void loop() {
  testEspNowLoop();
  //delay(600);
 
}


void testEspNowLoop(){

  if (digitalRead(BUTTON_PIN) == LOW) { // Taster gedrückt (gegen GND)
    ESPNowControler.reconnect();
  }

  rawX = JoystickLeft.getRawX(pinJoystickX);
  rawY = JoystickRight.getRawY(pinJoystickY);
  throttleENA = JoystickLeft.getLeftValue(rawX);
  throttleENB = JoystickRight.getRightValue(rawY);

  encoderLeft = ESPNowControler.getEncoderLeft();
  encoderRight = ESPNowControler.getEncoderRight();

  ESPNowControler.sendJoystickData(throttleENA, throttleENB);

  connection = ESPNowControler.connectionCheck();
  HTAdisplay.showStatusScreen(encoderLeft,encoderRight,connection);      // Hauptanzeige mit Status + Werten
  
  Serial.print("encoderLeft:");
  Serial.print(encoderLeft);
  Serial.print("encoderRight:");
  Serial.println(encoderRight);
   
}

void funktion() {
  Serial.println("Taste gedrückt -> Funktion aufgerufen!");
  // hier dein Code
}

