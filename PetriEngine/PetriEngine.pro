TEMPLATE		= lib

QT			   -= gui core
CONFIG		   += staticlib

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	PetriNetFactory.cpp \
	PQL/PQLExpressions.cpp \  
    Reachability/KarpMillerL1SearchStrategy.cpp \
	Reachability/DepthFirstReachabilitySearch.cpp \
	Reachability/CoverabilityTreeNode.cpp \
    Reachability/ReachabilitySearchStrategy.cpp

HEADERS		   += \
	AbstractPetriNetFactory.h \
	ProgressReporter.h \
	PetriNetFactory.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
	PQL/PQLExpressions.h \
    PQL/PQLParser.h \
    Reachability/ReachabilitySearchStrategy.h \
    Reachability/KarpMillerL1SearchStrategy.h \
	Reachability/DepthFirstReachabilitySearch.h \
	Reachability/CoverabilityTreeNode.h

TARGET			= ../PetriEngine

include(flex.pri)
include(bison.pri)

FLEXSOURCES		+= \
	PQL/PQLQueryTokens.l \
	PQL/PQLAssignmentTokens.l
BISONSOURCES	+= \
	PQL/PQLQueryParser.y \
	PQL/PQLAssignmentParser.y
