/**
 * @file AccessControl.cpp
 * @brief Implements access control logic using the MFRC522 RFID reader and LED feedback.
 *
 * This module verifies UIDs against a predefined list, 
 * provides visual feedback through the builtin LED, 
 * and outputs access results via the serial interface.
 */

#include "AccessControl.h"

/** 
 * @brief List of allowed UIDs.
 * 
 * Each UID is stored as a byte array of length MAX_UID_SIZE.
 */
const byte allowedUIDs[][MAX_UID_SIZE] = {
  {0x1A, 0x4A, 0x89, 0x9A},
  {0x24, 0x77, 0x97, 0xA7}
};

/** @brief Number of allowed UIDs in the system. */
const int numAllowedUIDs = 2;

/**
 * @brief Checks whether a given UID matches one of the allowed UIDs.
 *
 * @param[in] uid  Pointer to the UID byte array read from the RFID tag.
 * @param[in] size Number of bytes of the UID.
 * @return true if the UID is in the allowed list, false otherwise.
 */
bool isAllowedUID(byte* uid, byte size) {
  for (int i = 0; i < numAllowedUIDs; i++) {
    bool match = true;
    for (int j = 0; j < size; j++) {
      if (uid[j] != allowedUIDs[i][j]) {
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

/**
 * @brief Reads the current RFID tag, checks access permission, 
 *        and prints the result as a message over serial communication.
 *
 * The message format is:  
 * `UID=<UID_HEX>,Status=<granted/denied>`
 */
void sendMessage() {
  byte uid[MAX_UID_SIZE];
  byte size;

  // Try to read a new RFID tag
  if (!newCardPresented(uid, &size)) {
    return;
  }

  // Convert UID to HEX string for display
  String uidStr = "";
  for (byte i = 0; i < size; i++) {
    if (uid[i] < 0x10) uidStr += "0";
    uidStr += String(uid[i], HEX);
  }

  // Determine access status
  bool allowed = isAllowedUID(uid, size);
  String status = allowed ? "granted" : "denied";

  // Combine into a formatted message
  String message = "UID=" + uidStr + ",Status=" + status;

  // Output to serial monitor         \\ later to the SDcard
  Serial.println(message);

  // LED feedback
  if (allowed) {
    grantAccess();
  } else {
    denyAccess();
  }

  // Properly stop communication with the card
  rfidStop();
}
