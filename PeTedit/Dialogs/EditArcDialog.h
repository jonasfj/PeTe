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
#ifndef EDITARCDIALOG_H
#define EDITARCDIALOG_H

#include <QDialog>

namespace Ui {
    class EditArcDialog;
}

class EditArcDialog : public QDialog
{
    Q_OBJECT

public:
	explicit EditArcDialog(QWidget *parent = 0);
    ~EditArcDialog();
	void setInfo(const QString& info);
	int weight() const;
	void setWeight(int weight);
private:
    Ui::EditArcDialog *ui;
};

#endif // EDITARCDIALOG_H
