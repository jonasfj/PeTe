TEMPLATE		= lib

QMAKE_CXXFLAGS += $$system(llvm-config --cxxflags all)
QT			   -= gui core
CONFIG		   += staticlib
LIBS		   += $$system(llvm-config --libs all)

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	PQL/PQLExpressions.cpp \  
	Reachability/DepthFirstReachabilitySearch.cpp \
    PetriNetBuilder.cpp \
    ValidationBuilder.cpp \
	Reachability/ReachabilitySearchStrategy.cpp \
    Reachability/DFRSHash.cpp
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
    Reachability/State.h \
    Reachability/StateSet.h \
    Reachability/DFRSHash.h
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
