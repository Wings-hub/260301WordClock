from machine import Pin, SoftI2C, ADC
#prova di un commento'
import esp32
import machine
import math
import wifi
import time
import config
import gc

import bluetooth
import ble
from micropython import const
import struct

# ------------------------------------------------
#             DECLARE VARIABLE
# ------------------------------------------------
nvs = esp32.NVS('nvs')
# status variable
firststart = False
earlyrestart = False
earlyrestartcount = 0
# results variable
battery = 0
temperature = 0
humidity = 0
pressure = 0
trap = 0
traperror = b'00'
wet = 0
co2 = 0
tvoc = 0
dust = 0
pm1 = 0
pm2 = 0
pm4 = 0
pm10 = 0
irr = 0
anemoterstart = time.time()
anemotercount = 0
anemoterresult = 0
plumeterresult = 0
groundtemperature = 0
groundhumidity = 0

bluetooth = None
sig = None
sigfoxid = ""

# ------------------------------------------------
#             INIT
# ------------------------------------------------
print('----   INIT   ----')
# ------------------------------------------------
#             CHECK PRIMO AVVIO
# ------------------------------------------------
gc.enable()
# Lettura Orario Lopy per capire se è il primo avvio oppure no.
rtc = machine.RTC()
try:
    prima = [nvs.nvs_get('anno'), nvs.nvs_get('mese'), nvs.nvs_get('giorno'), nvs.nvs_get('ora'), nvs.nvs_get('minuti'),
             nvs.nvs_get('secondi')]  #da sostituire nvs.nvs_get con nvs.get_i32, in quanto la prima funzione non esiste per ESP32
except Exception as e:
    prima = [2099, 1, 1, 0, 0, 0, 0, None]
ora = rtc.datetime()
print("Ora attuale: ", ora) #(anno, mese, giorno, giorno della settimana, ore, minuti, secondi, microsecondi)

# -----------------------------------------------
#   Nel caso di primo avvio
# -----------------------------------------------
#  Considerato riavvio sotto i 10 secondi
if (ora[0] == 2000 and ora[1] == 1 and ora[2] == 1 and ora[4] == 0 and ora[5] == 0 and ora[6] < 10):
    print("-- PRIMO AVVIO --")
    firststart = True
    #config.WIFI = False  #questa riga non serve in quanto voglio il wifi anche al primo avvio



# -----------------------------------------------
#  INIT CONNETTIVITA'
# -----------------------------------------------
#NOTA: su alcuni sensori, se inserisco la connettività dopo il bluetooth, non viene rilevato l'id sigfox ("SIGFOX ID: ERROR: parse error")

if config.SIGFOX:
    try:
        print("START SIGFOX")
        import sigfox
        sig = sigfox.SIGFOX(None, True)
        sigfoxid = sig.getid()
        if config.DEBUG:
            sig.getpac()
    except Exception as e:
        print('Errore: ', e)

if config.WIFI:
    print("START WIFI")
    wifi.wificonnect()
    time.sleep(config.WIFI_RETRY_SECONDS)

#le seguenti 5 righe di codice sono state aggiunte per inviare il messaggio wifi nel cloud sulla stessa riga di Sigfox
#commentando le seguenti 5 righe, il messaggio wifi verrà inviato nel cloud su una nuova riga, diversa rispetto a quella Sigfox
    import sigfox
    sig = sigfox.SIGFOX(None, True)
    sigfoxid = sig.getid()
    if config.DEBUG:
        sig.getpac()
#else:
#    wifi.close_wifi()

print("firststart:", firststart)

if (firststart):    #prima era "if (firststart and False)", ma il bluetooth non si attivava mai (condizione sempre falsa)
    # -----------------------------------------------
    #   Avvia il bluetooth
    # ----------------------------------------------
    print("-- AVVIO BLUETOOTH--")  #funzionamento vedi ble.py da riga 169 a riga 186
    from ble import BLES
    import ubinascii

    bluetooth = BLES(name=config.BLE_NAME + "-" + str(ubinascii.hexlify(machine.unique_id()).decode().upper()))
    #config.WAIT_SECONDS = config.WAIT_SECONDS + config.BLE_OPEN_SECONDS

