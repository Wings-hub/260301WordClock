#include "CESPNowControler.h"
#include <esp_now.h>
#include <WiFi.h>

CESPNowControler::CESPNowControler(){
  uint8_t addr[6] = {0x34, 0xcd, 0xb0, 0x19, 0xbf, 0xdc};
  memcpy(broadcastAddress, addr, 6);
}

CESPNowControler* CESPNowControler::instance = nullptr;

void CESPNowControler::OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nDelivery Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Successfully" : "Delivery Fail");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery Success :)");
  } else {
    Serial.println("Delivery Fail :(");
  }
}

void CESPNowControler::OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
  if (instance && data_len == sizeof(struct_message)) {
    memcpy(&instance->Encoder_data, data, sizeof(instance->Encoder_data));
    instance->Encoder_left = instance->Encoder_data.left;
    instance->Encoder_right = instance->Encoder_data.right;
  }
}




void CESPNowControler::setup() {
  
  WiFi.mode(WIFI_STA);
  instance = this;
  
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; 
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer");
    return ;
  }
  
  esp_now_register_recv_cb(OnDataRecv);

}
 
void CESPNowControler::sendJoystickData(int throttleENA,int throttleENB) {
  Joystick_data.ena = throttleENA;
  Joystick_data.enb = throttleENB;

  result = esp_now_send(broadcastAddress, (uint8_t *) &Joystick_data, sizeof(Joystick_data));
}

bool CESPNowControler::connectionCheck(){
  
    if (result == ESP_OK) {
    //Serial.println("Sent Successfully");
    return true;
  }
  else {
    //Serial.println("Error sending the data");
    return false;
  }
}

float CESPNowControler::getEncoderLeft(){
  return Encoder_data.left;
}

float CESPNowControler::getEncoderRight(){
  return Encoder_data.right;
}

// ---- in CESPNowControler.cpp ----
bool CESPNowControler::reconnect() {
  // Bestehende ESP-NOW-Instanz sauber neu aufsetzen
  // 1) Deinit (ignorieren, wenn bereits aus)
  esp_now_deinit();
  delay(10);

  // 2) WLAN-Stack in Station-Mode sauber starten
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_STA);
  delay(10);

  // 3) ESP-NOW neu initialisieren
  if (esp_now_init() != ESP_OK) {
    // Serial.println("ESP-NOW init failed");
    return false;
  }

  // 4) Callbacks neu registrieren
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // 5) Peer (zur Sicherheit) neu setzen
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Falls Peer schon existiert, entfernen ignorieren wir hier (idempotent genug für die meisten Fälle)
  // esp_now_del_peer(broadcastAddress);

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    // Serial.println("Add peer failed");
    return false;
  }

  // Letztes Ergebnis zurücksetzen/neutralisieren
  result = ESP_OK;
  return true;
}


