/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "QueryThread.h"

#include "../NetItems/PetriNetScene.h"

#include <PetriEngine/PetriNetBuilder.h>

#include <PetriEngine/PQL/PQLParser.h>
#include <PetriEngine/PQL/PQL.h>
#include <PetriEngine/PQL/CompiledCondition.h>
#include <PetriEngine/PQL/Contexts.h>

using namespace PetriEngine;
using namespace PetriEngine::PQL;
using namespace PetriEngine::Reachability;

QueryThread::QueryThread(QString query,
						 QString strategy,
						 PetriNetScene *net,
						 bool jit,
						 QObject *parent)
 : QThread(parent), _strategy(strategy.toStdString())
{
	_finishTime = 0;
	_isAborted = false;
	abortLock.lock();

	//Build petri net
	PetriNetBuilder builder(jit);
	net->produce(&builder);
	_net = builder.makePetriNet();
	_m0 = builder.makeInitialMarking();
	_a0 = builder.makeInitialAssignment();

	//Parse query
	_query = ParseQuery(query.toStdString());
	if(_query){
		AnalysisContext context(*_net);
		if(jit){
			CompiledCondition* CC = new CompiledCondition(_query);
			CC->analyze(context);
			if(context.errors().size() > 0){
				delete CC;
				CC = NULL;
				_query = NULL;
			}else{
				if(CC->compile()){
					_query = CC;
				}else{
					delete CC;
					CC = NULL;
					_query = NULL;
				}
			}
		}else{
			_query->analyze(context);
			if(context.errors().size() > 0){
				delete _query;
				_query = NULL;
			}
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

	//Set the start clock
	_startClock = clock();

	//Run the strategy
	_result = strategy->reachable(*_net, _m0, _a0, _query);

	_finishTime = ((qreal)(clock() - _startClock)) / (qreal)CLOCKS_PER_SEC;
}

void QueryThread::abort(){
	if(!_isAborted){
		_isAborted = true;
		abortLock.unlock();
	}
}

void QueryThread::emitCompleted(){
	emit completed(this, _finishTime);
}

/** Emit progressChanged signal (used by QueryProgressReporter) */
void QueryThread::emitProgressChanged(qreal progress){
	qreal time = ((qreal)(clock() - _startClock)) / (qreal)CLOCKS_PER_SEC;
	emit progressChanged(this, progress, time);
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
