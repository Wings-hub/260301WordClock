/**
 * @file CJoystick.cpp
 * @brief Implementation of the CJoystick class for analog joystick handling.
 *
 * This class provides functionality to read directional and throttle inputs
 * from analog joysticks. It computes motor control signals based on these
 * inputs using differential steering logic. The output consists of two
 * PWM values for left and right motors.
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#include "CJoystick.h"

// Constructor: Initializes the joystick pins and internal variables
CJoystick::CJoystick() {


  // Initialize raw input variables
  rawThrottle = 0;
  rawX = 0;
  rawY = 0;

  valueLeft = 0;
  valueRight = 0;

  // Initialize processed values
  forwardBackward = 0;
  leftRight = 0;
  throttle = 0;

  // Initialize motor output values
  leftMotorPower = 0;
  rightMotorPower = 0;
  pwmLeft = 0;
  pwmRight = 0;

}

int CJoystick::getRawX(uint8_t pinJoystickX) {
  rawX = analogRead(pinJoystickX);        // Read throttle potentiometer
  return rawX;
}

int CJoystick::getRawY(uint8_t pinJoystickY) {
  rawY = analogRead(pinJoystickY);        // Read throttle potentiometer
  return rawY;
}

int CJoystick::getLeftValue(int rawX) {
  
  if (rawX <= 1000){
    valueLeft = map(rawX, 0, 1000, -255, 0);
  }
  else if (rawX > 1100){
    valueLeft = map(rawX, 1150, 4095, 0, 255);
  }
  else valueLeft = 0;

  //valueLeft = map(rawX, 0, 4095, -255, 255);
  return valueLeft;                          
}

int CJoystick::getRightValue(int rawY) {
  
  if (rawY <= 1000){
    valueRight = map(rawY, 0, 1000, -255, 0);
  }
  else if (rawY > 1100){
    valueRight = map(rawY, 1150, 4095, 0, 255);
  }
  else valueRight = 0;
    
  //valueRight = map(rawY, 0, 4095, -255, 255);
  
  return valueRight;                          
}

/*
int8_t CJoystick::getAnglePercentageX(int rawX) {
  
  if (rawX < 1000){
    AnglePercX = map(rawX, 0, 1000, -100, 0);
  }
  else if (rawX > 1100){
    AnglePercX = map(rawX, 1150, 4095, 0, 100);
  }
  else AnglePercX = 0;
  
  return AnglePercX;                          
}

int8_t CJoystick::getAnglePercentageY(int rawY) {
  
  if (rawY < 1000){
    AnglePercY = map(rawY, 0, 1000, -100, 0);
  }
  else if (rawY > 1100){
    AnglePercY = map(rawY, 1150, 4095, 0, 100);
  }
  else AnglePercY = 0;
  
  return AnglePercY;                          
}
*/

/*
// Reads the analog throttle input and maps it to a 0–100 scale
uint8_t CJoystick::getVelocityControl(uint8_t pinThrottle) {
  rawThrottle = analogRead(pinThrottle);        // Read throttle potentiometer
  throttle = map(rawThrottle, 0, 4095, 0, 100); // Normalize to percentage
  return throttle;
}

float CJoystick::getRawThrottle(uint8_t pinThrottle) {
  rawThrottle = analogRead(pinThrottle);        // Read throttle potentiometer
  throttle = rawThrottle * 3.3 / 4095.0;
  return throttle;
}

// Reads directional joystick values and maps them to [-100, 100]
int CJoystick::getControlLeftRight(uint8_t pinJoystickX) {
  rawX = analogRead(pinJoystickX);              // Read horizontal movement

  leftRight = map(rawX, 0, 4095, -100, 100);     // Left/Right steering

  return leftRight;                              // Return pointer to direction array
}



// Reads directional joystick values and maps them to [-100, 100]
int CJoystick::getControlForwardBackward(uint8_t pinJoystickY) {
  rawY = analogRead(pinJoystickY);              // Read vertical movement

  forwardBackward = map(rawY, 0, 4095, -100, 100); // Forward/Backward motion

  return forwardBackward;                              // Return pointer to direction array
}


*/

/*
// Computes motor control PWM values from joystick input
uint8_t CJoystick::getMotorControlValueLeft() {
  // Apply differential steering: throttle * (FB ± LR)
  leftMotorPower  = (forwardBackward + leftRight) * throttle / 100;
 
  // Constrain values to avoid PWM overflow
  leftMotorPower  = constrain(leftMotorPower, -100, 100);

  // Apply a deadzone to avoid motor twitching near zero
  if (abs(leftMotorPower) < 5)  leftMotorPower = 0;

  // Convert to absolute PWM values in range [0, 255]
  pwmLeft  = abs(leftMotorPower) * 255 / 100;

  return pwmLeft;                          // Return pointer to PWM array
}

// Computes motor control PWM values from joystick input
uint8_t CJoystick::getMotorControlValueRight() {
  // Apply differential steering: throttle * (FB ± LR)
  rightMotorPower = (forwardBackward - leftRight) * throttle / 100;

  // Constrain values to avoid PWM overflow
  rightMotorPower = constrain(rightMotorPower, -100, 100);

  // Apply a deadzone to avoid motor twitching near zero
  if (abs(rightMotorPower) < 5) rightMotorPower = 0;

  // Convert to absolute PWM values in range [0, 255]
  pwmRight = abs(rightMotorPower) * 255 / 100;

  return pwmRight;                          // Return pointer to PWM array
}

*/

