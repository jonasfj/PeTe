TEMPLATE	= subdirs

CONFIG	   += ordered

SUBDIRS		= \
	PetriEngine \
	PeTedit

PeTedit.depends = PetriEngine
