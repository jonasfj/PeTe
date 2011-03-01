/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

//Thanks to Nokia providing us with such cool examples, the completer code here
//is just a cleanup version of what you may find at:
//http://doc.trolltech.com/4.7/tools-customcompleter-textedit-cpp.html



#include "QueryEdit.h"
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QFile>
#include <QDebug>
#include <QStringList>

QueryEdit::QueryEdit(QWidget *parent) :
	QTextEdit(parent)
{
	_completer = NULL;
	this->setAcceptRichText(false);
}


void QueryEdit::initializeSpecialPowers(const QStringList& places){
	QCompleter* completer = new QCompleter(this->parent());

	QStringList words = places;

	//Add query reserved words
	QFile file(":/QueryEdit/QueryReservedWords.list");
	if(file.open(QIODevice::ReadOnly)){
		while(!file.atEnd()){
			QByteArray line = file.readLine();
			if(!line.isEmpty()){
				words.append(line.trimmed());
			}
		}
		file.close();
	}
	words.sort();

	completer->setModel(new QStringListModel(words, completer));
	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(false);
	this->setCompleter(completer);
}

void QueryEdit::setCompleter(QCompleter* completer){
	if(_completer){
		disconnect(_completer, 0, this, 0);
		delete _completer;
		_completer = NULL;
	}
	_completer = completer;
	if(!_completer)
		return;
	_completer->setWidget(this);
	_completer->setCompletionMode(QCompleter::PopupCompletion);
	_completer->setCaseSensitivity(Qt::CaseInsensitive);
	connect(_completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}

void QueryEdit::focusInEvent(QFocusEvent* e){
	if(_completer)
		_completer->setWidget(this);
	QTextEdit::focusInEvent(e);
}

void QueryEdit::keyPressEvent(QKeyEvent* e){
	if(_completer && _completer->popup()->isVisible()){
		switch (e->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			 e->ignore();
			 return;
		default:
			break;
		}
	}
	//Don't process shortcut, if there's a completer
	bool isShortcut = (e->key() == Qt::Key_E && e->modifiers() & Qt::AltModifier);
	if(!_completer || !isShortcut)
		QTextEdit::keyPressEvent(e);

	//Don't do anything
	bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (ctrlOrShift && e->text().isEmpty())
		return;

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word

	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();
	if(!isShortcut &&
	   (hasModifier || e->text().isEmpty() ||
		completionPrefix.length() < 1 ||
		eow.contains(e->text().right(1)))){
		_completer->popup()->hide();
		return;
	}

	if(completionPrefix != _completer->completionPrefix()){
		_completer->setCompletionPrefix(completionPrefix);
		_completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(_completer->popup()->sizeHintForColumn(0)
				+ _completer->popup()->verticalScrollBar()->sizeHint().width());
	//Make completer popup
	_completer->complete(cr);
}

QString QueryEdit::textUnderCursor() const{
	QTextCursor c = textCursor();
	c.select(QTextCursor::WordUnderCursor);
	return c.selectedText();
}

void QueryEdit::insertCompletion(const QString& completion){
	if(_completer->widget() != this)
		return;
	QTextCursor c = textCursor();
	int completed = completion.length() - _completer->completionPrefix().length();
	c.movePosition(QTextCursor::Left);
	c.movePosition(QTextCursor::EndOfWord);
	c.insertText(completion.right(completed));
	setTextCursor(c);
}
