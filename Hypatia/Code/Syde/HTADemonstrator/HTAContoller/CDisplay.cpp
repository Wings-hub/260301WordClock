#include "CDisplay.h"
#include "CESPNowControler.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR     0x3C



CDisplay::CDisplay()
  : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) // Initialisierungsliste
{
  now = 0;
  startTime = 0;
  startDisplayDelay = 2000;
  controller = nullptr;
}


void CDisplay::setup(CESPNowControler* ptr) {
  controller = ptr;
  Wire.begin(4, 5);  // SDA, SCL
  

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display init failed!");
    return;
  }

  startTime = millis();
  now = 0;
  showSetupScreen();     // Zeige Startbildschirm

  while (!controller->connectionCheck() || now < startTime + startDisplayDelay) {
    now = millis();
  }

  display.clearDisplay();  // Danach Anzeige löschen
}




// === Startbildschirm anzeigen ===
void CDisplay::showSetupScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 25);
  display.println("HTA");
  display.setCursor(15, 45);
  display.println("CONTROLER");
  display.display();
}

// === Statusanzeige mit ESP-NOW + VEL. LEFT/RIGHT ===
void CDisplay::showStatusScreen(float encoderLeft, float encoderRight, bool connection) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print("ESP NOW: ");
  display.println(connection ? "Connected" : "Not Connected");

  display.setCursor(0, 20);
  display.print("VEL. LEFT: ");
  display.print(encoderLeft, 2);

  display.setCursor(0, 35);
  display.print("VEL. RIGHT: ");
  display.print(encoderRight, 2);

  display.display();
}

