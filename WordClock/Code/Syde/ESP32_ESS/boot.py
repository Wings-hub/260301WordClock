# boot.py -- run on boot-up
import esp32
import network
import bluetooth
from machine import WDT
import uos
import sys
import config

print('===============================================================================')
print('Device: ' + uos.uname().machine)
print('Firmware: ' + uos.uname().release)
print('Python: ' + sys.version)
print('MicroPython: ' + str(sys.implementation.version[0]) + '.' + str(sys.implementation.version[1]) + '.' + str(sys.implementation.version[2]))
print('Internal Temperature: '+str(esp32.raw_temperature()))
print('===============================================================================')
if config.WAIT_SECONDS<100:
    wdt = WDT(timeout=(100 + config.DEEPSLEEP_SECONDS)*1000)
else :
    wdt = WDT(timeout=(config.WAIT_SECONDS + config.DEEPSLEEP_SECONDS)*1000)
wdt.feed()
print('Switching off WLAN')
network.WLAN(network.STA_IF).active(False)

print('Switching off Bluetooth')
bluetooth.BLE().active(False)

print('=========   BOOTED  ==========================================================')
