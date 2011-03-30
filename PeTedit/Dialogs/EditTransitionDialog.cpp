#include "EditTransitionDialog.h"
#include "ui_EditTransitionDialog.h"

EditTransitionDialog::EditTransitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTransitionDialog)
{
    ui->setupUi(this);
}

QString EditTransitionDialog::name() const{
	return ui->nameEdit->text();
}

// Set syntax highlighter keywords
void EditTransitionDialog::setIdentifiers(const QStringList& places, const QStringList& variables){
	ui->preconditions->initializeSpecialPowers(places, variables);
	ui->postconditions->initializeSpecialPowers(places, variables);
}

// Set the name text for transition
void EditTransitionDialog::setName(QString name){
	ui->nameEdit->setText(name);
}

QString EditTransitionDialog::preConditions() const{
	return ui->preconditions->toPlainText();
}

// Set the query text for pre-conditions
void EditTransitionDialog::setPreConditions(QString conditions){
	ui->preconditions->setText(conditions);
}

QString EditTransitionDialog::postConditions() const{
	return ui->postconditions->toPlainText();
}

// Set the query text for post-conditions
void EditTransitionDialog::setPostConditions(QString conditions){
	ui->postconditions->setText(conditions);
}

EditTransitionDialog::~EditTransitionDialog()
{
    delete ui;
}
