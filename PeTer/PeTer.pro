TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x $$system(llvm-config --cxxflags all) -fexceptions
QT		  		-= core gui
INCLUDEPATH		+= ../
LIBS			+=  -static -L../ -lPetriParse -lPetriEngine -llpsolve55 -lcolamd $$system(llvm-config --libs x86codegen jit core) -ldl -lm
PRE_TARGETDEPS	+= ../PetriEngine
CONFIG			+= console
CONFIG			+= link_pkgconfig
PKGCONFIG		+= igraph

SOURCES	   +=   peter.cpp

TARGET		 = PeTer
