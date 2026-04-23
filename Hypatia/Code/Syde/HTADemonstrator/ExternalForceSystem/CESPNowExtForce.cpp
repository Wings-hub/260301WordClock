#include <esp_now.h>
#include <WiFi.h>
#include "CESPNowExtForce.h"

CESPNowExtForce::CESPNowExtForce(){
uint8_t addr[] = {0x34, 0xcd, 0xb0, 0x19, 0xbf, 0x84};
  memcpy(broadcastAddress, addr, 6);
}

CESPNowExtForce* CESPNowExtForce::instance = nullptr;

//Objekt in Ino erstellen -> float daten vom Encoder übergeben

void CESPNowExtForce::OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nDelivery Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivered Successfully" : "Delivery Fail");
}

void CESPNowExtForce::OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&instance->Joystick_data, data, sizeof(struct_message));
  instance->Joystick_ena = instance->Joystick_data.ena;
  instance->Joystick_enb = instance->Joystick_data.enb;
}
 
void CESPNowExtForce::setup() {
  instance = this;
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    Serial.print("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; 
  peerInfo.encrypt = false;
       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.print("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}
 
void CESPNowExtForce::sendEncoderData(float leftEncoderVel, float rightEncoderVel) {
  Encoder_data.left = leftEncoderVel; /*Get the Temperature value*/
  Encoder_data.right = rightEncoderVel;       /*Get the Humidity value*/

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &Encoder_data, sizeof(Encoder_data));
   
  if (result == ESP_OK) {
    Serial.println("Sent Successfullt");
  }
  else {
    Serial.println("Getiing Error while sending the data");
  }
}

int CESPNowExtForce::getPWMEna() {
  return Joystick_data.ena;
}

int CESPNowExtForce::getPWMEnb() {
  return Joystick_data.enb;
}