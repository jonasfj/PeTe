TEMPLATE	= subdirs

CONFIG	   += ordered

SUBDIRS		= \
	PetriEngine \
	PeTedit \
	PeTer \
	PetriTests

PeTedit.depends = PetriEngine
PeTer.depends = PetriEngine
PetriTests.depends = PetriEngine

unix{
	#isEmpty(PREFIX){
	#	PREFIX = /usr
	#}

	PeTe.path = /usr/bin/
	PeTe.files = PeTe
	Desktop.path = /usr/share/applications/
	Desktop.files = PeTe.desktop
	PeTeIcon.path = /usr/share/icons/hicolor/scalable/apps/
	PeTeIcon.files = PeTedit/Icons/scalable/pete.svg
	INSTALLS += PeTe Desktop PeTeIcon
}
