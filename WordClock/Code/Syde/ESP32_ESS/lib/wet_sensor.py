from micropython import const
from machine import Pin
import binascii
import ubinascii
import config
import time

#Indirizzo del Dispositivo FDC
I2C_ADDR_0   =const(0x2A)
I2C_ADDR_1   =const(0x2B)
FDC_ADDRESS = I2C_ADDR_1
# Address is 0x2A (default) or 0x2B (if ADDR is high)

# Registri del Dispositivo FDC
#registers
FDC_DEVICE_ID           =const(0x7F)
FDC_MANUFACTURER_ID     =const(0x7E)
FDC_MUX_CONFIG          =const(0x1B)
FDC_CONFIG              =const(0x1A)
FDC_RCOUNT_CH0          =const(0x08)
FDC_RCOUNT_CH1          =const(0x09)
FDC_OFFSET_CH0          =const(0x0C)
FDC_OFFSET_CH1          =const(0x0D)
FDC_SETTLECOUNT_CH0     =const(0x10)
FDC_SETTLECOUNT_CH1     =const(0x11)
FDC_CLOCK_DIVIDER_CH0   =const(0x14)
FDC_CLOCK_DIVIDER_CH1   =const(0x15)
FDC_STATUS              =const(0x18)
FDC_STATUS_CONFIG       =const(0x19)
FDC_DATA_CH0            =const(0x00)
FDC_DATA_LSB_CH0        =const(0x01)
FDC_DATA_CH1            =const(0x02)
FDC_DATA_LSB_CH1        =const(0x03)
FDC_DRIVE_CURRENT_CH0   =const(0x1E)
FDC_DRIVE_CURRENT_CH1   =const(0x1F)
FDC_RESET_DEV           =const(0x1C)


#bitmasks
#Maschere di Bit per la verifica di un dato di misura disponibile sul canale, non letta
FDC_CH0_UNREADCONV =const(0x0008)         #denotes unread CH0 reading in STATUS register
FDC_CH1_UNREADCONV =const(0x0004)         #denotes unread CH1 reading in STATUS register
CH2_UNREADCONV =const(0x0002)         #denotes unread CH2 reading in STATUS register
CH3_UNREADCONV =const(0x0001)         #denotes unread CH3 reading in STATUS register
DEGLITCH_MASK  =const(0xFFF8)

# Maschere di Bit per lettura dei registri di misura a 28 bit
FDC_DATA_CHx_MASK_DATA  =const(0xFFFF)
FDC_DATA_CHx_MASK_ERRAW =const(0x1000)
FDC_DATA_CHx_MASK_ERRWD =const(0x2000)
FDC_DATA_CHx_MASK_GAIN0 =const(0xFFF0)
FDC_DATA_CHx_MASK_GAIN1 =const(0x3FFC)
FDC_DATA_CHx_MASK_GAIN2 =const(0x1FFE)
FDC_DATA_CHx_MASK_GAIN3 =const(0xFFF)

# mask for 28bit data to filter out flag bits
FDC2212_DATA_CHx_MASK_DATA          =const(0x0FFF)
FDC2212_DATA_CHx_MASK_ERRAW         =const(0x1000)
FDC2212_DATA_CHx_MASK_ERRWD         =const(0x2000)

# Numero di canali da campionare nel loop di misura
N_CHANNEL_MEAS =const(2)

FDC_TYPE    = const(1)  # 0=FDC2112, 1=FDC2212
MODE        = const(0)  # 0=TEST, 1=OPERATIVE
GAIN        = const(0)  # 0,1,2,3 FDC2112 funziona bene con guadagno 2


