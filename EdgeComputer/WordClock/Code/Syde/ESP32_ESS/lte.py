
import network
import time
import urequests

class lteHttpPost():
	def __init__(self):
		self.apn = None
		self.band = None
		self.LTEisAttached = False
		self.LTEisAttachedLast = False
		self.LTEisConnected = False
		self.LTEisConnectedLast = False

		self.start = True

	def initLTE(self, band, apn):
		self.apn = apn
		self.band = band
		print("INFO", "Starting initialization of LTE")

		try:
			self.lte = network.LTE()
			print("INFO", "LTE initialized")
		except:
			print("WARNING", "LTE init command failed")

		print("LTE is attached after init:", self.lte.isattached())
		print("LTE is connected after init:", self.lte.isconnected())
	# def sendmessage(self, url):
		# n = 0
        # self.url = "http://europe-west3-plasma-myth-295409.cloudfunctions.net/wifi?version=1"+ '&id='+sigfox_id+'&type=ENV1&time=0000'
		# while True:
		#
		# 	self.LTEisAttached = self.lte.isattached()
		#
		# 	if not self.LTEisAttached and (self.LTEisAttachedLast or self.start):
		# 		if not self.start:
		# 			print("WARNING", "Dettached from LTE network")
		# 		try:
		# 			self.lte.attach(band = self.band, apn = self.apn)
		# 			print("INFO", "LTE attach command sent")
		# 			self.start = False
		# 		except:
		# 			print("WARNING", "LTE attach command failed")
		#
		# 	if self.LTEisAttached and not self.LTEisAttachedLast:
		# 		print("INFO", "Attached to LTE network")
		# 		try:
		# 			self.lte.connect()
		# 			print("INFO", "LTE connect command sent")
		# 		except:
		# 			print("WARNING", "LTE connect command failed")
		#
		# 	self.LTEisAttachedLast = self.LTEisAttached
		#
		# 	self.LTEisConnected = self.lte.isconnected()
		#
		# 	if self.LTEisConnected and not self.LTEisConnectedLast:
		# 		print("INFO", "Connected to LTE network")
		#
		# 	elif not self.LTEisConnected and self.LTEisConnectedLast:
		# 		print("WARNING", "Disconnected from LTE network")
		#
		# 	self.LTEisConnectedLast = self.LTEisConnected
		#
		# 	if self.LTEisConnected:
		# 		n+=1
		# 		try:
		# 			response = urequests.post(url, headers={"Content-Type": "application/json", "Accept": "application/json"}, data='{"time": "%s"}' % (time.time()))
		# 			print("INFO", "%s. HTTP POST successful: %s" % (n, response.text))
		# 		except Exception as e:
		# 			print("WARNING", "%s. HTTP POST failed: %s" % (n, e))
		#
		# 	time.sleep(0.25)

	# def run(self, url):
    #     self.messagge = messagge
    #
    #     from network import Sigfox
        # print('Sending LTE messagge : ')
        # print(messagge)


        # sigfox = Sigfox(mode=Sigfox.SIGFOX, rcz=Sigfox.RCZ1)
        # sigfox_id = ubinascii.hexlify(sigfox.id()).decode().upper()
        #
        # url = "http://europe-west3-plasma-myth-295409.cloudfunctions.net/wifi?version=1"
        # # url = "https://europe-west3-cloud4thing.cloudfunctions.net/wifi?version=1"
        # url = url + '&id='+sigfox_id+'&type=ENV1&time=0000'
        #
        # jsondata = {}
        # jsondata = json.dumps(messagge)
        #
		# n = 0
		# while True:
		# 	self.LTEisAttached = self.lte.isattached()
		# 	if not self.LTEisAttached and (self.LTEisAttachedLast or self.start):
		# 		if not self.start:
		# 			print("WARNING", "Dettached from LTE network")
		# 		try:
		# 			self.lte.attach(band = self.band, apn = self.apn)
		# 			print("INFO", "LTE attach command sent")
		# 			self.start = False
		# 		except:
		# 			print("WARNING", "LTE attach command failed")
        #
		# 	if self.LTEisAttached and not self.LTEisAttachedLast:
		# 		print("INFO", "Attached to LTE network")
		# 		try:
		# 			self.lte.connect()
		# 			print("INFO", "LTE connect command sent")
		# 		except:
		# 			print("WARNING", "LTE connect command failed")
        #
		# 	self.LTEisAttachedLast = self.LTEisAttached
        #
		# 	self.LTEisConnected = self.lte.isconnected()
        #
		# 	if self.LTEisConnected and not self.LTEisConnectedLast:
		# 		print("INFO", "Connected to LTE network")
        #
		# 	elif not self.LTEisConnected and self.LTEisConnectedLast:
		# 		print("WARNING", "Disconnected from LTE network")
        #
		# 	self.LTEisConnectedLast = self.LTEisConnected
        #
		# 	# if self.LTEisConnected:
		# 	# 	n+=1
		# 	# 	try:
        #     #         print("INFO", "Sending Message")
		# 	# 		response = urequests.post(url, headers={"Content-Type": "application/json", "Accept": "application/json"}, data=messagge % (time.time()))
		# 	# 		print("INFO", "%s. messagge sended : %s" % (n, response.text))
		# 	# 	except Exception as e:
		# 	# 		print("WARNING", "%s. messagge send failed: %s" % (n, e))
        #
		# 	time.sleep(0.25)

print ("INIT")
