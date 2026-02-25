/**
 * @file RTC_Module.cpp
 * @brief Implementation of the DS3231 RTC module without object orientation.
 *
 * Provides basic functions for initializing the RTC, generating standardized
 * ISO 8601 timestamps, and sending them as serial messages.
 */

#include "RTC_Module.h"

RTC_DS3231 rtc;  ///< Global instance of the DS3231 RTC.

/**
 * @brief Initializes the DS3231 RTC module.
 *
 * Establishes I²C communication with the RTC and verifies connection.
 * The module must already be set to the correct time.
 */
void rtc_init(void) {
  if (!rtc.begin()) {
    Serial.println(F("Error: DS3231 not found!"));
    while (1);
  }

  Serial.println(F("RTC module DS3231 initialized successfully."));
}

/**
 * @brief Generates a timestamp in ISO 8601 format.
 *
 * @return Timestamp string formatted as YYYY-MM-DDTHH:MM:SS.
 */
String rtc_getTimestamp(void) {
  DateTime now = rtc.now();
  char buffer[21];
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d",
           now.year(), now.month(), now.day(),
           now.hour(), now.minute(), now.second());
  return String(buffer);
}

/**
 * @brief Sends the current timestamp to the serial monitor.
 *
 * Displays the standardized ISO 8601 timestamp.
 * This function can later be adapted to forward the timestamp
 * to an SD card or other logging system.
 */
void rtc_sendMessage(void) {
  String timestamp = rtc_getTimestamp();
  Serial.print(F("Timestamp: "));
  Serial.println(timestamp);
}
