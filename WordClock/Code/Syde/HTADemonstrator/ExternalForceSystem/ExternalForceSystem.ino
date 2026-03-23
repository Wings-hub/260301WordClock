/**wroom
 * @file BLEServer.ino
 * @brief Main application for ESP32 acting as BLE server.
 *
 * This sketch runs on the motor side and:
 *  - Advertises as a BLE server ("DeviceA")
 *  - Sends encoder velocity to a connected client (via notify)
 *  - Receives motor control commands from the client (via write)
 *
 * Author: [Your Name]
 * Date: [YYYY-MM-DD]
 */

#include "CESPNowExtForce.h"
#include "CL298N.h"
#include "CEncoder.h"
//#include "CJoystick.h"

// === BLE Server ===
//CBLEServerModule bleServer("DeviceA");  // Create server instance with advertised name
CEncoder encoderLeft(...);
CEncoder encoderRight(...);
CESPNowExtForce ESPNowExtForce;

// === Joystick Pins ===
//uint8_t pinThrottle  = 16; // Throttle (vertical potentiometer)

uint8_t pinJoystickX = 17; // X-axis: Left/Right
uint8_t pinJoystickY = 18; // Y-axis: Forward/Backward

// Create joystick instances:
// One for throttle (only throttle pin used, X and Y set to dummy 0)
// One for direction (only X and Y used, throttle pin set to dummy 0)
//CJoystick JoystickLeft;
//CJoystick JoystickRight;

//int leftRight = 0;
//int forwardBackward = 0;
//float vel = 0;


//motor Values
int pwmLeft = 0;
int pwmRight = 0;

// === Encoder Pins ===
uint8_t pinLeftEncoderA  = 4;
uint8_t pinLeftEncoderB  = 5;
uint8_t pinRightEncoderA = 8;
uint8_t pinRightEncoderB = 3;

// === Encoder Instances ===
CEncoder leftMotorEncoder(pinLeftEncoderA, pinLeftEncoderB);
CEncoder rightMotorEncoder(pinRightEncoderA, pinRightEncoderB);

// === Runtime Variables ===
float leftEncoderVel = 0.0f;            // Left wheel velocity
float rightEncoderVel = 0.0f;           // Right wheel velocity

// === Motor Driver ===
CL298N motorLeft;
CL298N motorRight;

int throttleENA = 0;
int throttleENB = 0;            // PWM values for left/right motor

//MotorControl lastReceivedPWM;           // Motor command received from client

//Led setup
int LED_pin = 18;

void setup() {                         ////////////////////////////////////////////////////////////////////////////////  setup  ////////////////
  Serial.begin(115200);                // Initialize serial communication
  delay(6000);                         // short delay to start the serial Monitor propertly 
  Serial.println("setup");
  testMotorSetup();
  testESPNowSetup();
  testEncoderSetup();
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, HIGH);  // turn the LED on when voltage on
}

void loop() {                          ////////////////////////////////////////////////////////////////////////////////  loop  /////////////////
  //testJoystickLoop();
  //Serial.println("test");
  //delay(500);
  testEspNowLoop();
  //testEncoderLoop();

}
/*
void ExternalForceSetup(){
  //bleServer.begin();                   // Start BLE server and begin advertising

  //leftMotorEncoder.setup();            // Initialize left encoder
  //rightMotorEncoder.setup();           // Initialize right encoder

  motorDriver.setup();                 // Initialize motor driver (L298N + PWM)

}
*/

void testESPNowSetup(){
  ESPNowExtForce.setup();                   // Start BLE server and begin advertising
}

void testEncoderSetup(){
  leftMotorEncoder.setup();            // Initialize left encoder
  rightMotorEncoder.setup();           // Initialize right encoder
}

void testMotorSetup(){
  motorLeft.setupLeft();                 // Initialize motor driver (L298N + PWM)
  motorRight.setupRight(); 
}


/*
void ExternalForceLoop(){
  // === Read and send encoder velocities ===
  //leftEncoderVel  = leftMotorEncoder.getTestValue();   // Read left encoder velocity
  //rightEncoderVel = rightMotorEncoder.getTestValue();  // Read right encoder velocity

  // Transmit velocity values to BLE client (notify)
  //bleServer.updateVelocity(leftEncoderVel, rightEncoderVel);

  // === Receive motor commands from BLE client ===
  //lastReceivedPWM = bleServer.getMotorControl();  // Fetch last written PWM values from BLE
    
  //motorLeft = lastReceivedPWM.pwmLeft;        // Update left PWM value
  //motorRight = lastReceivedPWM.pwmRight;       // Update right PWM value

  // === Apply PWM values to motors ===
  motorDriver.setMotorValueLeft(motorLeft);         // Drive motors with received values
  motorDriver.setMotorValueRight(motorRight);         // Drive motors with received values

  /*
  // Optional debug output
  Serial.printf("[Server] Sent Velocities: [%.2f, %.2f] | Received PWM: [%d, %d]\n",
    leftEncoderVel, rightEncoderVel,
    lastReceivedPWM.pwmLeft, lastReceivedPWM.pwmRight);
  
}
*/


