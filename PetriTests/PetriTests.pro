TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x $$system(llvm-config --cxxflags all) -fexceptions
QT		  		+= core gui
INCLUDEPATH		+= ../PetriEngine
LIBS			+= -L../ -lPetriEngine $$system(llvm-config --libs all)
PRE_TARGETDEPS	+= ../PetriEngine
CONFIG			+= console link_pkgconfig
PKGCONFIG		+= unittest++

SOURCES	   +=   main.cpp \
    PetriEngineTests/ParserTest.cpp \
    PetriEngineTests/DFSReachabilityTest.cpp \
    PetriEngineTests/FireTest.cpp \
    PetriEngineTests/StateSetTest.cpp \
    PetriEngineTests/BlockStackTest.cpp \
    PetriEngineTests/PriorityQueueTest.cpp

HEADERS		+= \

TARGET		 = PetriTests
