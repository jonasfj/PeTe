#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <QDialog>
#include "../Misc/QueryItem.h"

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
	explicit QueryDialog(QueryItem* item, QWidget *parent = 0);

	/** Set identifers to enable syntax highlighting and auto completion */
	void setIdentifiers(const QStringList& places, const QStringList& variables);

	QString name() const;
	QString query() const;
	QString strategy() const;

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
