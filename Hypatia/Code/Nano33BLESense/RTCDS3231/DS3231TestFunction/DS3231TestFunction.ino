 /**
 * @file functiontest.ino
 * @brief Functional test for the DS3231 RTC module.
 *
 * Displays current date, time, and temperature, and calculates
 * the number of days since 21 March 2021.
 *
 * @hardware Arduino Nano 33 BLE Sense Lite (3.3 V logic)
 * @module  DS3231 (5-pin RTC)
 */

#include <Wire.h>
#include "RTClib.h"

/// @brief RTC object for the DS3231 module.
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println(F("= DS3231 Function Test: Days since 21 March 2021 ="));

  if (!rtc.begin()) {
    Serial.println(F("Error: DS3231 not found on I2C bus!"));
    while (1);
  }

  // If the RTC lost power (e.g. battery removed), set time manually once.
  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power  please set time manually below."));

    // Example: 2 November 2025, 19:15:00 (CET = UTC + 1)
    // rtc.adjust(DateTime(2025, 11, 2, 19, 15, 0));

    Serial.println(F("RTC time has been set manually.\n"));
  }

  Serial.println(F("RTC initialized successfully.\n"));
}

void loop() {
  // Read current time from RTC
  DateTime now = rtc.now();

  // Reference date: 21 March 2021
  DateTime reference(2021, 3, 21, 0, 0, 0);

  // Calculate difference
  TimeSpan diff = now - reference;

  // Read internal temperature
  float temp = rtc.getTemperature();

  // Output to Serial Monitor
  Serial.println(F("--------"));
  Serial.print(F("Today: "));
  Serial.println(now.timestamp(DateTime::TIMESTAMP_DATE));

  Serial.print(F("Time: "));
  Serial.println(now.timestamp(DateTime::TIMESTAMP_TIME));

  Serial.print(F("Reference date: "));
  Serial.println(reference.timestamp(DateTime::TIMESTAMP_DATE));

  Serial.print(F("Days since 21.03.2021: "));
  Serial.println(diff.days());

  Serial.print(F("Temperature: "));
  Serial.print(temp, 2);
  Serial.println(F(" grad"));
  Serial.println(F("--------\n"));

  delay(600000); // 10-minute interval
}
