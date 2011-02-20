QT       += core gui

TARGET = PeTe
TEMPLATE = app


SOURCES += \
	main.cpp \
    mainwindow.cpp \
    netitems/petrinetscene.cpp \
    netitems/placeitem.cpp \
    netitems/transitionitem.cpp \
    netitems/arcitem.cpp \
    netitems/netitem.cpp \
    commands/insertplacecommand.cpp \
    commands/moveitemscommand.cpp \
    commands/inserttransitioncommand.cpp \
    netitems/petrinetview.cpp

HEADERS += \
	mainwindow.h \
	netitems/petrinetscene.h \
	netitems/placeitem.h \
    netitems/transitionitem.h \
    netitems/arcitem.h \
    netitems/netitem.h \
	commands/insertplacecommand.h \
    commands/commands.h \
    commands/moveitemscommand.h \
    commands/inserttransitioncommand.h \
    netitems/petrinetview.h

FORMS    += mainwindow.ui
