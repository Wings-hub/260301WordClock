/**
 * @file AccessControl.cpp
 * @brief Implements access control logic using the MFRC522 RFID reader and LED feedback.
 *
 * This module verifies access numbers against a predefined list,
 * provides visual feedback through the builtin LED,
 * and outputs access results via the serial interface.
 */

#include "AccessControl.h"

// ---------------------------------------------------------------------------
// Allowed access numbers
// ---------------------------------------------------------------------------

/**
 * @brief List of allowed access numbers (formerly UIDs).
 *
 * Each access number is stored as a byte array of length MAX_UID_SIZE.
 * The number of bytes may vary depending on the RFID tag type.
 */
const byte allowedAccessNumbers[][MAX_UID_SIZE] = {
  {0x1A, 0x4A, 0x89, 0x9A},
  {0x24, 0x77, 0x97, 0xA7}
};

/** @brief Number of allowed access numbers in the system. */
const int numAllowedAccessNumbers = 2;

// ---------------------------------------------------------------------------
// Core access control logic
// ---------------------------------------------------------------------------

/**
 * @brief Checks whether a given access number matches one of the allowed entries.
 *
 * @param[in] accessNumber Pointer to the byte array representing the access number read from the RFID tag.
 * @param[in] size         Number of bytes in the access number.
 * @return true if the access number is authorized, false otherwise.
 */
bool isAllowedAccessNumber(byte* accessNumber, byte size) {
  for (int i = 0; i < numAllowedAccessNumbers; i++) {
    bool match = true;
    for (int j = 0; j < size; j++) {
      if (accessNumber[j] != allowedAccessNumbers[i][j]) {
        match = false;
        break;
      }
    }
    if (match) return true;
  }
  return false;
}

/**
 * @brief Grants access: turns the builtin LED on for 5 seconds.
 */
void grantAccess() {
  LEDBuiltin(true);
  delay(5000);
  LEDBuiltin(false);
}

/**
 * @brief Denies access: blinks the builtin LED 5 times.
 */
void denyAccess() {
  for (int i = 0; i < 5; i++) {
    LEDBuiltin(true);
    delay(200);
    LEDBuiltin(false);
    delay(200);
  }
}

// ---------------------------------------------------------------------------
// Message handling (optional, for serial output)
// ---------------------------------------------------------------------------

/**
 * @brief Reads the current RFID tag, checks access permission,
 *        and prints the result as a message over serial communication.
 *
 * The message format is:
 * `AccessNumber=<HEX>, Status=<granted/denied>`
 */


 }