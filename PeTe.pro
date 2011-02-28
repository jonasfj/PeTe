TEMPLATE	= subdirs

CONFIG	   += ordered

SUBDIRS		= \
	PetriEngine \
	PeTedit \
	PetriTests

PeTedit.depends = PetriEngine
PetriTests.depends = PetriEngine
