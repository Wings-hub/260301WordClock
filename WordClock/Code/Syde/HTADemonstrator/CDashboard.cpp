#include "CDashboard.h"
#include "CHX711.h"
#include "CEncoder.h"

#define REMOTEXY_MODE__WIFI_POINT

#include <WiFi.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377
#include <RemoteXY.h>




#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 742 bytes
  { 255,12,0,82,0,223,2,19,0,0,0,0,31,2,106,200,200,84,2,1,
  0,13,0,130,0,5,21,95,244,236,216,114,27,186,129,27,152,56,8,5,
  76,35,5,64,176,69,114,114,111,114,32,77,97,115,115,97,103,101,58,32,
  0,131,90,55,21,33,172,74,26,8,5,175,164,186,83,101,116,117,112,0,
  6,129,227,188,84,12,3,2,37,6,64,176,72,88,55,49,49,32,70,111,
  114,99,101,58,0,129,243,10,40,14,103,2,46,6,64,176,69,110,99,111,
  100,101,114,32,86,101,108,111,99,105,116,121,58,0,68,2,28,51,129,102,
  9,96,54,50,173,36,135,108,101,102,116,0,114,105,103,104,116,0,68,6,
  42,96,101,2,9,96,54,51,173,16,135,36,114,97,119,0,102,105,108,116,
  101,114,101,100,0,80,68,0,2,8,153,23,52,2,65,23,8,0,176,173,
  31,31,82,65,87,0,79,70,70,0,2,9,160,11,19,27,65,23,8,0,
  176,173,31,31,70,105,108,116,101,114,0,79,70,70,0,2,10,164,11,19,
  52,65,23,8,0,176,173,31,31,80,68,0,79,70,70,0,2,20,160,12,
  19,102,65,23,8,0,176,173,31,31,76,101,102,116,0,79,70,70,0,2,
  33,160,12,19,127,65,23,8,0,176,173,31,31,82,105,103,104,116,0,79,
  70,70,0,67,4,62,38,33,41,75,128,7,100,177,173,11,23,0,130,251,
  213,114,255,253,252,216,114,27,186,130,58,105,48,95,107,12,90,17,11,173,
  130,54,64,21,95,107,42,90,39,11,173,129,226,10,85,14,5,4,50,6,
  64,176,72,88,55,49,49,32,67,97,108,105,98,114,97,116,105,111,110,58,
  0,129,214,14,117,14,99,5,42,6,64,176,70,105,108,116,101,114,32,86,
  97,114,105,97,98,108,101,115,58,0,129,17,14,96,14,111,18,27,5,64,
  176,70,105,108,116,101,114,32,68,101,112,116,104,58,0,4,61,74,7,86,
  142,16,35,8,128,177,26,67,68,74,21,24,177,16,14,10,78,177,173,2,
  129,37,38,65,12,111,49,21,5,64,176,75,112,32,86,97,108,117,101,58,
  0,4,78,40,19,6,142,47,35,8,128,177,26,67,97,33,7,24,177,46,
  14,10,78,177,173,2,129,17,14,96,14,100,36,36,6,64,176,80,68,32,
  86,97,114,105,97,98,108,101,115,58,0,129,46,102,50,12,111,60,21,5,
  64,176,75,100,32,86,97,108,117,101,58,0,4,78,104,19,6,142,58,35,
  8,128,177,26,67,97,98,7,24,177,57,14,10,78,177,173,2,129,47,107,
  50,12,111,70,24,5,64,176,68,101,97,100,98,97,110,100,58,0,4,79,
  109,19,6,142,69,35,8,128,177,26,67,98,102,7,24,177,68,14,10,78,
  177,173,2,131,92,183,14,19,5,72,29,9,5,175,164,186,66,97,99,107,
  0,9,2,81,160,12,19,5,13,29,9,0,176,173,31,31,79,70,70,0,
  83,116,97,114,116,0,2,4,45,15,21,5,40,72,9,0,176,173,31,31,
  79,70,70,0,83,116,97,110,100,105,110,103,32,80,111,115,46,0,2,5,
  50,15,21,5,52,72,9,0,176,173,16,31,79,70,70,0,72,97,110,103,
  105,110,103,32,80,111,115,46,0,67,95,43,7,24,5,24,72,14,100,177,
  173,35 };
  
