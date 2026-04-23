#ifndef CHX711_H
#define CHX711_H

#include <Arduino.h>
#include "HX711.h"
#include <vector>

class CHX711 {
public:
    CHX711();
    void setup(uint8_t dataPin, uint8_t clockPin);
    void tareZero();
    void calibrate();

    float getRaw();
    float getFiltered(int filterDepth);
    float getWeight();
    float getPid(float Kp, float Kd);

    float gewichtHTASystem;  

private:
    HX711 scale;

    float raw;
    float weight;
    float previousForce;

    std::vector<float> buffer; // dynamischer Ringpuffer
    int bufIndex;

    float proportionalTerm;
    float derivativeTerm;

    float referenceZero;
    float referenceCal;
    float calibrationFactor;

    unsigned long startCounter;
};

#endif // CHX711_H
