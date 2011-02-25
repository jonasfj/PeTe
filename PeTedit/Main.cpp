#include <QtGui/QApplication>
#include "MainWindow.h"

#include "DataFormats/PNMLParser.h"
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
