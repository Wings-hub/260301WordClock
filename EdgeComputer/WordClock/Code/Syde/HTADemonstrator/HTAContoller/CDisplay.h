#ifndef CDISPLAY_H
#define CDISPLAY_H

#include <Adafruit_SSD1306.h>

// Vorwärtsdeklaration – damit der Compiler den Typ kennt
class CESPNowControler;

class CDisplay {
  public:
    CDisplay();
    void setup(CESPNowControler* ptr);
    void showSetupScreen();
    void showStatusScreen(float encoderLeft, float encoderRight, bool connection);

  private:
    Adafruit_SSD1306 display;
    CESPNowControler* controller;
    int now;
    int startTime;
    int startDisplayDelay;
};

#endif
