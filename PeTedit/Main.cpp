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
#include <PetriEngine/PQL/Contexts.h>

using namespace PetriEngine;
using namespace PetriEngine::Reachability;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



	//Set metadata (Used by QSettings, etc)
	QCoreApplication::setApplicationName("PeTe");
	QCoreApplication::setApplicationVersion("0.1");
	QCoreApplication::setOrganizationName("PeTe Development Team");
	QCoreApplication::setOrganizationDomain("https://github.com/jopsen/PeTe");

	//Parsed Arguments:
	QString queryString;
	QString fileName;
	QString strategy;
	//QString queryString = "P1==1";
	//QString fileName = "/home/thomas/DAT4/PeTe-build-desktop/Testnetwork";

	bool sumoQuery = false;
	bool showGUI = true;
	QStringList args = a.arguments();
	for(int i = 1; i < args.size(); i++){
		if(args[i] == "--query") {
			queryString = args[++i];
			showGUI = false;
		} else if(args[i] == "--query-sumo"){
			queryString = args[++i];
			sumoQuery = true;
			showGUI = false;
		} else if(args[i] == "--strategies"){
			std::vector<std::string> strats = ReachabilitySearchStrategy::listStrategies();
			for(size_t i = 0; i < strats.size(); i++)
				printf("%s\n",strats[i].c_str());
			showGUI = false;
		} else if(args[i] == "--strategy"){
			strategy = args[++i];
			showGUI = false;
		} else if(args[i] == "--help") {
			printf("Usage: pete [net] [--query <query>] [--query-sumo <query>] [--help] [--strategies] [--strategy <strategy>]\n");
			showGUI = false;
		} else
			fileName = args[i];
	}

	// Run the GUI if no arguments were given
	if(showGUI){
		MainWindow w;
		w.show();
		return a.exec();
	}

	if(fileName.count() > 0){
		// Load file
		QFile file(fileName);
		if(!file.open(QIODevice::ReadOnly))
			return 8;
		PetriEngine::PetriNetBuilder builder;
		PNMLParser p;
		p.parse(&file, &builder);
		file.close();
		// Load network
		PetriNet* net = builder.makePetriNet();
		MarkVal* m0 = builder.makeInitialMarking();
		VarVal* a0 = builder.makeInitialAssignment();

		PQL::Condition* query;
		if(sumoQuery)
			query = PQL::ParseSUMoQuery(queryString.toStdString()).query;
		else
			query = PQL::ParseQuery(queryString.toStdString());

		//Load and analyze query
		if(!query) {
			std::cout<<"Syntax error in query"<<std::endl;
			return 9;
		}
		PQL::AnalysisContext context(*net);
		query->analyze(context);
		for(size_t i = 0; i < context.errors().size(); i++){
			std::cout<<context.errors()[i].toString()<<std::endl;
		}
		//Load up reachability engine
		ReachabilitySearchStrategy* strat;
		if(!strategy.isEmpty())
			strat = ReachabilitySearchStrategy::createStrategy(strategy.toStdString());
		else
			strat = new DFRSHash();

		ReachabilityResult result;
		result = strat->reachable(*net, m0, a0, query);
		delete query;
		query = NULL;
		delete strat;
		strat = NULL;

		//Print result
		std::cout<<result.explanation()<<std::endl;
	}
	return 0;
}
