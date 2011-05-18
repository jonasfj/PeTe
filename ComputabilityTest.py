#!/usr/bin/env python
import subprocess, time, os, shutil, sys
""" Script for generating random queries

Input: List of model files
Output: Files where each line contains a random query
"""

# Memory bound in KiB
MemoryBound = 1024*1024*1024*2

# Time to wait between process polls
PollTime = 2

# Start with interval at InitialPollTime and increase to PollTime is reached (Set 0 to disable)
InitialPollTime = 0.1

# Seconds before timeout
TimeOut = 60*2

Strategies = [
"BestFS Ultimate Edition DFS",
"BestFS Ultimate Edition BFS",
]

# NULL-Device
FNULL = open('/dev/null', 'w')

Kanban = [
#"Kanban5.pet", 
#"Kanban10.pet", 
#"Kanban20.pet", 
#"Kanban50.pet", 
"Kanban100.pet", 
"Kanban200.pet", 
"Kanban500.pet", 
"Kanban1000.pet"
]
FMS = [
#"FMS2.pet", 
#"FMS10.pet",
#"FMS20.pet", 
"FMS50.pet", 
"FMS100.pet", 
"FMS200.pet", 
"FMS500.pet"
]
MAPK = [
#"MAPK8.pet", 
#"MAPK40.pet", 
#"MAPK80.pet", 
#"MAPK160.pet", 
"MAPK320.pet",
"MAPK640.pet",
"MAPK1280.pet",
"MAPK2560.pet"
]

Models = Kanban + FMS + MAPK

ModelDir = "Samples/"

PeTer = "../PeTe-build-desktop/PeTer/PeTer"
OutputDir = "TestData/Computability/"

peterbin = os.path.abspath(PeTer)
modeldir = os.path.abspath(ModelDir) + "/"
outputdir = os.path.abspath(OutputDir) + "/"

# Copy PeTe to /tmp during tests
shutil.copyfile(peterbin, "/tmp/PeTer-Test-Bin")
shutil.copystat(peterbin, "/tmp/PeTer-Test-Bin")
peterbin = "/tmp/PeTer-Test-Bin"

def getMemory(pid):
	argvs = ["ps", "-p", str(pid), "-o", "vsz="]
	p2 = subprocess.Popen(argvs, stdout=subprocess.PIPE)
	memory = 0;
	for l in p2.stdout.readlines():
		try:
			memory = int(l)
		except: pass
	p2.wait()
	return memory

def genQuery(model):
	global peterbin, modeldir
	argvs = [peterbin, "--test", modeldir + model, "--gen-query"]
	p = subprocess.Popen(argvs, stdout=subprocess.PIPE, stderr=FNULL)
	p.wait()
	for line in p.stdout.readlines():
		if line.strip() != "":
			return line.strip()
	return None

def runQuery(model, strategy, query):
	global TimeOut, MemoryBound, QuickStepPoll, PollTime, peterbin, modeldir
	argvs = [peterbin, "--test", modeldir + model, "--strategy", strategy , "--literal-query", query]
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
	if p.poll() == None:
		p.kill()
		p.wait()
		return False
	return p.poll() == 0

Files = {}

#Setup headers and create files
for model in Models:
	fp = outputdir + model + ".result"
	if os.path.isfile(fp):
		Files[model] = open(fp, "a")
	else:
		Files[model] = open(fp, "w")
		result = "Query,\t"
		for strategy in Strategies:
			result += strategy.replace(",","-") + ","
		Files[model].write(result + "\n")

while True:
	for model in Models:
		f = Files[model]
		query = genQuery(model)
		result = query + ",\t"
		for strategy in Strategies:
			if runQuery(model, strategy, query):
				result += "1,\t"
			else:
				result += "0,\t"
		f.write(result + "\n")
		f.flush()