#nota errore del codice: la variabile "prima" non contiene il giorno della settimana
if (ora[0] == prima[0] and ora[1] == prima[1] and ora[2] == prima[2] and ora[3] == prima[3]):
    if (ora[4] >= prima[4] and (ora[4] - prima[4]) < 3):

        print("RIAVVIO SOTTO I 2 MINUTI")

        #print("Ora attuale (anno, mese, giorno, giorno della settimana, ore, minuti, secondi, microsecondi): ", ora)
        #print("Ora precedente (anno, mese, giorno, ore, minuti, secondi): ", prima)

        earlyrestart = True
# -----------------------------------------------
#      COUNT of EarlyRestart
# -----------------------------------------------
if earlyrestart:
    try:
        earlyrestartcountbefore = nvs.nvs_get('escount')  #da sostituire nvs.nvs_get con nvs.get_i32, in quanto la prima funzione non esiste per ESP32
        if (earlyrestartcountbefore > 0):
            earlyrestartcount = earlyrestartcountbefore + 1
        else:
            earlyrestartcount = 1
        nvs.set_i32('escount', earlyrestartcount)
    except Exception as e:
        print('early error')
        print(e)
nvs.set_i32('escount', earlyrestartcount)
nvs.commit()

# ------------------------------------------------
#            CHECK MOTIVO RIAVVIO
#   controlla il motivo per cui si è spento
#   e poi riacceso
# ------------------------------------------------
if machine.reset_cause() == machine.DEEPSLEEP_RESET:
    print('EXIT from DeepSleep')
    # if (machine.wake_reason()[0])==2: #RTC timer complete
    #     print('Timer completed')
else:
    if machine.reset_cause() == machine.WDT_RESET:
        print('EXIT from WD Restart')
    else:
        # La configurazione viene fatta solo dopo un reset ufficiale
        nvs.set_i32('keepalivecount', 0)
        nvs.set_i32('sgp30livecount', 0)
        nvs.set_i32('dustlivecount', 0)
        nvs.commit()

        # -----------------------------------------------
        #   Avvia il bluetooth
        # ----------------------------------------------
        #if bluetooth == None and False:                     #commentato il 21/10/24
        #    print("-- AVVIO BLUETOOTH POST RESET --")
        #    from ble import BLES
        #    import ubinascii

        #    bluetooth = BLES(name=config.BLE_NAME + "-" + str(ubinascii.hexlify(machine.unique_id()).decode().upper()))
        #    config.WAIT_SECONDS = config.WAIT_SECONDS + config.BLE_OPEN_SECONDS

# ------------------------------------------------
#           COUNT KEEP ALIVE
#    per fare in modo che un sensore non si
#    attivi a tutti i riavvii
# ------------------------------------------------
try:
    # Keep Alive Count
    keepalivecount = nvs.get_i32('keepalivecount')  #sostituito nvs.nvs_get con nvs.get_i32, in quanto la prima funzione non esiste per ESP32
    keepalivecount = keepalivecount + 1
    nvs.set_i32('keepalivecount', keepalivecount)
    # SGP30 Keep Alive Count
    sgp30livecount = nvs.get_i32('sgp30livecount')  #sostituito nvs.nvs_get con nvs.get_i32, in quanto la prima funzione non esiste per ESP32
    sgp30livecount = sgp30livecount + 1
    nvs.set_i32('sgp30livecount', sgp30livecount)
    # DUST Keep Alive Count
    dustkeepalivecount = nvs.get_i32('dustlivecount')  #sostituito nvs.nvs_get con nvs.get_i32, in quanto la prima funzione non esiste per ESP32
    dustkeepalivecount = dustkeepalivecount + 1
    nvs.set_i32('dustlivecount', dustkeepalivecount)
    if config.DEBUG:
        print("keepalivecount, sgp30livecount, dustkeepalivecount: ", keepalivecount, sgp30livecount, dustkeepalivecount)
except Exception as e:
    nvs.set_i32('keepalivecount', 0)
    nvs.set_i32('sgp30livecount', 0)
    nvs.set_i32('dustlivecount', 0)
    keepalivecount = 0
    sgp30livecount = 0
    dustkeepalivecount = 0
nvs.commit()









# -----------------------------------------------
#    INIT POWER
# -----------------------------------------------
if (config.PMSA003):
    pmsapower = Pin(9, Pin.OUT).value(1)
if (config.ANE):
    anepower = Pin(2, Pin.OUT).value(1)
