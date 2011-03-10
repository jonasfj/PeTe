TEMPLATE	= subdirs

CONFIG	   += ordered

SUBDIRS		= \
	PetriEngine \
	PeTedit \
	PetriTests

PeTedit.depends = PetriEngine
PetriTests.depends = PetriEngine

unix{
	isEmpty(PREFIX){
		PREFIX = /usr
	}

	PeTe.path = $$PREFIX/bin/
	PeTe.files = PeTe
	PetriEngine.path = $$PREFIX/lib/
	PetriEngine.files = libPetriEngine.so
	Desktop.path = $$PREFIX/share/applications/
	Desktop.files = PeTe.desktop
	INSTALLS += PeTe PetriEngine
}
