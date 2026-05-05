#include "SigfoxModule.h"

SigfoxModule::SigfoxModule(HardwareSerial &uart){
  _uart = &uart;
  _uart->begin(9600, SERIAL_8N1, SIGFOX_RX, SIGFOX_TX);
  setSigFoxMode();
}

void SigfoxModule::setSigFoxMode() {
  Serial.println("Set SigFox Mode LMS100A");
  sendCommand("AT+MODE=0\r");
  }

String SigfoxModule::getID() {
  String id = sendCommand("AT$ID\r");
  while (id.indexOf("ERROR") >= 0) {
    Serial.println("Error, retrying...");
    id = sendCommand("AT$ID\r");
    }
  id.trim(); 
  Serial.println("SIGFOX ID: " + id);
  return id;
}

String SigfoxModule::sendCommand(String cmd) {
  _uart->print(cmd);
  delay(1000);

  String response = "";
  unsigned long startMillis = millis();
  while (!_uart->available() && millis() - startMillis < 5000) {
    delay(100);
  }

  while (_uart->available()) {
    response += (char)_uart->read();
  }

  Serial.println("Received response: " + response);//Debug

  return response;
}

void SigfoxModule::sendMessage(String message) {
  Serial.println("Sending SIGFOX message: " + message);
  String hexMessage = stringToHex(message);
  sendCommand("AT$SF=" + hexMessage + "\r");
}

String SigfoxModule::stringToHex(String input) {
  String hexString = "";
  for (unsigned int i = 0; i < input.length(); i++) {
    hexString += String(input[i], HEX);
  }
  return hexString;
}
void SigfoxModule::resetSigfox() {
  Serial.println("Resetting Sigfox module...");
  sendCommand("AT$RESET\r");
  delay(5000);
}


