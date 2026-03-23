/**
 * @file ModulMFRC522.h
 * @brief Interface definitions for the MFRC522 RFID module.
 *
 * This header defines all constants, includes, and function prototypes
 * required to interface with the MFRC522 RFID reader.
 * The functions allow initialization, tag detection, access number reading,
 * conversion to human-readable strings, and proper termination.
 */

#ifndef MODUL_MFRC522_H
#define MODUL_MFRC522_H

#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>

// ----------------------------------------------------
// Configuration
// ----------------------------------------------------

/** @brief Slave Select (SS) pin for MFRC522 module. */
#define RFID_SS_PIN 10

/** @brief Reset (RST) pin for MFRC522 module. */
#define RFID_RST_PIN 9

/** @brief Maximum number of bytes an access number (UID) can have. */
#define MAX_UID_SIZE 10

// ----------------------------------------------------
// Function prototypes
// ----------------------------------------------------

/**
 * @brief Initializes the MFRC522 module and SPI communication.
 */
void rfidInit();

/**
 * @brief Detects a new RFID tag and reads its access number.
 * @param[out] accessNumber Pointer to array to store read bytes.
 * @param[out] size Pointer to variable that stores number of bytes read.
 * @return true if a new tag was detected and successfully read.
 */
bool newCardPresented(byte *accessNumber, byte *size);

/**
 * @brief Converts the access number bytes into a readable HEX string.
 * @param[in] accessNumber Pointer to byte array containing the access number.
 * @param[in] size Number of bytes that make up the access number.
 * @return String containing the access number in uppercase HEX.
 */
String accessNumber2HEXString(byte *accessNumber, byte size);

/**
 * @brief Properly stops communication with the current RFID tag.
 */
void rfidStop();

#endif
