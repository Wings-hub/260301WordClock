//Questo codice: -legge da due celle di carico HX711; -applica un filtro PID sul segnale per calcolare un'uscita; -converte il risultato PID in una tensione analogica tramite DAC;
//-visualizza i dati su pagina web; -permette calibrazione (zero+enter); -permette tuning dei PID via browser; -memorizza i parametri nella memoria non volatile

// Libraries
#include <WiFi.h>              //connessione WiFi dell'ESP32
#include <WebServer.h>         //creazione di un server web su ESP32
//#include <Adafruit_NeoPixel.h> //LED RGB
#include <FastLED.h>
#include <Adafruit_MCP4725.h>  //convertitore DAC MCP4725
#include <Wire.h>              //comunicazione I2C
#include <Preferences.h>       //accesso alla memoria non volatile (NVS) dell'ESP32
#include "HX711.h"             //lettura delle celle di carico tramite DAC
#include "image.h"             //contiene un'immagine
#include "webpage.h"           //dati statici della pagina HTML

// Global Variables
//Pin di collegamento per i due moduli HX711 (uno per ogni cella di carico)
const uint8_t clockPin1 = 5;
const uint8_t dataPin1 = 4;
const uint8_t clockPin2 = 7;
const uint8_t dataPin2 = 6;

//Pin di collegamento per il DAC (SDA/SCL per I2C e dacInputPin lettura analogica del segnale in uscita dal DAC)
const int SDA1_Pin = 41;
const int SCL1_Pin = 42;
const int SDA2_Pin = 39;
const int SCL2_Pin = 40;
const int dacInputPin1 = 8;
const int dacInputPin2 = 3;

//Istanze di oggetti per DAC
Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;

//Istanze di oggetti per HX711
HX711 scale1;
HX711 scale2;

//Variabili per pesi e calibrazione
long raw1 = 0, raw2 = 0;                        //valore grezzo letto da HX711
long lastRaw1 = 0, lastRaw2 = 0;
float Fl1 = 0, Fl2 = 0;                         //forza peso calcolata come: Fl = K*(raw-C) (valore filtrato e calibrato)
float Vd1 = 0, Vd2 = 0, vDAC1 = 0, vDAC2 = 0;   //uscita PID per il DAC (Vd) e tensione effettiva in uscita dal DAC (vDAC)
float K1 = 1.0, C1 = 0.0, K2 = 1.0, C2 = 0.0;   //fattori di calibrazione (K) e taratura (C)
bool zero1Set = false, zero2Set = false;
float WaveData = 4095.0;
float VdTEST = 0, vDAC1TEST = 0, vDAC2TEST = 0;

Preferences prefs;

int p = 10;
long buffer1[100], buffer2[100];  //buffer per media mobile
int bufIndex = 0;                 //indice circolare per il buffer

//Costanti del PID (solo PD)
float Kp1 = 1.2;
float Kd1 = 0.4;
float Kp2 = 1.2;
float Kd2 = 0.4;
float prevFl1 = 0, prevFl2 = 0;
float prevWaveData = 4095;

//Costante banda morta intorno a 1.5V
float B = 0;

//Costante per convertire i valori tra V (tensione) e 0-4095 (12 bit)
float conv = 1500.0;

//Dati di rete WiFi
const char *ssid = "TP-Link_460A";  //Massimo: "VodafoneMinelli"
const char *password = "75540937";  //Massimo: "1189sdafoS!"

//Indirizzi statici assegnati all'ESP32
IPAddress local_IP(192, 168, 0, 25);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 0, 1);
IPAddress secondaryDNS(8, 8, 8, 8);

WebServer server(80); //web server in ascolto sulla porta 80

//Stringhe per interfaccia web
String serialData1 = "Waiting for data...";
String serialData2 = "Waiting for data...";
String ipAddress = "Not connected";
String webWeight1 = "Waiting for weight...";
String webWeight2 = "Waiting for weight...";
bool startMeasurement1 = false;
bool startMeasurement2 = false;
bool startTest = false;

