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
	PQL/PQLExpressions.h \
    PQL/PQLParser.h
#	ReachabilitySearchStrategy.h \
#    DepthFirstReachabilitySearch.h \
#    CoverabilityTreeNode.h 

TARGET			= ../PetriEngine

include(flex.pri)
include(bison.pri)

FLEXSOURCES		+= \
	PQL/PQLQueryTokens.l \
	PQL/PQLAssignmentTokens.l
BISONSOURCES	+= \
	PQL/PQLQueryParser.y \
	PQL/PQLAssignmentParser.y
