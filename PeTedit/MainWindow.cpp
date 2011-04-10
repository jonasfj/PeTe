#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "NetItems/PetriNetScene.h"
#include "NetItems/PetriNetView.h"
#include "NetItems/NetItem.h"

#include <PetriEngine/PetriNetBuilder.h>
//#include "DepthFirstReachabilitySearch.h"
//#include "CoverabilityTreeNode.h"

#include "DataFormats/PNMLParser.h"
#include "DataFormats/PNMLBuilder.h"
#include "NetItems/PetriNetSceneBuilder.h"
#include "Widgets/VariableDelegate.h"

#include "Misc/ValidationIssuesModel.h"
#include "Misc/QueryModel.h"
#include "Misc/ProgressViewDelegate.h"

#include <PetriEngine/PQL/PQLParser.h>
#include <PetriEngine/PQL/PQL.h>

#include <QGraphicsView>
#include <QUndoView>
#include <QtGlobal>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTableView>

#include <QSvgGenerator>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
	undoGroup = new QUndoGroup(this);
	currentScene = NULL;

	//Add undoview to panel... We Should probably do a nicer panel
	//QLayout* layout = new QHBoxLayout(ui->panel);
	//layout->addWidget(new QUndoView(undoGroup, this));

	// Variable editor
	VariableDelegate* delegate = new VariableDelegate(this);
	ui->variableView->setItemDelegate(delegate);

	//Set delegate for query editor
	ui->queryView->setItemDelegateForColumn(1, new ProgressViewDelegate(this));

	// Set icons on variable buttons
	ui->addVariable->setIcon(QIcon::fromTheme("list-add"));
	ui->deleteVariable->setIcon(QIcon::fromTheme("list-remove"));

	// Set icons on query buttons
	ui->addQuery->setIcon(QIcon::fromTheme("list-add"));
	ui->deleteQuery->setIcon(QIcon::fromTheme("list-remove"));

	// Add actions for toggling dockwidgets
	QAction* toggleVariablesDock = ui->variableDock->toggleViewAction();
	toggleVariablesDock->setText(tr("Show variables"));
	ui->menuView->addAction(toggleVariablesDock);
	QAction* toggleQueryDock = ui->queryDock->toggleViewAction();
	toggleQueryDock->setText(tr("Show queries"));
	ui->menuView->addAction(toggleQueryDock);
	QAction* toggleValidationDock = ui->validationDock->toggleViewAction();
	toggleValidationDock->setText(tr("Show validation issues"));
	ui->menuView->addAction(toggleValidationDock);

	//Hide validation dock initially
	ui->validationDock->hide();

	// Decide which dock-widget gets the corners
	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	this->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

	// Action group for editing mode
	ui->InsertPlaceModeAction->setProperty("Mode", PetriNetScene::InsertPlaceMode);
	ui->PointerModeAction->setProperty("Mode", PetriNetScene::PointerMode);
	ui->InsertTransitionModeAction->setProperty("Mode", PetriNetScene::InsertTransitionMode);
	ui->InsertArcModeAction->setProperty("Mode", PetriNetScene::InsertArcMode);
	this->modeActionGroup = new QActionGroup(this);
	this->modeActionGroup->addAction(ui->InsertPlaceModeAction);
	this->modeActionGroup->addAction(ui->PointerModeAction);
	this->modeActionGroup->addAction(ui->InsertTransitionModeAction);
	this->modeActionGroup->addAction(ui->InsertArcModeAction);

	this->modeActionGroup->setExclusive(true);
	connect(this->modeActionGroup, SIGNAL(triggered(QAction*)),
			this, SLOT(modeActionGroup_triggered(QAction*)));

	// Create new document
	ui->NewTapnAction->trigger();
}

MainWindow::~MainWindow(){
    delete ui;
}

/** Create new document-tab */
void MainWindow::on_NewTapnAction_triggered(){
	QGraphicsView* view = new PetriNetView();
	PetriNetScene* scene = new PetriNetScene(this->undoGroup, view);
	ui->variableView->setModel(scene->variables());
	view->setScene(scene);
	view->setRenderHints(QPainter::Antialiasing |
						 QPainter::SmoothPixmapTransform |
						 QPainter::TextAntialiasing);
	view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	int index = ui->tabWidget->addTab(view, "New TAPN");
	ui->tabWidget->setCurrentIndex(index);
}

