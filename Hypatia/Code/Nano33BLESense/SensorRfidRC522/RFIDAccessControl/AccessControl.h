/**
 * @file AccessControl.h
 * @brief Function-based access control module combining RFID and LED functionality.
 *
 * This module connects the RFID reader (MFRC522) with the LED feedback system
 * to implement a simple and reusable access control mechanism.
 * It reads the UID from detected tags, compares it against a list of authorized IDs,
 * and signals the access status via the builtin LED.
 *
 * The functions are designed to be modular and reusable in other projects
 * without requiring knowledge of the internal RFID or LED implementation.
 */

#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <Arduino.h>
#include "LEDBuiltin.h"
#include "ModulMFRC522.h"

/**
 * @brief Checks whether a given UID is allowed to access the system.
 *
 * This function compares the UID read from the RFID tag
 * with a predefined list of allowed UIDs stored in the module.
 *
 * @param[in] uid  Pointer to the UID byte array (as read from the RFID reader).
 * @param[in] size Number of bytes in the UID.
 * @return true  if the UID matches one of the allowed entries.  
 * @return false if the UID is not in the allowed list.
 */
bool isAllowedAccessNumber(byte* accessNumber, byte size);


/**
 * @brief Grants access to an authorized user.
 *
 * Turns on the builtin LED for 5 seconds to indicate successful access.
 */
void grantAccess();

/**
 * @brief Denies access to an unauthorized user.
 *
 * Blinks the builtin LED five times to indicate denied access.
 */
void denyAccess();

/**
 * @brief Handles a complete access event.
 *
 * Performs the following steps:
 * 1. Reads the current RFID tag (if present).  
 * 2. Prints the UID to the Serial Monitor in hexadecimal format.  
 * 3. Verifies the UID against the allowed list.  
 * 4. Controls the LED feedback (granted/denied).  
 * 5. Stops communication with the RFID tag.
 *
 * @note This function is intended to be called repeatedly in the main loop.
 */


#endif // ACCESS_CONTROL_H
