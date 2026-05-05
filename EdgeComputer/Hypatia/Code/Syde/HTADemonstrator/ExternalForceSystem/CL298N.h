/**
 * @file CL298N.h
 * @brief Header file for L298N motor driver control class.
 *
 * The CL298N class provides basic control over two DC motors using the L298N
 * dual H-bridge driver. It supports direction control and PWM speed regulation
 * via ESP32's LEDC module.
 *
 * Usage:
 *  - Call `setup()` once in `setup()` to initialize PWM and direction pins.
 *  - Call `setMotorValues()` continuously to update motor speed and direction.
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#ifndef CL298N_H
#define CL298N_H

#include <Arduino.h>

class CL298N {
  public:
    /**
     * @brief Constructor for motor driver class.
     * Initializes pin assignments internally.
     */
    CL298N();

    /**
     * @brief Configure all direction pins and initialize PWM channels.
     */
    void setupLeft();
    void setupRight();

    /**
     * @brief Apply speed and direction to both motors.
     * @param motorControl Array of two integers: [pwmLeft, pwmRight]
     *        - Values should be in the range -255 to 255.
     *        - Sign indicates direction (positive = forward).
     */
    void setMotorControlValuesLeft( int throttleENA);
    void setMotorControlValuesRight( int throttleENB);

  private:
    // PWM output pins for both motors
    uint8_t pinLeftPWM;
    uint8_t pinRightPWM;

    // Direction control pins for left motor
    uint8_t pinLeftDir1;
    uint8_t pinLeftDir2;

    // Direction control pins for right motor
    uint8_t pinRightDir1;
    uint8_t pinRightDir2;
  
    int8_t throttleENA;
    int8_t throttleENB;

    int8_t ENARelationENB;
    int8_t ENBRelationENA;

    int PWMLeft;
    int PWMRight;

    int  LeftIn1;
    int  LeftIn2;
    int  RightIn1;
    int  RightIn2;
};

#endif
