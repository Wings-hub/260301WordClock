#ifndef MODULMFRC522_HH // defining bzw. definieren und sicherstellen dass nur einmal kompiliert ist
#define MODULMFRC522_HH

#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN  10
#define RST_PIN 9




extern MFRC522 rfid; // Objekt von einer Klasse definiert wo anders ...? Objektorientiertes programmieren


void rfidInit();


bool newCardPresented();

#endif