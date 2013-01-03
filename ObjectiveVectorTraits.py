# -*- coding: utf-8 -*-
import os, shutil, errno,math
import logging
logging.basicConfig(filename='evolve.log',level=logging.DEBUG)

def minimizing(i):
	if i == 0:
		return False
	return True
def maximizing(i):
	if i == 0:
		return True
	return False

def nObjectives():
	return 2 
def minimumBounds(i):
	if i == 0:
		return 1
	return 0.01
def maximumBounds(i):
	if i == 0:
		return 5
	return 0.99
 
def nParams():
	return 2

exp_folder = "/home/asaleh/Diplomka/moeo_wsn/exp/"
binary_folder = "/home/asaleh/Diplomka/moeo_wsn/basicIDS/"
template = "/home/asaleh/Diplomka/moeo_wsn/template.ini"

def initTemplate(templatePath,templateOut,dictValues):
        template = open(templatePath)
	text = template.read()
	for key, value in dictValues.iteritems():
	        text=text.replace(key,value)
        target = open(templateOut, 'w')
	target.write(text)

def getResults(pathToScalar):
        f = open(pathToScalar)
	text = f.read()
	lines = text.split("\n")
	flter = "BasicIDS.node"
	dictSumAns = {
	  "forwarders_falseNegatives":0.0,
          "forwarders_falsePositives":0.0,
          "forwarders_trueNegatives":0.0,
	  "forwarders_truePositives":0.0,
	  "Mean power consumption":0.0
	}
	for line in lines:
		if flter in line:
			for key in dictSumAns.keys():
				if key in line:
					out = float(line.split()[-1])
					dictSumAns[key]=dictSumAns[key]+out
	return dictSumAns

def evaluate(maxMonitorNodes,forwarderThreshold):
	s_maxMonitorNodes = str(math.floor(maxMonitorNodes))
        s_forwarderThreshold = str(math.floor(10*forwarderThreshold)/10)
        val_dict= {
	  '{{template-maxMonitorNodes}}':s_maxMonitorNodes,
	  '{{template-forwarderThreshold}}':s_forwarderThreshold
	}
        input_vals = "maxMonitorNodes-" + s_maxMonitorNodes + "_forwarderThreshold-" + s_forwarderThreshold
	dir_name =exp_folder + input_vals +"/"
	
        dexperiment = os.path.dirname( dir_name)
	dbinary = os.path.dirname(binary_folder)
	if os.path.exists(dexperiment) == False:
		shutil.copytree(dbinary,dexperiment)
		initTemplate(template,dir_name+"/test.ini",val_dict)
                os.system("cd "+dir_name+";./basicIDS -n \".;/home/asaleh/Diplomka/MiXiM/src;\" test.ini")
	results = getResults(dir_name+"/results/General-0.sca")

	efectivity = results["forwarders_trueNegatives"]+results["forwarders_truePositives"]
	inefectivity =results["forwarders_falseNegatives"]+results["forwarders_falsePositives"]
	logging.info("Genome: "+ input_vals +"; Fittness: truthfull-"+str(efectivity-inefectivity))

	return (efectivity,inefectivity,maxMonitorNodes)
