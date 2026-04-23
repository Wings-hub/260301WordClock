


#include "CLED.h"
#include "CConfiguration.h"
#include "CHX711Sensor.h"
#include "MCP4725.h"
#include <FastLED.h>  ///< Used for CRGB LED control (FastLED library).

// Global component instances
CConfiguration Configuration;    ///< Hardware configuration instance.
CHX711Sensor Loadcell;           ///< HX711 sensor interface.
MCP4725 DAC;                     ///< DAC output handler.
CLED LED;                        ///< LED indicator interface.




void setup() {
    Serial.begin(115200);
    delay(5000);  ///< Wait to ensure serial terminal is ready

    Loadcell.setup(Configuration.getHX711DataPin(), Configuration.getHX711ClockPin());
    DAC.setup(Configuration.MCP4725DataPin(), Configuration.MCP4725ClockPin());
}

void loop() {
    raw = Loadcell.getFilterValue();             ///< Get filtered sensor value
                                        ///< Copy for processing
    Vd = Loadcell.getPIDValue();       ///< Calculate PD output

    DAC.setData();                               ///< Send result to DAC
    LED.setColor(CRGB::Green);                   ///< Set LED to green (status OK)

    // This is syntactically invalid and likely a placeholder:
    // void CDashboard::update();
    // Replace with actual call like: dashboard.update();


}
