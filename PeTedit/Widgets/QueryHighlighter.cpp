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
#include "QueryHighlighter.h"
#include <QFile>

QueryHighlighter::QueryHighlighter(const QStringList& places,
								   const QStringList& variables,
								   QTextEdit *parent) :
    QSyntaxHighlighter(parent)
{
	QTextCharFormat placeFormat;
	placeFormat.setForeground(Qt::blue);
	foreach(const QString& place, places){
		HighlightingRule rule;
		rule.pattern = QRegExp("\\b" + place + "\\b");
		rule.format = placeFormat;
		rules.append(rule);
	}

	QTextCharFormat variableFormat;
	variableFormat.setForeground(Qt::darkGray);
	foreach(const QString& variable, variables){
		HighlightingRule rule;
		rule.pattern = QRegExp("\\b" + variable + "\\b");
		rule.format = variableFormat;
		rules.append(rule);
	}

	QTextCharFormat literalFormat;
	//literalFormat.setFontItalic(true);
	//literalFormat.setForeground(Qt::green);
	HighlightingRule literalRule;
	literalRule.pattern = QRegExp("\\b[0-9]+\\b");
	literalRule.format = literalFormat;
	rules.append(literalRule);

	QTextCharFormat reservedWordsFormat;
	reservedWordsFormat.setFontWeight(QFont::Bold);
	QFile file(":/QueryEdit/QueryReservedWords.list");
	if(file.open(QIODevice::ReadOnly)){
		while(!file.atEnd()){
			QString line = file.readLine().trimmed();
			if(!line.isEmpty()){
				HighlightingRule rule;
				rule.pattern = QRegExp(QRegExp::escape(line));
				rule.format = reservedWordsFormat;
				rules.append(rule);
			}
		}
		file.close();
	}
}

void QueryHighlighter::highlightBlock(const QString &text){
	foreach(HighlightingRule rule, rules){
		QRegExp exp(rule.pattern);
		int index = exp.indexIn(text);
		while(index >= 0){
			int length = exp.matchedLength();
			setFormat(index, length, rule.format);
			index = exp.indexIn(text, index + length);
		}
	}
}