struct {
    // input variables
  uint8_t switchRaw; // =1 if switch ON and =0 if OFF
  uint8_t switchFilter; // =1 if switch ON and =0 if OFF
  uint8_t switchPD; // =1 if switch ON and =0 if OFF
  uint8_t switchLeft; // =1 if switch ON and =0 if OFF
  uint8_t switchRight; // =1 if switch ON and =0 if OFF
  int8_t sliderFilter; // from 0 to 100
  int8_t sliderKp; // from 0 to 100
  int8_t sliderKd; // from 0 to 100
  int8_t sliderDeadband; // from 0 to 100
  uint8_t switchStartCal; // =1 if switch ON and =0 if OFF
  uint8_t switch_Standing; // =1 if switch ON and =0 if OFF
  uint8_t switch_Hanging; // =1 if switch ON and =0 if OFF

    // output variables
  float onlineGraphLeftVel;
  float onlineGraphRightVel;
  float onlineGraphRaw;
  float onlineGraphFilter;
  float onlineGraphPD;
  char textError[11]; // string UTF8 end zero
  float valueFilter;
  float valueKp;
  float valueKd;
  float valueDeadband;
  char textCalibration[35]; // string UTF8 end zero

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)

CDashboard::CDashboard()
  : Loadcell(HX711dataPin, HX711clockPin),
    leftMotorEncoder(pinLeftEncoderA, pinLeftEncoderB),
    rightMotorEncoder(pinRightEncoderA, pinRightEncoderB),
    DAC(DACdataPin, DACclockPin, DACInputPin)
{
  // Pin-Definitionen
  HX711dataPin = 4;
  HX711clockPin = 5;

  pinLeftEncoderA  = 3;
  pinLeftEncoderB  = 4;
  pinRightEncoderA = 5;
  pinRightEncoderB = 6;

  DACdataPin = 41;
  DACclockPin = 42;
  DACInputPin = 8;

  start = "start: put Demonstrator on wheels";
  tared = "tared: hold Demonstrator on screw";
  doneCal = "Calibration done";

  counter = 0;
}




void CDashboard::setup() 
{
  RemoteXY_Init ();
  leftMotorEncoder.setup();            // Initialize left encoder
  rightMotorEncoder.setup();           // Initialize right encoder
  DAC.setupAin(); // optional if read of analog Values wanted 
}

void CDashboard::update(){ 
  RemoteXY_Handler ();

  if (switchRawOn()){
    RemoteXY.onlineGraphRaw = Loadcell.getRaw();
  }

  if (switchFilterOn()){
    RemoteXY.onlineGraphFilter = Loadcell.getFilter();
  }

  if (switchPDOn()){
    int Kp = 0;
    int Kd = 0;
    RemoteXY.onlineGraphPD = Loadcell.getPid(Kp, Kd);
  }
  
  if (switchLeftOn()){
    RemoteXY.onlineGraphLeftVel = leftEncoder.getLinearVelocity();
  }

  if (switchRightOn()){
    RemoteXY.onlineGraphRightVel = rightEncoder.getLinearVelocity();
  }

  calibration();
}

bool CDashboard::switchRawOn(){
  return switchRaw;
}

bool CDashboard::switchFilterOn(){
  return switchFilter;
} 

bool CDashboard::switchPDOn(){
  return switchPD;
} 

bool CDashboard::switchLeftOn(){
  return switchPD;
} 

bool CDashboard::switchRightOn(){
  return switchRight;
} 

bool CDashboard::switchStartCalOn(){
  return switchStartCal;
} 

void CDashboard::setSwitchStartCalOff(){
  switchStartCal = 0;
}

bool CDashboard::switch_StandingOn(){
  return switch_Standing;
} 

void CDashboard::setSwitchStandinglOff(){
  switch_Standing = 0;
}

bool CDashboard::switch_HangingOn(){
  return switch_Hanging;
} 

void CDashboard::setSwitchHangingOff(){
  switch_Hanging = 0;
}

void CDashboard::setTextCalibration(String text){
  clearTextCalibration();
  text.toCharArray(textCalibration, text);
}

void CDashboard::clearTextCalibration(){
  memset(textCalibration, 0, sizeof(textCalibration)); //clear Array for new text
}

void CDashboard::setTextError(String text){
  clearTextError();
  text.toCharArray(textError, text);
}

void CDashboard::clearTextError(){
  memset(textError, 0, sizeof(textError)); //clear Array for new text
}
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead RemoteXY_delay() 


void CDashboard::calibration (){
  if (switchStartCalOn()){
      startCounter = millis();
      setTextCalibration(start); 

      while (switchStartCalOn()){
        counter = millis();
        if (switch_StandingOn()){
          startCounter = millis();
          Loadcell.tareZero();
          setSwitchStandingOff();
          setTextCalibration(tared); 
        }
        else if (switch_HangingOn()){
          startCounter = millis();
          Loadcell.calibrate();
          setSwitchHangingOff();
          setTextCalibration(doneCal); 
        }

        if (counter >= startCounter + 25000){
          break;
        }
      }
      setSwitchStartCalOff();
  }
}



