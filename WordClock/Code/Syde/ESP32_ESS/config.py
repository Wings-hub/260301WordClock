VERSION = 1

WAIT_SECONDS = 3500 # Quasi un ora utile per Whatchguard che riavvia
DEEPSLEEP = False
DEEPSLEEP_SECONDS = 150 # 3 minuti
KEEP_ALIVE_EVERY = 96 #Numero di tentativi dopodiche effettuare cmq il keep alive
BLE_OPEN_SECONDS = 30 #Numero di secondi in cui il Bluetooth rimane acceso al primo avvio
BLE_NAME='ESS'

I2_SCAN_RETRY = 2
I2C_WAITMLSEC = 2000

DEBUG = False
SIGFOX = False
LORA  = False
LTE = False
WIFI = False
WIFI_SSID = "SSID"
WIFI_PASS = "---"
WIFI_RETRY_SECONDS = 15 #Numero di secondi in cui il wifi riprova a collegarsi

ANE= False
SCD30 = False
PMSA003 = False
TRAP = False
PLV=False
TC=True

WETSET=0
BAT_MIN_LEV = 20
ANE_COEF = 1
ANE_SEC = 20
PLUVI_COEF = 0.5
DUST_TIME = 12
SCD30_FORCE_RECALIBRATON = 0
SCD30_TEMP_OFFSET = -1.5

SGP30_WARNING_SECONDS = 45 #Numero di secondi per far scaldare il sensore
SGP30_TIME = 1 #Per quante trasmissioni di fila il sensore non deve misurare (impostare in config.json)

TARATURA_IRR = 1

# Calibration:
#WET CONFIG
WET_REFERENCE_MEAS = 4204366
#REFERENCE_MEAS = 0
WET_DRY_MEAS = 4178351
WET_DELTA_MEAS = 258683 #Differenza fra DRY_MEAS e DRY_MEAS a Bagnato

#heartbeat
HEART_ALERT = 0xff0000
HEART_SEARCHING = 0xf05b00
HEART_GOOD = 0x00ff00

HEART_BLE = 0x0000FF
HEART_PURPLE = 0x8f00ff
HEART_GREEN = 0x005f00
HEART_WHITE = 0xffffff

import json
jsondata=""
jsonFields=""

def write_config(Key, Value):
    jsondata = read_config()
    file = open('config.json', 'w')
    jsondata[Key] = Value
    file.write(json.dumps(jsondata))
    file.close()

def read_config():
    global jsonFields
    if (jsonFields==""):
        jsonFile = open("config.json", "r")# Open the JSON file for reading
        jsonFields = json.load(jsonFile) # Read the JSON into the buffer
        jsonFile.close() # Close the JSON file

    return jsonFields

jsondata = read_config()

if 'waitseconds' in jsondata: WAIT_SECONDS = int(jsondata['waitseconds'])
if 'deepsleep' in jsondata: DEEPSLEEP = jsondata['deepsleep']
if 'bluetooth' in jsondata: BLE_OPEN_SECONDS = jsondata['bluetooth']
if 'deepsleepseconds' in jsondata: DEEPSLEEP_SECONDS = int(jsondata['deepsleepseconds'])
if 'keepalivetime' in jsondata: KEEP_ALIVE_EVERY = int(jsondata['keepalivetime'])
if 'debug' in jsondata: DEBUG = jsondata['debug']
if 'sigfox' in jsondata: SIGFOX = jsondata['sigfox']
if 'lora' in jsondata: LORA = jsondata['lora']
if 'wifi' in jsondata: WIFI = jsondata['wifi']
if 'lte' in jsondata: LTE = jsondata['lte']
if 'ane' in jsondata: ANE = jsondata['ane']
if 'plv' in jsondata: PLV = jsondata['plv']
if 'trap' in jsondata: TRAP = jsondata['trap']
if 'tc' in jsondata: TC = jsondata['tc']
if 'batminlev' in jsondata: BAT_MIN_LEV = jsondata['batminlev']
if 'scd30' in jsondata: SCD30 = jsondata['scd30']
if 'pmsa003' in jsondata: PMSA003 = jsondata['pmsa003']
if 'pluvicoef' in jsondata: PLUVI_COEF = jsondata['pluvicoef']
if 'anecoef' in jsondata : ANE_COEF = jsondata['anecoef']
if 'anesec' in jsondata : ANE_SEC = jsondata['anesec']
if 'dusttime' in jsondata : DUST_TIME = jsondata['dusttime']
if 'sgp30time' in jsondata : SGP30_TIME = jsondata['sgp30time']
if 'scd30forcerecal' in jsondata : SCD30_FORCE_RECALIBRATON = jsondata['scd30forcerecal']
if 'scd30tempoffset' in jsondata : SCD30_TEMP_OFFSET = jsondata['scd30tempoffset']
if 'taraturaIRR' in jsondata : TARATURA_IRR = jsondata['taraturaIRR']
if 'wetrefmeas' in jsondata : WET_REFERENCE_MEAS = jsondata['wetrefmeas']
if 'wetdrymeas' in jsondata : WET_DRY_MEAS = jsondata['wetdrymeas']
if 'wetdeltameas' in jsondata : WET_DELTA_MEAS = jsondata['wetdeltameas']
if 'wifissid' in jsondata : WIFI_SSID = jsondata['wifissid']
if 'wifipassword' in jsondata : WIFI_PASS = jsondata['wifipassword']
print("<----------- INIT CONFIG --------------->")
print(VERSION)
print(WAIT_SECONDS)
print(DEEPSLEEP_SECONDS)
print(KEEP_ALIVE_EVERY)
print(SIGFOX)
print(WIFI)
print(WIFI_SSID)
print(WIFI_PASS)
print(SGP30_TIME)
print(DUST_TIME)
print(WAIT_SECONDS)
print(DEEPSLEEP_SECONDS)


print("<----------- END CONFIG --------------->")
