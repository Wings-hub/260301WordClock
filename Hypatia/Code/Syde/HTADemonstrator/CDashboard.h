#ifndef CDASHBOARD_H
#define CDASHBOARD_H

#include <Arduino.h>
#include "CHX711.h"
#include "CEncoder.h"
#include "CMCP4725.h"


class CDashboard {
  public: 
  CDashboard();
  void setup();
  void update();

  bool switchRawOn();
  bool switchFilterOn();
  bool switchPDOn();
  bool switchLeftOn();
  bool switchRightOn();
  bool switchStartCalOn();
  void setSwitchStartCalOff();
  bool switch_StandingOn();
  void setSwitchStandinglOff();
  bool switch_HangingOn();
  void setSwitchHangingOff();
  void setTextCalibration(String text);
  void clearTextCalibration(String text);
  void setTextError();
  void clearTextError();
  void calibration();

  private:
    CHX711 Loadcell;
    CEncoder leftMotorEncoder;
    CEncoder rightMotorEncoder;
    CMCP4725 DAC;

    unsigned long startCounter;

    // RemoteXY-Switches
    uint8_t switchRaw;
    uint8_t switchFilter;
    uint8_t switchPD;
    uint8_t switchLeft;
    uint8_t switchRight;
    uint8_t switchStartCal;
    uint8_t switch_Standing;
    uint8_t switch_Hanging;

    char textError[11];
    char textCalibration[35];
    // Pins for HX711
    uint8_t HX711dataPin;
    uint8_t HX711clockPin;

    // Pins for encoder
    uint8_t pinLeftEncoderA;
    uint8_t pinLeftEncoderB;
    uint8_t pinRightEncoderA;
    uint8_t pinRightEncoderB;

    // I2C Pins for DAC
    uint8_t DACdataPin;
    uint8_t DACclockPin;

    // DAC Input Feedback (optional)
    uint8_t DACInputPin;

    String start; //33 char
    String tared ; //33 char
    String doneCal; //16 char

    int counter; //counting time while in calibration mode

};
#endif



