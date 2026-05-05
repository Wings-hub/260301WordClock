/**
 * @file checkCurrentVoltage.cpp
 * @brief Liest die Spannung über einen Spannungsteiler ein.
 *
 * @author Wojciech Zarzycki
 * @date 19.05.2025
 * 
 * @details
 * Der Spannungsteiler besteht aus zwei Widerständen:
 * - R1 = 62 kΩ (zwischen Batterie und analogem Pin)
 * - R2 = 100 kΩ (zwischen analogem Pin und GND)
 *
 * Die gemessene Spannung am analogen Pin (U2) entspricht:
 * U2 = Uin x (R2/(R1 + R2))
 
 *
 * Die Referenzspannung des ADC beträgt 3,3 V.
 */

#include <Arduino.h>

/// @brief Referenzspannung des ADC in Volt
float referenceVoltage = 3.3;

/// @brief Pin, an dem die Spannung gemessen wird
int voltageRead = A5;

/// @brief Widerstandswerte des Spannungsteilers (in Ohm)
const float R1 = 62000.0; ///< Widerstand R1 = 39 kΩ
const float R2 = 100000.0; ///< Widerstand R2 = 100 kΩ

/**
 * @brief Initialisierung des Pins und des seriellen Monitors
 */
void setup() {
  pinMode(voltageRead, INPUT); ///< Setzt den Pin A5 als Eingang
  Serial.begin(9600); ///< Startet die serielle Kommunikation mit 9600 Baud
}

/**
 * @brief Liest den Spannungswert am analogen Pin und gibt ihn seriell aus
 */
void loop() {
  int readValue = analogRead(voltageRead); ///< Liest den ADC-Wert (0–1023)
  
  // Umrechnung in eine Spannung in Volt
  float U2 = readValue * (referenceVoltage / 1023.0);

  // Berechnung der tatsächlichen Eingangsspannung (Akku-Spannung)
  float Uin = U2 * (R1 + R2) / R2;

  Serial.print("Gemessene Spannung (U2): ");
  Serial.print(U2); ///< Gibt die berechnete Spannung U2 aus
  Serial.print(" V, Akku-Spannung (Uin): ");
  Serial.print(Uin); ///< Gibt die berechnete Eingangsspannung Uin aus
  Serial.println(" V");

  delay(1000); ///< Wartet eine Sekunde bis zur nächsten Messung
}
