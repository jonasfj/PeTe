#include <QtGui/QApplication>
#include <QFile>
#include <iostream>
#include "MainWindow.h"

#include "DataFormats/PNMLParser.h"
#include <PetriEngine/PetriNetBuilder.h>
#include <PetriEngine/PetriNet.h>
#include <PetriEngine/PQL/PQLParser.h>
#include <PetriEngine/Reachability/DepthFirstReachabilitySearch.h>
#include <PetriEngine/Reachability/DFRSHash.h>
#include <PetriEngine/PQL/PQL.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//Parsed Arguments:
	QString queryString;
	QString fileName;
	//QString queryString = "P1==1";
	//QString fileName = "/home/thomas/DAT4/PeTe-build-desktop/Testnetwork";

	QStringList args = a.arguments();
	for(int i = 1; i < args.size(); i++){
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
	PetriEngine::PetriNetBuilder builder;
	PNMLParser p;
	p.parse(&file, &builder);
	file.close();
	// Load network
	PetriEngine::PetriNet* net = builder.makePetriNet();
	PetriEngine::MarkVal* m0 = builder.makeInitialMarking();
	PetriEngine::VarVal* a0 = builder.makeInitialAssignment();
	//Load and analyze query
	PetriEngine::PQL::Condition* query = PetriEngine::PQL::ParseQuery(queryString.toStdString());
	if(!query) {
		std::cout<<"Syntax error in query"<<std::endl;
		return 9;
	}
	PetriEngine::PQL::AnalysisContext context(*net);
	query->analyze(context);
	for(size_t i = 0; i < context.errors().size(); i++){
		std::cout<<context.errors()[i].toString()<<std::endl;
	}
	//Load up reachability engine
	PetriEngine::Reachability::DFRSHash dfs;
	PetriEngine::Reachability::ReachabilityResult result;
	result = dfs.reachable(*net, m0, a0, query);
	delete query;
	query = NULL;

	//Print result
	std::cout<<result.explanation()<<std::endl;

	return 0;
}
