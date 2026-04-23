from machine import Pin, UART
import time
import ubinascii
import config


class SIGFOX(object):
    _sigfox_id = ""
    _sigfox_pack = ""

    def __init__(self, uart, debug=False):
        # turno on pin power
        sigfoxpower = Pin(18, Pin.OUT)
        sigfoxpower.value(0)
        time.sleep(1)
        sigfoxpower.value(1)
        Pin(4, Pin.OUT).value(1)
        Pin(19, Pin.OUT).value(0)
        Pin(23, Pin.OUT).value(0)
        # time.sleep(1)

        # turn on uart power
        if (uart == None):
            self._uart = UART(2, 9600, tx=17, rx=16, timeout=5000)
            self._uart.init(baudrate=9600, bits=8, parity=None, stop=1)
        else:
            self._uart = uart

        # self.getid()
        # self.getpac()

    def getid(self):
        id = self._getuartcmd('AT$I=10\r')
        id = id.decode('utf8')
        while ("error" in id.lower()):
            print("errparse--->", id)
            id = self._getuartcmd('AT$I=10\r')
            id = id.decode('utf8')
        id = id.lstrip('0').rstrip('\n').rstrip('\r')
        print("SIGFOX ID : " + id)
        return id
        # print("SIGFOX ID : " + str(self._uart.read()))

    def getpac(self):
        print('GET PAC')
        print("PAC : " + str(self._getuartcmd('AT$I=11\r').decode('utf8')))

    def getsigfoxlibrary(self):
        print('GET SIGFOX LIBRARY')
        print("SIGFOX LIBRARY : " + str(self._getuartcmd('AT$I=9\r').decode('utf8')))

    def _getuartcmd(self, cmd):
        self._uart.write(cmd)
        time.sleep(1)
        retry = 0
        ' CICLO IN ATTESA DI ATTIVAZIONE SENSORE '
        while (self._uart.any() < 1) and (retry < 10):
            retry = retry + 1
            print("retry", retry)
            self._uart.write(cmd)
            time.sleep(1)
        return self._uart.read()

    # send some bytes
    def sendmessage(self, message):
        print('Sending SIGFOX message : ' + str(message))
        self._uart.write("AT$SF=")
        self._uart.write(ubinascii.hexlify(message))
        self._uart.write("\r")
        print('Sended SIGFOX message : ' + str(message))

    def sendBytesmessage(self, message):
        print('Sending SIGFOX message : ' + str(message))
        self._uart.write("AT$SF=")
        self._uart.write(message.hex())
        self._uart.write("\r")
        print('Sended SIGFOX message : ' + str(message))

        # time.sleep(60)

        while (self._uart.any() < 1):
            print('attendere prego')
            time.sleep(1)
        print("SIGFOX RETURN : " + str(self._uart.read()))

    def sendSCD30(self, temperature, humidity, co2):
        try:
            # Elimina i dati a destra dela virgola(.) se presente
            if temperature < 0:
                signtemperature = 1
            else:
                signtemperature = 0
            inttemperature = int(abs(temperature) // 1)
            digittemperature = int((abs(temperature) % 1) * 100 // 1)

            # humidity
            inthumidity = int(humidity // 1)
            digithumidity = int((humidity % 1) * 100 // 1)
            # co2
            intco2 = int(co2 // 1)
            if intco2 < 255:
                intco201 = int(0)
                intco202 = int(intco2)
            else:
                intco201 = int(intco2 // 256)
                intco202 = int(intco2 % 256)
            digitco2 = int((co2 % 1) * 100 // 1)
            # time.sleep(10)
            print('SIGFOX SEND SCD30')
            self.sendBytesmessage(bytes(
                [3, signtemperature, inttemperature, digittemperature, inthumidity, digithumidity, intco201, intco202,
                 digitco2]))
        except:
            self.sendmessage("ERRSCD30")

    def sendAmbient(self, temperature, humidity, pressure, co2, tvoc, battery):
        error = ""
        # try:
        # segno su temperatura
        error = "TMP"
        temperature = str(temperature)
        if temperature.find('.') >= 0:
            inttemperature = int(temperature[:temperature.find('.')])
            digitemperature = int(str(temperature)[str(temperature).find('.') + 1:])
        else:
            inttemperature = int(float(temperature) // 1)
            digitemperature = 0
        # segno su temperatura
        error = "TMPD"
        if inttemperature < 0:
            inttemperature = 200 + (-inttemperature)
        # humidity
        error = "HUM"
        inthumidity = int(float(humidity) // 1)
        # pressure
        error = "PRES"
        pressure = str(pressure)
        if pressure.find('.') >= 0:
            intpressure = int(pressure[:pressure.find('.')])
            digitpressure = int(str(pressure)[str(pressure).find('.') + 1:])
        else:
            intpressure = int(float(pressure) // 1)
            digitpressure = 0
        if intpressure < 255:
            intpressure01 = 0
            intpressure02 = intpressure
        else:
            intpressure01 = intpressure // 256
            intpressure02 = intpressure % 256
        # co2
        error = "CO2"
        intco2 = int(float(co2) // 1)
        if intco2 < 255:
            intco201 = 0
            intco202 = intco2
        else:
            intco201 = intco2 // 256
            intco202 = intco2 % 256
        # tvoc
        error = "VOC"
        inttvoc = int(float(tvoc) // 1)
        if inttvoc < 255:
            inttvoc01 = 0
            inttvoc02 = inttvoc
        else:
            inttvoc01 = inttvoc // 256
            inttvoc02 = inttvoc % 256
        # Send Messaggio
        inttemperature = int(inttemperature // 1) % 256
        inthumidity = int(inthumidity // 1) % 256
        intpressure01 = int(intpressure01 // 1) % 256
        intpressure02 = int(intpressure02 // 1) % 256
        intco201 = int(intco201 // 1) % 256
        intco202 = int(intco202 // 1) % 256
        inttvoc01 = int(inttvoc01 // 1) % 256
        inttvoc02 = int(inttvoc02 // 1) % 256
        digitemperature = int(digitemperature // 1) % 256
        digitpressure = int(digitpressure // 1) % 256
        battery = int(battery // 1) % 256
        # time.sleep(10)
        print('SIGFOX SEND AMBIENT')
        self.sendBytesmessage(bytes(
            [10, inttemperature, inthumidity, intpressure01, intpressure02, intco201, intco202, inttvoc01,
             inttvoc02, digitemperature, digitpressure, battery]))
        # except:
        #     self.sendmessage("ERRSAMBINVIO")

    def sendGroundAmbient(self, sensor, temperature, humidity, irr):
        error = ""
        try:
            # segno su temperatura
            # interror = int.from_bytes(error[1], "big")
            error = "TEMP"
            temperature = str(temperature)
            if temperature.find('.') >= 0:
                inttemperature = int(temperature[:temperature.find('.')])
                digittemperature = int(str(temperature)[str(temperature).find('.') + 1:str(temperature).find('.') + 3])
            else:
                inttemperature = int(float(temperature) // 1)
                digittemperature = 0
            # segno su temperatura
            error = "TEMPS"
            if inttemperature < 0:
                inttemperature = 200 + (-inttemperature)
            # humidity
            error = "HUM"
            inthumidity = int(float(humidity) // 1)
            # irr
            error = "IRR"
            irr = str(irr)
            if irr.find('.') >= 0:
                intirr = int(irr[:irr.find('.')])
                digitirr = int(str(irr)[str(irr).find('.') + 1:])
            else:
                intirr = int(float(irr) // 1)
                digitirr = 0
            error = "IRR2"
            if intirr < 255:
                intirr01 = 0
                intirr02 = intirr
            else:
                intirr01 = intirr // 256
                intirr02 = intirr % 256

            # Send Messaggio
            inttemperature = int(inttemperature // 1) % 256
            digittemperature = int(digittemperature // 1) % 256
            inthumidity = int(inthumidity // 1) % 256
            intirr01 = int(intirr01 // 1) % 256
            intirr02 = int(intirr02 // 1) % 256
            digitirr = int(digitirr // 1) % 256
            # time.sleep(10)
            print('SIGFOX SEND GROUND AMBIENT')
            self.sendBytesmessage(
                bytes([9, inttemperature, digittemperature, inthumidity, intirr01, intirr02, digitirr, sensor]))
        except:
            self.sendmessage("ERRGRND" + error)

    def sendWTRainWindPM(self, sensor, wet, trap, plumeterresult, anemoterresult, pm10, traperror):
        # wet/trap/pluvi/ane/pm10
        try:
            # Wet
            intwet = int(wet // 1)
            # TRAP
            inttrap = int(trap // 1)
            # inttraperror = int.from_bytes(traperror[1], "big")
            traperror = traperror[1] - 48
            # PLUVI
            intplumeterresult = int(float(plumeterresult) // 1)
            digitplumeterresult = int((plumeterresult % 1) * 100 // 1)
            # ANE
            intanemoterresult = int(float(anemoterresult) // 1)
            digitanemoterresult = int((anemoterresult % 1) * 100 // 1)
            # PM10
            pm10 = str(pm10)
            if pm10.find('.') >= 0:
                intpm10 = int(pm10[:pm10.find('.')])
            else:
                intpm10 = int(pm10)
            if intpm10 < 255:
                intpm1001 = 0
                intpm1002 = intpm10
            else:
                intpm1001 = intpm10 // 256
                intpm1002 = intpm10 % 256

            # Send Messaggio
            intwet = int(intwet // 1) % 256
            inttrap = int(inttrap // 1) % 256
            intplumeterresult = int(intplumeterresult // 1) % 256
            digitplumeterresult = int(digitplumeterresult // 1) % 256
            intanemoterresult = int(intanemoterresult // 1) % 256
            digitanemoterresult = int(digitanemoterresult // 1) % 256
            intpm1001 = int(intpm1001 // 1) % 256
            intpm1002 = int(intpm1002 // 1) % 256
            # time.sleep(10)
            print('SIGFOX SEND TRAP_RAIN_WIND')
            self.sendBytesmessage(bytes(
                [12, sensor, intwet, inttrap, intplumeterresult, digitplumeterresult, intanemoterresult,
                 digitanemoterresult, intpm1001, intpm1002, traperror]))
        except:
            self.sendmessage("ERR WTRWP")