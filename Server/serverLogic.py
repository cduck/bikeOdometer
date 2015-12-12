from tokenizer import *
import datetime, sys, stlToGraderConf, respondToQuery

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
	global numRequests, f

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
			f.close()
			stl = tokenToSTL(params['question'])
			conf = stlToGraderConf.stlToGraderConf(stl,filename)
			response = respondToQuery.respondToQueryContents(conf)
			f = open(filename, 'a')

			return {'answer': str(stl) + str(response), 'error': 0, 'errorDesc': 'peachy'}

		else: #this is not something we support
			print 'I see neither stl or data'
			return "<('')<"
	
	except KeyboardInterrupt: # If path is empty
		print 'Received CTRL+C'
		return "<{''}>"
		sys.exit()

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
	i = len(data)-1
	#while i >= 0 and timeStamp < data[i][0]:
	#	i += 1
	#data.insert(i, data_tuple)
	data.append(data_tuple)
	f.write('%2.8f %2.3f %2.3f %2.3f %2.3f\n'%data_tuple)

def twoByteHexToInt(s):
	v = int(s, 16)
	if v & 0x8000:
		v = -((v ^ 0xFFFF) + 1)
	return v