#define LED_PIN 21
#define NUM_LEDS 1
//Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[NUM_LEDS];

//void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
//    strip.setPixelColor(0, strip.Color(r, g, b));
//    strip.show();
//}

//handle...()-->funzioni che rispondono alle richieste HTTP del web server
//Reimposta i valori di calibrazione a quelli predefiniti, elimina tutti i valori salvati nella NVS e invia un messaggio di conferma
void handleResetAll() {
    startMeasurement1 = false;
    startMeasurement2 = false;
    webWeight1 = "Waiting for weight...";
    webWeight2 = "Waiting for weight...";
    zero1Set = false;
    zero2Set = false;
    startTest = false;
    //setLEDColor(0, 255, 0);
    leds[0] = CRGB::Green;
    FastLED.show();
    server.send(200, "text/plain", "All measurements reset");
}

void handleImage() {
    server.send_P(200, "image/png", (const char*)ImageData, sizeof(ImageData));
}

void handleSerialData1() {
    server.send(200, "text/plain", serialData1);  //"Waiting for data..."
}

void handleSerialData2() {
    server.send(200, "text/plain", serialData2);
}

void handleWeightData1() {
    server.send(200, "text/plain", webWeight1);  //"Waiting for weight..."
}

void handleWeightData2() {
    server.send(200, "text/plain", webWeight2);
}

//Avvia la lettura del peso impostando startMeasurement su true
void handleStart1() {
    if (!zero1Set || K1 == 1.0) {
        server.send(200, "text/plain", "Error: Zero and Enter must be set before starting!");
        return;
    }
    startMeasurement1 = true;
    server.send(200, "text/plain", "Measuring Weight One...");
    //setLEDColor(0, 0, 255);
    leds[0] = CRGB::Blue;
    FastLED.show();
}

void handleStart2() {
    if (!zero2Set || K2 == 1.0) {
        server.send(200, "text/plain", "Error: Zero and Enter must be set before starting!");
        return;
    }
    startMeasurement2 = true;
    server.send(200, "text/plain", "Measuring Weight Two...");
    //setLEDColor(0, 0, 255);
    leds[0] = CRGB::Blue;
    FastLED.show();
}

void handleStartWave() {
    startTest = true;
    //setLEDColor(0, 0, 255);
    leds[0] = CRGB::Magenta;
    FastLED.show();
}

void handleData() {
  if (!startMeasurement1 && !startMeasurement2 && !startTest) {   //Se nessuna delle due misurazioni è attiva...
    server.send(204, "application/json", "{}");     //...risponde con un JSON vuoto ({}) e HTTP status 204 (no content)
    return;
  }
  //Costruisce e invia un oggetto json
  String json = "{";
  json += "\"raw1\":" + String(raw1) + ",";  //prima era lastRaw1
  json += "\"Fl1\":" + String(Fl1) + ",";
  json += "\"raw2\":" + String(raw2) + ",";  //prima era lastRaw2
  json += "\"Fl2\":" + String(Fl2) + ",";
  json += "\"vDAC1\":" + String(vDAC1) + ",";
  json += "\"vDAC2\":" + String(vDAC2) + ",";
  json += "\"C1\":" + String(C1) + ",";
  json += "\"K1\":" + String(K1) + ",";
  json += "\"C2\":" + String(C2) + ",";
  json += "\"K2\":" + String(K2) + ",";
  json += "\"WaveData\":" + String(WaveData) + ",";
  json += "\"vDAC1TEST\":" + String(vDAC1TEST) + ",";
  json += "\"vDAC2TEST\":" + String(vDAC2TEST);
  json += "}";
  server.send(200, "application/json", json);
}

