#include "ModulMFRC522.h"



MFRC522 rfid(SS_PIN, RST_PIN);


void rfidInit(){     // Funktion - gibt nichts zurück - ruft aber rfid Init aus ModulMFRC522.h raus

  rfid.PCD_Init();

}

bool newCardPresented(){

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return false;      // false bedeutet keine neue Karte oder nicht gelesen
  } else { 
      return true;     // true - falls Karte gelesen oder erkannt dann true zurückgeben und weiter springen - weil wir zurückbekommen muss statt void - bool!
    
    } 
  
}
 
