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
#ifndef EDITTRANSITIONDIALOG_H
#define EDITTRANSITIONDIALOG_H

#include <QDialog>

namespace Ui {
    class EditTransitionDialog;
}

class EditTransitionDialog : public QDialog
{
    Q_OBJECT

public:

	QString name() const;
	void setName(QString name);

	void setIdentifiers(const QStringList& places, const QStringList& variables);

	QString preConditions() const;
	void setPreConditions(QString conditions);

	QString postConditions() const;
	void setPostConditions(QString conditions);

    explicit EditTransitionDialog(QWidget *parent = 0);
    ~EditTransitionDialog();

private:
	QString _name;
    Ui::EditTransitionDialog *ui;
};

#endif // EDITTRANSITIONDIALOG_H
