/**
 * @file powerSupplyIndicator.cpp
 * @brief Überwacht die Spannung mithilfe eines Spannungsteilers. Wenn die Spannung oberhalb 
 *        eines definierten Grenzwerts liegt, leuchtet die grüne LED, andernfalls die rote.
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

/// Referenzspannung des ADC (z.B. 3.3 V bei manchen Arduino-Boards)
float referenceVoltage = 3.3;

/** 
 * @brief Pin, an dem die Spannung gemessen wird
 */
int voltageRead = A5;

/**
 * @brief Widerstandswerte des Spannungsteilers (in Ohm)
 */
const float R1 = 62000.0; ///< Widerstand R1 = 62 kΩ
const float R2 = 100000.0; ///< Widerstand R2 = 100 kΩ

/// Grenzwert für die gemessene Spannung U2 (Spannung nach Spannungsteiler)
float noVoltageFromPowerSupply = 4.0;

/** 
 * @brief Pin der roten LED (Spannung unter Grenzwert)
 */
int ledRedPin = 5;

/** 
 * @brief Pin der grünen LED (Spannung über Grenzwert)
 */
int ledGreenPin = 4;

/**
 * @brief Setup-Funktion: Initialisiert Pins und Serielle Schnittstelle
 */
void setup() {
    Serial.begin(9600);
    pinMode(voltageRead, INPUT);
    pinMode(ledRedPin, OUTPUT);
    pinMode(ledGreenPin, OUTPUT);
}

/**
 * @brief Hauptprogramm: Liest Spannung, berechnet Eingangsspannung und steuert LEDs
 */
void loop() {
    int readValue = analogRead(voltageRead);

    /** 
     * @brief Berechnung der Spannung am Spannungsteiler-Ausgang (U2)
     */
    float U2 = readValue * (referenceVoltage / 1023.0);

    /**
     * @brief Berechnung der tatsächlichen Eingangsspannung Uin
     * basierend auf dem Spannungsteiler
     */
    float Uin = U2 * (R1 + R2) / R2;

    Serial.print("Gemessene Spannung (U2): ");
    Serial.print(U2); ///< Gibt die berechnete U2 aus
    Serial.print(" V, Eingangsspannung (Uin): ");
    Serial.print(Uin); ///< Gibt die berechnete Eingangsspannung Uin aus
    Serial.println(" V");

    if (U2 > noVoltageFromPowerSupply) { ///< Schaltet die grüne LED
        digitalWrite(ledGreenPin, HIGH);
        digitalWrite(ledRedPin, LOW);
    } else {                            ///< Schaltet die rote LED
        digitalWrite(ledGreenPin, LOW);
        digitalWrite(ledRedPin, HIGH);
    }

    delay(500);
}
