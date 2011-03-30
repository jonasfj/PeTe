#include "QueryDialog.h"
#include "ui_QueryDialog.h"
#include "../Widgets/QueryEdit.h"

#include <QMessageBox>

#include "PQL/PQLParser.h"
#include "Reachability/ReachabilitySearchStrategy.h"

QueryDialog::QueryDialog(QueryItem* item, QWidget *parent)
	: QDialog(parent), ui(new Ui::QueryDialog)
{
    ui->setupUi(this);
	ui->nameEdit->setText(item->name());
	ui->queryEdit->setPlainText(item->query());

	int strategyIndex  = 0;
	std::vector<std::string> strats = PetriEngine::Reachability::ReachabilitySearchStrategy::listStrategies();
	for(size_t i = 0; i < strats.size(); i++){
		ui->strategyBox->addItem(strats[i].c_str());
		if(item->strategy() == strats[i].c_str())
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
		delete cond;
		cond = NULL;
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

void QueryDialog::setIdentifiers(const QStringList &places, const QStringList& variables){
	ui->queryEdit->initializeSpecialPowers(places, variables);
}

QueryDialog::~QueryDialog()
{
    delete ui;
	ui = NULL;
}
