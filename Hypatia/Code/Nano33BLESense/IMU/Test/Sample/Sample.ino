#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>


int a = 0;
// global variables used for TensorFlow Lite (Micro)



BLEService Erkennung("180F");

// BLE Battery Level Characteristic
BLEUnsignedCharCharacteristic ErkennungChar("10",                  // standard 16-bit characteristic UUID
                                            BLERead | BLENotify);  // remote clients will be able to get notifications if this characteristic changes

void setup() {

  Serial.begin(9600);  // Startet die serielle Kommunikation mit 9600 Baud

  // initialize the IMU
  if (!IMU.begin()) {
    while (1)
      ;
  }
  // begin initialization
  if (!BLE.begin()) {
    while (1)
      ;
  }

  BLE.setLocalName("Erkennung");
  BLE.setAdvertisedService(Erkennung);         // add the service UUID
  Erkennung.addCharacteristic(ErkennungChar);  // add the battery level characteristic
  BLE.addService(Erkennung);                   // Add the battery service
  ErkennungChar.writeValue(0);

  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();
}

void loop() {
  float aX, aY, aZ;

  // wait for a BLE central
  BLEDevice central = BLE.central();

  if (IMU.accelerationAvailable()) {
    // read the acceleration and gyroscope data
    IMU.readAcceleration(aX, aY, aZ);

    Serial.print("A_x = ");
    Serial.print(aX);
    Serial.print(", A_y = ");
    Serial.print(aY);
    Serial.print(", A_z = ");
    Serial.println(aZ);
  }

  // if a central is connected to the peripheral:
//  if (central) {

    // while the central is connected:

  //  while (central.connected() == 1) {
    //  delay(32);
    //  ErkennungChar.writeValue(a);
    //  a = 0;
    //}
  //}

  delay(2000);
}
