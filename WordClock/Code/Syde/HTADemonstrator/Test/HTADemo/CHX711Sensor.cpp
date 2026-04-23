#include "CHX711Sensor.h"
#include "CConfiguration.h"
#include <Preferences.h>

/**
 * @brief Default constructor. Initializes control and filter parameters.
 */
CHX711Sensor::CHX711Sensor() {
    dataPin = 4;
    clockPin = 5;

    Kp = 1.2f;
    Kd = 0.4f;
    C = 0;
    K = 0;
    zeroSet = false;     ///< Flag to check if zero calibration was performed.
    refMass = 1000.0f;  ///< Reference mass used for calibration [g].
    delta;              ///< Delta value for gain calculation.
    raw = 0;              ///< Latest raw reading.
    lastRaw = 0;         ///< Previous raw reading for future use.
    Fl = 0;       ///< Filtered force value.
    prevFl = 0;   ///< Previous filtered force value for PD control.
    Vd = 0;       ///< Output voltage for DAC after PD calculation.

    bufferIndex = 0;
    ringBufferLenght = 100;
}

/**
 * @brief Initializes the HX711 with specified GPIO pins.
 * @param dataPin GPIO pin connected to the HX711 data output.
 * @param clockPin GPIO pin connected to the HX711 clock input.
 */
void CHX711Sensor::setup(uint8_t dataPin, uint8_t clockPin) {
    pinMode(dataPin, INPUT);
    pinMode(clockPin, OUTPUT);
    scale.begin(dataPin, clockPin);
}

/**
 * @brief Sets control parameters for filtering and PD regulation.
 * @param newKp Proportional gain.
 * @param newKd Derivative gain.
 * @param newB Reserved.
 * @param newC Reserved.
 * @param newK Scaling factor (calibration gain).
 */
void CHX711Sensor::setControlValues(int newKp, int newKd, int newB, int newC, int newK) {
    Kp = newKp;
    Kd = newKd;
    C = newC;
    K = newK;
}

/**
 * @brief Computes a filtered sensor value using a moving average.
 * @return Filtered raw value.
 */
long CHX711Sensor::getFilterValue() {
    bufferIndex = (bufferIndex + 1) % ringBufferLenght;
    ringBuffer[bufferIndex] = scale.read();

    long sum = 0;
    int count = (bufferIndex < ringBufferLenght) ? bufferIndex + 1 : ringBufferLenght;
    for (int i = 0; i < count; ++i) {
        sum += ringBuffer[i];
    }

    return sum / count;
}

/**
 * @brief Applies PD control to the current and previous values.
 * @param Fl Current force value.
 * @param prevFl Reference to previous value (will be updated).
 * @return Control signal (voltage).
 */
float CHX711Sensor::getPIDValue() {
    Fl = raw;
    float prop = Kp * Fl;
    float deriv = Kd * (Fl - prevFl);
    prevFl = Fl;
    return prop + deriv;
}

/**
 * @brief Sets the current value as the zero reference (tare).
 * @return 0 if successful.
 */
int CHX711Sensor::getZero() {
    scale.tare(20);  // Perform 20 readings for accurate tare
    C = scale.read();
    zeroSet = true;
    return 0;
}

/**
 * @brief Calibrates the sensor using a known reference mass.
 */
void CHX711Sensor::getEnter() {
    if (!zeroSet) {
        Serial.println("Error: Zero not set before calibration.");
        return;
    }

    delta = scale.read() - C;

    if (abs(delta) < 10) {
        Serial.println("Error: delta too small, cannot calibrate.");
        return;
    }

    K = refMass / delta;
}

/**
 * @brief Returns the raw reading from HX711 (unfiltered).
 * @return Raw value in weight units (from HX711 library).
 */
float CHX711Sensor::getRaw() {
    return scale.get_units();
}

/**
 * @brief Returns the mos*
*/