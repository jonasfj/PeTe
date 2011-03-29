#include "QueryDialog.h"
#include "ui_QueryDialog.h"
#include "../Widgets/QueryEdit.h"

#include <QMessageBox>

#include "PQL/PQLParser.h"

QueryDialog::QueryDialog(QueryItem* item, QWidget *parent)
	: QDialog(parent), ui(new Ui::QueryDialog)
{
    ui->setupUi(this);
	ui->nameEdit->setText(item->name());
	ui->queryEdit->setPlainText(item->query());
	//TODO: See QueryItem.h for how to implement the StrategyFactory
	ui->strategyBox->addItem("Karp-Miller L1");
	ui->strategyBox->addItem("Naive Karp-Miller DFS");

	//TODO: Refactor this, see QueryItem.h
	if(item->strategy() == "Karp-Miller L1")
		ui->strategyBox->setCurrentIndex(0);
	else
		ui->strategyBox->setCurrentIndex(1);
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

void QueryDialog::setIdentifiers(const QStringList &places){
	ui->queryEdit->initializeSpecialPowers(places);
}

QueryDialog::~QueryDialog()
{
    delete ui;
	ui = NULL;
}
