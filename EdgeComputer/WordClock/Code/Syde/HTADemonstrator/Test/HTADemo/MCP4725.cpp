#include "MCP4725.h"
#include <Wire.h>              //comunicazione I2C

/**
 * @brief Default constructor.
 * Initializes internal state and default pin values.
 */
MCP4725::MCP4725() {
    dataPin = 41;
    clockPin = 42;
    dacInputPin =8;
    B = 0;
    scaled = 0;
    Vd_mV = 0;
  
}

/**
 * @brief Initializes I2C bus and DAC device.
 * @param dataPin GPIO for I2C SDA.
 * @param clockPin GPIO for I2C SCL.
 */
void MCP4725::setup(uint8_t dataPin, uint8_t clockPin) {
    I2C.begin(dataPin, clockPin);
    dac.begin(0x60, &I2C);  // Address 0x60 is standard for MCP4725
}

void MCP4725::setupInput(uint8_t dacInputPin) {
    I2C.begin(dataPin, clockPin);
    dac.begin(0x60, &I2C);  // Address 0x60 is standard for MCP4725
}

/**
 * @brief Sends the internally constrained voltage value to the DAC.
 * 
 * ⚠️ Note: This requires `Vd` to be defined as a class member.
 * Currently, it must be set before calling `setData()`.
 */
void MCP4725::setDigitalData() {
    outputToDAC(dac, Vd_constrained);  // Use Vd_constrained as internal "Vd"
}
float MCP4725::getAnalogData(){
     return analogRead(Configuration.MCP4725InputPin()) * (3300.0 / 4095.0);
    
}
/**
 * @brief Converts a voltage to DAC format and sends it via I2C.
 * Applies soft-constrain to the input range (-1500 mV to 1500 mV) and uses a deadband.
 * 
 * @param dac Reference to the target DAC instance.
 * @param Vd Target voltage in millivolts.
 */
void MCP4725::outputToDAC(Adafruit_MCP4725& dac, float Vd) {
    Vd_constrained = constrain(Vd, -1500, 1500);

    if (Vd_constrained >= -B && Vd_constrained <= B) {
        scaled = 1500;  // Hold neutral voltage in deadband
    } else {
        Vd_mV = Vd_constrained + 1500.0f;
        scaled = (Vd_mV / 3000.0f) * 4095.0f;
    }

    dac.setVoltage(static_cast<uint16_t>(scaled), false);
}
