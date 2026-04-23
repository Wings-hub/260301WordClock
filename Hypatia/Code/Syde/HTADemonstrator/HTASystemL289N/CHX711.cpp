#include "CHX711.h"
#include "HX711.h"
#include <vector>

CHX711::CHX711() {
    gewichtHTASystem = 500.0f;  

    raw = 0.0f;
    weight = 0.0f;
    previousForce = 0.0f;

    bufIndex = 0;
    proportionalTerm = 0.0f;
    derivativeTerm = 0.0f;

    referenceZero = 0.0f;
    referenceCal = 0.0f;
    calibrationFactor = 1.0f;

    startCounter = 0;
}

void CHX711::setup(uint8_t dataPin, uint8_t clockPin) {
    scale.begin(dataPin, clockPin);
}

void CHX711::tareZero() {
    scale.tare(20);                     
    referenceZero = scale.get_units();  
}

void CHX711::calibrate() {
    scale.calibrate_scale(gewichtHTASystem, 20);  
    referenceCal = scale.get_units();   

    if (abs(referenceCal) > 1e-6f) {
        calibrationFactor = gewichtHTASystem / referenceCal;
    } else {
        calibrationFactor = 1.0f;  
    }
}

float CHX711::getRaw() {
    raw = scale.get_units();
    return raw;
}

float CHX711::getFiltered(int filterDepth) {
    if (filterDepth <= 0) return getRaw(); // Fallback bei falschem Wert

    // Puffergröße anpassen, falls nötig
    if (buffer.size() != static_cast<size_t>(filterDepth)) {
        buffer.assign(filterDepth, 0.0f); // Neu initialisieren
        bufIndex = 0;
    }

    getRaw();  

    buffer[bufIndex] = raw;
    bufIndex = (bufIndex + 1) % filterDepth;

    float sumBuffer = 0.0f;
    for (float val : buffer) {
        sumBuffer += val;
    }
    return sumBuffer / filterDepth;
}

float CHX711::getWeight() {
    getRaw();
    weight = calibrationFactor * (raw - referenceZero);
    return weight;
}

float CHX711::getPid(float Kp, float Kd) {
    getWeight();  

    proportionalTerm = Kp * weight;
    derivativeTerm = Kd * (weight - previousForce);
    previousForce = weight;

    return proportionalTerm + derivativeTerm;
}