/** Close document-tab */
void MainWindow::on_tabWidget_tabCloseRequested(int index){
	QGraphicsView* view = qobject_cast<QGraphicsView*>(ui->tabWidget->widget(index));
	if(view){
		//TODO: Consider asking user to save before close...
		ui->tabWidget->removeTab(index);
		view->deleteLater();
	}
}

/** Load petri net from file */
void MainWindow::on_OpenAction_triggered(){
	QString fname = QFileDialog::getOpenFileName(this, "Open Petri Net", QDir::homePath());
	if(fname != ""){
		QFile file(fname);
		if(!file.open(QIODevice::ReadOnly))
			return;
		QGraphicsView* view = new PetriNetView();
		PetriNetSceneBuilder builder(this->undoGroup, view);
		PNMLParser p;
		p.parse(&file, &builder);
		file.close();
		view->setScene(builder.makeScene());
		view->setRenderHints(QPainter::Antialiasing |
							 QPainter::SmoothPixmapTransform |
							 QPainter::TextAntialiasing);
		view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
		QFileInfo fi(fname);
		int index = ui->tabWidget->addTab(view, fi.baseName());
		ui->tabWidget->setCurrentIndex(index);
	}
}

/** Active document-tab changed: update state and connect signals and slots
 *
 * The checked state of actions (called UI state) is supposed to reflect the
 * state of the document, so movement in the document changes the UI state...
 * E.g. Mode is a property of the document, the UI just reflects this property.
 */
void MainWindow::on_tabWidget_currentChanged(int index){
	//Save previousScene and update currentScene
	PetriNetScene* previousScene = currentScene;
	QGraphicsView* view = qobject_cast<QGraphicsView*>(ui->tabWidget->widget(index));
	currentScene = NULL;
	if(view)
		currentScene = qobject_cast<PetriNetScene*>(view->scene());

	if(previousScene){
		disconnect(previousScene, SIGNAL(validationIssuesFound()),
					this, SLOT(validationIssuesFound()));
		disconnect(ui->validateAction, SIGNAL(triggered()),
					previousScene, SLOT(validate()));
		disconnect(previousScene, SIGNAL(modeChanged(PetriNetScene::Mode)),
				   this,  SLOT(currentScene_modeChanged(PetriNetScene::Mode)));
		disconnect(ui->refreshValidationButton, SIGNAL(clicked()),
					previousScene, SLOT(validate()));
		disconnect(ui->clearValidationButton, SIGNAL(clicked()),
					previousScene->validationIssues(), SLOT(clear()));
		disconnect(ui->validationView, SIGNAL(doubleClicked(QModelIndex)),
					previousScene, SLOT(showValidationIssue(QModelIndex)));
		ui->variableView->setModel(NULL);
		ui->queryView->setModel(NULL);
		ui->validationView->setModel(NULL);
	}

	if(currentScene){
		currentScene->setActive();
		ui->variableView->setModel(currentScene->variables());
		ui->queryView->setModel(currentScene->queries());
		ui->validationView->setModel(currentScene->validationIssues());

		connect(currentScene, SIGNAL(validationIssuesFound()),
				this, SLOT(validationIssuesFound()));
		connect(ui->validateAction, SIGNAL(triggered()),
				currentScene, SLOT(validate()));
		connect(this->currentScene, SIGNAL(modeChanged(PetriNetScene::Mode)),
				this, SLOT(currentScene_modeChanged(PetriNetScene::Mode)));
		connect(ui->refreshValidationButton, SIGNAL(clicked()),
				currentScene, SLOT(validate()));
		connect(ui->clearValidationButton, SIGNAL(clicked()),
				currentScene->validationIssues(), SLOT(clear()));
		connect(ui->validationView, SIGNAL(doubleClicked(QModelIndex)),
				currentScene, SLOT(showValidationIssue(QModelIndex)));

		this->currentScene_modeChanged(this->currentScene->mode());
	}
}

/** Mode changed in current document */
void MainWindow::currentScene_modeChanged(PetriNetScene::Mode mode){
	foreach(QAction* action, this->modeActionGroup->actions()){
		QVariant m = action->property("Mode");
		Q_ASSERT(m.isValid() && m.type() == QVariant::Int);
		if(m.toInt() == mode)
			action->setChecked(true);
	}
}

