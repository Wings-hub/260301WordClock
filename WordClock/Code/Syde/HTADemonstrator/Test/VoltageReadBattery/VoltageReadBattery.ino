const int pinVoltage = 10; // Pin connected to the voltage divider
float batteryVoltage = 0.0; // Variable to store the battery voltage

void setup() {
Serial.begin(9600); // Start serial communication
}

void loop() {
int rawValue = analogRead(pinVoltage); // Read the analog value
batteryVoltage = (rawValue * (5.0 / 1023.0)) * 2; // Calculate battery voltage
Serial.print("Battery Voltage: ");
Serial.print(batteryVoltage);
Serial.println(" V");
delay(1000); // Wait 1 second
}