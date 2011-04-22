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
#include "Widgets/MemoryMonitor.h"

#include "Misc/ValidationIssuesModel.h"
#include "Misc/QueryModel.h"
#include "Misc/ProgressViewDelegate.h"
#include "Misc/VariableModel.h"

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
#include <QSettings>

#include <QSvgGenerator>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
	currentScene = NULL;
	ui->statusBar->addPermanentWidget(new MemoryMonitor(this));

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

	createUndoActions();
	createToggleToolsbars();

	// Create new document
	ui->NewTapnAction->trigger();

	//Load settings
	loadSettings();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e){
	saveSettings();
	//TODO: Ask user if he wan't to quit do e->ignore() if not
	e->accept();
}

/** Create new document-tab */
void MainWindow::on_NewTapnAction_triggered(){
	PetriNetView* view = new PetriNetView();
	PetriNetScene* scene = new PetriNetScene(&undoGroup, view);
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
	QString fname = QFileDialog::getOpenFileName(this, "Open Petri Net", lastLoadSavePath);
	if(fname != ""){
		QFile file(fname);
		if(!file.open(QIODevice::ReadOnly))
			return;
		lastLoadSavePath = QFileInfo(fname).absoluteDir().absolutePath();
		PetriNetView* view = new PetriNetView();
		PetriNetSceneBuilder builder(&undoGroup, view);
		PNMLParser p;
		p.parse(&file, &builder, &builder);
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

		//Disconnect from query model (resizing)
		disconnect(previousScene->queries(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
				this, SLOT(resizeQueryView()));
		disconnect(previousScene->queries(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
				this, SLOT(resizeQueryView()));

		//Disconnect from variable model (resizing)
		disconnect(previousScene->variables(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
				this, SLOT(resizeVariableView()));
		disconnect(previousScene->variables(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
				this, SLOT(resizeVariableView()));

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

		//Set resize properties for headers, as the model must be added first
		connect(currentScene->queries(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
				this, SLOT(resizeQueryView()));
		connect(currentScene->queries(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
				this, SLOT(resizeQueryView()));
		resizeQueryView();

		//Set resize mode for headers, as the model must be added first
		connect(currentScene->variables(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
				this, SLOT(resizeVariableView()));
		connect(currentScene->variables(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
				this, SLOT(resizeVariableView()));

		resizeVariableView();

		resizeValidationView();

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
	QString fname = QFileDialog::getSaveFileName(this, "Save Petri Net as PNML", lastLoadSavePath);
	if(fname != ""){
		QFile file(fname);
		if(!file.open(QIODevice::WriteOnly))
			return;
		lastLoadSavePath = QFileInfo(fname).absoluteDir().absolutePath();
		PNMLBuilder builder(&file);
		currentScene->produce(&builder);
		builder.makePNMLFile();
		file.close();
	}
}


/******************** Variables ********************/

/** Set resize mode for VariableView */
void MainWindow::resizeVariableView(){
	ui->variableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
	ui->variableView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->variableView->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
}

/** Adds a new variable to the variableView table */
void MainWindow::on_addVariable_clicked()
{
	if(currentScene)
		currentScene->variables()->addVariable();
}

/** Removes a variable from the variableView table */
void MainWindow::on_deleteVariable_clicked()
{
	if(currentScene){
		currentScene->variables()->removeVariable(ui->variableView->currentIndex());
	}
}


/******************** Export ********************/

/** Save current scene to SVG */
void MainWindow::on_actionExport_SVG_triggered()
{
	if(!currentScene)
		return;
	QString fname = QFileDialog::getSaveFileName(this, "Export Petri Net to SVG", lastExportPath);
	if(fname != ""){
		QFile file(fname);
		if(!file.open(QIODevice::WriteOnly))
			return;
		lastExportPath = QFileInfo(fname).absoluteDir().absolutePath();

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

/******************** Validation ********************/

/** Set resize mode for ValidationView */
void MainWindow::resizeValidationView(){
	ui->validationView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
	ui->validationView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
}

/** Validation found issues */
void MainWindow::validationIssuesFound()
{
	if(!currentScene)
		return;
	if(ui->validationDock->isHidden() &&
	   currentScene->validationIssues()->rowCount() > 0){
		ui->validationDock->show();
		resizeValidationView();
	}
}

/******************** Queries ********************/

/** Set resize mode for QueryView */
void MainWindow::resizeQueryView(){
	ui->queryView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
	ui->queryView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
}


/** Add query
 * @remarks AddQuery (button) is connected to addQueryAction, so this is
 *			also handled here...
 */
void MainWindow::on_addQueryAction_triggered(){
	if(currentScene)
		currentScene->queries()->addQuery(this);
}

/** Edit query */
void MainWindow::on_queryView_doubleClicked(QModelIndex index){
	if(currentScene)
		currentScene->queries()->editQuery(index, this);
}

/** Run query*/
void MainWindow::on_runQueryButton_clicked(){
	if(currentScene)
		currentScene->queries()->runQuery(ui->queryView->currentIndex());
}

/** Stop query */
void MainWindow::on_stopQueryButton_clicked(){
	if(currentScene)
		currentScene->queries()->stopQuery(ui->queryView->currentIndex());
}


/** Delete query */
void MainWindow::on_deleteQuery_clicked(){
	if(currentScene)
		currentScene->queries()->removeQuery(ui->queryView->currentIndex());
}


void MainWindow::on_importSUMoQueriesAction_triggered(){
	if(currentScene){
		QString fname = QFileDialog::getOpenFileName(this, "Import SUMo Queries", lastImportPath);
		if(!fname.isEmpty()){
			lastImportPath = QFileInfo(fname).absoluteDir().absolutePath();
			QFile f(fname);
			f.open(QFile::ReadOnly);
			currentScene->queries()->importSUMoQueries(f);
			f.close();
		}
	}
}

/******************** Misc ********************/


void MainWindow::on_aboutAction_triggered()
{
	QMessageBox* msg = new QMessageBox(this);
	QString text = "PeTe Petri Net Tool v.0.1<br><br>";
			text.append("<b>Authors:</b><br>");
			text.append("Jonas Finnemann Jensen<br>");
			text.append("Thomas Nielsen<br>");
			text.append("Lars Kaerlund Oestergaard<br><br>");
			text.append("<b>Website:</b><br>");
			text.append("<a href='https://github.com/jopsen/PeTe'>PeTe @ github</a>");
	msg->setText(text );

	msg->show();
}

void MainWindow::createToggleToolsbars(){
	ui->menuView->addSeparator();
	ui->menuView->addAction(ui->editingToolBar->toggleViewAction());
	ui->menuView->addAction(ui->toolsToolBar->toggleViewAction());
}

void MainWindow::on_autoArrangeAction_triggered(){
	if(currentScene)
		currentScene->autoArrange();
}

/******************** Undo/Redo Handling ********************/

void MainWindow::createUndoActions(){
	ui->editingToolBar->addSeparator();
	QAction* undo = undoGroup.createUndoAction(this, tr("Undo"));
	QAction* redo = undoGroup.createRedoAction(this, tr("Redo"));
	undo->setIcon(QIcon(":/Icons/undo.svg"));
	undo->setShortcut(QKeySequence::Undo);
	redo->setIcon(QIcon(":/Icons/redo.svg"));
	redo->setShortcut(QKeySequence::Redo);
	ui->editingToolBar->addAction(undo);
	ui->editingToolBar->addAction(redo);
	ui->menuEdit->addAction(undo);
	ui->menuEdit->addAction(redo);
}

/******************** Settings ********************/

void MainWindow::saveSettings(){
	QSettings s;
	s.beginGroup("MainWindow");
	s.setValue("geometry",	this->saveGeometry());
	s.setValue("state",		this->saveState());
	s.setValue("LastLoadSavePath", lastLoadSavePath);
	s.setValue("LastExportPath", lastExportPath);
	s.setValue("LastImportPath", lastImportPath);
	s.endGroup();
}

void MainWindow::loadSettings(){
	QSettings s;
	s.beginGroup("MainWindow");
	this->restoreGeometry(		s.value("geometry").toByteArray());
	this->restoreState(			s.value("state").toByteArray());
	this->lastLoadSavePath	=	s.value("LastLoadSavePath", QDir::homePath()).toString();
	this->lastExportPath	=	s.value("LastExportPath", QDir::homePath()).toString();
	this->lastImportPath	=	s.value("LastImportPath", QDir::homePath()).toString();
	s.endGroup();
}
