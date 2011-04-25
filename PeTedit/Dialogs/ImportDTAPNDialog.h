#ifndef IMPORTDTAPNDIALOG_H
#define IMPORTDTAPNDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportDTAPNDialog;
}

class ImportDTAPNDialog : public QDialog
{
    Q_OBJECT

public:
	explicit ImportDTAPNDialog(QWidget *parent, QString defaultPath);
    ~ImportDTAPNDialog();

	QString filePath();
	int bound();
private slots:
	void on_browseButton_clicked();

private:
	QString defaultPath;
    Ui::ImportDTAPNDialog *ui;
};

#endif // IMPORTDTAPNDIALOG_H