# if (config.TRAP):
#     trappower=Pin('P13', mode=Pin.OUT, pull=None, alt=-1).value(1)
Pin(13, Pin.OUT).value(1)
time.sleep_us(config.I2C_WAITMLSEC)
# -----------------------------------------------
#    INIT I2C
# -----------------------------------------------
print('I2C Starting....')
if config.SCD30:
    i2c = SoftI2C(scl=Pin(22), sda=Pin(21), freq=50000)
else:
    i2c = SoftI2C(scl=Pin(22), sda=Pin(21), freq=100000)
i2cscan = i2c.scan()
x = 0
while i2cscan == [] and x < config.I2_SCAN_RETRY:
    print('I2C Scan...' + str(x))
    x = x + 1
    time.sleep_us(200)
    i2cscan = i2c.scan()
    if config.DEBUG:
        print("I2C Scan: ", i2cscan)

if (bluetooth != None):
    bluetooth.setI2C(i2c)


def check_bme280():
    # Ogni 30 minuti rilevo
    print("BME280 CHeck")
    from lib.bme280_float import BME280  # 118 vecchi, 119 cecchi
    if not (119 in i2cscan):
        print("------------------------")
        print("BME280 CHIP NON PRESENTE")
        print("------------------------")
        return
    global temperature
    global pressure
    global humidity
    try:
        bme = BME280(i2c=i2c)
        temperature = float(str((bme.values[0]).replace('C', '')))
        humidity = float(str((bme.values[2]).replace('%', '')))
        pressure = float(str((bme.values[1]).replace('hPa', '')))
        print("fine try")
    except Exception as e:
        print("errormessage: " + str(e))
        # pycom.rgbled(config.HEART_ALERT) # red
        #if config.WIFI:  #commentato questa parte perchè dà errore; la funzione sys.exc_info non è definita in MicroPython
            #import sys
            #wifi.sendmessage({'i2c': i2cscan, 'error': str(e), 'errormessage': str(sys.exc_info())})
        if config.SIGFOX:
            global sig
            sig.sendmessage("ERRBME280")


def check_thermocouple():
    # Ogni 30 minuti rilevo
    print("Thermo Couple CHeck")
    import lib.max6675
    import time
    global temperature
    global groundtemperature

    """
    try:
        Pin(13, Pin.OUT).value(1)
        time.sleep(2)
        thermo = lib.max6675.MAX6675(25, 27, 14)
        temperature = thermo.readCelsius() - 2
        Pin(13, Pin.OUT).value(0) """
    import time
    import machine
    ## Local Imports
    import aadafruit_max31865 as max31865
    # dddi
    RTD_NOMINAL = 100.0  ## Resistance of RTD at 0C
    RTD_REFERENCE = 430.0  ## Value of reference resistor on PCB
    RTD_WIRES = 2  ## RTD 2 wires
    ## Create Software SPI controller.  MAX31865 requires polarity of 0 and phase of 1.
    ## Currently, the micropython on the ESP32 does not support hardware SPI
    try:
        # SE LA PT100 è COLLEGATA AL TRAP USARE QUESTI PIN
        mosi = machine.Pin(26, machine.Pin.IN)
        miso = machine.Pin(25, machine.Pin.OUT)

        # SE LA PT100 è COLLEGATA AL PLV USARE QUESTI PIN
        #mosi = machine.Pin(33, machine.Pin.IN) # PIN dove è collegato PLV
        #miso = machine.Pin(32, machine.Pin.OUT)

        sck = machine.Pin(14, machine.Pin.OUT)

        spi = machine.SoftSPI(baudrate=100000, sck=sck, mosi=mosi, miso=miso, polarity=0, phase=1)
        cs1 = machine.Pin(27, machine.Pin.OUT, value=1)

        sensore = max31865.MAX31865(
            spi, cs1,
            wires=RTD_WIRES,
            rtd_nominal=RTD_NOMINAL,
            ref_resistor=RTD_REFERENCE)

        Pin(13, Pin.OUT).value(1)
        time.sleep(3)
        lettura = sensore.temperature
        #  groundtemperature = sensore.temperature
        temperature = sensore.temperature
        print("tempmax=")
        print(lettura)
        # groundtemperature = thermo.readCelsius() - 2
        Pin(13, Pin.OUT).value(0)
    except Exception as e:
        print("errormessage: " + str(e))
        # pycom.rgbled(config.HEART_ALERT) # red
        #if config.WIFI:  #commentato questa parte perchè dà errore; la funzione sys.exc_info non è definita in MicroPython
            #import sys
            #wifi.sendmessage({'i2c': i2cscan, 'error': str(e), 'errormessage': str(sys.exc_info())})
        if config.SIGFOX:
            global sig
            sig.sendmessage("ERRBMTC")


