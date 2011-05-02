#!/usr/bin/env python
import subprocess, time, os, shutil

# Python script to run batch tests of PeTe

print "PeTe batch test script"

#####################################################################
#                  Start of Default Configuration                   #
#####################################################################

# Please use test-config.py for custom config

# Path to PeTe (Relative to execution path)
PeTe = "../PeTe-build-desktop/PeTe"

# Path to samples (Relative to execution path)
ModelDir = "Samples/"

# List of strategies to ignore
IgnoreList = IgnoreList = [
"Naive DFS with Hash",
"Random DFS with Hash",
"Naive BFS with Hash",
"DFS-ArcCount",
"DFS-Delta",
"DFS-TokenCost",
#"Linear over-approximation",
"BestFS-Delta (Sum, Extreme)",
"BestFS-Delta-Deep (Sum, Extreme)",
"BestFS-Delta (Sum, Extreme) Lookahead 1",
"BestFS-Delta (Sum, Extreme) Lookahead 2",
"BestFS-Delta (Sum, Extreme) Lookahead 3",
"BestFS-Delta (Sum, Extreme) Lookahead 4",
"BestFS-Delta (Sum, Extreme) Lookahead 5"]

# List of models to run on
Models = ["MAPK", "DTAPN", "Kanban", "FMS"]

# Memory bound in KiB
MemoryBound = 1048576

# Time to wait between process polls
PollTime = 2

# Start with interval at InitialPollTime and increase to PollTime is reached (Set 0 to disable)
InitialPollTime = 0.2

# Seconds before timeout
TimeOut = 60

# Number of queries to run from each model (0 for all)
QueriesToRun = 0

# Query prefixes to ignore
QueryPrefixIgnoreList = [
#"fOK",
"fNOK",
]

#####################################################################
#                   End of Default Configuration                    #
#####################################################################

#load config
try:
	kill = False
	import testconfig
	kill = True
	PeTe 			= testconfig.PeTe
	ModelDir		= testconfig.ModelDir
	IgnoreList 		= testconfig.IgnoreList
	MemoryBound		= testconfig.MemoryBound
	Models 			= testconfig.Models
	PollTime 		= testconfig.PollTime
	InitialPollTime	= testconfig.InitialPollTime
	TimeOut 		= testconfig.TimeOut
	QueriesToRun 	= testconfig.QueriesToRun
	QueryPrefixIgnoreList = testconfig.QueryPrefixIgnoreList
except:
	print "Failed to load config"
	if kill:
		print "Attempted to load config, but failed check that you have all settings!"
		exit()

# Find some absolute paths
petebin = os.path.abspath(PeTe)
modeldir = os.path.abspath(ModelDir) + "/"

# Copy PeTe to /tmp during tests
shutil.copyfile(petebin, "/tmp/PeTe-Test-Bin")
shutil.copystat(petebin, "/tmp/PeTe-Test-Bin")
petebin = "/tmp/PeTe-Test-Bin"

# NULL-Device
FNULL = open('/dev/null', 'w')

# Get all available strategies from PeTe
p = subprocess.Popen([petebin, "--strategies"], stdout=subprocess.PIPE)
strategies = [i.strip() for i in p.stdout.readlines()]
p.wait()

Kanban = ["Kanban5.pet", "Kanban10.pet", "Kanban20.pet", "Kanban50.pet", "Kanban100.pet", "Kanban200.pet", "Kanban500.pet", "Kanban1000.pet"]
FMS = ["FMS2.pet", "FMS10.pet", "FMS50.pet", "FMS100.pet", "FMS200.pet", "FMS500.pet"]
MAPK = ["MAPK8.pet", "MAPK40.pet", "MAPK80.pet", "MAPK160.pet", "MAPK320.pet"]
DTAPN = ["DTAPNs/PrimeNet-7.pet", "DTAPNs/PrimeNet-11.pet", "DTAPNs/PrimeNet-13.pet", "DTAPNs/PrimeNet-17.pet", "DTAPNs/PrimeNet-19.pet"]
modellists = []
for m in Models:
	if m == "MAPK": modellists += (MAPK,)
	if m == "DTAPN": modellists += (DTAPN,)
	if m == "Kanban": modellists += (Kanban,)
	if m == "FMS": modellists += (FMS,)

def getMemory(pid):
	argvs = ["ps", "-p", str(pid), "-o", "vsz="]
	#"ps -p " + str(pid) + " -o vsz=" shell=True
	p2 = subprocess.Popen(argvs, stdout=subprocess.PIPE)
	memory = 0;
	for l in p2.stdout.readlines():
		try:
			memory = int(l)
		except: pass
	p2.wait()
	return memory

def run(model, strategy, query):
	global TimeOut, MemoryBound, QuickStepPoll, PollTime
	argvs = [petebin, model, "--strategy", strategy , "--query", query]
	p = subprocess.Popen(argvs, stdout=subprocess.PIPE, stderr=FNULL)
	t = 0.0
	peekMem = 0
	if InitialPollTime == 0:
		pt = PollTime
	else:
		pt = InitialPollTime
	while t < TimeOut and p.poll() == None:
		mem = getMemory(p.pid)
		if mem > peekMem: peekMem = mem
		if peekMem > MemoryBound:
			break
		if InitialPollTime > 0 and pt < PollTime:
			pt += InitialPollTime
			if pt > PollTime: pt = PollTime
		t += pt
		time.sleep(pt)
	retval = None
	if p.poll() != None:
		retval = ""
		if p.poll() != 0:
			retval = "PeTe error, returned: " + str(p.poll())
		for l in p.stdout.readlines():
			retval += l.strip()	
	if p.poll() == None:
		p.kill()
	if not retval and peekMem > MemoryBound:
		p.wait()
		return False, os.path.basename(model) + ",\t" + query + ",\t" + strategy + ",\tOutOfMemory,\t-,\t-,\t-,\t-", peekMem
	elif not retval and  not t < TimeOut:
		p.wait()
		return False, os.path.basename(model) + ",\t" + query + ",\t" + strategy + ",\tOutOfTime,\t-,\t-,\t-,\t-", peekMem
	p.wait()
	return True, retval, peekMem

def listqueries(model):
	qp = subprocess.Popen(petebin + " " + model + " --list-queries", shell=True, stdout=subprocess.PIPE)
	qp.wait()
	queries = []
	for q in qp.stdout.readlines():
		queries += [q.strip(),]
	return queries


""" Run models, where scaledModels is a list of scaled models ordered from small to large """
def runScaledModels(scaledModels):
	global QueriesToRun
	for strategy in strategies:
		if strategy in IgnoreList:
			continue
		for model in scaledModels:
			queries = listqueries(modeldir + model)
			queriesrun = 0
			failed = True
			for query in queries:
				skipByPrefix = False
				for prefix in QueryPrefixIgnoreList:
					skipByPrefix |= query.startswith(prefix)
				if skipByPrefix: continue
				ret, data, mem = run(modeldir + model, strategy, query)
				print data.strip() + ",\t" + str(mem)
				failed = failed and not ret
				queriesrun += 1
				if QueriesToRun != 0 and queriesrun >= QueriesToRun:
					break
			if failed:
				break

for ml in modellists:
	runScaledModels(ml)
