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
#include <QtGui/QApplication>
#include <QFile>
#include <iostream>
#include "MainWindow.h"
#include "time.h"

#include "DataFormats/PNMLParser.h"
#include "Misc/QueryListBuilder.h"
#include <PetriEngine/PetriNetBuilder.h>
#include <PetriEngine/PetriNet.h>
#include <PetriEngine/PQL/PQLParser.h>
#include <PetriEngine/Reachability/ReachabilitySearchStrategy.h>
#include <PetriEngine/PQL/PQL.h>
#include <PetriEngine/PQL/Contexts.h>

using namespace PetriEngine;
using namespace PetriEngine::Reachability;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//Set metadata (Used by QSettings, etc)
	QCoreApplication::setApplicationName("PeTe");
	QCoreApplication::setApplicationVersion("1.0");
	QCoreApplication::setOrganizationName("PeTe Development Team");
	QCoreApplication::setOrganizationDomain("https://github.com/jopsen/PeTe");

	//Parsed Arguments:
	QString queryName;
	QString queryString;
	QString fileName;
	QString strategy;

	bool showGUI = true;
	bool listQueries = false;
	QStringList args = a.arguments();
	for(int i = 1; i < args.size(); i++){
		if(args[i] == "--query") {
			queryName = args[++i];
			showGUI = false;
		} else if(args[i] == "--query-string") {
			queryString = args[++i];
			showGUI = false;
		}else if(args[i] == "--list-queries") {
			listQueries = true;
			showGUI = false;
		}else if(args[i] == "--strategies"){
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
		QueryListBuilder qBuilder;
		PNMLParser p;
		p.parse(&file, &builder, &qBuilder);
		file.close();
		// Load network
		PetriNet* net = builder.makePetriNet();
		MarkVal* m0 = builder.makeInitialMarking();
		VarVal* a0 = builder.makeInitialAssignment();
		QList<QueryModel::Query> queries = qBuilder.makeQueries();

		if(listQueries){
			for(QList<QueryModel::Query>::iterator it = queries.begin(); it != queries.end(); it++)
				printf("%s\n",(*it).name.toStdString().c_str());
			return 0;
		}

		// Find the query
		if(!queryName.isNull()){
			for(QList<QueryModel::Query>::iterator it = queries.begin(); it != queries.end(); it++){
				if((*it).name == queryName){
					queryString = (*it).query;
					break;
				}
			}
		}

		if(queryString.isNull()){
			std::cerr<<"Query was not found in the given network.";
			return 1;
		}

		PQL::Condition* query = PQL::ParseQuery(queryString.toStdString());

		//Load and analyze query
		if(!query) {
			std::cerr<<"Syntax error in query"<<std::endl;
			return 2;
		}
		PQL::AnalysisContext context(*net);
		query->analyze(context);
		for(size_t i = 0; i < context.errors().size(); i++){
			std::cerr<<context.errors()[i].toString()<<std::endl;
		}

		//Load up reachability engine
		ReachabilitySearchStrategy* strat;
		strat = ReachabilitySearchStrategy::createStrategy(strategy.toStdString());

		ReachabilityResult result;

		clock_t startClock = clock();
		result = strat->reachable(*net, m0, a0, query);
		qreal finishTime = ((qreal)(clock() - startClock)) / (qreal)CLOCKS_PER_SEC;

		delete query;
		query = NULL;
		delete strat;
		strat = NULL;

		// Trim file name incase of alternate folder
		QString name;
		bool chop = false;
		int index = 0;
		for(int i = 0; i < fileName.size(); i++){
			if(fileName.at(i) == '/'){
				chop = true;
				index = i;
			}
		}
		if(chop)
			name = fileName.remove(0, index+1);

		//Print result
		std::string r;
		if(result.result() == ReachabilityResult::Satisfied)
			r = "Satisfied";
		else if(result.result() == ReachabilityResult::NotSatisfied)
			r = "Not satisfiable";
		else{
			Q_ASSERT(result.result() == ReachabilityResult::Unknown);
			r = "Unknown";
		}
		if(queryName.isNull())
			queryName = "Custom Query";

		std::cout<<name.toStdString()<<",\t"<<queryName.toStdString()<<",\t"<<strategy.toStdString()<<",\t"<<r<<",\t"<<finishTime<<",\t"<<result.expandedStates()<<",\t"<<result.exploredStates()<<",\t"<<result.pathLength()<<std::endl;
	}
	return 0;
}
