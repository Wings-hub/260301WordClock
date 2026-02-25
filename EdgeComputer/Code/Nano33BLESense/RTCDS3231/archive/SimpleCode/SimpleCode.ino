/**
 * @file sketch_simple_code_RTC.ino
 * @brief Minimal example to verify I2C communication with a DS1307 RTC module using RTClib.
 *
 * This code checks whether the RTC is connected and responding on the I2C bus.
 * It does not read or set the time. Intended for basic hardware verification.
 */

#include <Wire.h>
#include "RTClib.h"

/// RTC object for communication with the DS1307 real-time clock
RTC_DS1307 rtc;

/**
 * @brief Arduino setup function.
 *
 * Initializes the I2C interface and checks if the RTC module is present.
 * If the device is not found, an error message is printed and execution halts.
 * Otherwise, a success message is printed via serial.
 */
void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial port (for boards with native USB)

  if (!rtc.begin()) {
    Serial.println("RTC not found");
    while (1); // Stop execution
  }

  Serial.println("RTC connected");
}

/**
 * @brief Arduino loop function.
 *
 * This function remains empty, as this is only a connection test.
 */
void loop() {
  // Nothing to do
}
