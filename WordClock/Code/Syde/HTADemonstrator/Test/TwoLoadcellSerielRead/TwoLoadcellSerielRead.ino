#include "HX711.h"

HX711 myScale1;
HX711 myScale2;

uint8_t dataPin1  = 5;  //  for ESP32
uint8_t clockPin1 = 4;  //  for ESP32
uint8_t dataPin2  = 7;  //  for ESP32
uint8_t clockPin2 = 6;  //  for ESP32

float weight1;
float weight2;

void setup() {
 Serial.begin(115200);
    delay(6000);

  myScale1.begin(dataPin1, clockPin1);
  myScale2.begin(dataPin2, clockPin2);
  
    Serial.println("\nEmpty the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();

  myScale1.tare(20);
  myScale2.tare(20);

   Serial.println("\nPut 1000 gram in the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();

  float calibrationFactor1 = myScale1.get_units(10)/1000;
  float calibrationFactor2 = myScale1.get_units(10)/1000;
  myScale1.set_scale(calibrationFactor1);
  myScale2.set_scale(calibrationFactor2);


}

void loop() {
  weight1 = myScale1.get_units(10);
  weight2 = myScale2.get_units(10);

  Serial.print("Measured Weight 1:");
  Serial.print(weight1);
  Serial.print("Measured Weight 2:");
  Serial.println(weight2);
}
