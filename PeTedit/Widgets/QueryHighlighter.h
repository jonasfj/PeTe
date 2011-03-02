#ifndef QUERYHIGHLIGHTER_H
#define QUERYHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QueryHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

	struct HighlightingRule{
		QRegExp pattern;
		QTextCharFormat format;
	};
public:
	explicit QueryHighlighter(const QStringList& places, QTextEdit* parent);

	void highlightBlock(const QString &text);
signals:

public slots:

private:
	QList<HighlightingRule> rules;
};

#endif // QUERYHIGHLIGHTER_H