class WET(object):

    _BUFFER = bytearray(8)
    RESOLUTION = 2**28

    def __init__(self, i2c, debug=False):
        Pin(13, Pin.OUT).value(1)
        time.sleep_us(config.I2C_WAITMLSEC)

        self._i2c = i2c

        # try:
        if self.get_device_id() not in ('3055','3054'):
            raise RuntimeError('Failed to find FDC2212/FDC2214, check wiring!')
        self.start()
        # except:
        #     print("WET - INIT - ERRORE")

        
    def get_device_id(self):
        # self._i2c.writeto(FDC_DEVICE_ID,0x3054)
        # utime.sleep_ms(30)
        time.sleep(0.5)
        deviceid = self.from_registry(address=FDC_ADDRESS, registry= FDC_DEVICE_ID)
        manifactureid =self.from_registry(FDC_ADDRESS, FDC_MANUFACTURER_ID)
        if config.DEBUG:
            print ("DEVICE ID ", deviceid)
            print ("MANIFACTURE ID ", manifactureid)

        return deviceid.decode("utf-8")
    def start(self):
        #Imposto la configurazione del dispositivo
        self.to_registry(FDC_ADDRESS,FDC_CONFIG, '1C81')
        if config.DEBUG:
            print ("CONFIG ", self.from_registry(FDC_ADDRESS, FDC_CONFIG))

        self.start_channel_0()
        self.start_channel_1()

        #Impostazione Auto-Scan MUX_CONFIG REGISTER
        # Autoscan: 0 = single channel, selected by CONFIG.ACTIVE_CHAN
        # | Autoscan sequence. b00 for chan 1-2, b01 for chan 1-2-3, b10 for chan 1-2-3-4
        # | |         Reserved - must be b0001000001
        # | |         |  Deglitch frequency. b001 for 1 MHz, b100 for 3.3 MHz, b101 for 10 Mhz, b111 for 33 MHz
        # | |         |  |
        # ARR0001000001DDD -> b0000 0010 0000 1000 -> 0x208
        muxVal = 0x0208 | (0x04 << 13) | 0x05 # ' 33293'

        self.to_registry(FDC_ADDRESS,FDC_MUX_CONFIG,'820D')
        if config.DEBUG:
            print ("FDC_MUX_CONFIG ", self.from_registry(FDC_ADDRESS, FDC_MUX_CONFIG))

        # if (FDC_TYPE == 0):
        #     if GAIN ==0:
        #         self._i2c.writeto_mem(FDC_ADDRESS,FDC_RESET_DEV,0x000)
        #     else:
        #         if (GAIN == 1):
        #             self._i2c.writeto_mem(FDC_ADDRESS,FDC_RESET_DEV,0x200)
        #         else:
        #             if (GAIN == 2):
        #                 self._i2c.writeto_mem(FDC_ADDRESS,FDC_RESET_DEV,0x400)
        #             else :
        #                 if (GAIN == 3) :
        #                     self._i2c.writeto_mem(FDC_ADDRESS,FDC_RESET_DEV,0x600)
    def start_channel_0(self):
        #Imposto la conifigurazione del dispositivo
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_CONFIG, 0x1C81)
        #SETTLE_COUNT REGISTER
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_SETTLECOUNT_CH0,0x64)
        self.to_registry(FDC_ADDRESS,FDC_SETTLECOUNT_CH0, '64')

        #R_COUNT REGISTER
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_RCOUNT_CH0,0xFFFF)
        self.to_registry(FDC_ADDRESS,FDC_RCOUNT_CH0, 'FFFF')
        #OFFSET REGISTER
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_OFFSET_CH0,0x0000)
        self.to_registry(FDC_ADDRESS,FDC_OFFSET_CH0, '0000')

        #CLOCK DIVIDER REGISTER (per definire la fref)
        #  Reserved
        #  | Sensor Frequency Select. b01 = /1 = sensor freq 0.01 to 8.75MHz; b10 = /2 = sensor freq 0.01 to 10 or 5 to 10 MHz
        #  | | Reserved
        #  | | |         Reference divider. Must be > 1. fref = fclk / this register`
        #  | | |         |
        # 00FF00RRRRRRRRRR -> 0001000000000001 -> 0x1001
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_CLOCK_DIVIDER_CH0,0x1001)
        self.to_registry(FDC_ADDRESS,FDC_CLOCK_DIVIDER_CH0, '1001')

        # 00FF00RRRRRRRRRR -> 0010000000000001 -> 0x2001
        #self._i2c.writeto_mem(FDC_ADDRESS,FDC_CLOCK_DIVIDER_CH0,0x2001);
        #DRIVE_CURRENT_REGISTER+
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_DRIVE_CURRENT_CH0,0xF000)
        self.to_registry(FDC_ADDRESS,FDC_DRIVE_CURRENT_CH0, 'F000')

        if config.DEBUG:
            print ("FDC_SETTLECOUNT_CH0 ", self.from_registry(FDC_ADDRESS, FDC_SETTLECOUNT_CH0,1))
            print ("FDC_RCOUNT_CH0 ", self.from_registry(FDC_ADDRESS, FDC_RCOUNT_CH0))
            print ("FDC_OFFSET_CH1 ", self.from_registry(FDC_ADDRESS, FDC_OFFSET_CH0))
            print ("FDC_CLOCK_DIVIDER_CH0 ", self.from_registry(FDC_ADDRESS, FDC_CLOCK_DIVIDER_CH0))
            print ("FDC_DRIVE_CURRENT_CH0 ", self.from_registry(FDC_ADDRESS, FDC_DRIVE_CURRENT_CH0))
    def start_channel_1(self):
        #SETTLE_COUNT REGISTER
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_SETTLECOUNT_CH1,0x64)
        self.to_registry(FDC_ADDRESS,FDC_SETTLECOUNT_CH1, '64')

        #R_COUNT REGISTER
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_RCOUNT_CH1,0xFFFF)
        self.to_registry(FDC_ADDRESS,FDC_RCOUNT_CH1, 'FFFF')

        #OFFSET REGISTER
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_OFFSET_CH1,0x0000)
        self.to_registry(FDC_ADDRESS,FDC_OFFSET_CH1, '0000')

        #CLOCK DIVIDER REGISTER (per definire la fref)
        #  Reserved
        #  | Sensor Frequency Select. b01 = /1 = sensor freq 0.01 to 8.75MHz; b10 = /2 = sensor freq 0.01 to 10 or 5 to 10 MHz
        #  | | Reserved
        #  | | |         Reference divider. Must be > 1. fref = fclk / this register`
        #  | | |         |
        # 00FF00RRRRRRRRRR -> 0001000000000001 -> 0x1001
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_CLOCK_DIVIDER_CH1,0x1001)
        self.to_registry(FDC_ADDRESS,FDC_CLOCK_DIVIDER_CH1, '1001')

        # 00FF00RRRRRRRRRR -> 0010000000000001 -> 0x2001
        #self._i2c.writeto_mem(FDC_ADDRESS,FDC_CLOCK_DIVIDER_CH1,0x2001);
        #DRIVE_CURRENT_REGISTER
        # self._i2c.writeto_mem(FDC_ADDRESS,FDC_DRIVE_CURRENT_CH1,0xF000)
        self.to_registry(FDC_ADDRESS,FDC_DRIVE_CURRENT_CH1, 'f000')

        if config.DEBUG:
            print ("FDC_SETTLECOUNT_CH1 ", self.from_registry(FDC_ADDRESS, FDC_SETTLECOUNT_CH1,1))
            print ("FDC_RCOUNT_CH1 ", self.from_registry(FDC_ADDRESS, FDC_RCOUNT_CH1))
            print ("FDC_OFFSET_CH1 ", self.from_registry(FDC_ADDRESS, FDC_OFFSET_CH1))
            print ("FDC_CLOCK_DIVIDER_CH1 ", self.from_registry(FDC_ADDRESS, FDC_CLOCK_DIVIDER_CH1))
            print ("FDC_DRIVE_CURRENT_CH1 ", self.from_registry(FDC_ADDRESS, FDC_DRIVE_CURRENT_CH1))
    def set_dry_ref(self):
        print("SET DRY")
        N_MEAS = 5

        #DRY_MEAS = 0
        total_ch0_raw=[0,0,0,0,0]
        total_ch1_raw=[0,0,0,0,0]
        average_ch0_raw=[0,0,0,0,0]
        average_ch1_raw=[0,0,0,0,0]
        total_ch0_proc=0
        total_ch1_proc=0
        for j in range(N_MEAS):
            for i in range(N_MEAS):
                total_ch0_raw[j] += self.get_measure(0)
                total_ch1_raw[j] += self.get_measure(1)

            # index = j;
            average_ch0_raw[j] = round(total_ch0_raw[j]/N_MEAS)
            average_ch1_raw[j] = round(total_ch1_raw[j]/N_MEAS)
            total_ch0_proc += average_ch0_raw[j]
            total_ch1_proc += average_ch1_raw[j]

        average_ch0_proc = round(total_ch0_proc/N_MEAS)
        average_ch1_proc = round(total_ch1_proc/N_MEAS)
        config.write_config("wetdrymeas", average_ch0_proc)
        config.write_config("wetrefmeas", average_ch1_proc)

        if config.DEBUG:
            print(average_ch0_proc)
            print(average_ch1_proc)


    def set_wet_ref(self):
        print("SET WET")
        N_MEAS = 5
        #DRY_MEAS = 0
        total_ch0_raw=[0,0,0,0,0]
        total_ch1_raw=[0,0,0,0,0]
        average_ch0_raw=[0,0,0,0,0]
        average_ch1_raw=[0,0,0,0,0]
        total_ch0_proc=0
        total_ch1_proc=0
        for j in range(N_MEAS):
            for i in range(N_MEAS):
                total_ch0_raw[j] += self.get_measure(0)
                total_ch1_raw[j] += self.get_measure(1)
            # index = j;
            average_ch0_raw[j] = round(total_ch0_raw[j]/N_MEAS)
            average_ch1_raw[j] = round(total_ch1_raw[j]/N_MEAS)
            total_ch0_proc += average_ch0_raw[j]
            total_ch1_proc += average_ch1_raw[j]

        average_ch0_proc = round(total_ch0_proc/N_MEAS)
        average_ch1_proc = round(total_ch1_proc/N_MEAS)
        config.write_config("wetdeltameas", average_ch1_proc - average_ch0_proc)
        if config.DEBUG:
            print(average_ch0_proc)
            print(average_ch1_proc)
    def get_status(self):
        status = self.from_registry(FDC_ADDRESS, FDC_STATUS)
        # print ('status --> ', status)
        return status
    def get_measure(self, channel):
        attempts = 100
        measure = 0

        #scelgo il registro per la lettura del dato di misura ed il bit di verifica della sua disponibilità nel registro di stato sulla base del canale di misura scelto
        if  channel==0:
            address_msb   = FDC_DATA_CH0
            address_lsb   = FDC_DATA_LSB_CH0
            bitUnreadConv = FDC_CH0_UNREADCONV
        if  channel==1:
            address_msb   = FDC_DATA_CH1
            address_lsb   = FDC_DATA_LSB_CH1
            bitUnreadConv = FDC_CH1_UNREADCONV

        # verifico che sul canale sia disonibile un dato da leggere (100 tentativi di attesa)
        status = self.get_status()
        # while (attempts and  not(status & bitUnreadConv)) :
        # print (FDC_CH0_UNREADCONV+FDC_CH1_UNREADCONV)
        while (attempts and not(status == bitUnreadConv or status == b'000c')) :
            # status = self._i2c.readfrom_mem(FDC_ADDRESS, FDC_STATUS, 2)
            #print (attempts)
            status = self.get_status()
            attempts=attempts-1

        # se al primo tentativo è disponibile un dato da leggere, lo leggo ed attendo che sia disponnibile un nuovo dato da leggere (la misura potrebbe essere "vecchia")
        if (attempts == 100):
            measure = self._read_measure(msb=address_msb, lsb = address_lsb)
            status = self.get_status()
            while (attempts and not(status == bitUnreadConv or status == b'000c')) :
                status = self.get_status()
                attempts=attempts-1

        # se riesco ad avere un nuovo dato disponibile entro 100 tentativi lo leggo e lo restituisco come misura, altrimenti non restituisco niente e considero la misura non eseguita
        if (attempts):
