#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoGroup>
#include <QActionGroup>
#include <QTableView>

#include "NetItems/PetriNetScene.h"

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
	void validationIssuesFound();

	void on_deleteVariable_clicked();
	void on_addVariable_clicked();
	void on_OpenAction_triggered();
	void on_SaveAction_triggered();
	void on_NewTapnAction_triggered();
	void on_tabWidget_currentChanged(int index);
	void on_tabWidget_tabCloseRequested(int index);
	void currentScene_modeChanged(PetriNetScene::Mode mode);
	void modeActionGroup_triggered(QAction* action);
	void on_actionExport_SVG_triggered();
	void on_deleteQuery_clicked();
	void on_queryView_doubleClicked(QModelIndex index);
	void on_addQueryAction_triggered();
	void on_runQueryButton_clicked();
	void on_stopQueryButton_clicked();
	};

#endif // MAINWINDOW_H
