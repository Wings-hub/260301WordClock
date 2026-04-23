/**
 * @file main.ino
 * @brief Main program for the RFID Access Control System.
 *
 * This file defines the visible program flow for the access control application.
 * The technical implementation (RFID communication, identifier comparison, LED control)
 * is fully encapsulated in the respective modules:
 * - ModulMFRC522: Handles tag detection and identifier reading
 * - AccessControl: Verifies identifiers and controls LED feedback
 * - LEDBuiltin: Generic LED library provided by the supervisor
 *
 * @details
 * The loop performs the following logical steps:
 * 1. Wait for a new RFID tag.
 * 2. Read and verify its access number.
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
 * 2. Reads the access number if present.
 * 3. Compares it against the allowed list.
 * 4. Grants or denies access via LED feedback.
 * 5. Prints the access result to the Serial Monitor.
 * 6. Stops communication with the tag before the next cycle.
 */
void loop() {
  LEDBuiltin(false); ///< Keep LED off while idle.

  byte accessNumber[MAX_UID_SIZE]; ///< Buffer for storing the access number bytes.
  byte size;                       ///< Variable to store the access number length.

  /** 
   * @step 1 Detect a new RFID tag.
   * If no tag is detected, the function returns immediately.
   */
  if (newCardPresented(accessNumber, &size)) {

    String status;

    /**
     * @step 2 Verify whether the detected access number is authorized.
     */
    if (isAllowedUID(accessNumber, size)) {
      grantAccess();  ///< Access granted – LED ON for 5 s
      status = "granted";
    } else {
      denyAccess();   ///< Access denied – LED blinks 5 times
      status = "denied";
    }

    // Convert the access number to a readable HEX string
    String HEXStr = uid2HEXString(accessNumber, size);

    // Compose the status message
    String message = "AccessNumber=" + HEXStr + ", Status=" + status;

    /**
     * @step 3 Output the result message (access number and status) to the Serial Monitor.
     */
    Serial.println(message);

    /**
     * @step 4 Properly stop communication with the current RFID tag.
     */
    rfidStop();
  }

  delay(2000); ///< Small pause before
