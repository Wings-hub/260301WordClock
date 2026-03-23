#include "CConfiguration.h"

/**
 * @brief Default constructor for CConfiguration.
 *
 * Initializes an instance with predefined static GPIO pin values.
 */
CConfiguration::CConfiguration() {
    // No runtime initialization required.
}

/**
 * @brief Gets the GPIO pin for HX711 data line.
 * @return Configured HX711 data GPIO.
 */
uint8_t CConfiguration::getHX711DataPin() {
    return HX711DataPin;
}

/**
 * @brief Gets the GPIO pin for HX711 clock line.
 * @return Configured HX711 clock GPIO.
 */
uint8_t CConfiguration::getHX711ClockPin() {
    return HX711ClockPin;
}

/**
 * @brief Gets the GPIO pin for MCP4725 SDA (I2C data).
 * @return Configured MCP4725 SDA GPIO.
 */
uint8_t CConfiguration::getMCP4725DataPin() {
    return MCP4725DataPin;
}

/**
 * @brief Gets the GPIO pin for MCP4725 SCL (I2C clock).
 * @return Configured MCP4725 SCL GPIO.
 */
uint8_t CConfiguration::getMCP4725ClockPin() {
    return MCP4725ClockPin;
}

uint8_t CConfiguration::getMCP4725InputPin() {
    return MCP4725IntputPin;
}