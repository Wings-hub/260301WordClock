#ifndef MCP4725_H
#define MCP4725_H

#include <Adafruit_MCP4725.h>
#include <Wire.h>              //comunicazione I2C

/**
 * @class MCP4725
 * @brief Interface class for controlling the Adafruit MCP4725 DAC via I2C.
 */
class MCP4725 {
public:
    /**
     * @brief Default constructor.
     * Initializes default pin values and internal variables.
     */
    MCP4725();

    /**
     * @brief Initializes the I2C interface with given SDA and SCL pins.
     * @param dataPin GPIO for I2C SDA.
     * @param clockPin GPIO for I2C SCL.
     */
    void setup(uint8_t dataPin, uint8_t clockPin);

    /**
     * @brief Sets and sends the constrained voltage value to the DAC.
     * Uses internal `Vd_constrained` value.
     */
    void setDigitalData();
    void getAnalogData();

    /**
     * @brief Outputs a given voltage value to a provided DAC instance.
     * @param dac Reference to an external DAC object (advanced use).
     * @param Vd Target voltage in mV (or custom unit scale).
     */
    void outputToDAC(Adafruit_MCP4725& dac, float Vd);

private:
    Adafruit_MCP4725 dac;     ///< DAC driver instance.
    TwoWire I2C =TwoWire(0);; ///< Custom I2C bus.

    uint8_t dataPin;          ///< SDA pin.
    uint8_t clockPin;         ///< SCL pin.
    uint8_t dacInputPin1;
    float scaled;             ///< Scaled DAC value (0–4095).
    float Vd_constrained;     ///< Voltage value after applying bounds.
    float Vd_mV;              ///< Intermediate voltage value in mV.

    int B;                    ///< Deadband threshold for DAC output (optional).
};

#endif
