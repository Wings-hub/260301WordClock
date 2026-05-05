import time
import config
import ubinascii

#STATUS: 24 3a 8 0 19 40 cc 1c
#RESET:  24 3a 8 0 14 40 cf b2
GET_COUNTER = (b'\x24\x3A\x08\x08\x22\x40\x6C\x86') #40140000000037
RESET_COUNTER = (b'\x24\x3A\x08\x01\x14\x40\xD8\x32') #40140000000037
RUN_CALIBRATION = (b'\x24\x3A\x08\x02\x19\x40\xE7\x9C') #40140000000037
START_AUTOCALIB = (b'\x24\x3A\x08\x05\x20\x40\x88\x8A') #40140000000037
STOP_AUTOCALIB = (b'\x24\x3A\x08\x06\x21\x40\xB7\x0C') #40140000000037
class TRAP:
    _error = b'\x00'

    # index of list
    # float array of values
    def __init__(self, uart=1, debug=False):
        self._debug = debug
        self._uart = uart

    def getErrorCode(self):
        return self._error

    def calibration(self):
        print('TRAP CALIBRATION')
        #12-11-00-00-00-00-23
        self._uart.write(RUN_CALIBRATION)
        retry = 0
        ' CICLO IN ATTESA DI ATTIVAZIONE SENSORE '
        while (not self._uart.any()>0) and (retry<10):
            if config.DEBUG:
                print("retry calibration", retry)
            time.sleep(1)
            retry = retry + 1
            self._uart.write(RUN_CALIBRATION)

    def start_autocalibration(self):
        print('TRAP start autocalibration')
        time.sleep(10)
        self._uart.write(START_AUTOCALIB)

    def stop_autocalibration(self):
        print('TRAP stop autocalibration')
        self._uart.write(STOP_AUTOCALIB)

    def reset_counter(self):
        print('TRAP reset COUNTER')
        # 12-11-00-00-00-00-23
        self._uart.write(RESET_COUNTER)
        retry = 0
        ' CICLO IN ATTESA DI ATTIVAZIONE SENSORE '
        while (not self._uart.any()>0) and (retry<10):
            if config.DEBUG:
                print("retry reset", retry)
            time.sleep(1)
            retry = retry + 1
            self._uart.write(RESET_COUNTER)

    def get_status(self):
        print('TRAP --> getStatus')
        self._uart.write(GET_COUNTER)
        time.sleep(1)
        retry = 0
        ' CICLO IN ATTESA DI ATTIVAZIONE SENSORE '
        while (not self._uart.any()>0) and (retry<10):
            if config.DEBUG:
                print("retry", retry)
            time.sleep(1)
            retry = retry + 1
            self._uart.write(GET_COUNTER)

        #print(self._uart.read())
        header = ubinascii.hexlify(self._uart.read(6))
        bug_counter = ubinascii.hexlify(self._read_little_endian(nrbytes=4))
        packet_too_short_count = ubinascii.hexlify(self._uart.read(1))
        packet_too_long_count = ubinascii.hexlify(self._uart.read(1))
        packet_malformed_count = ubinascii.hexlify(self._uart.read(1))
        autocalibration = ubinascii.hexlify(self._uart.read(1))
        err_code = ubinascii.hexlify(self._uart.read(1))
        parity = ubinascii.hexlify(self._uart.read(2))
        self._error = err_code

        #header = ubinascii.hexlify(self._uart.read(6))
        #adc_bug_detenction = ubinascii.hexlify(self._uart.read(1))
        #adc_bug_release = ubinascii.hexlify(self._uart.read(1))
        #bug_detection_min_n_pulse = ubinascii.hexlify(self._uart.read(1))
        #bug_counter = ubinascii.hexlify(self._read_little_endian(nrbytes=4))
        #last_bug_detection_n_pulse = ubinascii.hexlify(self._uart.read(4))
        #last_bug_detection_adc_value = ubinascii.hexlify(self._uart.read(4))
        #last_bug_detection_delta_adc_value = ubinascii.hexlify(self._uart.read(4))
        #last_bug_detection_prev_delta_adc_value = ubinascii.hexlify(self._uart.read(4))
        #adc_value = ubinascii.hexlify(self._uart.read(4))
        #delta_adc_value = ubinascii.hexlify(self._uart.read(4))
        #prev_delta_adc_value = ubinascii.hexlify(self._uart.read(4))
        #prev_adc_avarage_value = ubinascii.hexlify(self._uart.read(4))
        #warmup_adc_avarage_value = ubinascii.hexlify(self._uart.read(4))
        #warmup_counter = ubinascii.hexlify(self._uart.read(8))
        #warmup_active = ubinascii.hexlify(self._uart.read(1))
        #warmup_error = ubinascii.hexlify(self._uart.read(1))

        if config.DEBUG:
            print('header:',header)
            #print('adc_bug_detenction:',adc_bug_detenction)
            #print('adc_bug_release:',adc_bug_release)
            #print('bug_detection_min_n_pulse:',bug_detection_min_n_pulse)
            #print('bug_counter:',bug_counter)
            #print('last_bug_detection_n_pulse:',last_bug_detection_n_pulse)
            #print('adc_value:',adc_value)
            #print('warmup_adc_avarage_value:',warmup_adc_avarage_value)
            #print('warmup_active:',warmup_active)
            #print('warmup_error:',warmup_error)
            print('numero di insetti: ', int(bug_counter,16))
            print('packet_too_short_count: ', packet_too_short_count)
            print('packet_too_long_count: ', packet_too_long_count)
            print('packet_malformed_count: ', packet_malformed_count)
            print('autocalibration: ',autocalibration)
            print('err_code: ', err_code)

        recv=self._uart.read()
        return int(bug_counter,16)
    def _read_little_endian(self, nrbytes):
        result=''
        for i in range(nrbytes):
            result = self._uart.read(1)+result
        return result
