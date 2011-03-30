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
