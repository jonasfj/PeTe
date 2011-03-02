#include "QueryDialog.h"
#include "ui_QueryDialog.h"

QueryDialog::QueryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryDialog)
{
    ui->setupUi(this);
}


QString QueryDialog::query(){
	return ui->queryEdit->toPlainText();
}

void QueryDialog::setQuery(QString query){
	ui->queryEdit->setPlainText(query);
}

void QueryDialog::setPlaces(const QStringList &places){
	ui->queryEdit->initializeSpecialPowers(places);
}


QueryDialog::~QueryDialog()
{
    delete ui;
}
