TEMPLATE	 = app

QMAKE_CXXFLAGS	+= -std=c++0x
QT		  		+= core gui svg
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
	Dialogs/QueryDialog.cpp \
	Dialogs/EditTransitionDialog.cpp \
    Widgets/QueryEdit.cpp \
    Widgets/QueryHighlighter.cpp \
    Commands/RenameItemCommand.cpp \
    Commands/EditPlaceCommand.cpp \
    Commands/EditArcCommand.cpp \
    Commands/DeleteItemCommand.cpp \
    Commands/EditTransitionCommand.cpp \
    Widgets/VariableDelegate.cpp \
    Misc/QueryItem.cpp \
    Commands/EditQueryCommand.cpp \
    Commands/AddRemoveQueryCommand.cpp \
    NetItems/PetriNetSceneBuilder.cpp \
    DataFormats/PNMLBuilder.cpp


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
    Dialogs/EditTransitionDialog.h \
	Dialogs/QueryDialog.h \
    Widgets/QueryEdit.h \
    Widgets/QueryHighlighter.h \
    Commands/RenameItemCommand.h \
    Commands/EditPlaceCommand.h \
    Commands/EditArcCommand.h \
    Commands/DeleteItemCommand.h \
    Commands/EditTransitionCommand.h \
    Widgets/VariableDelegate.h \
    Misc/QueryItem.h \
    Commands/EditQueryCommand.h \
    Commands/AddRemoveQueryCommand.h \
    NetItems/PetriNetSceneBuilder.h \
    DataFormats/PNMLBuilder.h


FORMS		+= MainWindow.ui \
    Dialogs/EditArcDialog.ui \
    Dialogs/EditPlaceDialog.ui \
    Dialogs/EditTransitionDialog.ui \
    Dialogs/QueryDialog.ui

TARGET		 = ../PeTe

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    Widgets/QueryReservedWords.list
