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

	void loadSettings();
	void saveSettings();
	void closeEvent(QCloseEvent *e);
private slots:
	void resizeQueryView();
	void resizeVariableView();
	void resizeValidationView();
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
	void on_aboutAction_triggered();
	void on_undoAction_triggered();
	void on_redoAction_triggered();

	void on_importSUMoQueriesAction_triggered();

private:
	/******************** Cached Settings ********************/
	/** Last folder loaded or saved to (handled by load-/saveSettings) */
	QString lastLoadSavePath;
	/** Last folder exported to (handled by load-/saveSettings) */
	QString lastExportPath;
	/** Last folder imported from (handled by load-/saveSettings) */
	QString lastImportPath;
};

#endif // MAINWINDOW_H
