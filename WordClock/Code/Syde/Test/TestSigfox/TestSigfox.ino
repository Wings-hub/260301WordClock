#include <Arduino.h>
#include <SigfoxModule.h>

#include "SigfoxModule.h"

#define RXD2 13  //  RX-Pin
#define TXD2 14  //  TX-Pin
#define SIGFOX_PWR 6


// Builds Sigfox-Object with UART2
SigfoxModule sigfox(Serial2);

void setup() {
    Serial2.begin(9600);
    digitalWrite(SIGFOX_PWR, HIGH);  // Turn on the module
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    delay(6000);

    Serial.println("Starting Sigfox Test...");
    
    // Initialisise the Sigfox-Modul
    sigfox.setSigFoxMode();

    // gets the Sigfox ID
    String sigfoxID = sigfox.getID();
    Serial.println("Device ID: " + sigfoxID);
}

void loop() {
    // Example to send a Message
    sigfox.sendMessage("HELLO1234");
    delay(10000);
}

