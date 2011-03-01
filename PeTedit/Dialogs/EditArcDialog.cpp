#include "EditArcDialog.h"
#include "ui_EditArcDialog.h"

EditArcDialog::EditArcDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditArcDialog)
{
    ui->setupUi(this);
}

void EditArcDialog::setInfo(const QString &info){
	ui->infoLabel->setText(info);
}

int EditArcDialog::weight() const {
	return ui->spinWeight->value();
}

void EditArcDialog::setWeight(int weight){
	ui->spinWeight->setValue(weight);
}

EditArcDialog::~EditArcDialog()
{
    delete ui;
}
