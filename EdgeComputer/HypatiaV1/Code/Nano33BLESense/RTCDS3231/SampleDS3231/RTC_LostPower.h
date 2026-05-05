/**
 * @file RTC_LostPower.h
 * @brief Modular interface for the DS3231 RTC with power-loss detection and timestamp generation.
 *
 * Provides functions for initializing the DS3231 RTC, detecting power or battery loss,
 * generating standardized ISO 8601 formatted timestamps,
 * and printing data to the serial monitor.
 */

#ifndef RTC_LostPower_H
#define RTC_LostPower_H

#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"

/**
 * @brief Initializes the DS3231 RTC module.
 *
 * Establishes I^2C communication with the DS3231 and checks for power loss.
 * If the RTC lost power (battery empty or missing), the time must be set manually
 * using rtc.adjust(DateTime(year, month, day, hour, minute, second)).
 */
void rtc_init(void);

/**
 * @brief Reads and formats the current time.
 *
 * Retrieves the current time from the RTC and returns it as a formatted String
 * following the ISO 8601 standard (YYYY-MM-DDTHH:MM:SS).
 *
 * @return String containing the formatted timestamp.
 */
String rtc_getTimestamp(void);

/**
 * @brief Sends the formatted timestamp to the serial monitor.
 *
 * This function can later be adapted for logging or integration in other modules.
 */
void rtc_sendMessage(void);

#endif 
