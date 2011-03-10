TEMPLATE		= lib

QT			   -= gui core
CONFIG		   += staticlib

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	PetriNetFactory.cpp \
    DepthFirstReachabilitySearch.cpp \
    CoverabilityTreeNode.cpp \
    CTL/CTLExprs.cpp \
    CTL/CTLParser.cpp

HEADERS		   += \
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

TARGET			= ../PetriEngine

include(flex.pri)
include(bison.pri)

FLEXSOURCES		= CTL/CTLTokens.l
BISONSOURCES	= CTL/CTLParser.y
