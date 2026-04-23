/**
 * @file ModulMFRC522.cpp
 * @brief Implementation of the RFID reader module for the MFRC522.
 *
 * This module provides initialization, card detection, UID reading, 
 * and proper termination of communication with RFID tags.
 * It encapsulates all hardware-related operations of the MFRC522 reader
 * to ensure reusability in other projects.
 */

#include "ModulMFRC522.h"

// Define the MFRC522 instance using the pin configuration from ModulMFRC522.h
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

/**
 * @brief Initializes the SPI interface and the MFRC522 RFID reader.
 *
 * This function must be called once in the setup phase before 
 * any tag detection or UID reading is performed. 
 * It configures the SPI bus and prepares the RFID module for operation.
 */
void rfidInit() {
  SPI.begin();
  rfid.PCD_Init();
  Serial.println(F("RFID reader initialized."));
}

/**
 * @brief Checks for a new RFID tag and reads its UID.
 *
 * This function verifies if a new RFID tag is present near the reader.
 * If a tag is detected, the UID bytes are copied into the provided buffer
 * and the size of the UID is returned via the pointer parameter.
 *
 * @param[out] uid  Pointer to an array where the tag UID will be stored.
 * @param[out] size Pointer to a variable that will contain the UID length in bytes.
 * 
 * @return true  if a new tag was detected and successfully read.  
 * @return false if no tag is present or reading failed.
 *
 * @note The UID is provided as a byte array and must be interpreted 
 *       according to the tag type (usually 4, 7, or 10 bytes long).
 */
bool newCardPresented(UID uid, byte* size) {
  if (!rfid.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!rfid.PICC_ReadCardSerial()) {
    return false;
  }

  // Copy UID bytes into the provided array
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid[i] = rfid.uid.uidByte[i];
  }
  *size = rfid.uid.size;

  return true;
}

/**
 * @brief Stops communication with the currently active RFID tag.
 *
 * This function halts communication with the active tag and 
 * stops any ongoing cryptographic operation in the reader.
 * It should always be called after a successful tag read 
 * to properly release the RFID module.
 */
void rfidStop() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
