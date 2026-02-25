/**
 * @file SampleDS3231.ino
 * @brief Simple application to test the DS3231 RTC power-loss behavior and timestamp output.
 *
 * This example initializes the DS3231 RTC module, checks for power loss,
 * and prints standardized ISO 8601 formatted timestamps to the serial monitor every 10 seconds.
 */

#include "RTC_LostPower.h"

void setup(void) {
  Serial.begin(9600);
  while (!Serial);

  rtc_init();   // Initialize RTC and check for power-loss status
}

void loop(void) {
  rtc_sendMessage();   // Print the current timestamp
  delay(10000);        // 10-second interval
}
