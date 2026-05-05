/**
 * @file TestRGB.ino
 *
 * @brief Arduino sketch for RGB LED WS2812B
 *
 * @author Elmar Wings
 * @version 1.0
 */

// Library for RGB LED WS2812B
#include <Adafruit_NeoPixel.h>

/**
 * @brief Define the pin to which the WS2812B LED is connected
 */
#define LED_PIN 21

/**
 * @brief Number of LEDs connected to the strip
 */
#define NUM_LEDS 1

/**
 * @brief Position of LED in the strip
 
   The strip on the board has just one LED. Therefore, the position is always 0.
 */
#define LED_POS 0


/**
 * @brief Create a NeoPixel object
 * @param num_leds Number of LEDs connected to the strip
 * @param pin Pin to which the WS2812B LED is connected
 * @param type Type of LED strip (NEO_GRB + NEO_KHZ800 for WS2812B)
 */
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

/**
 * @brief Setup function to initialize the LED strip
 */
void setup() {
  /**
   * @brief Initialize the LED strip
   */
  strip.begin();
  /**
   * @brief Make sure all LEDs are off
   */
  strip.show();
}

/**
 * @brief Main loop function to control the LED strip
 */
void loop() {
  /**
   * @brief Set the LED to red
   */
  strip.setPixelColor(LED_POS, strip.Color(255, 0, 0)); // red
  /**
   * @brief Show the updated LED color
   */
  strip.show();
  /**
   * @brief Wait for one second
   */
  delay(1000);

  /**
   * @brief Set the LED to green
   */
  strip.setPixelColor(LED_POS, strip.Color(0, 255, 0)); // green
  /**
   * @brief Show the updated LED color
   */
  strip.show();
  /**
   * @brief Wait for one second
   */
  delay(1000);

  /**
   * @brief Set the LED to blue
   */
  strip.setPixelColor(LED_POS, strip.Color(0, 0, 255)); // blue
  /**
   * @brief Show the updated LED color
   */
  strip.show();
  /**
   * @brief Wait for one second
   */
  delay(1000);
}