void handleSet() {
  //Legge i parametri p, kp e kd dalla richiesta HTTP
  //Note teoriche: server.hasArg("p")-->Controlla se la richiesta HTTP contiene un argomento chiamato "p" (es: ...?p=25)
  //server.arg("p")-->Restituisce il valore dell’argomento "p" come String (es: "25")
  //.toInt()-->Converte la String "25" in intero 25
  //constrain(valore, min, max)-->Limita il valore entro un intervallo-->se è minore di min, restituisce min; se è maggiore di max, restituisce max
  if (server.hasArg("p")) p = constrain(server.arg("p").toInt(), 1, 100);  //controlla se è stato passato un parametro p nella richiesta HTTP e lo assegna alla variabile "p" nell'intervallo compreso tra 1 e 100
  if (server.hasArg("kp1")) Kp1 = server.arg("kp1").toFloat();
  if (server.hasArg("kd1")) Kd1 = server.arg("kd1").toFloat();
  if (server.hasArg("kp2")) Kp2 = server.arg("kp2").toFloat();
  if (server.hasArg("kd2")) Kd2 = server.arg("kd2").toFloat();
  if (server.hasArg("B")) B = server.arg("B").toFloat();
  if (server.hasArg("conv")) conv = server.arg("conv").toFloat();
  //Salva i parametri nella NVS (Preferences)
  prefs.begin("pid", false);
  prefs.putInt("p", p);
  prefs.putFloat("kp1", Kp1);
  prefs.putFloat("kd1", Kd1);
  prefs.putFloat("kp2", Kp2);
  prefs.putFloat("kd2", Kd2);
  prefs.putFloat("B", B);
  prefs.putFloat("conv", conv);
  prefs.end();
  server.send(200, "text/plain", "OK");
}

//Media mobile semplice dei valori grezzi per ridurre il rumore sui dati da HX711
long filter(HX711 &scale, long *buffer, long &lastRaw) {
  lastRaw = scale.read();                            //legge valore attuale (ultima lettura)
  //Serial.printf("lastRaw: %ld\n", lastRaw);
  buffer[bufIndex] = lastRaw;                        //inserisce valore attuale nel buffer circolare
  long sum = 0;
  //Note teoriche: assegna a count il numero di campioni da usare nella media mobile, ma senza superare il limite massimo p
  //bufIndex è l’indice corrente nel buffer circolare
  //p è la lunghezza massima della media mobile
  //(?__:__) è un operatore ternario, alternartiva di:
  //if (bufIndex < p) count = bufIndex + 1;
  //else count = p;
  int count = (bufIndex < p) ? bufIndex + 1 : p;     //p determina quanti campioni vengono mediati
  for (int i = 0; i < count; i++) sum += buffer[i];
  float testvalue = sum / count;
  //Serial.printf("testvalue: %.2f\n", testvalue);
  return sum / count;                                //media dei valori nel buffer
}

//Calcolo di P e D
float pid1(float Fl, float& prevFl) {
  float prop = Kp1 * Fl;
  float deriv = Kd1 * (Fl - prevFl);
  prevFl = Fl;
  return prop + deriv;    //output: tensione da inviare al DAC1
}

float pid2(float Fl, float& prevFl) {
  float prop = Kp2 * Fl;
  float deriv = Kd2 * (Fl - prevFl);
  prevFl = Fl;
  return prop + deriv;    //output: tensione da inviare al DAC2
}

float pidTEST1(float WaveData, float& prevWaveData) {
  float prop = Kp1 * (WaveData / 4095.0) * 3300.0;                    //converto in mV
  float deriv = Kd1 * ((WaveData - prevWaveData) / 4095.0) * 3300.0;  //converto in mV
  prevWaveData = WaveData;
  Serial.printf("prop: %.2f, deriv: %.2f\n", prop, deriv);
  return prop + deriv;    //output: tensione da inviare al DAC1; nota: può venire fuori un valore superiore a 1500mV
}

float pidTEST2(float WaveData, float& prevWaveData) {
  float prop = Kp2 * (WaveData / 4095.0) * 3300.0;                    //converto in mV
  float deriv = Kd2 * ((WaveData - prevWaveData) / 4095.0) * 3300.0;  //converto in mV
  prevWaveData = WaveData;
  Serial.printf("prop: %.2f, deriv: %.2f\n", prop, deriv);
  return prop + deriv;    //output: tensione da inviare al DAC2; nota: può venire fuori un valore superiore a 1500mV
}

