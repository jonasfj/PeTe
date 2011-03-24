TEMPLATE		= lib

QT			   -= gui core
CONFIG		   += staticlib

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	PetriNetFactory.cpp \
	PQL/PQLExpressions.cpp 
 #   DepthFirstReachabilitySearch.cpp \
#    CoverabilityTreeNode.cpp 

HEADERS		   += \
	AbstractPetriNetFactory.h \
	ProgressReporter.h \
	PetriNetFactory.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
	PQL/PQLExpressions.h
#	ReachabilitySearchStrategy.h \
#    DepthFirstReachabilitySearch.h \
#    CoverabilityTreeNode.h 

TARGET			= ../PetriEngine

include(flex.pri)
include(bison.pri)

#FLEXSOURCES		= CTL/CTLTokens.l
#BISONSOURCES	= CTL/CTLParser.y
