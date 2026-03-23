/**
 * @file SimpleCodeUID.ino
 * @brief Simple example: Reads the UIDs of multiple RFID tags sequentially.
 *
 * This example demonstrates how to use an MFRC522 RFID reader to
 * detect cards or tags and print their unique identifiers (UIDs)
 * to the Serial Monitor.
 *
 *
 * @date 2025-09-18
 */

#include <SPI.h>      ///< SPI library for communication with the RC522
#include <MFRC522.h>  ///< MFRC522 library for controlling the RFID reader

#define SS_PIN  10    ///< Slave Select (SDA) pin for the RC522
#define RST_PIN 9     ///< Reset pin for the RC522

/**
 * @brief RFID reader object instance.
 *
 * This instance handles all communication with the RC522 - including initialization, reading tags, and stopping communication.
 */
MFRC522 rfid(SS_PIN, RST_PIN);

/**
 * @brief Arduino setup function.
 *
 * - Initializes serial communication (9600 baud), the SPI bus & the MFRC522 RFID reader.
 * - Prints a startup message to the Serial Monitor.
 */
void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for the serial connection to be ready
    }
    Serial.println("Hello World");

    SPI.begin();       ///< Start the SPI bus
    rfid.PCD_Init();   ///< Initialize the RC522 reader

    Serial.println(F("Arduino RFID: Place tags one after another..."));
}

/**
 * @brief Arduino loop function.
 *
 * Runs continuously and checks if a new RFID tag is present.
 * If a tag is detected:
 * - Reads the UID - prints it to the Serial Monitor in hexadecimal format
 * - Halts communication with the tag
 */
void loop() {
    if (!rfid.PICC_IsNewCardPresent()) return;   ///< Check if a new card is present
    if (!rfid.PICC_ReadCardSerial())   return;   ///< Read the UID

    // Print UID in hexadecimal format
    Serial.print(F("UID: "));
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) Serial.print('0');
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(' ');
    }
    Serial.println();

    rfid.PICC_HaltA();      ///< Halt communication with the tag
    rfid.PCD_StopCrypto1(); ///< Stop encryption on the PCD

    delay(500); ///< Small delay to avoid repeated reads of the same tag
}

