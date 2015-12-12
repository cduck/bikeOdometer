import subprocess

def runSTL(stlFile):
	query_results = subprocess.check_output(["./cpsgrader/Simulators/CPSFileGrader/CPSFileGrader", stlFile], shell=False)

	#results_file = open("./queryResults.txt")
	#query_results = results_file.read()
	#results_file.close()
	print 'QUERY RESULTS:', repr(query_results)
	return query_results

def parseForAnswer(query):
	iStart = query.find("THE_FORMULA_IS_")
	if (iStart < 0):
		return "Error"
	else: 
		result = query[iStart+len("THE_FORMULA_IS_")]
		return result=='T'

def respondToQueryContents(stlFileContents):
	stlFile = "stlFileTemp.stl"
	with open(stlFile,"w") as FILE:
		FILE.write(stlFileContents)
	return respondToQuery(stlFile)

def respondToQuery(stlFile):
	response = runSTL(stlFile)
	answer = parseForAnswer(response)
	return answer


if __name__ == '__main__':
	import stlToGraderConf
	import tokenizer
	import sys


	tokens = tokenizer.tokenizeQuery("Did my distance exceed 7 meters?")
	stl = tokenizer.tokenToSTL(tokens)
	print 'stl=',stl
	stlFileContents = stlToGraderConf.stlToGraderConf(stl, sys.argv[1])
	stlFile = "stlFileTemp.stl"
	with open(stlFile,"w") as FILE:
		FILE.write(stlFileContents)
	print respondToQuery(stlFile)