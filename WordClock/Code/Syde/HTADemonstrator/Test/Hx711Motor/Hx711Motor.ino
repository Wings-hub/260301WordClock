/*  Arduino DC Motor Control - PWM | H-Bridge | L298N  -  Example 01

    by Dejan Nedelkovski, www.HowToMechatronics.com
*/
#include "HX711.h"
#define enA 7
#define in1 15
#define in2 16

//Loadcell
uint8_t dataPin = 5;
uint8_t clockPin = 4;

HX711 scale;

int rotDirection = 0;


void setup() {
  Serial.begin(115200);
  delay(5000);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  scale.begin(dataPin, clockPin);
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nEmpty the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();

  scale.tare();
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nPut 1000 gram in the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();

  scale.calibrate_scale(1000, 5);
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nScale is calibrated, press a key to continue");
  //  Serial.println(scale.get_scale());
  //  Serial.println(scale.get_offset());
  while(!Serial.available());
  while(Serial.available()) Serial.read();
  
  Serial.print("start");
}

void loop() {
  
  int force = scale.get_units(10); // Read HX711 value
  int pwmOutput = map(force, -1000, 1000, 0 , 255); // Map the force value from 0 to 255
  analogWrite(enA, pwmOutput); // Send PWM signal to L298N Enable pin

  // Read button - Debounce
  Serial.print("force:");
  // If button is pressed - change rotation direction
  if (force < 0  && rotDirection == 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    rotDirection = 1;
  }
  // If button is pressed - change rotation direction
  if (force >= 0  && rotDirection == 1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    rotDirection = 0;
  }
}
