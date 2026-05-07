#include <Adafruit_NeoPixel.h>
// Define the pin to which the WS2812B LED is connected
#define LED_PIN 21
// We only have one LED
#define NUM_LEDS 1
// Create a NeoPixel object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
void setup() {
// Initialize the LED
strip.begin();
strip.show(); // Make sure all LEDs are off
}
void loop() {
// Set the LED to red
strip.setPixelColor(0, strip.Color(255, 0, 0)); // red
strip.show();
delay(50); // wait for one second
// Set the LED to green
strip.setPixelColor(0, strip.Color(0, 255, 0)); // green
strip.show();
delay(50); // wait for one second
// Set the LED to blue
strip.setPixelColor(0, strip.Color(0, 0, 255)); // blue
strip.show();
delay(50); // wait for one second
}