//Converte una tensione compresa tra -1500mV a +1500mV (Vd) in una tensione 0-3000mV per il DAC
void outputToDAC(Adafruit_MCP4725& dac, float Vd) {
  float scaled;
  float Vd_constrained = constrain(Vd, -1500, 1500);  //Vd può essere un valore maggiore di 1500 mV, quindi uso la funzione constrain
  float Bmezzi = B / 2.0;
  if (Vd_constrained >= -Bmezzi && Vd_constrained <= Bmezzi){
    scaled = (conv / 3300.0) * 4095.0;
  }
  else{
    float Vd_mV = constrain(Vd, -1500, 1500) + 1500;
    scaled = (Vd_mV / 3300.0) * 4095.0;
  }
  //Serial.printf("scaled: %.2f\n", scaled);
  dac.setVoltage((uint16_t)scaled, false);
}

//Converte una tensione compresa tra -1500mV a +1500mV (Vd) in una tensione 0-3300 mV per il DAC
void outputToDACTEST(Adafruit_MCP4725& dac, float VdTEST) {
  float scaled;
  //float Vd_mV = constrain(VdTEST, -1500, 1500) + 1500;
  float VdTEST_constrained = constrain(VdTEST, -1500, 1500);
  float Bmezzi = B / 2.0;
  if (VdTEST_constrained >= -Bmezzi && VdTEST_constrained <= Bmezzi){
    VdTEST = 1500.0;
  }
  if (VdTEST > 3300){
    VdTEST = 3300.0;
  }
  else if (VdTEST < 0){
    VdTEST = 0;
  }
  scaled = (VdTEST / 3300.0) * 4095.0;    //conversione
  dac.setVoltage((uint16_t)scaled, false);
}

void outputToDACTESTsemplificato(Adafruit_MCP4725& dac, float WaveData) {
  //float Vd_mV = constrain(VdTEST, -1500, 1500) + 1500;
  float scaled = WaveData;  //nessuna conversione rispetto alla funzione outputToDACTEST
  dac.setVoltage((uint16_t)scaled, false);
}

//Carica i parametri da NVS; se non presenti, usa i valori di default
void loadPrefs() {
  prefs.begin("pid", true);
  //Nota teorica: prefs.getInt("p", 10) legge dalla NVS il valore associato alla chiave "p"; se non esiste, restituisce 10
  p = constrain(prefs.getInt("p", 10), 1, 100);
  Kp1 = prefs.getFloat("kp1", 1.2);
  Kd1 = prefs.getFloat("kd1", 0.4);
  Kp2 = prefs.getFloat("kp2", 1.2);
  Kd2 = prefs.getFloat("kd2", 0.4);
  B = prefs.getFloat("B", 0);
  conv = prefs.getFloat("conv", 1500.0);
  C1 = prefs.getFloat("C1", 0);
  K1 = prefs.getFloat("K1", 1.0);
  C2 = prefs.getFloat("C2", 0);
  K2 = prefs.getFloat("K2", 1.0);
  prefs.end();
}

//Imposta il valore di zero per ciascuna bilancia (calcolo tara)
void zero1() {
  scale1.tare(20);      //20 letture interne per una buona accuratezza
  C1 = scale1.read();   //C1 è l'offset per il calcolo successivo
  zero1Set = true;
}

void zero2() {
  scale2.tare(20);
  C2 = scale2.read();
  zero2Set = true;
}

//Imposta il fattore di calibrazione, K1 o K2, rispetto a un peso noto di 1000g
void enter1() {
  if (!zero1Set) {
    Serial.println("Error: Zero1 not set before Enter1");
    return;
  }
  float refMass = 1000.0;              //peso noto
  float delta = scale1.read() - C1;    //(lettura corrente - tara)
  if (abs(delta) < 10) {               //se la variazione è troppo piccola, non esegue la calibrazione
    Serial.println("Error: delta1 too small, cannot calibrate!");
    return;
  }
  K1 = refMass / delta;
  saveCalibration();
}

