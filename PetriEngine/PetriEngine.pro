TEMPLATE	= lib

QT		   -= gui core
CONFIG	   += dll

SOURCES	   += \
	StandardOutputProgressReporter.cpp \
	CTL/CTLParser.cpp \
	CTL/PlaceExpr.cpp \
	PetriNet.cpp \
	PetriNetFactory.cpp \
    DepthFirstReachabilitySearch.cpp

HEADERS	   += \
	AbstractPetriNetFactory.h \
	ProgressReporter.h \
	CTL/PlaceExpr.h \
	CTL/CTLParser.h \
	CTL/CTLExpr.h \
	PetriNetFactory.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
	ReachabilitySearchStrategy.h \
    DepthFirstReachabilitySearch.h

TARGET		= ../PetriEngine
