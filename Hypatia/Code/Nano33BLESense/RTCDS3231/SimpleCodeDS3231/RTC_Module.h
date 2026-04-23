/**
 * @file RTC_Module.h
 * @brief Simple modular RTC interface.
 *
 * Provides initialization, ISO 8601 timestamp generation,
 * and message output for the DS3231 real-time clock (RTC).
 * The structure is modular, allowing easy integration into
 * larger systems such as data loggers or access control units.
 */

#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"

/**
 * @brief Initializes the DS3231 RTC module.
 *
 * Starts I²C communication with the DS3231 and verifies device presence.
 * This function does not modify the stored time.
 */
void rtc_init(void);

/**
 * @brief Returns the current timestamp in ISO 8601 format.
 *
 * Generates a string formatted as `YYYY-MM-DDTHH:MM:SS`
 * representing the current date and time read from the DS3231 RTC.
 *
 * @return Formatted timestamp as an Arduino String object.
 */
String rtc_getTimestamp(void);

/**
 * @brief Sends the current timestamp as a serial message.
 *
 * Prints the formatted timestamp to the serial monitor in the form
 * `Timestamp: YYYY-MM-DDTHH:MM:SS`.
 * In later versions this function can be adapted to store the data
 * on an SD card or forward it to other modules.
 */
void rtc_sendMessage(void);


#endif 
