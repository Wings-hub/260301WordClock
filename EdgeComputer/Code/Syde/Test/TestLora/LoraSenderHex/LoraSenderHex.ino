//P2P Hypatia to Gateway Up and Downlink works
#include <Arduino.h>
#include <HardwareSerial.h>

#define LORA_UART_NUM     2
#define LORA_RX_PIN       13
#define LORA_TX_PIN       14
#define LORA_BAUD_RATE    9600

HardwareSerial LoRaSerial(LORA_UART_NUM);

void sendCommand(const char* cmd);
void readDebugResponse();
void handleLoRaToSerial();
void handleSerialToLoRa();

void setup() {
  Serial.begin(115200);
  delay(5000);
  while (!Serial);
  Serial.println("[BOOT] ESP32 LoRa Gateway Initializing...");

  LoRaSerial.begin(LORA_BAUD_RATE, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);
  Serial.println("[INFO] LoRa UART initialized.");

  // Soft reset
  sendCommand("ATZ\r");
  delay(1000);
  readDebugResponse();

  // Set LoRa mode
  sendCommand("AT+MODE=1\r");
  delay(500);
  readDebugResponse();

  // Set frequency band to EU868 (Band ID 5)
  sendCommand("AT+BAND=5\r");
  delay(500);
  readDebugResponse();

  // Basic ping
  sendCommand("AT\r");
  delay(500);
  readDebugResponse();

  // Look for Gateway
  sendCommand("AT+JOIN=1\r");
  delay(500);
  readDebugResponse();

  Serial.println("[READY] LSM100A configured for EU868. Enter AT commands below.");
}

void loop() {
  handleLoRaToSerial();
  handleSerialToLoRa();
}

void sendCommand(const char* cmd) {
  Serial.print("[SEND] ");
  Serial.println(cmd);
  for (int i = 0; i < strlen(cmd); i++) {
    LoRaSerial.write(cmd[i]);
    Serial.print(" 0x");
    Serial.print(cmd[i], HEX);
  }
  Serial.println();
}

void readDebugResponse() {
  unsigned long start = millis();
  while (millis() - start < 1000) {
    if (LoRaSerial.available()) {
      uint8_t c = LoRaSerial.read();
      //Serial.print("[RECV] 0x");
      //Serial.print(c, HEX);
      //Serial.print(" ('");
      Serial.print((char)c);
      //Serial.print("')");
    }
  }
}

void handleLoRaToSerial() {
  while (LoRaSerial.available()) {
    char c = LoRaSerial.read();
    Serial.print(c);
  }
}

void handleSerialToLoRa() {
  while (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (!cmd.isEmpty()) {
      // Wenn es ein AT-Befehl ist, sende ihn direkt weiter
      if (cmd.startsWith("AT")) {
        Serial.print("[ESP32 > LoRa] Sending: ");
        Serial.println(cmd);
        LoRaSerial.print(cmd);
        LoRaSerial.write('\r');
      } else {
        // Text in HEX umwandeln
        String hexPayload = "";
        for (size_t i = 0; i < cmd.length(); i++) {
          char hex[3];
          sprintf(hex, "%02X", (uint8_t)cmd[i]);
          hexPayload += hex;
        }

        // AT+SEND-Befehl erzeugen
        String atCommand = "AT+SEND=1:0:" + hexPayload;
        Serial.print("[ESP32 > LoRa] Converted to: ");
        Serial.println(atCommand);
        LoRaSerial.print(atCommand);
        LoRaSerial.write('\r');
      }
    }
  }
}



