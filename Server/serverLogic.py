from tokenizer import *
import datetime

# Global state variables
numRequests = 0
data = []
firstTime = 0.0 # Keeps track of the first time 
min_ind = 1000000000000000000
max_ind = -1
filename = 'data%s.txt'%datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
f = open(filename,'w')

def handleRequest(path=[], params={}):
	# Declare globals
	global numRequests

	numRequests += 1
	print
	print
	print 'Now Processsing Request Num: %d'%numRequests

	try: # If path isn't empty

		if (path[0] == 'datum'): #this is a datalogging request
			recordDatum(params['raw'],params['ts'],params['index'])
			return 'OK-datum'

		if (path[0] == 'data'): #this is a datalogging request, we will use this one the most for best performance
			recordData(params['raw'],params['ts'],params['index'])
			return 'OK-data'

		elif (path[0] == 'stl'):  #this is a query request
			print
			print tokenizeQuery(params['question'])
			print
			#TODO: Form STL proposition to be evaluated
			#TODO: Form response after evaluating STL query
			return {'answer': 'T', 'error': 0, 'errorDesc': 'poopy'}

		else: #this is not something we support
			print 'I see neither stl or data'
			return "<('')<"

	except BaseException as e: # If path is empty
		print 'Error: Bad Request', e
		return "{>''}>"

def recordData(dataStrings,timeStrings,indexStrings):
	# Assuming data comes like 'data1,data2,data3,data4'
	splittedDataStrings = dataStrings.split(',')
	splittedTimeStrings = timeStrings.split(',')
	splittedIndexStrings = indexStrings.split(',')
	
	for i in range(len(splittedDataStrings)):
		dataString = splittedDataStrings[i]
		timeString = splittedTimeStrings[i]
		indexString = splittedIndexStrings[i]
		recordDatum(dataString,timeString,indexString)		

def recordDatum(dataString,timeString,indexString):
	global data, min_ind, max_ind
	
	index = int(indexString,10)
	if min_ind > index:
		min_ind = index
	if max_ind < index:
		max_ind = index
	
	if (numRequests == 1):
		global firstTime
		firstTime = float(timeString)
		timeStamp = 0.0
	else:
		timeStamp = float(timeString)-firstTime
	attitude = twoByteHexToInt(dataString[1:5])/100.0
	incline = twoByteHexToInt(dataString[5:9])/100.0
	tot_dist = twoByteHexToInt(dataString[9:13])/100.0
	speed = twoByteHexToInt(dataString[13:17])/100.0
	
	data_tuple = (timeStamp,attitude,incline,tot_dist,speed)
	data.append(data_tuple)
	f.write('%2.3f %2.3f %2.3f %2.3f %2.3f\r\n'%data_tuple)

def twoByteHexToInt(s):
	v = int(s, 16)
	if v & 0x8000:
		v = -((v ^ 0xFFFF) + 1)
	return v







