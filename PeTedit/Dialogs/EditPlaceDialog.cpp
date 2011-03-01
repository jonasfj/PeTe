#include "EditPlaceDialog.h"
#include "ui_EditPlaceDialog.h"

EditPlaceDialog::EditPlaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditPlaceDialog)
{
    ui->setupUi(this);
}

QString EditPlaceDialog::name(){
	return ui->nameEdit->text();
}

void EditPlaceDialog::setName(QString name){
	ui->nameEdit->setText(name);
}

int EditPlaceDialog::tokens(){
	return ui->tokenSpin->value();
}

void EditPlaceDialog::setTokens(int tokens){
	ui->tokenSpin->setValue(tokens);
}

EditPlaceDialog::~EditPlaceDialog()
{
    delete ui;
}
