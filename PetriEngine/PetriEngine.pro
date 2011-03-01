TEMPLATE	= lib

QT		   -= gui core
CONFIG	   += dll

SOURCES	   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	PetriNetFactory.cpp \
    DepthFirstReachabilitySearch.cpp \
    CoverabilityTreeNode.cpp \
    CTL/CTLExprs.cpp \
    CTL/CTLParser.cpp

HEADERS	   += \
	AbstractPetriNetFactory.h \
	ProgressReporter.h \
	PetriNetFactory.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
	ReachabilitySearchStrategy.h \
    DepthFirstReachabilitySearch.h \
    CoverabilityTreeNode.h \
    CTL/CTLExprs.h \
    CTL/CTLParser.h

TARGET		= ../PetriEngine
