/**
 * @file main.ino
 * @brief Main program for the RFID Access Control System.
 *
 * This file defines the visible program flow for the access control application.
 * The technical implementation (RFID communication, UID comparison, LED control)
 * is fully encapsulated in the respective modules:
 * - ModulMFRC522: Handles tag detection and UID reading
 * - AccessControl: Verifies UIDs and controls LED feedback
 * - LEDBuiltin: Generic LED library provided by the supervisor
 *
 * @details
 * The loop performs the following logical steps:
 * 1. Wait for a new RFID tag.
 * 2. Read and verify its UID.
 * 3. Provide visual feedback via the builtin LED.
 * 4. Output the result to the Serial Monitor.
 * 5. Stop communication before the next cycle.
 */

#include "AccessControl.h"

/**
 * @brief Initializes all subsystems and prepares the system for operation.
 *
 * Called once at startup.
 * - Initializes serial communication for debugging and monitoring.
 * - Initializes the builtin LED for feedback.
 * - Initializes the MFRC522 RFID reader via SPI.
 */
void setup() {
  Serial.begin(9600);
  while (!Serial); ///< Wait until serial communication is ready.

  LEDBuiltinsetup();   ///< Initialize builtin LED (ready for visual feedback)
  rfidInit();          ///< Initialize RFID reader and SPI bus

  Serial.println(F("RFID Access Control System ready..."));
}

/**
 * @brief Main program loop: performs one complete access control cycle.
 *
 * The function continuously:
 * 1. Waits for a new RFID tag.
 * 2. Reads the UID if present.
 * 3. Compares it against the allowed list.
 * 4. Grants or denies access via LED feedback.
 * 5. Prints the access result to the Serial Monitor.
 * 6. Stops communication with the tag before the next cycle.
 */
void loop() {
  LEDBuiltin(false); ///< Keep LED off while idle.

  byte uid[MAX_UID_SIZE]; ///< Buffer for storing the UID bytes.
  byte size;              ///< Variable to store the UID length.

  /** 
   * @step 1 Detect a new RFID tag.
   * If no tag is detected, the function returns immediately.
   */
  if (!newCardPresented(uid, &size)) {
    return;
  }

  /**
   * @step 2 Verify whether the detected UID is authorized.
   */
  if (isAllowedUID(uid, size)) {
    grantAccess();  /// Access granted - LED ON for 5 s
  } else {
    denyAccess();   /// Access denied - LED blinks 5 times
  }

  /**
   * @step 3 Output the result message (UID and status) to the Serial Monitor.
   */
  sendMessage();

  /**
   * @step 4 Properly stop communication with the current RFID tag.
   */
  rfidStop();

  delay(500); ///< Small pause before the next access attempt.
}
