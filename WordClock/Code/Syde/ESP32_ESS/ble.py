# This example demonstrates a simple temperature sensor peripheral.
#
# The sensor's local value updates every second, and it will notify
# any connected central every 10 seconds.
import bluetooth
from micropython import const
import config
import struct
import time

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_GATTS_READ_REQUEST = const(4)
_IRQ_SCAN_RESULT = const(5)
_IRQ_SCAN_DONE = const(6)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
_IRQ_GATTC_DESCRIPTOR_DONE = const(14)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_GATTC_INDICATE = const(19)
_IRQ_GATTS_INDICATE_DONE = const(20)

_FLAG_BROADCAST = const(0x0001)
_FLAG_READ = const(0x0002)
_FLAG_WRITE_NO_RESPONSE = const(0x0004)
_FLAG_WRITE = const(0x0008)
_FLAG_NOTIFY = const(0x0010)
_FLAG_INDICATE = const(0x0020)
_FLAG_AUTHENTICATED_SIGNED_WRITE = const(0x0040)

# Advertising payloads are repeated packets of the following form:
#   1 byte data length (N + 1)
#   1 byte type (see constants below)
#   N bytes type-specific data

_ADV_TYPE_FLAGS = const(0x01)
_ADV_TYPE_NAME = const(0x09)
_ADV_TYPE_UUID16_COMPLETE = const(0x3)
_ADV_TYPE_UUID32_COMPLETE = const(0x5)
_ADV_TYPE_UUID128_COMPLETE = const(0x7)
_ADV_TYPE_UUID16_MORE = const(0x2)
_ADV_TYPE_UUID32_MORE = const(0x4)
_ADV_TYPE_UUID128_MORE = const(0x6)
_ADV_TYPE_APPEARANCE = const(0x19)

# org.bluetooth.service code
_SYDE_UUID = bluetooth.UUID(0x0000)
#_SYDE_UUID = bluetooth.UUID('0000000000000000')

# INFO Charatteristic
_VERSIONE = (bluetooth.UUID('0000000000000100') , _FLAG_READ | _FLAG_WRITE ,)
# Connection Charatteristic
_SIGFOX = (bluetooth.UUID('0000000000000210')   , _FLAG_READ | _FLAG_WRITE ,)
_WIFI = (bluetooth.UUID('0000000000000230')     , _FLAG_READ | _FLAG_WRITE ,)
_WIFI_SSID = (bluetooth.UUID('0000000000000240'), _FLAG_READ | _FLAG_WRITE ,)
_WIFI_PASS = (bluetooth.UUID('0000000000000250'), _FLAG_READ | _FLAG_WRITE ,)
# SENSOR Charatteristic
_ANE = (bluetooth.UUID('0000000000000310')   , _FLAG_READ | _FLAG_WRITE ,)
_PLV = (bluetooth.UUID('0000000000000320')   , _FLAG_READ | _FLAG_WRITE ,)
_TRAP = (bluetooth.UUID('0000000000000330')   , _FLAG_READ | _FLAG_WRITE ,)
_POW = (bluetooth.UUID('0000000000000340')   , _FLAG_READ | _FLAG_WRITE ,)
_WET = (bluetooth.UUID('0000000000000350')   , _FLAG_READ | _FLAG_WRITE ,)

# SETTINGS Charatteristic
_DEBUG = (bluetooth.UUID('0000000000000410')   , _FLAG_READ | _FLAG_WRITE ,)
_SGP30 = (bluetooth.UUID('0000000000000450')   , _FLAG_READ | _FLAG_WRITE ,)
_DUST = (bluetooth.UUID('0000000000000460')   , _FLAG_READ | _FLAG_WRITE ,)
_WAIT = (bluetooth.UUID('0000000000000480')   , _FLAG_READ | _FLAG_WRITE ,)
_DEEP = (bluetooth.UUID('0000000000000495')   , _FLAG_READ | _FLAG_WRITE ,)


_SYDE_SERVICE = (
    _SYDE_UUID,
    (_VERSIONE, _SIGFOX, _WIFI, _WIFI_SSID, _WIFI_PASS, _ANE, _PLV, _TRAP, _POW, _WET, _DEBUG, _SGP30, _DUST, _WAIT, _DEEP),
)


