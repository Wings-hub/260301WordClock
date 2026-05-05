/*
   -- HX711Test --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.13 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.15.01 or later version;
     - for iOS 1.12.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLE

#include <BLEDevice.h>

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 69 bytes
  { 255,3,0,4,0,62,0,19,0,0,0,0,31,1,106,200,1,1,4,0,
  1,4,22,24,24,0,2,31,90,101,114,111,0,1,33,23,24,24,0,2,
  31,69,110,116,101,114,0,1,64,24,24,24,0,2,31,83,116,97,114,116,
  0,68,6,80,96,63,1,8,36 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t buttonZero; // =1 if button pressed, else =0
  uint8_t buttonEnter; // =1 if button pressed, else =0
  uint8_t buttonStart; // =1 if button pressed, else =0

    // output variables
  float onlineGraphHX711;

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



#include "HX711.h"
HX711 scale;

uint8_t dataPin  = 4;  //  for ESP32
uint8_t clockPin = 5;
float c = 0.0;
float delta = 0.0;
int refMass = 1000;

void setup() 
{
  RemoteXY_Init (); 
  
  
 scale.begin(dataPin, clockPin);

 scale.calibrate_scale(refMass, 5);


  
}

void loop() 
{ 


  if (RemoteXY.buttonZero == 1){
    scale.tare();
    c = scale.read();
  };

  if (RemoteXY.buttonEnter==1 ){
    scale.calibrate_scale(refMass, 5);
  };


  RemoteXY.onlineGraphHX711 = scale.get_units(1);


  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead RemoteXY_delay() 


}