#include "EditTransitionDialog.h"
#include "ui_EditTransitionDialog.h"

EditTransitionDialog::EditTransitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTransitionDialog)
{
    ui->setupUi(this);
	QStringList empty;
	ui->preconditions->initializeSpecialPowers(empty);
	ui->postconditions->initializeSpecialPowers(empty);
}

QString EditTransitionDialog::name() const{
	return ui->nameEdit->text();
}

void EditTransitionDialog::setName(QString name){
	ui->nameEdit->setText(name);
}

EditTransitionDialog::~EditTransitionDialog()
{
    delete ui;
}
