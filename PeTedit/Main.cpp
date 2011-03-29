#include <QtGui/QApplication>
#include <QFile>
#include <iostream>
#include "MainWindow.h"

#include "DataFormats/PNMLParser.h"
#include "PetriNetFactory.h"
#include "PetriNet.h"
#include "PQL/PQLParser.h"
#include "Reachability/DepthFirstReachabilitySearch.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//Parsed Arguments:
	QString queryString;
	QString fileName;
	//QString queryString = "P1==1";
	//QString fileName = "/home/thomas/DAT4/PeTe-build-desktop/Testnetwork";

	QStringList args = a.arguments();
	for(size_t i = 1; i < args.size(); i++){
		if(args[i] == "--query")
			queryString = args[++i];
		else
			fileName = args[i];
	}

	if(queryString.isEmpty()){
		MainWindow w;
		w.show();
		return a.exec();
	}

	// Load file
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
		return 8;
	PetriEngine::PetriNetFactory fac;
	PNMLParser p;
	p.parse(&file, &fac);
	file.close();
	// Load network
	PetriEngine::PetriNet* net = fac.makePetriNet();
	PetriEngine::MarkVal* m0 = fac.makeInitialMarking();
	PetriEngine::VarVal* a0 = fac.makeInitialAssignment();
	//Load and analyze query
	PetriEngine::PQL::Condition* query = PetriEngine::PQL::ParseQuery(queryString.toStdString());
	if(!query) {
		std::cout<<"Syntax error in query"<<std::endl;
		return 9;
	}
	PetriEngine::PQL::AnalysisContext context(*net, true);
	query->analyze(context);
	for(int i = 0; i < context.errors().size(); i++){
		std::cout<<context.errors()[i].toString()<<std::endl;
	}
	//Load up reachability engine
	PetriEngine::Reachability::DepthFirstReachabilitySearch dfs;
	bool reachable = dfs.reachable(*net, m0, a0, query);
	delete query;
	query = NULL;

	//Print result
	std::string retString = reachable ? "Query is satisfiable!" : "Query is NOT satisfiable!";
	std::cout<<retString<<std::endl;

	return 0;
}
