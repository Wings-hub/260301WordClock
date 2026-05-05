
#ifndef CESPNOWEXTFORCE_H
#define CESPNOWEXTFORCE_H

#include <esp_now.h>
#include <WiFi.h>

    typedef struct struct_message {
      float left;
      float right;
      int ena;
      int enb;
    } struct_message;

class CESPNowExtForce {
  public:

    CESPNowExtForce();
    static void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status);
    static void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len);
    void setup();
    void sendEncoderData(float leftEncoderVel, float rightEncoderVel);
    int getPWMEna();
    int getPWMEnb();
  private:
    static CESPNowExtForce* instance;
    esp_now_peer_info_t peerInfo;

    int Joystick_ena;
    int Joystick_enb;

    uint8_t broadcastAddress[6];

    struct_message Encoder_data;
    struct_message Joystick_data;

    String success;


};
#endif
