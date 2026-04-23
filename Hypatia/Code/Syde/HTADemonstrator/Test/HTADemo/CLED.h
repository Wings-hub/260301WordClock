#ifndef CLED_H
#define CLED_H

#include <FastLED.h>  ///< For CRGB and FastLED control

#define LED_PIN 21      ///< Default GPIO pin for the status LED. //Onboard LED fest definiert
#define NUM_LEDS 1      ///< Number of addressable LEDs (WS2812B etc.)

/**
 * @class CLED
 * @brief Class for controlling a status LED using the FastLED library.
 */
class CLED {
public:
    /**
     * @brief Default constructor. Initializes internal LED buffer.
     */
    CLED();

    /**
     * @brief Sets the LED to a specific color.
     * @param RGBValue Desired color as CRGB object.
     * @return 0 if successful.
     */
    int setColor(CRGB RGBValue);

    /**
     * @brief Turns the LED on with white color.
     * @return 0 if successful.
     */
    int setOn();

    /**
     * @brief Turns the LED off (black).
     * @return 0 if successful.
     */
    int setOff();

private:
    CRGB leds[NUM_LEDS];  ///< LED buffer for FastLED.
};

#endif