void enter2() {
  if (!zero2Set) {
    Serial.println("Error: Zero2 not set before Enter2");
    return;
  }
  float refMass = 1000.0;
  float delta = scale2.read() - C2;
  if (abs(delta) < 10) {
    Serial.println("Error: delta2 too small, cannot calibrate!");
    return;
  }
  K2 = refMass / delta;
  saveCalibration();
}

//Salva i valori C e K nella NVS
void saveCalibration() {
  prefs.begin("pid", false);
  prefs.putFloat("C1", C1);
  prefs.putFloat("K1", K1);
  prefs.putFloat("C2", C2);
  prefs.putFloat("K2", K2);
  prefs.end();
}

TwoWire I2C_1 = TwoWire(0);  //Bus 0
TwoWire I2C_2 = TwoWire(1);  //Bus 1

void setup() {
  Serial.begin(115200);
  delay(5000);
  I2C_1.begin(SDA1_Pin, SCL1_Pin);
  I2C_2.begin(SDA2_Pin, SCL2_Pin);
  dac1.begin(0x60, &I2C_1);  //usa il primo bus
  dac2.begin(0x60, &I2C_2);  //usa il secondo bus
  //Wire.begin(SDA1_Pin, SCL1_Pin);      //inizializza comunicazione I2C con DAC1
  //Wire.begin(SDA2_Pin, SCL2_Pin);      //inizializza comunicazione I2C con DAC2
  //dac1.begin(0x60); dac2.begin(0x61);    //inizializza DAC agli indirizzi I2C 0x60 e 0x61
  //strip.begin(); strip.show();
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  //setLEDColor(255, 165, 0);
  leds[0] = CRGB::Yellow;
  FastLED.show();

  pinMode(dacInputPin1, INPUT);
  pinMode(dacInputPin2, INPUT);
  
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  //setLEDColor(0, 255, 0);
  leds[0] = CRGB::Green;
  FastLED.show();

  ipAddress = WiFi.localIP().toString();  //ottiene l'indirizzo IP assegnato dal router all'ESP32, come oggetto IPAddress, e lo converte in stringa
  Serial.println("Connected. IP: " + ipAddress);

  scale1.begin(dataPin1, clockPin1);   //inizializza cella di carico
  scale2.begin(dataPin2, clockPin2);

  loadPrefs();
  
  //Registra le rotte e avvia il server
  //Note teoriche: una rotta è un percorso URL che il server riconosce e gestisce con una funzione specifica
  //es.: la rotta http://192.168.1.123/ (indirizzo IP della rete) chiama la funzione handleRoot() che serve la pagina web principale (se qualcuno visita tale indirizzo)
  //ogni server.on("percorso", funzione) registra una rotta e le associa una funzione da eseguire in risposta
  server.on("/", []() { server.send_P(200, "text/html", MAIN_page); });
  server.on("/set", handleSet);
  server.on("/data", handleData);   //es.: la rotta http://192.168.1.123/data chiama la funzione handleData() che invia dati in json
  server.on("/image", handleImage);
  server.on("/resetall", handleResetAll);
  server.on("/zero1", [](){ zero1(); server.send(200, "text/plain", "Zero1 done"); });
  server.on("/zero2", [](){ zero2(); server.send(200, "text/plain", "Zero2 done"); });
  server.on("/enter1", [](){ enter1(); server.send(200, "text/plain", "Enter1 done"); });
  server.on("/enter2", [](){ enter2(); server.send(200, "text/plain", "Enter2 done"); });
  server.on("/start1", handleStart1);         //es.: la rotta http://192.168.1.123/start1 chiama la funzione handleStart1() che avvia la misurazione 1
  server.on("/start2", handleStart2);
  server.on("/startWave", handleStartWave);
  server.on("/weight1", handleWeightData1);
  server.on("/weight2", handleWeightData2);
  server.on("/serial1", handleSerialData1);
  server.on("/serial2", handleSerialData2);
  server.begin();
}

