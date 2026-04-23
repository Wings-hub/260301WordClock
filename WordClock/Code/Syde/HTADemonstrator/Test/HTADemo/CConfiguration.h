#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

#include <Arduino.h>

/**
 * @class CConfiguration
 * @brief Central configuration class for all hardware-related GPIO settings.
 *
 * Provides pin definitions for HX711 and MCP4725 devices.
 */
class CConfiguration {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the configuration with fixed predefined pin mappings.
     */
    CConfiguration();

    /**
     * @brief Returns the GPIO pin used for HX711 data.
     * @return GPIO number for HX711 data line.
     */
    uint8_t getHX711DataPin();

    /**
     * @brief Returns the GPIO pin used for HX711 clock.
     * @return GPIO number for HX711 clock line.
     */
    uint8_t getHX711ClockPin();

    /**
     * @brief Returns the GPIO pin used for MCP4725 SDA (data).
     * @return GPIO number for MCP4725 SDA.
     */
    uint8_t getMCP4725DataPin();

    /**
     * @brief Returns the GPIO pin used for MCP4725 SCL (clock).
     * @return GPIO number for MCP4725 SCL.
     */
    uint8_t getMCP4725ClockPin();

    uint8_t getMCP4725InputPin();
    
}
private:
    const uint8_t HX711DataPin = 4;   ///< HX711 data pin.
    const uint8_t HX711ClockPin = 5;  ///< HX711 clock pin.
    const uint8_t MCP4725DataPin = 41;  ///< MCP4725 I2C SDA pin.
    const uint8_t MCP4725ClockPin = 42; ///< MCP4725 I2C SCL pin.
    const uint8_t MCP4725IntputPin = 8; //Input pin dac
};

#endif
