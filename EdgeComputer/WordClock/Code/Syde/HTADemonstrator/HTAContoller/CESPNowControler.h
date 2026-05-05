#ifndef CESPNOWCONTROLER_H
#define CESPNOWCONTROLER_H

#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  float left;
  float right;
  int ena;
  int enb;
} struct_message;

class CESPNowControler{
  public:
    CESPNowControler();
    static void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status);
    static void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len);
    void setup();
    bool connectionCheck();
    void sendJoystickData(int throttleENA,int throttleENB);
    float getEncoderLeft();
    float getEncoderRight();
    bool reconnect();

  private:
    static CESPNowControler* instance;
    uint8_t broadcastAddress[6];
    float Encoder_left;
    float Encoder_right;

    struct_message Encoder_data;
    struct_message Joystick_data;
    esp_now_peer_info_t peerInfo;
    esp_err_t result;
};
#endif