/** Propagate mode change to document */
void MainWindow::modeActionGroup_triggered(QAction *action){
	QVariant m = action->property("Mode");
	Q_ASSERT(m.isValid() && m.type() == QVariant::Int);
	if(currentScene)
		currentScene->setMode((PetriNetScene::Mode)m.toInt());
}

void MainWindow::on_SaveAction_triggered()
{
	if(!currentScene)
		return;
	QString fname = QFileDialog::getSaveFileName(this, "Save Petri Net as PNML");
	if(fname != ""){
		QFile file(fname);
		if(!file.open(QIODevice::WriteOnly))
			return;
		PNMLBuilder builder(&file);
		currentScene->produce(&builder);
		builder.makePNMLFile();
		file.close();
	}
}

/** Adds a new variable to the variableView table */
void MainWindow::on_addVariable_clicked()
{
	//TODO: Check if dummy variable exists already
	int num=0;
	if(currentScene){
		while(true){
			QString id = "x" + QString::number(num);
			if (!currentScene->findVariable(id)){
				currentScene->addVariable(id,0,0);
				break;
			}else
				num++;
		}
	}
}

/** Removes a variable from the variableView table */
void MainWindow::on_deleteVariable_clicked()
{
	if(currentScene){
		const QModelIndexList selection = ui->variableView->selectionModel()->selectedIndexes();
		if(selection.length()>0){
			QModelIndex index = selection.at(0);
			if(currentScene->variables()->rowCount() > index.row()){
				currentScene->variables()->removeRow(index.row());
			}
		}
	}
}


/** Save current scene to SVG */
void MainWindow::on_actionExport_SVG_triggered()
{
	if(!currentScene)
		return;
	QString fname = QFileDialog::getSaveFileName(this, "Export Petri Net to SVG");
	if(fname != ""){
		QFile file(fname);
		if(!file.open(QIODevice::WriteOnly))
			return;

		QSvgGenerator generator;
		generator.setOutputDevice(&file);
		generator.setSize(currentScene->sceneRect().size().toSize());
		generator.setViewBox(currentScene->sceneRect());

		QPainter painter;
		painter.begin(&generator);
		currentScene->render(&painter);
		painter.end();

		file.close();
	}
}

/** Validation found issues */
void MainWindow::validationIssuesFound()
{
	if(!currentScene)
		return;
	if(ui->validationDock->isHidden() &&
	   currentScene->validationIssues()->rowCount() > 0){
		ui->validationDock->show();
		//TODO: Call this method when appropriate
		ui->validationView->resizeColumnsToContents();
	}
}


/** Add query
 * @remarks AddQuery (button) is connected to addQueryAction, so this is
 *			also handled here...
 */
void MainWindow::on_addQueryAction_triggered(){
	if(!currentScene)
		return;
	currentScene->queries()->addQuery(this);
}

/** Edit query */
void MainWindow::on_queryView_doubleClicked(QModelIndex index){
	if(!currentScene)
		return;
	currentScene->queries()->editQuery(index, this);
}

/** Run query*/
void MainWindow::on_runQueryButton_clicked()
{
	if(!currentScene)
		return;
	currentScene->queries()->runQuery(ui->queryView->currentIndex());
}

/** Stop query */
void MainWindow::on_stopQueryButton_clicked()
{
	if(!currentScene)
		return;
	currentScene->queries()->stopQuery(ui->queryView->currentIndex());
}


/** Delete query */
void MainWindow::on_deleteQuery_clicked(){
	if(!currentScene)
		return;
	currentScene->queries()->removeQuery(ui->queryView->currentIndex());
}

void MainWindow::on_aboutAction_triggered()
{
	QMessageBox* msg = new QMessageBox(this);
	QString text = "PeTe Petri Net Tool v.0.314<br><br>";
			text.append("<b>Authors:</b><br>");
			text.append("Jonas Finnemann Jensen<br>");
			text.append("Thomas Nielsen<br>");
			text.append("Lars Kaerlund Oestergaard<br><br>");
			text.append("<b>Website:</b><br>");
			text.append("<a href='https://github.com/jopsen/PeTe'>PeTe @ github</a>");
	msg->setText(text );

	msg->show();
}
