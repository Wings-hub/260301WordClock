/**
 * @file ModulMFRC522.cpp
 * @brief Implementation of the RFID reader module for the MFRC522.
 *
 * This module provides initialization, card detection, access number reading,
 * and proper termination of communication with RFID tags.
 * It encapsulates all hardware-related operations of the MFRC522 reader
 * to ensure reusability and clear separation of hardware functionality.
 */

#include "ModulMFRC522.h"

// ---------------------------------------------------------------------------
// Global instance
// ---------------------------------------------------------------------------

/**
 * @brief Creates the global MFRC522 instance.
 *
 * Uses the pin definitions from ModulMFRC522.h to configure
 * the Slave Select (SS) and Reset (RST) pins.
 */
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

// ---------------------------------------------------------------------------
// Function implementations
// ---------------------------------------------------------------------------

/**
 * @brief Initializes the SPI interface and the MFRC522 RFID reader.
 *
 * This function must be called once during system startup.
 * It initializes the SPI communication and prepares the MFRC522 module
 * to detect and communicate with RFID transponders.
 *
 * @note Must be called before any tag detection or reading functions.
 */
void rfidInit() {
  SPI.begin();
  rfid.PCD_Init();
  Serial.println(F("RFID reader initialized."));
}

/**
 * @brief Detects a new RFID tag and reads its access number.
 *
 * This function checks whether a new RFID tag is present in the reader’s range.
 * If a tag is detected and successfully read, its access number (UID bytes)
 * is copied into the provided array.
 *
 * @param[out] accessNumber Pointer to an array where the tag’s access number bytes will be stored.
 * @param[out] size         Pointer to a variable that receives the number of bytes read.
 * @return true  If a new tag is detected and successfully read.
 * @return false If no tag is present or the reading failed.
 *
 * @note The number of bytes can vary depending on the tag type (commonly 4, 7, or 10 bytes).
 */
bool newCardPresented(byte *accessNumber, byte *size) {
  if (!rfid.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!rfid.PICC_ReadCardSerial()) {
    return false;
  }

  // Copy UID bytes into the provided access number array
  for (byte i = 0; i < rfid.uid.size; i++) {
    accessNumber[i] = rfid.uid.uidByte[i];
  }

  *size = rfid.uid.size;
  return true;
}

/**
 * @brief Converts the access number bytes into a readable HEX string.
 *
 * This helper function transforms the raw access number bytes (UID)
 * into a formatted hexadecimal string for display or logging.
 * Leading zeros are automatically added for values below 0x10.
 *
 * @param[in]  accessNumber Pointer to the byte array containing the access number.
 * @param[in]  size         Number of bytes that make up the access number.
 * @return A String object containing the formatted access number in uppercase HEX.
 *
 * @code
 * byte accessNumber[4] = {0x12, 0xAF, 0x03, 0x9C};
 * String result = accessNumberToHEXString(accessNumber, 4);
 * // result = "12AF039C"
 * @endcode
 */
String accessNumberToHEXString(byte *accessNumber, byte size) {
  String result = "";
  for (byte i = 0; i < size; i++) {
    if (accessNumber[i] < 0x10) result += "0"; ///< Add leading zero if < 0x10
    result += String(accessNumber[i], HEX);
  }
  result.toUpperCase();
  return result;
}

/**
 * @brief Properly stops communication with the currently active RFID tag.
 *
 * This function ends communication with the tag and stops
 * any cryptographic operations on the MFRC522 module.
 * It should be called after each successful read cycle
 * to properly reset the reader for the next tag.
 */
void rfidStop() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
