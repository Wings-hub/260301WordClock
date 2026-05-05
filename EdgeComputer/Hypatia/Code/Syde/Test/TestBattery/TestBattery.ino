/**
 * @file TestBattery.ino
 * @brief Reads the battery voltage from a voltage divider connected to an analog pin. The onboard voltage divider divide the value by 2. Therefore, there is a factor 2 inside.
 */

/**
 * @brief Pin connected to the voltage divider
 * @details This pin is used to read the analog value from the voltage divider.
 */
const int pinVoltage = 10;

/**
 * @brief Variable to store the battery voltage
 * @details This variable is used to store the calculated battery voltage. 
 */
float batteryVoltage = 0.0;

/**
 * @brief Factor of the voltage divider
 * @details The voltage divider onboard divides the voltage by 2.
 */
#define VoltageDividerFactor  2

/**
 * @brief Maximal voltage of the battery
 * @details The battery has maximal voltage which is store in this variable.
 */
#define VoltageMaximal  5.0

/**
 * @brief Setup function to initialize the serial communication
 * @details This function is called once at the beginning of the program.
 */
void setup() {
 /**
 * @brief Start serial communication at 9600 baud
 * @details This sets the serial communication speed to 9600 bits per second.
 */
 Serial.begin(9600);
}

/**
 * @brief Main loop function to read the battery voltage
 * @details This function is called repeatedly until the program is stopped.
 */
void loop() {
 /**
 * @brief Read the analog value from the pin connected to the voltage divider
 * @details This reads the analog value from the pin and stores it in the `rawValue` variable.
 */
 int rawValue = analogRead(pinVoltage);

 /**
 * @brief Calculate the battery voltage based on the analog value
 * @details This formula assumes a 5V reference voltage and a 10-bit ADC.
 * @note The formula is: (analog value * (5.0 / 1023.0)) * 2
 */
 batteryVoltage = VoltageDividerFactor * (rawValue * (VoltageMaximal / 1023.0));

 /**
 * @brief Print the battery voltage to the serial console
 * @details This prints the calculated battery voltage to the serial console.
 */
 Serial.print("Battery Voltage: ");
 Serial.print(batteryVoltage);
 Serial.println(" V");

 /**
 * @brief Wait 1 second before reading the voltage again
 * @details This waits 1 second before reading the voltage again.
 */
 delay(1000);
}