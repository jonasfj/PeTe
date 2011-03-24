TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x
QT		  		+= core gui
INCLUDEPATH		+= ../PetriEngine
LIBS			+= -L../ -lPetriEngine
PRE_TARGETDEPS	+= ../PetriEngine
CONFIG			+= console link_pkgconfig
PKGCONFIG		+= unittest++

SOURCES	   +=   main.cpp

HEADERS		+= \

TARGET		 = PetriTests