void loop() {
  if (startMeasurement1) {
    raw1 = filter(scale1, buffer1, raw1); //legge valore grezzo e lo filtra
    Fl1 = K1 * (raw1 - C1);               //calcola Fl1 usando K1 e C1
    Vd1 = pid1(Fl1, prevFl1);             //applica PID1 per calcolare Vd1
    outputToDAC(dac1, Vd1);               //manda valore al DAC1
    vDAC1 = analogRead(dacInputPin1) * (3300.0 / 4095.0);                             //legge valore analogico dal pin associato al DAC1
    //Se il valore Fl1 è valido e K1 è finito, converte FL1 in stringa con 2 decimali, altrimenti scrive "Error"
    //Note teoriche: isnan-->numero non valido; isinf-->numero infinito
    //(?_:_) è un operatore ternario (sostituisce if...else...)
    webWeight1 = (!isnan(Fl1) && !isinf(K1)) ? String(Fl1, 2) : "Error";              //aggiorna stringa web
    //Serial.printf("raw1: %ld, Fl1: %.2f, K1: %.2f, C1: %.2f\n", raw1, Fl1, K1, C1);                  //DEBUG
    //Serial.printf("Vd1: %.2f, vDAC1: %.2f\n", Vd1, vDAC1);                                           //DEBUG
    //Serial.printf("p: %d, Kp1: %.2f, Kd1: %.2f, Kp2: %.2f, Kd2: %.2f\n", p, Kp1, Kd1, Kp2, Kd2);     //DEBUG
    //Serial.printf("B: %.2f, conv: %.2f\n", B, conv);                                                 //DEBUG
  }

  if (startMeasurement2) {
    raw2 = filter(scale2, buffer2, raw2);
    Fl2 = K2 * (raw2 - C2);
    Vd2 = pid2(Fl2, prevFl2);
    outputToDAC(dac2, Vd2);
    vDAC2 = analogRead(dacInputPin2) * (3300.0 / 4095.0);
    webWeight2 = (!isnan(Fl2) && !isinf(K2)) ? String(Fl2, 2) : "Error";
    //Serial.printf("raw2: %ld, Fl2: %.2f, K2: %.2f, C2: %.2f\n", raw2, Fl2, K2, C2);                  //DEBUG
    //Serial.printf("Vd2: %.2f, vDAC2: %.2f\n", Vd2, vDAC2);                               //DEBUG
  }

  if (startTest) {
    if (WaveData <= 0){
      WaveData = 4095.0;
    }
    else {
      WaveData -= 105.0;
    }
    //Serial.printf("wave data: %.2f\n", WaveData);

    //VdTEST = pidTEST(WaveData, prevWaveData);            //applica PID per calcolare VdTEST
    //outputToDACTEST(dac1, VdTEST);

    outputToDACTESTsemplificato(dac1, WaveData);
    outputToDACTESTsemplificato(dac2, WaveData);
    vDAC1TEST = analogRead(dacInputPin1) * (3300.0 / 4095.0);
    vDAC2TEST = analogRead(dacInputPin2) * (3300.0 / 4095.0);
    Serial.printf("wave data: %.2f, VdTEST (mV): %.2f, vDAC1TEST (mV): %.2f, vDAC2TEST (mV): %.2f\n", WaveData, VdTEST, vDAC1TEST, vDAC2TEST);
    //Serial.printf("p: %d, Kp1: %.2f, Kd1: %.2f, Kp2: %.2f, Kd2: %.2f\n", p, Kp1, Kd1, Kp2, Kd2);      //DEBUG
    //Serial.printf("B: %.2f, conv: %.2f\n", B, conv);                                                  //DEBUG

    delay(500);
  }
  bufIndex = (bufIndex + 1) % p;  //aggiorna bufIndex per il filtro
  server.handleClient();          //chiama server.handleClient() per gestire richieste web entranti
  delay(10);
}