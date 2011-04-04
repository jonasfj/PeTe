#include "QueryThread.h"

#include "../NetItems/PetriNetScene.h"

#include "PetriEngine/PetriNetBuilder.h"

#include "PetriEngine/PQL/PQLParser.h"
#include "PetriEngine/PQL/PQLExpressions.h"


using namespace PetriEngine;
using namespace PetriEngine::PQL;
using namespace PetriEngine::Reachability;

QueryThread::QueryThread(QString query,
						 QString strategy,
						 PetriNetScene *net,
						 QObject *parent)
 : QThread(parent), _strategy(strategy.toStdString())
{
	_isAborted = false;
	abortLock.lock();

	//Build petri net
	PetriNetBuilder builder;
	net->produce(&builder);
	_net = builder.makePetriNet();
	_m0 = builder.makeInitialMarking();
	_a0 = builder.makeInitialAssignment();

	//Parse query
	_query = ParseQuery(query.toStdString());
	if(_query){
		AnalysisContext context(*_net);
		_query->analyze(context);
		if(context.errors().size() > 0){
			delete _query;
			_query = NULL;
		}
	}
	connect(this, SIGNAL(finished()), this, SLOT(emitCompleted()));
	connect(this, SIGNAL(terminated()), this, SLOT(emitCompleted()));
}

/** Main execution method */
void QueryThread::run(){
	//Don't every reuse this thread... bad stuff could happen, who knows or cares
	if(_isAborted)
		return;
	if(!_query){
		_result = ReachabilityResult(ReachabilityResult::Unknown,
									 "Please validate the query!");
		abort();
		return;
	}

	//Create reachability strategy
	ReachabilitySearchStrategy* strategy;
	strategy = ReachabilitySearchStrategy::createStrategy(_strategy);

	//Create and set progress reporter
	QueryProgressReporter reporter(*this);
	strategy->setProgressReporter(&reporter);

	//Run the strategy
	_result = strategy->reachable(*_net, _m0, _a0, _query);
}

void QueryThread::abort(){
	if(!_isAborted){
		_isAborted = true;
		abortLock.unlock();
	}
}

void QueryThread::emitCompleted(){
	emit completed(this);
}

/** Emit progressChanged signal (used by QueryProgressReporter) */
void QueryThread::emitProgressChanged(double progress){
	emit progressChanged(this, progress);
}

QueryThread::QueryProgressReporter::QueryProgressReporter(QueryThread &thread)
 : _thread(thread) {}

void QueryThread::QueryProgressReporter::reportProgress(double status){
	_thread.emitProgressChanged(status);
}

bool QueryThread::QueryProgressReporter::abortRequested(){
	if(!_thread.abortLock.tryLock())
		return false;
	// Unlock so that this method can be invoked again
	_thread.abortLock.unlock();
	return true;
}
