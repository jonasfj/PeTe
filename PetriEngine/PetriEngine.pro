TEMPLATE		= lib

QT			   -= gui core
CONFIG		   += staticlib

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	PQL/PQLExpressions.cpp \  
	Reachability/DepthFirstReachabilitySearch.cpp \
	Reachability/CoverabilityTreeNode.cpp \
    Reachability/ReachabilitySearchStrategy.cpp \
    PetriNetBuilder.cpp

HEADERS		   += \
	ProgressReporter.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
	PQL/PQLExpressions.h \
    PQL/PQLParser.h \
    Reachability/ReachabilitySearchStrategy.h \
	Reachability/DepthFirstReachabilitySearch.h \
	Reachability/CoverabilityTreeNode.h \
    PetriNetBuilder.h \
    AbstractPetriNetBuilder.h \
    Reachability/ReachabilityResult.h

TARGET			= ../PetriEngine

include(flex.pri)
include(bison.pri)

FLEXSOURCES		+= \
	PQL/PQLQueryTokens.l \
	PQL/PQLAssignmentTokens.l
BISONSOURCES	+= \
	PQL/PQLQueryParser.y \
	PQL/PQLAssignmentParser.y
