#!/usr/bin/env python
import subprocess, time, os

# Python script to run batch tests of PeTe

print "PeTe batch test script"

FNULL = open('/dev/null', 'w')

petedir = "../PeTe-build-desktop/PeTe"

# Get all available strategies from PeTe
p = subprocess.Popen(petedir + " --strategies",shell=True,stdout=subprocess.PIPE)
strategies = [i.strip() for i in p.stdout.readlines()]
p.wait()


modeldir = "Samples/"
Kanban = ["Kanban5.pet","Kanban10.pet","Kanban20.pet","Kanban50.pet", "Kanban100.pet"]
FMS = ["FMS2.pet","FMS10.pet"]
modellists = [Kanban, FMS]

def getPeTeMemory():
	p2 = subprocess.Popen("ps -C PeTe -o vsz=", shell=True, stdout=subprocess.PIPE)
	memory = 0;
	for l in p2.stdout.readlines():
		try:
			memory = int(l)
		except: pass
	p2.wait()
	return memory

def run(model, strategy, query, timeout = 5*60, memoryBound = 1048576, pollstep = 3):
	cmd = petedir + " " + model + " --strategy \"" + strategy + "\" --query \"" + query + "\""
	p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=FNULL)
	t = 0
	memout = False
	while t < timeout and p.poll() == None:
		if getPeTeMemory() > memoryBound:
			memout = True
			break
		t += pollstep
		time.sleep(pollstep)
	retval = None
	if p.poll() != None:
		retval = ""
		if p.poll() != 0:
			retval = "PeTe error, returned: " + str(p.poll())
		for l in p.stdout.readlines():
			retval += l.strip()	
	if p.poll() == None:
		subprocess.Popen("kill -KILL `pidof PeTe`", shell=True)
	if not retval and memout:
		return False, os.path.basename(model) + ",\t" + query + ",\t" + strategy + ",\tOutOfMemory,\t-,\t-,\t-"
	elif not retval and  not t < timeout:
		return False, os.path.basename(model) + ",\t" + query + ",\t" + strategy + ",\tOutOfTime,\t-,\t-,\t-"
	p.wait()
	return True, retval

def listqueries(model):
	qp = subprocess.Popen(petedir + " " + model + " --list-queries", shell=True, stdout=subprocess.PIPE)
	qp.wait()
	queries = []
	for q in qp.stdout.readlines():
		queries += [q.strip(),]
	return queries

#print run(modeldir + "Kanban50.pet", "Naive DFS with Hash", "fOK546289")

""" Run models, where scaledModels is a list of scaled models ordered from small to large """
def runScaledModels(scaledModels, queriesToRun = 0):
	for strategy in strategies:
		for model in scaledModels:
			queries = listqueries(modeldir + model)
			queriesrun = 0
			failed = True
			for query in queries:
				ret, data = run(modeldir + model, strategy, query)
				print data.strip()
				failed = failed and not ret
				queriesrun += 1
				if queriesToRun != 0 and queriesrun >= queriesToRun:
					break
			if failed:
				break

for ml in modellists:
	runScaledModels(ml, 2)
