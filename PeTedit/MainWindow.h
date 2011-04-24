#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoGroup>
#include <QActionGroup>
#include <QTableView>
#include <QDoubleSpinBox>

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
	QUndoGroup undoGroup;
	/** Action group for selecting mode */
	QActionGroup* modeActionGroup;
	/** Zoom spin box, used for zooming */
	QDoubleSpinBox* zoomToolSpinBox;

	void loadSettings();
	void saveSettings();
	void closeEvent(QCloseEvent *e);
	void createUndoActions();
	void createToggleToolsbars();
	void createZoomTool();
private slots:
	void updateWindowTitle();
	void resizeQueryView();
	void resizeVariableView();
	void resizeValidationView();
	void validationIssuesFound();
	void on_deleteVariable_clicked();
	void on_addVariable_clicked();
	void on_OpenAction_triggered();
	void on_SaveAction_triggered();
	void on_NewPNDVAction_triggered();
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
	void on_importSUMoQueriesAction_triggered();
	void on_autoArrangeAction_triggered();
	void on_saveAsAction_triggered();

	void on_alignHorizontalAction_triggered();

	void on_alignVerticalAction_triggered();

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
