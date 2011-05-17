#!/usr/bin/env python
import subprocess, time, os, shutil, sys

Kanban = [
#"Kanban5.pet", 
#"Kanban10.pet", 
#"Kanban20.pet", 
#"Kanban50.pet", 
#"Kanban100.pet", 
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

for model in Models:
	lines = open(model + ".result", "r").readlines()
	sum1 = 0
	sum2 = 0
	count = 0
	for line in lines[1:]:
		q, s1, s2, empty = line.split(",\t")
		sum1 += int(s1)
		sum2 += int(s2)
		count += 1
	q, s1, s2, empty = lines[0].split(",")
	print model + ":"
	print "\t" + s1.strip() + "," + str(sum1) + "," + str(count)
	print "\t" + s2.strip() + "," + str(sum2) + "," + str(count)
