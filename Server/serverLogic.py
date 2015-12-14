from tokenizer import *
import datetime, sys, stlToGraderConf, respondToQuery, streamData.py

# Global state variables
numRequests = 0
data = []
firstTime = 0.0 # Keeps track of the first time 
# min_ind = 1000000000000000000
# max_ind = -1
filename = 'data%s.txt'%datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
f = open(filename,'w')

# global graph variables
sp = []
dis = []
t = []
fig = []
flag = 0

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
      if params.has_key('check') and params['check'][0].lower() == 'y':
        return handleQuestion(params, recheck=True)
      return 'OK-datum'

    if (path[0] == 'data'): #this is a datalogging request, we will use this one the most for best performance
      recordData(params['raw'],params['ts'],params['index'])
      if params['check'][0].lower() == 'y':
        return handleQuestion(params, recheck=True)
      return 'OK-data'

    elif (path[0] == 'stl'):  #this is a query request
      return handleQuestion(params)
    else: #this is not something we support
      print 'I see neither stl or data'
      return "<('')<"
  
  except KeyboardInterrupt: # If ctrl-c received
    print 'Received CTRL+C'
    return "<{''}>"
    sys.exit()

  except BaseException as e: # If path is empty
    print 'Error: BAD REQUEST   ', e
    return "{>''}>"

def handleQuestion(params, recheck=False):
  if len(data) == 0:
    return {'answer': '', 'stl': '', 'error': 2, 'errorDesc': 'No data'}
  global f
  if recheck and lastQuestionGlobal is None:
    return "No last question"
  f.close()
  try:
    if recheck:
      response, stl = recheckQuestion()
    else:
      response, stl = respondToQuestion(params['question'])
    ret = {'answer': str(response), 'stl': strForStl(stl), 'error': 0, 'errorDesc': 'peachy'}
  except BaseException as e:
    print 'CRASH PROCESSING QUESTION', e
    ret = {'answer': '', 'stl': '', 'error': 1, 'errorDesc': 'Ambiguous query'}
  try:
    f = open(filename, 'a')
  except BaseException as e:
    print 'ERROR REOPENING TRACE FILE'
  return ret

lastQuestionGlobal = None
lastStlGlobal = None
lastStlFileGlobal = None
def respondToQuestion(question):
  global lastQuestionGlobal, lastStlGlobal, lastStlFileGlobal
  lastQuestionGlobal = None
  stl = queriesToSTL(question)
  lastStlGlobal = stl
  conf = stlToGraderConf.stlToGraderConf(stl,filename)
  response, stlFile = respondToQuery.respondToQueryContents(conf)
  lastStlFileGlobal = stlFile
  lastQuestionGlobal = question
  return response, stl

def recheckQuestion():
  if lastQuestionGlobal is None:
    return "No last question"
  response = respondToQuery.respondToQuery(lastStlFileGlobal)
  stl = lastStlGlobal
  return response, stl

def strForStl(stl):
  if not hasattr(stl, '__iter__'):
    return str(stl)
  #if len(stl) >= 1:
  #  output = strForStl(stl) + '('
  #  stl = stl[1:]
  #else:
  output = '('

  first = True
  for part in stl:
    if first:
      first = False
    else:
      output += ', '
    output += strForStl(part)
  output += ')'
  return output

def recordData(dataStrings,timeStrings,indexStrings):
  global sp, dis, t, fig, flag

  # Assuming data comes like 'data1,data2,data3,data4'
  splittedDataStrings = dataStrings.split(',')
  splittedTimeStrings = timeStrings.split(',')
  splittedIndexStrings = indexStrings.split(',')
  
  for i in range(len(splittedDataStrings)):
    dataString = splittedDataStrings[i]
    timeString = splittedTimeStrings[i]
    indexString = splittedIndexStrings[i]
    recordDatum(dataString,timeString,indexString)    

  sp, dis, t, fig, flag = regraphData(sp,dis,t,fig,flag)

def recordDatum(dataString,timeString,indexString):
  global data, min_ind, max_ind
  
  # index = int(indexString,10)
  # if min_ind > index:
  #   min_ind = index
  # if max_ind < index:
  #   max_ind = index
  
  attitude = twoByteHexToInt(dataString[1:5])/100.0
  incline = twoByteHexToInt(dataString[5:9])/100.0
  tot_dist = twoByteHexToInt(dataString[9:13])/100.0
  speed = twoByteHexToInt(dataString[13:17])/100.0
  if (len(data) < 1):
    global firstTime
    firstTime = float(timeString)
    timeStamp = 0.0
    avg_speed = speed
  else:
    timeStamp = float(timeString)-firstTime
    avg_speed = (speed+data[-1][5]*len(data))/(len(data)+1.0)

  data_tuple = (timeStamp,attitude,incline,tot_dist,speed,avg_speed)
  data.append(data_tuple)
  f.write('%2.8f %2.3f %2.3f %2.3f %2.3f %2.3f\n'%data_tuple)

def twoByteHexToInt(s):
  v = int(s, 16)
  if v & 0x8000:
    v = -((v ^ 0xFFFF) + 1)
  return v







