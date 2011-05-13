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
#include "ImportDTAPNDialog.h"
#include "ui_ImportDTAPNDialog.h"

#include <QFileDialog>

ImportDTAPNDialog::ImportDTAPNDialog(QWidget *parent, QString defaultPath) :
    QDialog(parent),
    ui(new Ui::ImportDTAPNDialog)
{
	this->defaultPath = defaultPath;
    ui->setupUi(this);
}

ImportDTAPNDialog::~ImportDTAPNDialog(){
    delete ui;
}

void ImportDTAPNDialog::on_browseButton_clicked(){
	QString path = defaultPath;
	if(!ui->fileLineEdit->text().isEmpty())
		path = ui->fileLineEdit->text();
	QString fname = QFileDialog::getOpenFileName(this, tr("Select DTAPN"), path);
	if(!fname.isEmpty())
		ui->fileLineEdit->setText(fname);
}

QString ImportDTAPNDialog::filePath(){
	return ui->fileLineEdit->text();
}

int ImportDTAPNDialog::bound(){
	return ui->boundSpinBox->value();
}
