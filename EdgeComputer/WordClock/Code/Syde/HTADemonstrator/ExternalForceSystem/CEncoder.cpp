/**
 * @file CEncoder.cpp
 * @brief Implementation of a basic quadrature encoder handler for ESP32.
 *
 * This class reads a rotary quadrature encoder using two digital input pins.
 * It tracks the position based on direction changes and estimates velocity
 * using a fixed time interval.
 *
 * Features:
 *  - Reads encoder steps (increment/decrement)
 *  - Calculates position
 *  - Estimates velocity in ticks/second (based on delta position and time)
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#include "CEncoder.h"
#define MILLISTEPS 100                   
//Magic numbers suchen  im Code ----------------------------------------------------------------------------------------------

// Constructor: Accepts two pins for quadrature encoder signal A and B
CEncoder::CEncoder(uint8_t pinEncoderA, uint8_t pinEncoderB) {

  encoder0PinA = pinEncoderA;           // Channel A (step signal)
  encoder0PinB = pinEncoderB;           // Channel B (direction signal)
  encoder0PinALast = LOW;               // Store previous state for edge detection

  // Initialize position tracking and velocity computation variables
  valueNow = 0;
  testValue = 0;
  encoder0Pos = 0;
  lastEncoderPos = 0;

  currentTime = 0;
  lastTime = 0;

  deltaPos = 0;
  deltaTime = 0;
  velocity = 0.0f;

}

// Setup function: Configure encoder pins as inputs and initialize state
void CEncoder::setup() {
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);

  encoder0PinALast = digitalRead(encoder0PinA); // Initialize last state
}

// Reads encoder position using rising edge detection on Channel A
int CEncoder::getPosition() {
  valueNow = digitalRead(encoder0PinA);

  // Detect rising edge on channel A
  if ((encoder0PinALast == LOW) && (valueNow == HIGH)) {
    // Determine direction using channel B
    if (digitalRead(encoder0PinB) == LOW) {
      encoder0Pos--; // Counter-clockwise
    } else {
      encoder0Pos++; // Clockwise
    }
  }

  encoder0PinALast = valueNow; // Update last state
  return encoder0Pos;          // Return absolute position
}

// Calculates velocity based on position change over time
float CEncoder::getVelocity() {
  getPosition();                            // Update position
  currentTime = millis();                   // Get current time

  // Compute velocity every 100 milliseconds
  if (currentTime - lastTime >= MILLISTEPS) {
    deltaPos = encoder0Pos - lastEncoderPos;         // Change in steps
    deltaTime = (currentTime - lastTime) / 1000.0f;   // Convert ms to seconds

    encoderVelocity = deltaPos / deltaTime;          // Ticks per second

    // Store current values for next iteration
    lastEncoderPos = encoder0Pos;
    lastTime = currentTime;
  }

  return encoderVelocity;                  // Return most recent velocity
}

int CEncoder::getTestValue() {
  testValue = digitalRead(encoder0PinA);

  // Detect rising edge on channel A
  if ((encoder0PinALast == LOW) && (testValue == HIGH)) {
    // Determine direction using channel B
    if (digitalRead(encoder0PinB) == LOW) {
      encoder0Pos--; // Counter-clockwise
    } else {
      encoder0Pos++; // Clockwise
    }
  }

  encoder0PinALast = testValue; // Update last state
  Serial.print("encoder0Pos");
  return encoder0Pos;          // Return absolute position
}

// mm/s basierend auf 67 mm Raddurchmesser
float CEncoder::getLinearVelocity() {
  float ticksPerSec = getVelocity();
  const float wheelCircumference = 3.1416f * 0.067f; // in m
  const float ticksPerRevolution = 20.0f;          // 400 PPR * X4

  float v_mps = (ticksPerSec / ticksPerRevolution) * wheelCircumference;
  float v_mmps = v_mps * 1000.0f;                    // Umrechnung in mm/s
  return v_mmps;
}


