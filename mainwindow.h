#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoGroup>
#include <QActionGroup>

#include "netitems/petrinetscene.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
	/** Current scene, NULL if none */
	PetriNetScene* currentScene;
	/** Undogroup for the documents */
	QUndoGroup* undoGroup;
	/** Action group for selecting mode */
	QActionGroup* modeActionGroup;

private slots:
	void on_NewTapnAction_triggered();
	void on_tabWidget_currentChanged(int index);
	void on_tabWidget_tabCloseRequested(int index);
	void currentScene_modeChanged(PetriNetScene::Mode mode);
	void modeActionGroup_triggered(QAction* action);
};

#endif // MAINWINDOW_H
