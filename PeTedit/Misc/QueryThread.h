#ifndef QUERYTHREAD_H
#define QUERYTHREAD_H

#include <QThread>
#include <QMutex>

#include "PetriEngine/Reachability/ReachabilitySearchStrategy.h"

#include <string>

class PetriNetScene;

/** Thread class for running a ReachabilityStrategy */
class QueryThread : public QThread
{
    Q_OBJECT

	/** Progress reporter for the ReachabilityStrategy */
	class QueryProgressReporter : public PetriEngine::ProgressReporter{
	public:
		QueryProgressReporter(QueryThread& thread);
		void reportProgress(double status);
		bool abortRequested();
	private:
		QueryThread& _thread;
	};
public:
	explicit QueryThread(QString query,
						 QString strategy,
						 PetriNetScene* net,
						QObject *parent);

	/** Get the result of this query, assuming it finished, otherwise undefined */
	const PetriEngine::Reachability::ReachabilityResult& result() const{
		return _result;
	}
	/** True, if abortion have been requested, or forced */
	bool isAborted() const { return _isAborted; }
protected:
	void run();
private:
	bool _isAborted;
	QMutex abortLock;
	std::string _strategy;
	PetriEngine::PQL::Condition* _query;
	PetriEngine::PetriNet* _net;
	PetriEngine::MarkVal* _m0;
	PetriEngine::VarVal* _a0;
	PetriEngine::Reachability::ReachabilityResult _result;
	void emitProgressChanged(double progress);
private slots:
	void emitCompleted();
signals:
	/** Occurs when the strategy reports progress */
	void progressChanged(QueryThread* thread, qreal progress);

	/** Report that this thread was either finished or terminated */
	void completed(QueryThread* thread);
public slots:
	/** Abort this operation, no garentee as to when this happens is provided */
	void abort();
};

#endif // QUERYTHREAD_H
