#include "LED.h"


void LEDInit(){
  pinMode(LED_BUILTIN, OUTPUT);

  LEDturnOff();
}


void LEDturnOn(){
  digitalWrite(LED_BUILTIN, LOW);
}

void LEDturnOff(){
  digitalWrite(LED_BUILTIN, HIGH);
}