#            time.sleep(1)
            #leggo la misura su 32 bit, concatenando le letture fatte sui due registri msb e lsb (unsigned long)
            measure = self._read_measure(msb=address_msb, lsb = address_lsb)
            while measure == 0:
                print ('retry to read measure')
                measure = self._read_measure(msb=address_msb, lsb=address_lsb)
            return measure
        else:
            # Non riesco a rilevare la misura perchè il canale nel registro STATUS non è valido
            return 0
    def from_registry(self, address, registry, bytes=2):
        #time.sleep(1)
        try:
            value = self._i2c.readfrom_mem(address, registry, bytes)
            return ubinascii.hexlify(value)
        except:
            print ("ERROR - WET - FROM REGISTRY - READ: ")
    def to_registry(self, address, registry, value):
        self._i2c.writeto_mem(address,registry, binascii.unhexlify(value))
    def _read_measure(self, msb, lsb):
        measure = 0
        # leggo la misura su 32 bit, concatenando le letture fatte sui due registri msb e lsb (unsigned long)

        msbvalue = self.from_registry(FDC_ADDRESS, msb)
        lsbvalue = self.from_registry(FDC_ADDRESS, lsb)
        if msbvalue==None :
            print ('error to read msb')
            return 0
        if lsbvalue==None:
            print ('error to read lsb')
            return 0
        # print(int(msbvalue,16))
        # print(int(msbvalue,16) & FDC_DATA_CHx_MASK_DATA)
        # print((int(msbvalue,16) & FDC_DATA_CHx_MASK_DATA)<<16)

        # print(int(lsbvalue,16))
        measure = int(self.from_registry(FDC_ADDRESS, msb),16)
        measure = (measure & FDC_DATA_CHx_MASK_DATA)<<16
        measure |= int(lsbvalue,16)

        # meas = (self._i2c.readfrom_mem(FDC_ADDRESS,msb,2) & FDC_DATA_CHx_MASK_DATA) << 16
        # meas = (self._i2c.readfrom_mem(FDC_ADDRESS,msb,2)) << 16
        # meas |= self._i2c.readfrom_mem(FDC_ADDRESS,lsb)
        if config.DEBUG:
            if msb == FDC_DATA_CH0:
                print('------------- read measure CHANNEL 0 ---------------')
            else:
                print('------------- read measure CHANNEL 1 ---------------')
            print (measure)
            print('------------- end read measure ---------------')
        return measure
