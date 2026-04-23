/**
 * @file ModulMFRC522.h
 * @brief RFID reader module for the MFRC522.
 *
 * This header file provides the interface for using the MFRC522 RFID reader.
 * It defines all necessary constants, data types, and function prototypes
 * required for initializing, reading, and stopping communication with RFID tags.
 *
 * The module encapsulates the hardware-level functionality to make it 
 * reusable in other embedded applications without modifying the main program.
 */

#ifndef MODUL_MFRC522_H
#define MODUL_MFRC522_H

#include <MFRC522.h>
#include <SPI.h>

/** 
 * @brief Pin definition for Slave Select (SDA/SS) connection of the MFRC522 module.
 */
#define RFID_SS_PIN  10

/** 
 * @brief Pin definition for Reset (RST) connection of the MFRC522 module.
 */
#define RFID_RST_PIN 9

/**
 * @brief Maximum UID size in bytes supported by this implementation.
 *
 * Standard ISO/IEC 14443A MIFARE tags usually use 4, 7, or 10-byte UIDs.
 */
#define MAX_UID_SIZE 4

/**
 * @typedef UID
 * @brief Defines a byte array type representing an RFID UID.
 */
typedef byte UID[MAX_UID_SIZE];

/**
 * @brief Initializes the MFRC522 reader and the SPI communication.
 *
 * This function must be called once before performing any RFID operations.
 * It configures the SPI interface and prepares the MFRC522 reader for tag detection.
 */
void rfidInit();

/**
 * @brief Checks if a new RFID tag is present and reads its UID.
 *
 * This function verifies if a new RFID tag is within range.
 * If a tag is detected, the UID bytes are copied into the provided array.
 *
 * @param[out] uid  Pointer to a byte array where the tag UID will be stored.
 * @param[out] size Pointer to a variable that will receive the UID length in bytes.
 *
 * @return true  if a new tag was detected and successfully read.  
 * @return false if no tag was detected or reading failed.
 */
bool newCardPresented(UID uid, byte* size);

/**
 * @brief Stops communication with the currently active RFID tag.
 *
 * This function halts communication with the detected tag and stops
 * any ongoing cryptographic operation on the MFRC522 reader.
 */
void rfidStop();

#endif // MODUL_MFRC522_H
