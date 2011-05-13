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
#ifndef QUERYEDIT_H
#define QUERYEDIT_H

#include <QWidget>
#include <QTextEdit>
#include <QCompleter>

class QueryEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit QueryEdit(QWidget *parent = 0);

	void initializeSpecialPowers(const QStringList& places, const QStringList& variables);
	void setCompleter(QCompleter* completer);
	QCompleter* completer() {return _completer;}
protected:
	void keyPressEvent(QKeyEvent* e);
	void focusInEvent(QFocusEvent *e);
private:
	QCompleter* _completer;
	QString textUnderCursor() const;
private slots:
	void insertCompletion(const QString& completion);
signals:

public slots:

};

#endif // QUERYEDIT_H
