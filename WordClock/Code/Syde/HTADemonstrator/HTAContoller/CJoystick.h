/**
 * @file CJoystick.h
 * @brief Header file for analog joystick input handling.
 *
 * The CJoystick class provides an abstraction for working with one or two
 * analog joysticks on an ESP32. It maps throttle and directional inputs
 * to motor control outputs suitable for PWM-based motor drivers.
 *
 * Inputs:
 *  - Throttle (mapped from analog 0–1023 to 0–100)
 *  - Direction (X and Y axes mapped to -100 to +100)
 *
 * Outputs:
 *  - Motor power values converted to PWM values in the range [0, 255]
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#ifndef CJOYSTICK_H
#define CJOYSTICK_H

#include <Arduino.h>

class CJoystick {
  public:
    /**
     * @brief Constructor
     * @param pinX Analog input pin for horizontal movement (left/right)
     * @param pinY Analog input pin for vertical movement (forward/backward)
     * @param pinThrottle Analog input pin for throttle control (e.g., slider or separate joystick)
     */
    CJoystick();
    
    /*

    uint8_t getVelocityControl(uint8_t pinThrottle);

    int getControlLeftRight(uint8_t pinJoystickX);
    int getControlForwardBackward(uint8_t pinJoystickY);

    float getRawThrottle(uint8_t pinThrottle);



    uint8_t getMotorControlValueLeft();
    uint8_t getMotorControlValueRight();

    */

    int getRawX(uint8_t pinJoystickX);
    int getRawY(uint8_t pinJoystickY);
    //int8_t getAnglePercentageX(int rawX);
    //int8_t getAnglePercentageY(int rawY);
    int getLeftValue(int rawX);
    int getRightValue(int rawY);

    

  private:
    // Joystick pin assignments
    uint8_t pinJoystickX;     // X-axis input pin
    uint8_t pinJoystickY;     // Y-axis input pin
    uint8_t pinThrottle;      // Throttle input pin

    // Raw analog values from ADC
    float rawThrottle;
    int rawX;
    int rawY;
    int valueLeft;
    int valueRight;

    // Mapped control values (after scaling)
    int8_t forwardBackward;      // Y-direction motion
    int8_t leftRight;            // X-direction motion
    float throttle;             // Throttle percentage (0–100)

    // Internal motor power values (-100 to 100)
    int8_t leftMotorPower;
    int8_t rightMotorPower;

    // Final PWM outputs (0 to 255)
    uint8_t pwmLeft;
    uint8_t pwmRight;

    int8_t AnglePercX;
    int8_t AnglePercY;

};

#endif
