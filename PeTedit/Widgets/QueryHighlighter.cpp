#include "QueryHighlighter.h"
#include <QFile>

QueryHighlighter::QueryHighlighter(const QStringList& places, QTextEdit *parent) :
    QSyntaxHighlighter(parent)
{
	QTextCharFormat placeFormat;
	placeFormat.setForeground(Qt::blue);
	foreach(QString place, places){
		HighlightingRule rule;
		rule.pattern = QRegExp("\\b" + place + "\\b");
		rule.format = placeFormat;
		rules.append(rule);
	}

	QTextCharFormat literalFormat;
	literalFormat.setFontItalic(true);
	literalFormat.setForeground(Qt::green);
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
