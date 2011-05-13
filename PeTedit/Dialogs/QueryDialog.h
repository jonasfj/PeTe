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
#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <QDialog>
#include "../Misc/QueryModel.h"

#include <vector>
#include <string>

namespace Ui {
    class QueryDialog;
}

class QueryDialog : public QDialog
{
    Q_OBJECT

public:
	/** Create instance of QueryDialog, will not take ownership of item !*/
	explicit QueryDialog(const QueryModel::Query& query, QWidget *parent = 0);

	/** Set identifers to enable syntax highlighting and auto completion */
	void setIdentifiers(const QStringList& places, const QStringList& variables);

	QString name() const;
	QString query() const;
	QString strategy() const;
	bool jit() const;

	void accept();

    ~QueryDialog();
private:
	/** Names of places (for context analysis) */
	std::vector<std::string> _places;
	/** Names of variables (for context analysis) */
	std::vector<std::string> _variables;
    Ui::QueryDialog *ui;
};

#endif // QUERYDIALOG_H
