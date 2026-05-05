/**
 * @file CL298N.cpp
 * @brief Implementation of the L298N dual H-bridge motor driver control.
 *
 * This class provides functions to control two DC motors using PWM signals and direction pins
 * via an L298N driver. PWM signals are generated using the ESP32's LEDC (PWM) module.
 *
 * Features:
 *  - Independent control of left and right motors
 *  - Direction handling based on sign of input
 *  - PWM range: 0–255 (8-bit resolution)
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#include "CL298N.h"
#include <Arduino.h>


// Constructor: Set pin mappings for motor driver
CL298N::CL298N() {
  // Assign motor control pins to internal variables
  pinLeftPWM    = 15;  // PWM output for left motor
  pinRightPWM   = 16;  // PWM output for right motor
  pinLeftDir1   = 42;  // Direction pin 1 for left motor
  pinLeftDir2   = 41;  // Direction pin 2 for left motor
  pinRightDir1  = 40;  // Direction pin 1 for right motor
  pinRightDir2  = 39; // Direction pin 2 for right motor

  throttleENA = 0;
  throttleENB = 0;

  ENARelationENB = 0;
  ENBRelationENA = 0;

  PWMLeft = 0;
  PWMRight = 0;
 
  LeftIn1 = LOW;
  LeftIn2= LOW;
  RightIn1= LOW;
  RightIn2= LOW;

  deadband = 0;

}

// Setup: Configure all motor control pins and initialize PWM
void CL298N::setupLeft() {
  // Set direction pins as outputs
  pinMode(pinLeftDir1, OUTPUT);
  pinMode(pinLeftDir2, OUTPUT);

  digitalWrite(pinLeftDir1, LOW);
  digitalWrite(pinLeftDir2, LOW);
}


// Setup: Configure all motor control pins and initialize PWM
void CL298N::setupRight() {
  // Set direction pins as outputs
  
  pinMode(pinRightDir1, OUTPUT);
  pinMode(pinRightDir2, OUTPUT);

  digitalWrite(pinRightDir1, LOW);
  digitalWrite(pinRightDir2, LOW);

}

void CL298N::setMotorControlValuesLeft(int throttleENA) {
  if (throttleENA > 0){
    LeftIn1 = HIGH;
    LeftIn2 = LOW;
  }
  else if (throttleENA < 0){
    LeftIn1 = LOW;
    LeftIn2 = HIGH;
  }
  else {
    LeftIn1 = LOW;              // motor is off
    LeftIn2 = LOW;
  }

  digitalWrite(pinLeftDir1, LeftIn1);              // motor is moving forward
  digitalWrite(pinLeftDir2, LeftIn2);
  analogWrite(pinLeftPWM, abs(throttleENA)); 
}

void CL298N::setMotorControlValuesRight( int throttleENB) {
  
  if (throttleENB > 0){
    RightIn1 = HIGH;
    RightIn2 = LOW;
  }
  else if (throttleENB < 0){
    RightIn1 = LOW;
    RightIn2 = HIGH;
  }
  else {
    RightIn1 = LOW;              // motor is off
    RightIn2 = LOW;
  }

  digitalWrite(pinRightDir1, RightIn1);              // motor is moving forward
  digitalWrite(pinRightDir2, RightIn2);
  analogWrite(pinRightPWM, abs(throttleENB));
  
}

void CL298N::setHTAValues(float throttle){

  if (throttle > deadband){
    pwm = map(throttle,1.25,3,0,255);
    RightIn1 = HIGH;
    RightIn2 = LOW;
    LeftIn1 = HIGH;
    LeftIn2 = LOW;
  }
  else if (throttle < deadband){
    pwm = map(throttle,0,1.25,0,255);
    RightIn1 = LOW;
    RightIn2 = HIGH;
    LeftIn1 = LOW;
    LeftIn2 = HIGH;
  }
  else {
    RightIn1 = LOW;              // motor is off
    RightIn2 = LOW;
    LeftIn1 = LOW;              // motor is off
    LeftIn2 = LOW;
  }

  digitalWrite(pinRightDir1, RightIn1);              // motor is moving forward
  digitalWrite(pinRightDir2, RightIn2);
  digitalWrite(pinLeftDir1, LeftIn1);              // motor is moving forward
  digitalWrite(pinLeftDir2, LeftIn2);

  analogWrite(pinRightPWM, abs(pwm));
  analogWrite(pinLeftPWM, abs(pwm)); 
}
/*

void CL298N::setMotorValueLeft(int throttleENA) {
  if (throttleENA >= 0){
    digitalWrite(pinLeftDir1, HIGH);              // motor is moving forward
    digitalWrite(pinLeftDir2, LOW);
  }

  else if (throttleENA < 0){
    
    digitalWrite(pinLeftDir1, LOW);              // motor is moving backward
    digitalWrite(pinLeftDir2, HIGH);
  }
  else {
    digitalWrite(pinLeftDir1, LOW);              // motor is off
    digitalWrite(pinLeftDir2, LOW);
  }
  //ledcWrite(pinLeftPWM, throttleENA);

  analogWrite(pinLeftPWM, abs(throttleENA));
}

void CL298N::setMotorValueRight(int throttleENB) {
  if (throttleENB >= 0){
    digitalWrite(pinRightDir1, HIGH);              // motor is moving forward
    digitalWrite(pinRightDir2, LOW);
  }

  else if (throttleENB < 0){
    digitalWrite(pinRightDir1, LOW);              // motor is moving backward
    digitalWrite(pinRightDir2, HIGH);
    
  }
  else {
    digitalWrite(pinRightDir1, LOW);              // motor is off
    digitalWrite(pinRightDir2, LOW);
  }
  
  analogWrite(pinRightPWM, abs(throttleENB));
  //ledcWrite(pinRightPWM, throttleENB);

}

/*
int CL298N::setMotorValueLeft(uint8_t AnglePercX, uint8_t AnglePercY) {

  if (AnglePercY > 0 ){
    if (AnglePercX > 0){                            // getting throttle via Joystick Y
      throttleENA = map(AnglePercY,0,100,0,255);       //100% of throttle for Motor A no matter what angle
      digitalWrite(pinLeftDir1, HIGH);              // motor is moving forward
      digitalWrite(pinLeftDir2, LOW);
    }
    else if(AnglePercX < 0 && AnglePercX > -50){
      throttleENA = map(AnglePercY,0,100,0,255);         // getting throttle via Joystick Y
      ENARelationENB = map(AnglePercX,-50,0,0,100);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMLeft = throttleENA * ENARelationENB /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMLeft = int(PWMLeft);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, HIGH);                // motor is moving forward
      digitalWrite(pinLeftDir2, LOW);
    }

    else {
      throttleENA = map(AnglePercY,0,100,0,255);         // getting throttle via Joystick Y
      ENARelationENB = map(AnglePercX,-100,-50,100,0);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMLeft = throttleENA * ENARelationENB /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMLeft = int(PWMLeft);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, LOW);                 // motor is moving backward to circulate on the spot
      digitalWrite(pinLeftDir2, HIGH);      
    }
  }

  if (AnglePercY < 0 ){
    if (AnglePercX > 0){                            // getting throttle via Joystick Y
      throttleENA = map(AnglePercY,-100,0,255,0);      //100% of throttle for Motor A no matter what angle
      digitalWrite(pinLeftDir1, LOW);               // motor is moving backward
      digitalWrite(pinLeftDir2, HIGH);
    }
    else if(AnglePercX < 0 && AnglePercX > -50){
      throttleENA = map(AnglePercY,-100,0,255,0);         // getting throttle via Joystick Y
      ENARelationENB = map(AnglePercX,-50,0,0,100);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMLeft = throttleENA * ENARelationENB /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMLeft = int(PWMLeft);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, LOW);                // motor is moving backward
      digitalWrite(pinLeftDir2, HIGH);
    }

    else {
      throttleENA = map(AnglePercY,-100,0,255,0);         // getting throttle via Joystick Y
      ENARelationENB = map(AnglePercX,-100,-50,100,0);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMLeft = throttleENA * ENARelationENB /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMLeft = int(PWMLeft);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, HIGH);                 // motor is moving forward to circulate on the spot
      digitalWrite(pinLeftDir2, LOW);      
    }
  }
    return PWMLeft;
}

int CL298N::setMotorValueRight(uint8_t AnglePercX, uint8_t AnglePercY) {

  if (AnglePercY > 0 ){
    if (AnglePercX < 0){                            // getting throttle via Joystick Y 
      throttleENB = map(AnglePercY,0,100,0,255);       //100% of throttle for Motor A no matter what angle
      digitalWrite(pinLeftDir1, HIGH);              // motor is moving forward
      digitalWrite(pinLeftDir2, LOW);
    }
    else if(AnglePercX > 0 && AnglePercX < 50){
      throttleENB = map(AnglePercY,0,100,0,255);         // getting throttle via Joystick Y
      ENBRelationENA = map(AnglePercX,0,50,100,0);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMRight = throttleENB * ENBRelationENA /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMRight = int(PWMRight);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, HIGH);                // motor is moving forward
      digitalWrite(pinLeftDir2, LOW);
    }

    else {
      throttleENB = map(AnglePercY,0,100,0,255);         // getting throttle via Joystick Y
      ENBRelationENA = map(AnglePercX,50,100,0,100);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMRight = throttleENB * ENBRelationENA /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMLeft = int(PWMLeft);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, LOW);                 // motor is moving backward to circulate on the spot
      digitalWrite(pinLeftDir2, HIGH);      
    }
  }

  if (AnglePercY < 0 ){
    if (AnglePercX < 0){                            // getting throttle via Joystick Y
      throttleENB = map(AnglePercY,-100,0,255,0);      //100% of throttle for Motor A no matter what angle
      digitalWrite(pinLeftDir1, LOW);               // motor is moving backward
      digitalWrite(pinLeftDir2, HIGH);
    }
    else if(AnglePercX > 0 && AnglePercX < 50){
      throttleENB = map(AnglePercY,-100,0,255,0);         // getting throttle via Joystick Y
      ENBRelationENA = map(AnglePercX,0,50,100,0);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMRight = throttleENB * ENBRelationENA /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMRight = int(PWMRight);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, LOW);                // motor is moving backward
      digitalWrite(pinLeftDir2, HIGH);
    }

    else {
      throttleENB = map(AnglePercY,-100,0,255,0);         // getting throttle via Joystick Y
      ENBRelationENA = map(AnglePercX,50,100,0,100);   // percentage of ENA needed compared to ENB (on 100%) to reach angle
      PWMRight = throttleENB * ENBRelationENA /100;       //PWM for left motor is dependend on throttle as well as the angle 
      PWMRight = int(PWMRight);                         // to make sure the PWM Value is a intager between 0 and 255 
      digitalWrite(pinLeftDir1, HIGH);                 // motor is moving forward to circulate on the spot
      digitalWrite(pinLeftDir2, LOW);      
    }
  }
    return PWMRight;
}
*/
/* Falls analogWrite nicht funktioniert
  // Initialize PWM channels with 1kHz frequency and 8-bit resolution
  ledcSetup(0, 1000, 8);  // Channel 0 for left motor
  ledcSetup(1, 1000, 8);  // Channel 1 for right motor

  // Attach output pins to PWM channels
  ledcAttachPin(pinLeftPWM, 0);
  ledcAttachPin(pinRightPWM, 1);
*/



/*
// Apply motor control values (PWM and direction) to the L298N driver
void CL298N::setMotorValueLeft(uint8_t motorControlLeft) {
  pwmLeft = motorControlLeft;   // PWM value for left motor

  // Set left motor direction
  if (pwmLeft >= 0) {
    digitalWrite(pinLeftDir1, HIGH);
    digitalWrite(pinLeftDir2, LOW);
  } else {
    digitalWrite(pinLeftDir1, LOW);
    digitalWrite(pinLeftDir2, HIGH);
  }


  // Apply absolute PWM values to the motors (range: 0–255)
  analogWrite(pinLeftPWM, abs(pwmLeft)); //Left motor on 
}

void CL298N::setMotorValueRight(uint8_t motorControlRight) {
  pwmRight = motorControlRight;  // PWM value for right motor

  // Set right motor direction
  if (pwmRight >= 0) {
    digitalWrite(pinRightDir1, HIGH);
    digitalWrite(pinRightDir2, LOW);
  } else {
    digitalWrite(pinRightDir1, LOW);
    digitalWrite(pinRightDir2, HIGH);
  }

  // Apply absolute PWM values to the motors (range: 0–255)
  analogWrite(pinRightPWM, abs(pwmRight)); // Right motor on 
}
*/




