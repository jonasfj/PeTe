TEMPLATE	= subdirs

CONFIG	   += ordered

SUBDIRS		= \
	PetriEngine \
	PeTedit \
	PetriTests

PeTedit.depends = PetriEngine
PetriTests.depends = PetriEngine

unix{
	PeTe.path = /usr/bin/
	PeTe.files = PeTe
	PetriEngine.path = /usr/lib/
	PetriEngine.files = libPetriEngine.so
	Desktop.path = /usr/share/applications/
	Desktop.files = PeTe.desktop
	INSTALLS += PeTe PetriEngine
}
