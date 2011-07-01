TEMPLATE		= lib

QT			   -= gui core
CONFIG		   += staticlib
INCLUDEPATH	   += ../

SOURCES	   += \
	xmlsp/xmlsp_dom.cpp \
    xmlsp/xmlsp.cpp \
    PNMLParser.cpp

HEADERS		+= \
    xmlsp/xmlsp_dom.h \
    xmlsp/xmlsp.h \
    PNMLParser.h

TARGET			= ../PetriParse
