#include "QueryDialog.h"
#include "ui_QueryDialog.h"

QueryDialog::QueryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryDialog)
{
    ui->setupUi(this);
}

void QueryDialog::setPlaces(const QStringList &places){
	ui->queryEdit->initializeSpecialPowers(places);
}


QueryDialog::~QueryDialog()
{
    delete ui;
}
