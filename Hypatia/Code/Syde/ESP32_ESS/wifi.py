from network import WLAN
import network
import machine
import urequests
import urequest
import json
import time
import config
import ubinascii

station = None
id = str(ubinascii.hexlify(machine.unique_id()).decode().upper())
def wificonnect():
    global station
    print('INIT WIFI CONNECTION')
    station = WLAN(network.STA_IF)
    station.active(True)

    # nets = station.scan()
    for (ssid, bssid, channel, RSSI, authmode, hidden) in station.scan():
        if (ssid == None):
            print(ssid)
        else:
            ssid = ssid.decode('utf-8')
            if (config.DEBUG):
                print(ssid)
            if str(ssid) == config.WIFI_SSID:
                i = 0
                station.connect(str(ssid), config.WIFI_PASS)
                print('WIFI connecting....')
                while not station.isconnected():
                    i += 1
                    if i > config.WIFI_RETRY_SECONDS:
                        print('WIFI forced closed')
                        return
                    machine.idle()  # save power while waiting
                    time.sleep(1)

    if station.isconnected():
        print('WIFI connected')
    time.sleep(config.WIFI_RETRY_SECONDS)
    print(station.ifconfig())
    return station

def close_wifi():
    global station                      #questa riga e la successiva sono state aggiunte in quanto erano mancanti
    station = WLAN(network.STA_IF)

    print('Closing WIFI...')
    station.active(False)   #prima era WLAN.active(False) (non definita la variabile WLAN))
    print('WIFI Closed')

def sendmessage(sigfoxid, message):
    global id
    if sigfoxid != "":
        id = sigfoxid
    print('Sending Wifi message : ' + id)
    print(message)

    global station                      #questa riga e la successiva sono state aggiunte in quanto erano mancanti
    station = WLAN(network.STA_IF)

    if not station.isconnected():
        wificonnect()

    if not station.isconnected():    #se fallisce la connessione, non provare a inviare il messaggio per evitare che un errore blocchi l'esecuzione del codice
        print("Connessione fallita: messaggio non inviato")
        return

    headers = {'Content-Type': 'application/json'}
    data = (json.dumps(message))

    url = "http://europe-west3-plasma-myth-295409.cloudfunctions.net/wifi"
    url = url + '?version=1&id=' + str(id) + '&type=ENV1&time=0000'
    res = urequests.post(url, data=data, headers=headers)  #il file urequests non c'è
    try:
        # res = urequests.post(url, data=data, headers=headers)
        print('HTTP response Code :' + str(res.status_code))
        if res.status_code != 200:
            print(res.text)
            raise Exception(res.text)
        res.close()
        print('Sended Wifi message')
    except:
        print("ERRORE WIFI")

print('loaded WIFI module on wifi : ' + id)