void testEspNowLoop(){
  leftEncoderVel  = leftMotorEncoder.getLinearVelocity();   // Read left encoder velocity
  rightEncoderVel = rightMotorEncoder.getLinearVelocity();  // Read right encoder velocity

  ESPNowExtForce.sendEncoderData(leftEncoderVel, rightEncoderVel);
  throttleENA = ESPNowExtForce.getPWMEna();
  throttleENB = ESPNowExtForce.getPWMEnb();

  Serial.print("PWM left:");
  Serial.print(throttleENA);
  Serial.print("PWM right:");
  Serial.println(throttleENB);
  motorLeft.setMotorControlValuesLeft(throttleENA);
  motorRight.setMotorControlValuesRight(throttleENB);

}

void motorTest(){
  motorLeft.setMotorControlValuesLeft(255);   // linker Motor vorwärts
  motorRight.setMotorControlValuesRight(0);   // rechter Motor aus
  Serial.println("left");
  delay(2000);

  motorLeft.setMotorControlValuesLeft(0);
  motorRight.setMotorControlValuesRight(255);
  Serial.println("right");
  delay(2000);

  motorLeft.setMotorControlValuesLeft(255);
  motorRight.setMotorControlValuesRight(255);
  Serial.println("both");
  delay(2000);

}


/*
void testEncoderLoop(){
  // === Read and send encoder velocities ===
  leftEncoderVel  = leftMotorEncoder.getVelocity();   // Read left encoder velocity
  rightEncoderVel = rightMotorEncoder.getLinearVelocity();  // Read right encoder velocity

  Serial.print("Encoder left:");
  Serial.print(leftEncoderVel);

  Serial.print("Encoder right:");
  Serial.println(rightEncoderVel);
}
/*


/*
void testJoystickLoop(){

  rawX = JoystickLeft.getRawX(pinJoystickX);
  rawY = JoystickRight.getRawY(pinJoystickY);
  throttleENA = JoystickLeft.getLeftValue(rawX);
  throttleENB = JoystickRight.getRightValue(rawY);
  motorDriver.setMotorControlValues( throttleENA, throttleENB);

   // Initial read to ensure joystick values are ready
  //vel = JStickVelocity.getVelocityControl(pinThrottle);     // Update throttle (0–100)
  //leftRight =   JStickDirection.getControlLeftRight(pinJoystickX);  // Update direction (mapped -100 to 100)
  //forwardBackward = JStickDirection.getControlForwardBackward(pinJoystickY);   // Update direction (mapped -100 to 100)
  
  vel = JStickVelocity.getRawThrottle(pinThrottle);     // Update throttle (0–100)
  leftRight =   JStickDirection.getRawX(pinJoystickX);  // Update direction (mapped -100 to 100)
  forwardBackward = JStickDirection.getRawY(pinJoystickY);   // Update direction (mapped -100 to 100)
  
  Serial.print("raw X:");
  Serial.print(rawX);

  Serial.print("raw Y:");
  Serial.print(rawY);

  Serial.print("Joystick X:");
  Serial.print(throttleENA);

  Serial.print("Joystick Y:");
  Serial.print(throttleENB);
 
  Serial.print("PWM Left:");
  Serial.print(pwmLeft);

  Serial.print("PWM Right:");
  Serial.print(pwmRight);
  
}
*/


/*
void testMotorLoop(){
  motorLeft = 255;
  motorRight = 255;
  
  // === Apply PWM values to motors ===
  motorDriver.setMotorValueLeft(motorLeft);         // Drive motors with received values
  motorDriver.setMotorValueRight(motorRight);         // Drive motors with received values       

  Serial.print("motor left: ");
  Serial.print(motorLeft);

  Serial.print("motor right: ");
  Serial.println(motorRight);

}
*/

/*
void testJoystickMotorComLoop(){
  // Initial read to ensure joystick values are ready
  vel = JStickVelocity.getVelocityControl();     // Update throttle (0–100)
  
  leftRight =   JStickDirection.getControlLeftRight();  // Update direction (mapped -100 to 100)
  forwardBackward = JStickDirection.getControlForwardBackward();   // Update direction (mapped -100 to 100)

  motorLeft = JStickDirection.getMotorControlValueLeft();
  motorRight = JStickDirection.getMotorControlValueRight();

  // === Apply PWM values to motors ===
  motorDriver.setMotorValueLeft(motorLeft);
  motorDriver.setMotorValueRight(motorRight);         // Drive motors with received values

  Serial.print("Velocity Joy:");
  Serial.print(vel);

  Serial.print("Direction Joy left and right:");
  Serial.print(leftRight);

  Serial.print("Direction Joy forward and backward:");
  Serial.print(forwardBackward);


  Serial.print("motor left: ");
  Serial.print(motorLeft);

  Serial.print("motor right: ");
  Serial.println(motorRight);
}
*/