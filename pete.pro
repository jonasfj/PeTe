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
    netitems/petrinetview.cpp \
    petriengine/petrinet.cpp \
    petriengine/petrinetfactory.cpp \
    petriengine/standardoutputprogressreporter.cpp \
    petriengine/depthfirstreachabilitysearch.cpp \
    petriengine/ctl/placeexpr.cpp \
    petriengine/ctl/ctlparser.cpp

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
    netitems/petrinetview.h \
    petriengine/petrinet.h \
    petriengine/abstractpetrinetfactory.h \
    petriengine/petrinetfactory.h \
    petriengine/reachabilitysearchstrategy.h \
    petriengine/progressreporter.h \
    petriengine/standardoutputprogressreporter.h \
    petriengine/depthfirstreachabilitysearch.h \
    petriengine/ctl/ctlexpr.h \
    petriengine/ctl/placeexpr.h \
    petriengine/ctl/ctlparser.h

FORMS    += mainwindow.ui
