#include "CLED.h"
#include <FastLED.h>

/**
 * 
 * @brief Constructor for CLED.
 * Initializes the FastLED driver with the predefined LED type and pin.
 */
CLED::CLED() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
}

/**
 * @brief Sets the LED to a specified RGB color.
 * @param RGBValue Desired CRGB color value.
 * @return 0 if successful.
 */
int CLED::setColor(CRGB RGBValue) {
    leds[0] = RGBValue;
    FastLED.show();
    return 0;
}

/**
 * @brief Turns the LED on (white color).
 * @return 0 if successful.
 */
int CLED::setOn() {
    leds[0] = CRGB::White;
    FastLED.show();
    return 0;
}

/**
 * @brief Turns the LED off (black).
 * @return 0 if successful.
 */
int CLED::setOff() {
    leds[0] = CRGB::Black;
    FastLED.show();
    return 0;
}
