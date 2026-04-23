#include <Arduino.h>

#ifndef SIGFOX_MODULE_H
#define SIGFOX_MODULE_H

class SigfoxModule {
    private:
      HardwareSerial *_uart; // UART for Sigfox
      //Define the Sigfox Pinout
      int SIGFOX_TX = 14;  // USART2_TX
      int SIGFOX_RX = 13;  // USART2_RX
      int SIGFOX_RESET = 30;  // NRST (Reset Pin)
      int SIGFOX_POWER = 6;  // Power control pin (if applicable)

   

      //Define Variables for Sigfox ID and Pack
      String _sigfox_id = "";
      String _sigfox_pack = "";
    
    public:
      SigfoxModule(HardwareSerial &uart);
      String getID();
      void setSigFoxMode();
      String sendCommand(String cmd);
      void sendMessage(String message);
      String stringToHex(String input);
      void resetSigfox();
};  

#endif
