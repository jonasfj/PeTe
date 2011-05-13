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
	//TODO: Save places and variables, for test parsing and context analysis
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
