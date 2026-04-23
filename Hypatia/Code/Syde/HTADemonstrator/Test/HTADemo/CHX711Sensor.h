#ifndef CHX711SENSOR_H
#define CHX711SENSOR_H

#include <Arduino.h>
#include "HX711.h"

/**
 * @class CHX711Sensor
 * @brief Interface class for the HX711 load cell amplifier, including filtering and PD control.
 */
class CHX711Sensor {
public:
    /**
     * @brief Default constructor.
     */
    CHX711Sensor();

    /**
     * @brief Initializes HX711 with data and clock GPIO pins.
     * @param dataPin GPIO pin connected to HX711 data line.
     * @param clockPin GPIO pin connected to HX711 clock line.
     */
    void setup(uint8_t dataPin, uint8_t clockPin);

    /**
     * @brief Sets parameters for control and filtering (PD controller and optional filter settings).
     * @param newMedianP Median filter length (currently unused).
     * @param newKp Proportional gain.
     * @param newKd Derivative gain.
     * @param newB Reserved parameter.
     * @param newC Reserved parameter.
     * @param newK Scaling factor.
     */
    void setControlValues( int newKp, int newKd, int newB, int newC, int newK);

    /**
     * @brief Returns a filtered raw value using a moving average filter.
     * @return Filtered value from HX711.
     */
    long getFilterValue();

    /**
     * @brief Calculates the PD controller output from force values.
     * @param Fl Current force value.
     * @param prevFl Reference to the previous force value (updated internally).
     * @return Control output voltage.
     */
    float getPIDValue(float Fl, float& prevFl);

    /**
     * @brief Performs zero-point calibration (tare).
     * @return 0 if successful, non-zero if failed.
     */
    int getZero();

    /**
     * @brief Performs reference mass calibration and calculates gain factor.
     */
    void getEnter();

    /**
     * @brief Reads the current raw sensor value (unfiltered).
     * @return Raw sensor reading.
     */
    float getRaw();

    /**
     * @brief Returns the processed force value after filtering and PID.
     * @return Final computed force value.
     */
    float getValue();

private:
    HX711 scale;              ///< HX711 sensor instance.

    uint8_t dataPin;          ///< Data GPIO pin.
    uint8_t clockPin;         ///< Clock GPIO pin.

    int Kp;                   ///< Proportional gain.
    int Kd;                   ///< Derivative gain.
    int C;                    ///< Reserved parameter.
    int K;                    ///< Scaling factor (calibration gain).
    int ringBufferLenght;     ///< Ring buffer length.
    long ringBuffer[100];     ///< Ring buffer for filtering.
    int bufferIndex;          ///< Current index in the ring buffer.

    bool zeroSet;     ///< Flag to check if zero calibration was performed.
    float refMass;  ///< Reference mass used for calibration [g].
    float delta;              ///< Delta value for gain calculation.
    int raw;              ///< Latest raw reading.
    long lastRaw;         ///< Previous raw reading for future use.
    float Fl;       ///< Filtered force value.
    float prevFl;   ///< Previous filtered force value for PD control.
    float Vd;       ///< Output voltage for DAC after PD calculation.

    // Future calibration members (currently unused):
    // float calibrationFactor;
    // MedianFilter<long> filter;
};

#endif
