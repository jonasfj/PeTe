TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x
QT		  		+= core gui
INCLUDEPATH		+= ../PetriEngine
LIBS			+= -L../ -lPetriEngine
PRE_TARGETDEPS	+= ../PetriEngine

SOURCES	   += \
	Main.cpp \
	MainWindow.cpp \
	NetItems/PetriNetScene.cpp \
	NetItems/PlaceItem.cpp \
	NetItems/NetItem.cpp \
	NetItems/ArcItem.cpp \
	NetItems/TransitionItem.cpp \
	NetItems/PetriNetView.cpp \
	Commands/InsertTransitionCommand.cpp \
	Commands/InsertPlaceCommand.cpp \
	Commands/MoveItemsCommand.cpp \
	Commands/InsertArcCommand.cpp \
	DataFormats/PNMLParser.cpp \
    Dialogs/EditArcDialog.cpp \
    Dialogs/EditPlaceDialog.cpp \
    Dialogs/EditTransitionDialog.cpp

HEADERS		+= \
	NetItems/TransitionItem.h \
	NetItems/NetItem.h \
	NetItems/PetriNetScene.h \
	NetItems/ArcItem.h \
	NetItems/PlaceItem.h \
	NetItems/PetriNetView.h \
	Commands/Commands.h \
	Commands/MoveItemsCommand.h \
	Commands/InsertPlaceCommand.h \
	Commands/InsertTransitionCommand.h \
	MainWindow.h \
    DataFormats/PNMLParser.h \
    Commands/InsertArcCommand.h \
    Dialogs/EditArcDialog.h \
    Dialogs/EditPlaceDialog.h \
    Dialogs/EditTransitionDialog.h

FORMS		+= MainWindow.ui \
    Dialogs/EditArcDialog.ui \
    Dialogs/EditPlaceDialog.ui \
    Dialogs/EditTransitionDialog.ui

TARGET		 = ../PeTe
