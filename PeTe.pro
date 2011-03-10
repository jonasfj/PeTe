TEMPLATE	= subdirs

CONFIG	   += ordered

SUBDIRS		= \
	PetriEngine \
	PeTedit \
	PetriTests

PeTedit.depends = PetriEngine
PetriTests.depends = PetriEngine

unix{
	#isEmpty(PREFIX){
	#	PREFIX = /usr
	#}

	PeTe.path = /usr/bin/
	PeTe.files = PeTe
	Desktop.path = /usr/share/applications/
	Desktop.files = PeTe.desktop
	INSTALLS += PeTe Desktop
}