def check_SHT31():
    # Ogni 30 minuti rilevo
    print("SHT31 CHeck")
    from lib.sht31 import SHT31
    if not (68 in i2cscan):
        print("------------------------")
        print("SHT31 CHIP NON PRESENTE")
        print("------------------------")
        return
    global groundtemperature
    global groundhumidity
    try:
        sht31 = SHT31(i2c=i2c)
        groundtemperature, groundhumidity = sht31.get_temp_humi()
        # print(result.value[0])
        # temperature = float(result.values[0])
        # humidity = result.values[1]
    except Exception as e:
        print("errormessage: " + str(e))
        #if config.WIFI:  #tolto questa parte perchè dà errore; la funzione sys.exc_info non è definita in MicroPython
            #import sys
            #wifi.sendmessage({'i2c': i2cscan, 'error': str(e), 'errormessage': str(sys.exc_info())})
        if config.SIGFOX:
            global sig
            sig.sendmessage("ERRSHT31")


def check_INA219():
    # Ogni 30 minuti rilevo
    print("INA219 CHeck")
    from lib.ina219 import INA219
    from logging import INFO
    if not (64 in i2cscan):
        print("------------------------")
        print("INA219 CHIP NON PRESENTE")
        print("------------------------")
        return
    # global temperature
    global irr
    try:
        from machine import Pin, I2C
        from lib.ina219 import INA219
        from logging import INFO

        SHUNT_OHMS = 0.1
        NUM_INA_READINGS = 10
        totcurrent = 0
        for j in range(NUM_INA_READINGS):
            ina = INA219(SHUNT_OHMS, i2c, log_level=INFO)
            ina.configure(ina.RANGE_16V, ina.GAIN_AUTO)
            current = ina.current()
            if current >= 0:
                totcurrent = totcurrent + current

            if config.DEBUG:
                print("Bus Voltage: %.3f V" % ina.voltage())
                print("Current: %.3f mA" % ina.current())
                print("Power: %.3f mW" % ina.power())
            time.sleep(0.8)
        irr = float(((totcurrent / NUM_INA_READINGS) * 100 // 1) / 100)
        # inserire equazione per il calcolo di irr
        irr = (((config.TARATURA_IRR * ((0.070182 * irr * irr) + (19.196 * irr)))) * 100 // 1 / 100)
        if irr > 1500:
            irr = 1500  #il range di misura dell'IRR è (0-1500)
    except Exception as e:
        print("errormessage: " + str(e))
        #if config.WIFI:
        #    import sys
        #    wifi.sendmessage({'i2c': i2cscan, 'error': str(e), 'errormessage': 'INA'})
        if config.SIGFOX:
            global sig
            sig.sendmessage("ERRINA")


def check_sgp30():
    print("SGP30 Times CHeck")
    # ogni 4 volte
    global sgp30livecount
    global keepalivecount      #riga aggiunta 19/09/24
    if (sgp30livecount > config.SGP30_TIME) :
       nvs.set_i32('sgp30livecount', 0)
       keepalivecount = 0
       return
    else:
       if sgp30livecount!=1:
           print('sgp30livecount', sgp30livecount)
           return
    print("SGP30 CHeck")
    import lib.uSGP30
    if not (88 in i2cscan):
        print("------------------------")
        print("SGP30 CHIP NON PRESENTE")
        print("------------------------")
        return

    global co2
    global tvoc
    global temperature
    global humidity
    co2eq=0
    try:
        x = 0
        sgp30 = lib.uSGP30.SGP30(i2c)
        print("SGP30---> INIT - Riscaldamento del sensore...")
        time.sleep(config.SGP30_WARNING_SECONDS)     #tempo necessario per riscaldare il sensore dopo l'inizializzazione
        #se vengono prese delle misure quando il sensore è freddo, spesso esce fuori un errore
        #per questo motivo, prima riscaldo il sensore, e dopo prendo le misure
        print("SGP30---> Misurazione...")
        while x<5:      #prima c'era config.SGP30_WARNING_SECONDS
            x=x+1
            if config.DEBUG:
                print("Waiting.."+str(x))
            time.sleep(1)
            co2eq, tvoc = sgp30.measure_iaq()
            time.sleep(1)  #aggiunto il 18-11
            if config.DEBUG:
                print(sgp30.measure_iaq())
        if (tvoc > 32765):        #valore molto alto che potrebbe essere un errore: misura un'altra volta per sicurezza
            while x<config.SGP30_WARNING_SECONDS:
                x=x+1
                if config.DEBUG:
                    print("Waiting.." + str(x))
                time.sleep(1)
                co2eq, tvoc = sgp30.measure_iaq()
                if config.DEBUG:
                    print(sgp30.measure_iaq())
        # nel caso che la CO2 non fosse già stata misurata
        if (co2 == 0):
            co2 = co2eq
    except Exception as e:
        print("errormessage: "+str(e))
        #sgp30livecount = 1    #ripeti la misura nella prossima trasmissione
        #if config.WIFI:  #tolto questa parte perchè dà errore; la funzione sys.exc_info non è definita in MicroPython
            #import sys
            #wifi.sendmessage({'i2c': i2cscan, 'error': str(e), 'errormessage': str(sys.exc_info())})


# def force_recalibration_SCD30(arg):
#     print("------------------------")
#     print("SCD30 FORCE RECALIBRATION")
#     print("------------------------")
#     from lib.scd30 import SCD30
#     scd30 = SCD30(i2c, 0x61)
#     scd30.soft_reset()
#     time.sleep(1)
#     scd30.set_forced_recalibration(1190)


def check_SCD30():
    print("SCD30 CHeck")


    import scd30
    global i2c
    global i2cscan
    print(i2c)
    print(i2c.scan())
    if not (97 in i2cscan):
        print("------------------------")
        print("SCD30 CHIP NON PRESENTE")
        print("------------------------")
        return


    global co2
    global temperature
    global humidity
    validcount = 0
    total_co2 = 0
    co2 = 0
    scd30 = scd30.SCD30(i2c, 97)


    if config.SCD30_FORCE_RECALIBRATON > 0 :
        print("FORCE RECALIBRATION : ",config.SCD30_FORCE_RECALIBRATON)
        scd30.set_forced_recalibration(config.SCD30_FORCE_RECALIBRATON)
        config.write_config("scd30forcerecal" ,0)

    # Wait for sensor data to be ready to read (by default every 2 seconds)
    while scd30.get_status_ready() != 1:
        print("SCD30 WARMUP")
        time.sleep(2)
    print("SCD30 READY")
    while validcount < 1 or co2 < 150 or math.isnan(co2):
        try:
            print("SCD30 READING")
            co2, temperature, humidity = scd30.read_measurement()
            time.sleep(2)

            # MEDIA DOPO 10 LETTURE
            if not math.isnan(co2):  #se il valore co2 non è NaN (ovvero se è un numero)
                validcount += 1
                total_co2 += co2
                print("--")
                print(co2)
        except Exception as e:
            from uio import StringIO
            import sys
            print("SCD30 - Error :")
            s = StringIO()
            sys.print_exception(e, s)
            print(s.getvalue)
            # pycom.rgbled(config.HEART_ALERT) # red
            if config.WIFI:
                wifi.sendmessage({'i2c': i2cscan, 'error': str(e), 'errormessage': s.getvalue})
    # co2 = round(total_co2 / validcount, 2)
    temperature = temperature + config.SCD30_TEMP_OFFSET
    print('---- SCD30 Result ----')
    print(co2)
    print(temperature)
    print(humidity)


def check_WET():
    print('WET')
    import wet_sensor
    global i2c
    global wet
    try:
        wetdevice = wet_sensor.WET(i2c, debug=False)

        N_MEAS = 5
        total_ch0_raw = [0, 0, 0, 0, 0]
        total_ch1_raw = [0, 0, 0, 0, 0]
        average_ch0_raw = [0, 0, 0, 0, 0]
        average_ch1_raw = [0, 0, 0, 0, 0]
        total_ch0_proc = 0
        total_ch1_proc = 0
        for j in range(N_MEAS):
            for i in range(N_MEAS):
                total_ch0_raw[j] += wetdevice.get_measure(0)
                total_ch1_raw[j] += wetdevice.get_measure(1)
            # index = j;
            average_ch0_raw[j] = round(total_ch0_raw[j] / N_MEAS)
            average_ch1_raw[j] = round(total_ch1_raw[j] / N_MEAS)
            total_ch0_proc += average_ch0_raw[j]
            total_ch1_proc += average_ch1_raw[j]

        average_ch0_proc = round(total_ch0_proc / N_MEAS)
        average_ch1_proc = round(total_ch1_proc / N_MEAS)

        k_drift = (average_ch1_proc - config.WET_REFERENCE_MEAS) / 1.1
        wetness_meas = config.WET_DRY_MEAS - average_ch0_proc + k_drift

        # Coefficienti del polinomio di grado 3 utilizzato per l'applicazione della funzione di trasferimento
        # "Misura Capacità -> Percentuale Bagnatura"
        # C1=2.1907916908e-06b
        # C2=-2.0970020644e-02
        # wet = int(((C1*(wetness_meas))+C2)*100)
        wet = int((wetness_meas * 100) / config.WET_DELTA_MEAS)
        if (wet < 0):
            wet = 0
        if (wet > 100):
            wet = 100

        if config.DEBUG:
            print(wet)
    except:
        print("Error reading WET")


def check_TRAP():
    print('TRAP')
    import trapsensor
    global trap
    global traperror
    # try:
    from machine import UART
    uart = UART(1, baudrate=9600, tx=26, rx=25)
    trappo = trapsensor.TRAP(uart=uart, debug=config.DEBUG)
    trap = trappo.get_status()
    trappo.reset_counter()
    time.sleep(2)
    trappo.calibration()
    time.sleep(2)
    trappo.start_autocalibration()
    # trap = trappo.get_status()
    traperror = trappo.getErrorCode()
    if traperror == b'00':
        print("----- NO ERROR-----")
    else:
        print("-----ERROR-----")
        print(traperror)


def check_PLV():
    print('---PLV---')
    import pluvisensor
    global plumeterresult
    global pluviometercount
    try:
        from machine import UART

        uart = UART(1, 9600, tx=33, rx=32)
        time.sleep(1)
        # uart.deinit()
        uart.init(9600, bits=8, parity=None, stop=1)

        pluviometer = pluvisensor.PLV(uart=uart, debug=config.DEBUG)
        pluviometercount = pluviometer.get_counter()
        plumeterresult = pluviometercount * config.PLUVI_COEF
        pluviometer.reset_counter()
        pluviometer.reboot()
    except Exception as e:
        print("Error reading PLV")

    time.sleep(3)


def check_PMSA003():
    # ogni 12 volte
    print('PMSA003')
    global dustkeepalivecount
    print('dust live')
    print(dustkeepalivecount)
    if (dustkeepalivecount > config.DUST_TIME):
        nvs.set_i32('dustlivecount', 0)  #sostituito esp32.NVS.nvs_set con nvs.set_i32, in quanto la prima funzione non esiste in MicroPython
        keepalivecount = 0
        if (config.DUST_TIME != 0):
            return
    else:
        if (dustkeepalivecount != 1 and config.DUST_TIME != 0):
            print('dustlivecount', dustkeepalivecount)
            return

    global pm1
    global pm2
    global pm10
    global dust
    print('init plantower')

    from pmsa003 import PMSA003
    try:
        # ttl1power=Pin('P9', mode=Pin.OUT, pull=Pin.PULL_UP, alt=-1).value(1)
        pms_sensor = PMSA003(machine.UART(1, baudrate=9600, pins=("P10", "P11")))
        print('plantower wakep')
        pms_sensor.wake_up()
        print('plantower measurements')
        pms_data = pms_sensor.measurements[1]
        print(pms_data)
        print('plantower OFF')
        pms_sensor.power_off()
        print('RISULTATO')
        pm1 = pms_data['cpm10']
        pm2 = pms_data['cpm25']
        pm10 = pms_data['cpm100']
        dust = pm10
    except:
        print("ERRORE IN FASE DI COLLEGAMENTO PMSA003")
    # SPENGI IL SENSORE POLVERE
    pmsapower = Pin('P9', mode=Pin.OUT, pull=None, alt=-1).value(0)


def status_battery():
    global battery
    NUM_ADC_READINGS = 30
    status = 0
    try:
        # create an ADC object
        adcP = ADC(Pin(34, Pin.IN))
        adcP.atten(ADC.ATTN_11DB)
        result = 0
        if earlyrestart:
            time.sleep(0.5)  # Nel caso di primo avvio aspetta mezzo secondo il condensatore
        for n in range(NUM_ADC_READINGS):
            result = result + adcP.read()
        # adcP.deinit()

        result = int((result / NUM_ADC_READINGS) // 1)
        resultvolt = (((0.001544715 * result) + 0.6486903) * 100 // 1) / 100
        battery = int(((result - 1500) / 7.6) // 1 + 1)
        if battery < 5:
            battery = 5
        if battery > 100:
            battery = 100
        if config.DEBUG:
            print("battery: ", result, resultvolt, battery)
    except:
        print("Error reading BATTERY")
        time.sleep(5)


def anemometer_check(arg):
    global anemotercount
    anemotercount = anemotercount + 1
    if config.DEBUG:
        print('ane')
        print(anemotercount)


def anemometer_result():
    # Per anemometro :(n°impulsi/2 )/t (sec) = giri/sec nell’intervallo di campionatura
    # In pratica ogni impulso dispari è un giro.
    # Giri/sec =  6.28rad/sec x 0.085m = m/sec velocità vento
    # Mettiamo anche un coefficiente di aggiustamento, per ora impostato a 1
    # Quindi:
    # 1(coefficiente correttivo) x 0.5(n°impulsi)x6.28x0.085/t(tempo campionamento in secondi)
    global anemotercount
    global anemoterresult
    # anemotercount = anemotercount
    # anemoterresult = (((config.ANE_COEF) * (anemotercount/4)*(6.28)*0.051)/config.ANE_SEC )
    anemoterresult = (((config.ANE_COEF) * (anemotercount / 4) * (6.28) * 0.080) / config.ANE_SEC)
    # ' SPENGI PIN '
    pinane = Pin(15, Pin.IN)  # ANEMOMETER
    # pinane.callback(Pin.IRQ_FALLING, None)
    print('-------- Anemometer Result -----------')
    print(anemoterresult)


def start_checking_anemometer():
    # Ogni 30 minuti rilevo per 10 secondi
    print(' ---- CHECK ANEMOMETER ----')
    pinane = Pin(15, Pin.IN)  # ANEMOMETER
    #    trigger is the type of event that triggers the callback. Possible values are:
    #    Pin.IRQ_FALLING interrupt on falling edge.
    #    Pin.IRQ_RISING interrupt on rising edge.
    #    Pin.IRQ_LOW_LEVEL interrupt on low level.
    #    Pin.IRQ_HIGH_LEVEL interrupt on high level.
    pinane.irq(trigger=Pin.IRQ_FALLING, handler=anemometer_check)
    anemoterstart = time.time()


def send_in_deepsleep():
    if not config.DEEPSLEEP:
        return
    # ---------------------------
    # ENTRATA IN DEEP SLEEP
    # ---------------------------
    ora = rtc.datetime()
    nvs.set_i32('anno', ora[0])
    nvs.set_i32('mese', ora[1])
    nvs.set_i32('giorno', ora[2])
    nvs.set_i32('ora', ora[3])
    nvs.set_i32('minuti', ora[4])
    nvs.set_i32('secondi', ora[5])
    nvs.commit()

    # prima = [2099, 1, 1, 0, 0, 0,  0, None]
    print('ENTERED IN DEEP SLEEP...')
    machine.deepsleep(config.DEEPSLEEP_SECONDS * 1000)


def check_handler(button):
    print('... checking ...')
    # ------------------------------------------------
    # Controllo livello batteria
    # ------------------------------------------------
    global battery
    status_battery()
    #  I2C
    if (119 in i2cscan):  # 118 vecchi ; 119 cecchi
        check_bme280()
    if (97 in i2cscan):
        check_SCD30()
    if (88 in i2cscan):
        check_sgp30()
    if (43 in i2cscan):
        check_WET()
    if (64 in i2cscan):
        check_INA219()
    if (68 in i2cscan):
        check_SHT31()
    # SPENGI LINEA I2C
    # i2c.deinit()
    if (not firststart):
        Pin(13, Pin.OUT).value(0)
    else:
        if (bluetooth == None):
            Pin(13, Pin.OUT).value(0)

    # ANALISI TEMPERATURA TRAMITE TERMOCOPPIA
    if (config.TC):
        check_thermocouple()

    # ANALISI POLVERI
    if (config.PMSA003):
        check_PMSA003()
    # Insetti
    if config.TRAP:
        check_TRAP()
    #  PLUVIOMETRO
    if config.PLV:
        check_PLV()
    if (config.ANE):
        pinanepower = Pin(2, Pin.OUT)  # POWER ANE
        pinanepower.value(1)
        start_checking_anemometer()
        time.sleep(config.ANE_SEC)
        anemometer_result()
        pinanepower.value(0)

    # ----------------------------
    #  piccole correzioni
    # ----------------------------
    global humidity
    if float(str(humidity)) > 100:
        humidity = 100

    print('i2c: ', i2cscan)
    print('Battery: ', battery)
    print('Temperature: ', temperature)
    print('Humidity: ', humidity)
    print('Pressure: ', pressure)
    print('Wet: ', wet)
    print('ANE: ', anemoterresult)
    print('Trap: ', trap)
    print('PLV: ', plumeterresult)
    print('CO2: ', co2)
    print('TVOC: ', tvoc)
    print('Dust: ', dust)
    print('Pm1: ', pm1)
    print('Pm2: ', pm2)
    print('Pm4: ', pm4)
    print('Pm10: ', pm10)
    print('Irr: ', irr)
    print('Ground Temperature: ', groundtemperature)
    print('Ground Humidity: ', groundhumidity)

    # pycom.heartbeat(False)
    if config.WIFI: #and not firststart:  #commentato la seconda parte della riga perchè voglio inviare il messaggio wifi anche al primo avvio
        data = {'battery': battery, 'temperature': temperature, 'humidity': humidity, "pressure": pressure, }
        if (97 in i2cscan):  # SCD30
            data.update({'co2': co2, })
        if (88 in i2cscan):  # SGP30
            data.update({'co2': co2, 'tvoc': tvoc, })
        if (43 in i2cscan):
            data.update({'wet': wet, })
        if (config.TRAP):
            data.update({'trap': trap})
        if (config.PLV):
            data.update({'rain': plumeterresult})
        if (config.ANE):  # ANEMOMETER
            data.update({'wind': anemoterresult})
        if (config.PMSA003):
            data.update({'dust': dust, 'pm1': pm1, 'pm2': pm2, 'pm10': pm10, })
        if (68 in i2cscan):  # SHT31
            data.update({'groundtemperature': groundtemperature, 'groundhumidity': groundhumidity})
        if (64 in i2cscan):  # INA
            data.update({'irr': irr})
        data.update({'i2c': i2cscan})
        gc.collect()
        wifi.sendmessage(sigfoxid, data)


    if config.SIGFOX:
        gc.collect()
        global sig
        if (97 in i2cscan) or (config.SCD30):
            sig.sendSCD30(temperature, humidity, co2)
        else:
            sig.sendAmbient(temperature, humidity, pressure, co2, tvoc, battery)

        if ((43 in i2cscan) or config.TRAP or config.PLV or config.ANE or config.PMSA003):
            sensor = 0
            if (43 in i2cscan):
                sensor = 1
            if (config.TRAP):
                sensor = sensor + 2
            if (config.PLV):
                sensor = sensor + 4
            if (config.ANE):
                sensor = sensor + 8
            if (config.PMSA003):
                sensor = sensor + 16

            # sig = sigfox.SIGFOX(None, False)
            sig.sendWTRainWindPM(sensor, wet, trap, plumeterresult, anemoterresult, pm10, traperror)

        if (64 in i2cscan) or (68 in i2cscan):  # SHT30 o INA
            sensor = 0
            if (68 in i2cscan):  # SHT31
                sensor = 1
            if (64 in i2cscan):  # INA
                sensor = sensor + 2

            # sig = sigfox.SIGFOX(None, False)
            sig.sendGroundAmbient(sensor, groundtemperature, groundhumidity, irr)


# ------------------------------------------------
#  CONTROLLI!!
# ------------------------------------------------
print('--- INIT CHECKING ---')
check_handler(None)
print('--- END CHECKING ---')

# ------------------------------------------------
#            SLEEP
# ------------------------------------------------
print('ENTER IN SLEEP')
if (config.ANE):
    time.sleep(config.WAIT_SECONDS - config.ANE_SEC)
else:
    time.sleep(config.WAIT_SECONDS)
print('EXIT FROM SLEEP')
# ------------------------------------------------
#            END PROCEDURE
# ------------------------------------------------
print('----   THE END ----')

# ------------------------------------------------
#            DEEP SLEEP
# ------------------------------------------------
if (config.DEEPSLEEP):  #nota: deve essere sempre attivo; deepsleepseconds deve essere sempre maggiore di waitseconds
    send_in_deepsleep()
    #if (firststart):
    #    if (bluetooth != None):
    #        send_in_deepsleep()
    #else:
    #    send_in_deepsleep()
