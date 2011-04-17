TEMPLATE		= lib

QMAKE_CXXFLAGS += $$system(llvm-config --cxxflags all)
QT			   -= gui core
CONFIG		   += staticlib

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	Reachability/DepthFirstReachabilitySearch.cpp \
    PetriNetBuilder.cpp \
    ValidationBuilder.cpp \
	Reachability/ReachabilitySearchStrategy.cpp \
    Reachability/DFRSHash.cpp \
    PQL/Expressions.cpp \
    PQL/PQL.cpp \
    Reachability/HashUnderApproximation.cpp \
    Reachability/BestFirstReachabilityStrategy.cpp \
    Reachability/ClosestFirstReachability.cpp \
    PQL/CompiledCondition.cpp
	#Reachability/CoverabilityTreeNode.cpp

HEADERS		   += \
	ProgressReporter.h \
	PetriNet.h \
	StandardOutputProgressReporter.h \
    PQL/PQLParser.h \
    Reachability/ReachabilitySearchStrategy.h \
    PetriNetBuilder.h \
    AbstractPetriNetBuilder.h \
    Reachability/ReachabilityResult.h \
    ValidationBuilder.h \
	Reachability/DepthFirstReachabilitySearch.h \
	Structures/State.h \
	Structures/StateSet.h \
    Reachability/DFRSHash.h \
    PQL/PQL.h \
    PQL/Expressions.h \
    Structures/BlockStack.h \
    Structures/PriorityQueue.h \
    Reachability/HashUnderApproximation.h \
    Reachability/BestFirstReachabilityStrategy.h \
    Reachability/ClosestFirstReachability.h \
    PQL/CompiledCondition.h
	#Reachability/CoverabilityTreeNode.h

TARGET			= ../PetriEngine

include(flex.pri)
include(bison.pri)

FLEXSOURCES		+= \
	PQL/PQLQueryTokens.l \
	PQL/PQLAssignmentTokens.l \
	PQL/SUMoQueryTokens.l
BISONSOURCES	+= \
	PQL/PQLQueryParser.y \
	PQL/PQLAssignmentParser.y \
	PQL/SUMoQueryParser.y
