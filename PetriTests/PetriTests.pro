TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x $$system(llvm-config --cxxflags all) -fexceptions
QT		  		+= core gui
INCLUDEPATH		+= ../PetriEngine
LIBS			+= -L../ -lPetriEngine -llpsolve55 -lcolamd $$system(llvm-config --libs all)
PRE_TARGETDEPS	+= ../PetriEngine
CONFIG			+= console link_pkgconfig
PKGCONFIG		+= unittest++

SOURCES	   +=   main.cpp \
    PetriEngineTests/ParserTest.cpp \
    PetriEngineTests/DFSReachabilityTest.cpp \
    PetriEngineTests/FireTest.cpp \
    PetriEngineTests/StateSetTest.cpp \
    PetriEngineTests/BlockStackTest.cpp \
    PetriEngineTests/PriorityQueueTest.cpp \
    PetriEngineTests/CompiledConditionTest.cpp

HEADERS		+= \

TARGET		 = PetriTests
