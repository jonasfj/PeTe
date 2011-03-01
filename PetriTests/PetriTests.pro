TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x
QT		  		+= core gui
INCLUDEPATH		+= ../PetriEngine PeTedit
LIBS			+= -L../ -lPetriEngine
PRE_TARGETDEPS	+= ../PetriEngine
CONFIG			+= console link_pkgconfig
PKGCONFIG		+= unittest++

SOURCES	   += \
	PeTedit/MainWindow.cpp \
	PeTedit/NetItems/PetriNetScene.cpp \
	PeTedit/NetItems/PlaceItem.cpp \
	PeTedit/NetItems/NetItem.cpp \
	PeTedit/NetItems/ArcItem.cpp \
	PeTedit/NetItems/TransitionItem.cpp \
	PeTedit/NetItems/PetriNetView.cpp \
	PeTedit/Commands/InsertTransitionCommand.cpp \
	PeTedit/Commands/InsertPlaceCommand.cpp \
	PeTedit/Commands/InsertArcCommand.cpp \
	PeTedit/Commands/MoveItemsCommand.cpp \
	PeTedit/DataFormats/PNMLParser.cpp \
	main.cpp \
    PetriEngineTests/ReachabilityStrategyTest.cpp

HEADERS		+= \
	PeTedit/NetItems/TransitionItem.h \
	PeTedit/NetItems/NetItem.h \
	PeTedit/NetItems/PetriNetScene.h \
	PeTedit/NetItems/ArcItem.h \
	PeTedit/NetItems/PlaceItem.h \
	PeTedit/NetItems/PetriNetView.h \
	PeTedit/Commands/Commands.h \
	PeTedit/Commands/MoveItemsCommand.h \
	PeTedit/Commands/InsertPlaceCommand.h \
	PeTedit/Commands/InsertTransitionCommand.h \
	PeTedit/MainWindow.h \
	PeTedit/DataFormats/PNMLParser.h

FORMS		+= 	PeTedit/MainWindow.ui
TARGET		 = PetriTests