class BLES:
    _bleactive = True
    _i2c=None

    def __init__(self, ble=None, name=config.BLE_NAME+' '):
        if ble==None:
            self._ble = bluetooth.BLE()
        else:
            self._ble = ble

        self._ble.active(True)
        self._bleactive = True
        self._ble.irq(self._irq)
        (_VERSIONE, _SIGFOX, _WIFI, _WIFI_SSID, _WIFI_PASS, _ANE, _PLV, _TRAP, _POW, _WET, _DEBUG, _SGP30, _DUST,
         _WAIT, _DEEP),
        ((self._version, self._sigfox, self._wifi, self._wifi_ssid, self._wifi_pswd,
          self._ane, self._plv, self._trap, self._pow, self._wet, self._debug, self._sgp30, self._dust,
          self._wait, self._deep,),) \
            = self._ble.gatts_register_services((_SYDE_SERVICE,))

        self.set_version(config.VERSION)

        if (config.SIGFOX):
            self.set_sigfox(1)
        else:
            self.set_sigfox(0)

        if (config.WIFI):
            self.set_wifi(1)
        else:
            self.set_wifi(0)

        self.set_wifi_ssid(config.WIFI_SSID)
        self.set_wifi_pswd(config.WIFI_PASS)


        if (config.ANE):
            self.set_ane(1)
        else:
            self.set_ane(0)

        if (config.PLV):
            self.set_plv(1)
        else:
            self.set_plv(0)

        if (config.TRAP):
            self.set_trap(1)
        else:
            self.set_trap(0)

        if (config.PMSA003):
            self.set_pow(1)
        else:
            self.set_pow(0)

        if (config.WETSET):
            self.set_drywet(1)
        else:
            self.set_drywet(0)

       # if (config.WETSET):
     #       self.set_wet_ref(1)
      #  else:
       #     self.set_wet_ref(0)

        if (config.DEBUG):
            self.set_debug(1)
        else:
            self.set_debug(0)

        self.set_sgp30(config.SGP30_TIME)
        self.set_dust(config.DUST_TIME)
        self.set_wait(config.WAIT_SECONDS)
        self.set_deep(config.DEEPSLEEP_SECONDS)


        self._connections = set()
        self._payload = self.advertising_payload(name=name, services=[_SYDE_UUID])
        self._advertise()
        print("BLE START ADVERTISE")

        global CONNESSIONE
        CONNESSIONE = False

        print("Secondi a disposizione per connetterti con il bluetooth: ", config.BLE_OPEN_SECONDS)
        time.sleep(config.BLE_OPEN_SECONDS)  #tempo di attesa dopo il quale il bluetooth si stacca se non ti connetti (solo al primo riavvio del sensore)

        if CONNESSIONE == False:  #se non ti sei connesso, disattiva il bluetooth
            print("Connessione: ", CONNESSIONE)
            self._ble.active(False)
            self._bleactive = False
            print("BLE CLOSED")
        else:  #se ti sei connesso, resetta la variabile CONNESSIONE su False e dai un certo tempo per eseguire le operazioni sull'app
            print("Connessione: ", CONNESSIONE)
            CONNESSIONE = False
            print("Hai 5 minuti per eseguire le operazioni sull'app...")
            time.sleep(300)  #se sfori questo tempo, il sensore si riavvierà da solo
            import machine
            machine.reset()


    def setI2C(self, i2c):
        self._i2c = i2c


    def close(self):
        self._ble.active(False)
        self._bleactive = False
        print("BLE CLOSED")

    def _irq(self, event, data):
        # Track connections so we can send notifications.
        if event == _IRQ_CENTRAL_CONNECT:
            conn_handle, _, _ = data
            self._connections.add(conn_handle)
            print("CONNECTED")
            global CONNESSIONE
            CONNESSIONE = True
        elif event == _IRQ_CENTRAL_DISCONNECT:
            conn_handle, _, _ = data
            self._connections.remove(conn_handle)
            print("DISCONNECTED")

            # Restart AFTER disconnect
            import machine
            machine.reset()
        elif event == _IRQ_GATTC_WRITE_DONE:
            print("WRITE")
            # A gattc_write() has completed.
            # Note: The value_handle will be zero on btstack (but present on NimBLE).
            # Note: Status will be zero on success, implementation-specific value otherwise.
            conn_handle, value_handle, status = data
        elif event == _IRQ_GATTS_WRITE:
            print("WRITING........")
            conn_handle, attr_handle = data
            if (attr_handle==self._sigfox):
                if (self.read_sigfox()==b'\x01'):
                    print('WRITE CONFIG SIGFOX--> 1')
                    config.write_config('sigfox', True)
                else:
                    print('WRITE CONFIG SIGFOX--> 0')
                    config.write_config('sigfox', False)
                return

            if (attr_handle == self._wifi):
                if (self.read_wifi()==b'\x01'):
                    print('WRITE CONFIG WIFI--> 1')
                    config.write_config('wifi', True)
                else:
                    print('WRITE CONFIG WIFI--> 0')
                    config.write_config('wifi', False)
                return

            if (attr_handle == self._wifi_ssid):
                print('WRITE CONFIG SSID')
                buffer = self._ble.gatts_read(self._wifi_ssid)
                message = buffer.decode('UTF-8').strip()
                print (message)
                config.write_config("wifissid", message)
                return
            if (attr_handle == self._wifi_pswd):
                print('WRITE CONFIG PASSWORD')
                buffer = self._ble.gatts_read(self._wifi_pswd)
                message= buffer.decode('UTF-8').strip()
                config.write_config("wifipassword", message)
                return
            if (attr_handle == self._ane):
                if (self.read_ane() == b'\x01'):
                    print('WRITE CONFIG ANE--> 1')
                    config.write_config('ane', True)
                else:
                    print('WRITE CONFIG ANE--> 0')
                    config.write_config('ane', False)
                return
            if (attr_handle == self._plv):
                if (self.read_plv() == b'\x01'):
                    print('WRITE CONFIG PLV--> 1')
                    config.write_config('plv', True)
                else:
                    print('WRITE CONFIG PLV--> 0')
                    config.write_config('plv', False)
                return
            if (attr_handle == self._trap):
                if (self.read_trap() == b'\x01'):
                    print('WRITE CONFIG TRAP--> 1')
                    config.write_config('trap', True)
                else:
                    print('WRITE CONFIG TRAP--> 0')
                    config.write_config('trap', False)
                return
            if (attr_handle == self._pow):
                if (self.read_pow() == b'\x01'):
                    print('WRITE CONFIG PM--> 1')
                    config.write_config('pm', True)
                else:
                    print('WRITE CONFIG PM--> 0')
                    config.write_config('pm', False)
                return
            if (attr_handle == self._wet):
                if(self.read_drywet()== b'\x02'):
                    print("calcola il dry")
                    import wet_sensor
                    #try:
                    wetdevice = wet_sensor.WET(self._i2c, debug=False)
                    wetdevice.set_dry_ref()
                    #except:
                    #    print("Errore in fase di collegamento WET x DRY")
                    return

                if (self.read_drywet()== b'\x03'):
                    print("calcola il wet")
                    import wet_sensor
                    try:
                        wetdevice = wet_sensor.WET(self._i2c, debug=False)
                        wetdevice.set_wet_ref()
                    except:
                        print("Errore in fase di collegamento WET x Wet")

            if (attr_handle == self._debug):
                if (self.read_debug() == b'\x01'):
                    print('WRITE CONFIG DEBUG--> 1')
                    config.write_config('debug', True)
                else:
                    print('WRITE CONFIG DEBUG--> 0')
                    config.write_config('debug', False)
                return
            if (attr_handle == self._sgp30):
                print('WRITE CONFIG SGP30')
                buffer = self._ble.gatts_read(self._sgp30)
                message = buffer.decode('UTF-8').strip()
                print(message)
                config.write_config("sgp30time", int(message))
                return
            if (attr_handle == self._dust):
                print('WRITE CONFIG DUST')
                buffer = self._ble.gatts_read(self._dust)
                message = buffer.decode('UTF-8').strip()
                print(message)
                config.write_config("dusttime", int(message))
                return

            if (attr_handle == self._wait):
                print('WRITE CONFIG WAIT')
                buffer = self._ble.gatts_read(self._wait)
                message = buffer.decode('UTF-8').strip()
                print(message)
                config.write_config("waitseconds", int(message))
                return
            if (attr_handle == self._deep):
                print('WRITE CONFIG DEEP')
                buffer = self._ble.gatts_read(self._deep)
                message = buffer.decode('UTF-8').strip()
                print(message)
                config.write_config("deepsleepseconds", int(message))
                return
        elif event == _IRQ_GATTC_READ_RESULT:
            print("READ RESULT")
            # A read completed successfully.
            conn_handle, value_handle, char_data = data
        elif event == _IRQ_GATTC_READ_DONE:
            # print("READ")
            # A gattc_read() has completed.
            # Note: The value_handle will be zero on btstack (but present on NimBLE).
            # Note: Status will be zero on success, implementation-specific value otherwise.
            conn_handle, value_handle, status = data
    def _advertise(self, interval_us=500000):  #è consigliato un valore più basso, per es. 100000 ms
        self._ble.gap_advertise(interval_us, adv_data=self._payload)

    def advertising_payload(limited_disc=False, br_edr=False, name=None, services=None, appearance=0):  #consigliato "self" al posto di "limited_disc=False, br_edr=False"
        payload = bytearray()

        def _append(adv_type, value):
            nonlocal payload
            payload += struct.pack("BB", len(value) + 1, adv_type) + value

        _append(
            _ADV_TYPE_FLAGS,
            struct.pack("B", (0x01 if limited_disc else 0x02) + (0x18 if br_edr else 0x04)),  #consigliato "0x06" al posto di "(0x01 if limited_disc else 0x02) + (0x18 if br_edr else 0x04)"
        )

        if name:
            _append(_ADV_TYPE_NAME, name)  #consigliato "name.encode('utf-8')" al posto di "name"

        if services:
            for uuid in services:
                b = bytes(uuid)
                if len(b) == 2:
                    _append(_ADV_TYPE_UUID16_COMPLETE, b)
                elif len(b) == 4:
                    _append(_ADV_TYPE_UUID32_COMPLETE, b)
                elif len(b) == 16:
                    _append(_ADV_TYPE_UUID128_COMPLETE, b)
        # See org.bluetooth.characteristic.gap.appearance.xml
        if appearance:
            _append(_ADV_TYPE_APPEARANCE, struct.pack("<h", appearance))
        return payload

    def read_sigfox(self):
        return self._ble.gatts_read(self._sigfox)
    def read_wifi(self):
        return self._ble.gatts_read(self._wifi)
    def read_wifi_ssid(self):
        return self._ble.gatts_read(self._wifi_ssid)
    def read_wifi_pswd(self):
        return self._ble.gatts_read(self._wifi_pswd)
    def read_ane(self):
        return self._ble.gatts_read(self._ane)
    def read_plv(self):
        return self._ble.gatts_read(self._plv)
    def read_trap(self):
        return self._ble.gatts_read(self._trap)
    def read_pow(self):
        return self._ble.gatts_read(self._pow)
    def read_drywet(self):
        return self._ble.gatts_read(self._wet)
    def read_debug(self):
        return self._ble.gatts_read(self._debug)
    def read_sgp30(self):
        return self._ble.gatts_read(self._sgp30)
    def read_dust(self):
        return self._ble.gatts_read(self._dust)
    def read_wait(self):
        return self._ble.gatts_read(self._wait)
    def read_deep(self):
        return self._ble.gatts_read(self._deep)
    def set_version(self, version):
        if self._bleactive:
            self._ble.gatts_write(self._version, struct.pack("<h", version))
    def set_sigfox(self, sigfox):
        if self._bleactive:
            self._ble.gatts_write(self._sigfox, struct.pack("<h", sigfox))
    def set_wifi(self, wifi):
        if self._bleactive:
            self._ble.gatts_write(self._wifi, struct.pack("<h", wifi))
    def set_wifi_ssid(self, wifissid):
        if self._bleactive:
            self._ble.gatts_write(self._wifi_ssid, wifissid)
    def set_wifi_pswd(self, wifipswd):
        if self._bleactive:
            self._ble.gatts_write(self._wifi_pswd, wifipswd)
    def set_ane(self, ane):
        if self._bleactive:
            self._ble.gatts_write(self._ane, struct.pack("<h", ane))
    def set_plv(self, plv):
        if self._bleactive:
            self._ble.gatts_write(self._plv, struct.pack("<h", plv))
    def set_trap(self, trap):
        if self._bleactive:
            self._ble.gatts_write(self._trap, struct.pack("<h", trap))
    def set_pow(self, pow):
        if self._bleactive:
            self._ble.gatts_write(self._pow, struct.pack("<h", pow))
    def set_drywet(self, wet):
        if self._bleactive:
            self._ble.gatts_write(self._wet, struct.pack("<h", wet))
    def set_debug(self, debug):
        if self._bleactive:
            self._ble.gatts_write(self._debug, struct.pack("<h", debug))
    def set_sgp30(self, sgp30):
        if self._bleactive:
            self._ble.gatts_write(self._sgp30, struct.pack('<H', int(sgp30)))
    def set_dust(self, dust):
        if self._bleactive:
            self._ble.gatts_write(self._dust, struct.pack('<H', int(dust)))
    def set_wait(self, wait):
        if self._bleactive:
            self._ble.gatts_write(self._wait, struct.pack('<H', int(wait)))
    def set_deep(self, deep):
        if self._bleactive:
            self._ble.gatts_write(self._deep, struct.pack('<H', int(deep)))
