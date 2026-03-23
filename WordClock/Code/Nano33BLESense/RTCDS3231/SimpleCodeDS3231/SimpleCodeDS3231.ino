/**
 * @file SimpleCodeDS3231.ino
 * @brief Functional test for the DS3231 RTC module.
 *
 * This simple Code demonstrates the modular RTC functions.
 * It initializes the DS3231 RTC and prints standardized ISO 8601 formatted
 * timestamps to the serial monitor every 10 seconds.
 */

#include "RTC_Module.h"

/**
 * @brief Arduino setup function.
 *
 * Initializes serial communication and the RTC module.
 * Displays a startup message confirming successful initialization.
 */
void setup(void) {
  Serial.begin(9600);
  while (!Serial);

  Serial.println(F("RTC DS3231 Timestamp Format":);
  rtc_init();
}

/**
 * @brief Arduino main loop function.
 *
 * Calls rtc_sendMessage() periodically to display the current timestamp
 * every 10 seconds.
 */
void loop(void) {
  rtc_sendMessage();
  delay(10000);  // 10-second interval
}
