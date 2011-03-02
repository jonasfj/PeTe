#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <QDialog>

namespace Ui {
    class QueryDialog;
}

class QueryDialog : public QDialog
{
    Q_OBJECT

public:

	QString query();
	void setQuery(QString query);

    explicit QueryDialog(QWidget *parent = 0);
    ~QueryDialog();
private:
	QString _query;
    Ui::QueryDialog *ui;
};

#endif // QUERYDIALOG_H
