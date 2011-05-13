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
#ifndef IMPORTDTAPNDIALOG_H
#define IMPORTDTAPNDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportDTAPNDialog;
}

class ImportDTAPNDialog : public QDialog
{
    Q_OBJECT

public:
	explicit ImportDTAPNDialog(QWidget *parent, QString defaultPath);
    ~ImportDTAPNDialog();

	QString filePath();
	int bound();
private slots:
	void on_browseButton_clicked();

private:
	QString defaultPath;
    Ui::ImportDTAPNDialog *ui;
};

#endif // IMPORTDTAPNDIALOG_H
