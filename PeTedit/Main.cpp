#include <QtGui/QApplication>
#include "MainWindow.h"

#include <StandardOutputProgressReporter.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

PetriEngine::StandardOutputProgressReporter().reportProgress(0.9);;

    return a.exec();
}
