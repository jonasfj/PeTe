TEMPLATE		= lib

QT			   -= gui core
CONFIG		   += staticlib

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	PQL/PQLExpressions.cpp \  
	Reachability/DepthFirstReachabilitySearch.cpp \
    PetriNetBuilder.cpp \
    ValidationBuilder.cpp \
	Reachability/ReachabilitySearchStrategy.cpp
	#Reachability/CoverabilityTreeNode.cpp

HEADERS		   += \
	ProgressReporter.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
	PQL/PQLExpressions.h \
    PQL/PQLParser.h \
    Reachability/ReachabilitySearchStrategy.h \
    PetriNetBuilder.h \
    AbstractPetriNetBuilder.h \
    Reachability/ReachabilityResult.h \
    ValidationBuilder.h \
    Reachability/KarpMillerL1SearchStrategy.h \
	Reachability/DepthFirstReachabilitySearch.h \
    Reachability/GeneralState.h
	#Reachability/CoverabilityTreeNode.h

TARGET			= ../PetriEngine

include(flex.pri)
include(bison.pri)

FLEXSOURCES		+= \
	PQL/PQLQueryTokens.l \
	PQL/PQLAssignmentTokens.l
BISONSOURCES	+= \
	PQL/PQLQueryParser.y \
	PQL/PQLAssignmentParser.y
