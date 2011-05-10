TEMPLATE		= lib

#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

QMAKE_CXXFLAGS += $$system(llvm-config --cxxflags all)# | sed -e "s/-fomit-frame-pointer//")
QT			   -= gui core
CONFIG		   += staticlib link_pkgconfig
PKGCONFIG	   += igraph

SOURCES		   += \
	StandardOutputProgressReporter.cpp \
	PetriNet.cpp \
	Reachability/DepthFirstReachabilitySearch.cpp \
    PetriNetBuilder.cpp \
    ValidationBuilder.cpp \
	Reachability/ReachabilitySearchStrategy.cpp \
    PQL/Expressions.cpp \
    PQL/PQL.cpp \
    Reachability/HashUnderApproximation.cpp \
    PQL/CompiledCondition.cpp \
    Structures/DistanceMatrix.cpp \
    LayoutBuilder.cpp \
    Reachability/RandomDFS.cpp \
    Reachability/HeuristicDFS.cpp \
    Reachability/BreadthFirstReachabilitySearch.cpp \
    DTAPN/DTAPNTranslator.cpp \
    DTAPN/DTAPNPrinter.cpp \
    Reachability/BestFirstReachabilitySearch.cpp \
    Structures/StateConstraints.cpp \
    Reachability/LinearOverApprox.cpp \
    Reachability/BestFSCooling.cpp \
    Structures/SmartState.cpp \
    Reachability/MagicSearch.cpp
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
    PQL/PQL.h \
    PQL/Expressions.h \
    Structures/BlockStack.h \
    Structures/PriorityQueue.h \
    Reachability/HashUnderApproximation.h \
    PQL/CompiledCondition.h \
    PQL/Contexts.h \
    Structures/DistanceMatrix.h \
    Structures/StateAllocator.h \
    LayoutBuilder.h \
    Reachability/RandomDFS.h \
    Reachability/HeuristicDFS.h \
    Reachability/BreadthFirstReachabilitySearch.h \
    AbstractDTAPNBuilder.h \
    DTAPN/DTAPNTranslator.h \
    DTAPN/AbstractDTAPNBuilder.h \
    DTAPN/DTAPNPrinter.h \
    Reachability/BestFirstReachabilitySearch.h \
    Structures/StateConstraints.h \
    Reachability/LinearOverApprox.h \
    Structures/EnhancedPriorityQueue.h \
    Reachability/BestFSCooling.h \
    Structures/SmartState.h \
    Structures/SmartStateAllocator.h \
    Structures/SmartStateSet.h \
    Reachability/MagicSearch.h \
    Structures/Scale.h
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
