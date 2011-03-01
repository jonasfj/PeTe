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
    explicit QueryDialog(QWidget *parent = 0);

	/** Set places to enable syntax highlighting and auto completion */
	void setPlaces(const QStringList& places);

    ~QueryDialog();

private:
    Ui::QueryDialog *ui;
};

#endif // QUERYDIALOG_H
