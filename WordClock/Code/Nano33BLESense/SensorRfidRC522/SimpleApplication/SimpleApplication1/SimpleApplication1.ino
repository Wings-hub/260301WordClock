/**
 * @file RFID_AccessControl_BuiltinLED.ino
 * @brief RFID Access Control with MFRC522 and builtin LED (Arduino Nano 33 BLE Sense Lite)
 *
 * - Allowed UIDs → LED stays ON for 5 seconds.
 * - Invalid UIDs → LED blinks 5 times.
 * - No tag → LED stays OFF.
 *
 * @date 2025-09-24
 * @version 1.6
 */
#include "LED.h"
#include "ModulMFRC522.h"






// Allowed UIDs
byte allowedUIDs[2][4] = {                //Array vom typ byte! hier also nur Informationen
  {0x1A, 0x4A, 0x89, 0x9A},   // UID 1
  {0x24, 0x77, 0x97, 0xA7}    // UID 2
};

void setup() {
  Serial.begin(9600);
  while (!Serial); // wartet auf serielle Verbindung mit dem Rechner - erst dann startet das Programm 

  SPI.begin();
  rfidInit();

  LEDInit(); // Initialisieren und dann ausschalten siehe LED.cpp

  

  Serial.println(F("RFID Access Control System ready..."));
}

void loop() {
  // LED always OFF when idle
    LEDturnOff();

  bool cardCheck = newCardPresented();

  if (!cardCheck) {                                                          // CARDcHECK IN DIE Klammer oder newCardPresented()
    return; // no Card - also zurück zum anfang 
  }

  // Print UID
  Serial.print(F("UID: "));
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Check UID
  if (isAllowedUID(rfid.uid.uidByte, rfid.uid.size)) {
    grantAccess();
  } else {
    denyAccess();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(500);
}

/**
 * @brief Check if a UID is in the allowed list.
 */
bool isAllowedUID(byte *uid, byte size) {
  for (byte i = 0; i < 2; i++) {
    bool match = true;
    for (byte j = 0; j < size; j++) {
      if (allowedUIDs[i][j] != uid[j]) {
        match = false;
        break;
      }
    }
    if (match) return true;
  }
  return false;
}

/**
 * @brief Access granted → LED ON for 5 seconds.
 */
void grantAccess() {
  Serial.println(F("✅ Access granted – LED ON for 5s"));
  LEDturnOn();   // ON
  delay(5000);
  LEDturnOff();  // OFF
}

/**
 * @brief Access denied → LED blinks 5 times.
 */
void denyAccess() {
  Serial.println(F("❌ Access denied – LED BLINKING 5x"));
  for (int i = 0; i < 5; i++) {
    LEDturnOn();   // ON
    delay(5000);
    LEDturnOff();  // OFF
    delay(500);
  }
}

