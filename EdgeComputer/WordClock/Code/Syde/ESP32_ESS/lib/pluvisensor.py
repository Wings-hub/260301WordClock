import time
import config
import ubinascii


# Enable Low Power Mode            -> 12-10-00-00-00-00-22
# Disable Low Power Mode           -> 12-10-01-00-00-00-23
# Get Counter                      -> 12-11-00-00-00-00-23
# Get Diagnostic                   -> 12-12-00-00-00-00-24
# Clear Error                      -> 12-13-00-00-00-00-25
# Reset Counter                    -> 12-14-00-00-00-00-26
# Reboot Pluviometer               -> 12-20-00-00-00-00-32
ENABLE_LOW_POWER = (b'\x23\x10\x01\x00\x00\x00\x34') #23100100000034
DISABLE_LOW_POWER = (b'\x23\x10\x00\x00\x00\x00\x03') #2310000000003
GET_COUNTER = (b'\x23\x11\x00\x00\x00\x00\x34') #23110000000034
GET_DIAGNOSTIC = (b'\x23\x12\x00\x00\x00\x00\x35') #23120000000035
CLEAR_ERROR = (b'\x23\x13\x00\x00\x00\x00\x36') #23130000000036
RESET_COUNTER = (b'\x23\x14\x00\x00\x00\x00\x37') #23140000000037'
REBOOT = (b'\x23\x20\x00\x00\x00\x00\x43') #23200000000043'

#GET_COUNTER = (b'\x01\x02\x03\x04')
# GET_COUNTER = ('AAAA')
# RESET_COUNTER = ('BBBB')

class PLV:
    # index of list
    # float array of values
    def __init__(self, uart=1, debug=False):
        self._debug = debug
        self._uart = uart
        #self.reboot()

    def reboot(self):
        print('PLV restart')
        time.sleep(1)
        self._uart.write(REBOOT)
        time.sleep(1)

    def reset_counter(self):
        print('PLV reset COUNTER')
        self._uart.write(RESET_COUNTER)
        time.sleep(1)
        retry = 0
        ' CICLO IN ATTESA DI ATTIVAZIONE SENSORE '
        while (not self._uart.any()>0) and (retry<10):
            if self._uart :
                print("retry", retry)
            time.sleep(1)
            retry = retry + 1
            self._uart.write(RESET_COUNTER)

    def get_counter(self):
        print('GET COUNTER')
        self._uart.write(GET_COUNTER)
        time.sleep(1)
        self._uart.flush()
       # time.sleep(2)
        retry = 0
        ' CICLO IN ATTESA DI ATTIVAZIONE SENSORE '
        while (self._uart.any()<1) and (retry<10):
            if config.DEBUG:
                print("retry", retry)
            time.sleep(1)
            retry = retry + 1
            self._uart.write(GET_COUNTER)
            time.sleep(1)
            self._uart.flush()

        header = ubinascii.hexlify(self._uart.read(2))
        counter = ubinascii.hexlify(self._uart.read(1))

        if config.DEBUG:
            print("header-->",header)
            print("counter1-->",counter)

        return int(counter,16)

    def _read_little_endian(self, nrbytes):
        result=''
        for i in range(nrbytes):
            result = self._uart.read(1)+result
        return result
