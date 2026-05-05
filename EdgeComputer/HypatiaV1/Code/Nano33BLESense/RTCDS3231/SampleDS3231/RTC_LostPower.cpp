/**
 * @file RTC_LostPower.cpp
 * @brief Implementation of DS3231 RTC functions with power-loss detection and ISO 8601 timestamp output.
 *
 * This module provides initialization, manual time adjustment, timestamp generation,
 * and serial output for the DS3231 RTC.
 * The clock continues running as long as the backup battery is functional.
 * If both main power and backup battery are lost, the time must be manually adjusted once.
 */

#include "RTC_LostPower.h"

RTC_DS3231 rtc;  ///< Global instance of the DS3231 RTC.

/**
 * @brief Initializes the DS3231 RTC module and checks for power loss.
 *
 * If the RTC lost power (for example, if the backup battery was empty or removed),
 * the time must be set manually using rtc.adjust().
 *
 * Example usage (uncomment only once to set the correct time):
 * @code
 * rtc.adjust(DateTime(2025, 11, 6, 10, 45, 0));
 * @endcode
 *
 * After the correct time has been set, comment out or remove the rtc.adjust() line.
 * The DS3231 will then continue to keep accurate time as long as the backup battery is operational.
 */
void rtc_init(void) {
  if (!rtc.begin()) {
    Serial.println(F("Error: DS3231 not found!"));
    while (1);
  }


  // Check for power loss (e.g., battery empty or removed)
  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power -> manual time adjustment required."));

    // --- Manual time adjustment (only needed once) ---
    // ! Uncomment this line ONCE to set the current time, then comment it again !
    rtc.adjust(DateTime(2025, 11, 6, 10, 59, 0));  // Example: set manually
  } else {
    Serial.println(F("RTC running normally. Time preserved."));
  }

  Serial.println("RTC initialized successfully.\n");
}

/**
 * @brief Retrieves the current time and formats it as an ISO 8601 timestamp.
 *
 * @return Formatted timestamp string, e.g., "2025-11-06T10:45:25".
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
 * @brief Prints the current timestamp to the serial monitor.
 *
 * This function retrieves the current timestamp using rtc_getTimestamp()
 * and prints it to the serial monitor for verification.
 */
void rtc_sendMessage(void) {
  String timestamp = rtc_getTimestamp();
  Serial.print("Timestamp: ");
  Serial.println(timestamp);
}
