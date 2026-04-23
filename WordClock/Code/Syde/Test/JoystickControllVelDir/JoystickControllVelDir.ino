// Joystick-Pins (Analogwerte)
const int pinJoystickX = A0;   // Steuerung links/rechts
const int pinJoystickY = A1;   // Steuerung vorwärts/rückwärts
const int pinThrottle   = A2;  // Gashebel (Geschwindigkeit)

// Motortreiber-Pins
const int pinLeftPWM     = 5;  // PWM für linken Motor
const int pinRightPWM    = 6;  // PWM für rechten Motor
const int pinLeftDir1    = 7;  // Richtung linker Motor
const int pinLeftDir2    = 8;
const int pinRightDir1   = 9;  // Richtung rechter Motor
const int pinRightDir2   = 10;

void setup() {
  pinMode(pinLeftDir1, OUTPUT);
  pinMode(pinLeftDir2, OUTPUT);
  pinMode(pinRightDir1, OUTPUT);
  pinMode(pinRightDir2, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Joystickwerte einlesen (0 bis 1023)
  int rawX = analogRead(pinJoystickX);
  int rawY = analogRead(pinJoystickY);
  int rawThrottle = analogRead(pinThrottle);

  // Normierung:
  // - Joystick X/Y: -1.0 bis +1.0 (Mittelwert bei ~512)
  // - Gashebel:     0.0 bis 1.0
  float steering = ((float)rawX - 512) / 512.0;           // Links/​Rechts
  float forwardBackward = ((float)rawY - 512) / 512.0;    // Vor/​Zurück
  float throttle = (float)rawThrottle / 1023.0;           // 0 bis 100 %

  // Motorleistung berechnen (Differentialsteuerung)
  float leftMotorPower  = forwardBackward + steering;
  float rightMotorPower = forwardBackward - steering;

  // Mit Gas multiplizieren
  leftMotorPower  *= throttle;
  rightMotorPower *= throttle;

  // Begrenzen auf -1.0 bis +1.0
  leftMotorPower  = constrain(leftMotorPower, -1.0, 1.0);
  rightMotorPower = constrain(rightMotorPower, -1.0, 1.0);

  // In PWM umrechnen (0–255), Vorzeichen bestimmt Richtung
  int pwmLeft  = abs(leftMotorPower) * 255;
  int pwmRight = abs(rightMotorPower) * 255;

  // Richtung linker Motor
  if (leftMotorPower >= 0) {
    digitalWrite(pinLeftDir1, HIGH);
    digitalWrite(pinLeftDir2, LOW);
  } else {
    digitalWrite(pinLeftDir1, LOW);
    digitalWrite(pinLeftDir2, HIGH);
  }

  // Richtung rechter Motor
  if (rightMotorPower >= 0) {
    digitalWrite(pinRightDir1, HIGH);
    digitalWrite(pinRightDir2, LOW);
  } else {
    digitalWrite(pinRightDir1, LOW);
    digitalWrite(pinRightDir2, HIGH);
  }

  // PWM ausgeben
  analogWrite(pinLeftPWM, pwmLeft);
  analogWrite(pinRightPWM, pwmRight);

  // Debug-Ausgabe (optional)
  Serial.print("Lenkung: "); Serial.print(steering, 2);
  Serial.print("  Vor/Rück: "); Serial.print(forwardBackward, 2);
  Serial.print("  Gas: "); Serial.print(throttle, 2);
  Serial.print("  → Links: "); Serial.print(leftMotorPower, 2);
  Serial.print("  Rechts: "); Serial.println(rightMotorPower, 2);

  delay(20); // Optional für flüssige Steuerung
}
