/**
 * @file rtc_simple_application.ino
 * @brief Simple application to measure heart rate signal and log timestamped data to an SD card.
 *
 * This example uses a DS1307 RTC to generate timestamps and stores
 * raw pulse values from an analog sensor together with the time on the SD card.
 * The data is written as a formatted string in CSV-like structure.
 */

#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

/// Analog input pin for pulse sensor
const int heartRatePin = A0;

/// Chip select pin for SD card module
const int chipSelect = 10;

/// RTC object for DS1307 real-time clock
RTC_DS1307 rtc;

/**
 * @brief Arduino setup function.
 *
 * Initializes the serial port, RTC module, and SD card.
 * Sets the RTC to compile time if it is not running.
 */
void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial connection

  pinMode(heartRatePin, INPUT);

  if (!rtc.begin()) {
    Serial.println("RTC not found");
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set to compile time
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("SD initialization failed");
    while (1);
  }

  Serial.println("System ready");
}

/**
 * @brief Arduino main loop function.
 *
 * Repeatedly reads the analog pulse signal, gets the current timestamp from the RTC,
 * formats the data into a string, writes it to the SD card, and prints it to the serial monitor.
 */
void loop() {
  int pulseRaw = analogRead(heartRatePin);
  DateTime now = rtc.now();

  String logEntry = formatLog(now, pulseRaw);
  writeToSD("log.txt", logEntry);
  Serial.println(logEntry);

  delay(1000); // Wait 1 second
}

/**
 * @brief Formats the timestamp and pulse value into a single string.
 * @param dt The current time as DateTime object.
 * @param value The raw analog value from the pulse sensor.
 * @return A formatted string (e.g. "2025-05-27T14:45:01;512").
 */
String formatLog(const DateTime& dt, int value) {
  String line = dt.timestamp(DateTime::TIMESTAMP_FULL);
  line += ";";
  line += value;
  return line;
}

/**
 * @brief Writes a line of data to the SD card.
 * @param filename The name of the file to write to.
 * @param data The string data to write.
 */
void writeToSD(const char* filename, const String& data) {
  File file = SD.open(filename, FILE_WRITE);
  if (file) {
    file.println(data);
    file.close();
  } else {
    Serial.println("SD write error");
  }
}
