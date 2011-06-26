/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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

	void on_translateDTAPNAction_triggered();

	void on_printAction_triggered();

	void on_TAPAALExportAction_triggered();

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
