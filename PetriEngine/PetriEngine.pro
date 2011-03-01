TEMPLATE	= lib

QT		   -= gui core
CONFIG	   += dll

SOURCES	   += \
	StandardOutputProgressReporter.cpp \
	CTL/CTLParser.cpp \
	PetriNet.cpp \
	PetriNetFactory.cpp \
    DepthFirstReachabilitySearch.cpp \
    CoverabilityTreeNode.cpp

HEADERS	   += \
	AbstractPetriNetFactory.h \
	ProgressReporter.h \
	CTL/CTLParser.h \
	PetriNetFactory.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
	ReachabilitySearchStrategy.h \
    DepthFirstReachabilitySearch.h \
    CoverabilityTreeNode.h

TARGET		= ../PetriEngine
