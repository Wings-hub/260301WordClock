// === Motor-Pin-Zuweisungen ===
const int pinLeftPWM    = 15;
const int pinRightPWM   = 16;

const int pinLeftDir1   = 39;
const int pinLeftDir2   = 40;

const int pinRightDir1  = 41;
const int pinRightDir2  = 42;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Setup gestartet.");

  // Richtungs-Pins konfigurieren
  pinMode(pinLeftDir1, OUTPUT);
  pinMode(pinLeftDir2, OUTPUT);
  pinMode(pinRightDir1, OUTPUT);
  pinMode(pinRightDir2, OUTPUT);

  // PWM-Pins konfigurieren (falls nötig)
  pinMode(pinLeftPWM, OUTPUT);
  pinMode(pinRightPWM, OUTPUT);
}

void loop() {
  Serial.println("Links vorwärts");
  digitalWrite(pinLeftDir1, HIGH);
  digitalWrite(pinLeftDir2, LOW);
  analogWrite(pinLeftPWM, 255);

  digitalWrite(pinRightDir1, LOW);
  digitalWrite(pinRightDir2, LOW);
  analogWrite(pinRightPWM, 0);
  delay(3000);

  Serial.println("Rechts vorwärts");
  digitalWrite(pinLeftDir1, LOW);
  digitalWrite(pinLeftDir2, LOW);
  analogWrite(pinLeftPWM, 0);

  digitalWrite(pinRightDir1, HIGH);
  digitalWrite(pinRightDir2, LOW);
  analogWrite(pinRightPWM, 255);
  delay(3000);

  Serial.println("Links rückwärts");
  digitalWrite(pinLeftDir1, LOW);
  digitalWrite(pinLeftDir2, HIGH);
  analogWrite(pinLeftPWM, 255);

  digitalWrite(pinRightDir1, LOW);
  digitalWrite(pinRightDir2, LOW);
  analogWrite(pinRightPWM, 0);
  delay(3000);

  Serial.println("Rechts rückwärts");
  digitalWrite(pinLeftDir1, LOW);
  digitalWrite(pinLeftDir2, LOW);
  analogWrite(pinLeftPWM, 0);

  digitalWrite(pinRightDir1, LOW);
  digitalWrite(pinRightDir2, HIGH);
  analogWrite(pinRightPWM, 255);
  delay(3000);
}
