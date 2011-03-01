TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x
QT		  		+= core gui
INCLUDEPATH		+= ../PetriEngine PeTedit
LIBS			+= -L../ -lPetriEngine
PRE_TARGETDEPS	+= ../PetriEngine
CONFIG			+= console link_pkgconfig
PKGCONFIG		+= unittest++

SOURCES	   += $$system(find -L -name "*.cpp" | grep -v ^./PeTedit/Main.cpp$)

HEADERS		+= $$system(find -L -name "*.h")


FORMS		+= 	PeTedit/MainWindow.ui
TARGET		 = PetriTests
