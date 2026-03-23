/**
 * @file CEncoder.h
 * @brief Header file for incremental quadrature encoder interface.
 *
 * This class provides basic position tracking and velocity estimation
 * for a rotary quadrature encoder using two digital input pins (A and B).
 * It is suitable for use with gear motors or wheels in robotics systems.
 *
 * Features:
 *  - Rising-edge detection on channel A
 *  - Direction detection using channel B
 *  - Position count and velocity (ticks per second)
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#ifndef CENCODER_H
#define CENCODER_H

#include <Arduino.h>

class CEncoder {
  public:
    /**
     * @brief Constructor
     * Note: Call setup() after instantiation to configure the input pins.
     */
    CEncoder(uint8_t pinEncoderA, uint8_t pinEncoderB);

    /**
     * @brief Configures encoder pins as digital inputs and initializes state
     */
    void setup();

    /**
     * @brief Reads and updates position based on encoder edge detection
     * @return Current encoder tick count (signed)
     */
    int getPosition();

    /**
     * @brief Estimates velocity in ticks per second
     * @return Encoder velocity as float
     */
    float getVelocity();

    int getTestValue();
    float getLinearVelocity();

  private:
    // Encoder pin assignments (to be set by constructor)
    //uint8_t pinEncoderA;          // Channel A (step)
    //uint8_t pinEncoderB;          // Channel B (direction)
    uint8_t encoder0PinA;           // Channel A (step signal)
    uint8_t encoder0PinB;
    int encoder0PinALast;
    // State for encoder edge detection
    int valueNow;                 // Current digital read value on A
    int testValue;                 // Current digital read value on A in test function
    volatile long encoder0Pos;   // Absolute encoder position (tick count)
    long lastEncoderPos;         // Position during previous velocity update
    float encoderVelocity;
    // Timing for velocity calculation
    float currentTime;   // Current time in milliseconds
    float lastTime;      // Last update time

    // Computed change for velocity
    long deltaPos;               // Tick change
    float deltaTime;             // Time delta (seconds)
    float velocity;              // Calculated velocity (ticks/second)
};

#endif
