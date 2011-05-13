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
#include "QueryDialog.h"
#include "ui_QueryDialog.h"
#include "../Widgets/QueryEdit.h"

#include <QMessageBox>

#include <PetriEngine/PQL/PQLParser.h>
#include <PetriEngine/Reachability/ReachabilitySearchStrategy.h>
#include <PetriEngine/PQL/PQL.h>
#include <PetriEngine/PQL/Contexts.h>

QueryDialog::QueryDialog(const QueryModel::Query& query, QWidget *parent)
	: QDialog(parent), ui(new Ui::QueryDialog)
{
    ui->setupUi(this);
	ui->nameEdit->setText(query.name);
	ui->queryEdit->setPlainText(query.query);
	ui->jitCheckBox->setChecked(query.jit);

	int strategyIndex  = 0;
	std::vector<std::string> strats = PetriEngine::Reachability::ReachabilitySearchStrategy::listStrategies();
	for(size_t i = 0; i < strats.size(); i++){
		ui->strategyBox->addItem(strats[i].c_str());
		if(query.strategy == strats[i].c_str())
			strategyIndex = i;
	}
	ui->strategyBox->setCurrentIndex(strategyIndex);
}

void QueryDialog::accept(){
	//Test for parsing errors
	QString query = ui->queryEdit->toPlainText();
	PetriEngine::PQL::Condition* cond = PetriEngine::PQL::ParseQuery(query.toStdString());
	//TODO: Do context analysis here and display errors we get from this
	//		Errors should be annotated in the tree and reported during analysis
	if(cond){
		PetriEngine::PQL::AnalysisContext context(_places, _variables);
		cond->analyze(context);

		bool hasError = false;
		for(size_t i = 0; i < context.errors().size(); i++){
			QMessageBox::warning(this, "Context analysis error", context.errors()[i].text().c_str());
			//TODO: Underline the error in the view...
			hasError = true;
		}

		delete cond;
		cond = NULL;

		if(!hasError)
			QDialog::accept();
	}else{
		QMessageBox::warning(this, "Parsing error!", "There was some parsing error!");
	}
}


QString QueryDialog::name() const{
	return ui->nameEdit->text();
}

QString QueryDialog::query() const{
	return ui->queryEdit->toPlainText();
}

QString QueryDialog::strategy() const{
	return ui->strategyBox->currentText();
}

bool QueryDialog::jit() const{
	return ui->jitCheckBox->isChecked();
}

void QueryDialog::setIdentifiers(const QStringList &places, const QStringList& variables){
	ui->queryEdit->initializeSpecialPowers(places, variables);
	_places.clear();
	foreach(const QString& place, places)
		_places.push_back(place.toStdString());
	_variables.clear();
	foreach(const QString& variable, variables)
		_variables.push_back(variable.toStdString());
}

QueryDialog::~QueryDialog()
{
    delete ui;
	ui = NULL;
}
