#include "ImportDTAPNDialog.h"
#include "ui_ImportDTAPNDialog.h"

#include <QFileDialog>

ImportDTAPNDialog::ImportDTAPNDialog(QWidget *parent, QString defaultPath) :
    QDialog(parent),
    ui(new Ui::ImportDTAPNDialog)
{
	this->defaultPath = defaultPath;
    ui->setupUi(this);
}

ImportDTAPNDialog::~ImportDTAPNDialog(){
    delete ui;
}

void ImportDTAPNDialog::on_browseButton_clicked(){
	QString path = defaultPath;
	if(!ui->fileLineEdit->text().isEmpty())
		path = ui->fileLineEdit->text();
	QString fname = QFileDialog::getOpenFileName(this, tr("Select DTAPN"), path);
	if(!fname.isEmpty())
		ui->fileLineEdit->setText(fname);
}

QString ImportDTAPNDialog::filePath(){
	return ui->fileLineEdit->text();
}

int ImportDTAPNDialog::bound(){
	return ui->boundSpinBox->value();
}
