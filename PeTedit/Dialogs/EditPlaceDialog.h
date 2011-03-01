#ifndef EDITPLACEDIALOG_H
#define EDITPLACEDIALOG_H

#include <QDialog>

namespace Ui {
    class EditPlaceDialog;
}

class EditPlaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPlaceDialog(QWidget *parent = 0);
    ~EditPlaceDialog();

	QString name();
	void setName(QString name);

	int tokens();
	void setTokens(int tokens);

private:
	QString _name;
	int _tokens;
    Ui::EditPlaceDialog *ui;
};

#endif // EDITPLACEDIALOG_H
