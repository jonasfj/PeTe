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
#include "DataFormats/DTAPNParser.h"
#include "Dialogs/ImportDTAPNDialog.h"

#include <PetriEngine/PQL/PQLParser.h>
#include <PetriEngine/PQL/PQL.h>
#include <PetriEngine/DTAPN/DTAPNTranslator.h>
#include <PetriEngine/LayoutBuilder.h>
#include <PetriEngine/DTAPN/DTAPNPrinter.h>

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
	zoomToolSpinBox = NULL;
	ui->statusBar->addPermanentWidget(new MemoryMonitor(this));

	// Variable editor
	VariableDelegate* delegate = new VariableDelegate(this);
	ui->variableView->setItemDelegate(delegate);

	//Set delegate for query editor
	ui->queryView->setItemDelegateForColumn(1, new ProgressViewDelegate(this));

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
	createZoomTool();

	// Listen for clean state changed
	connect(&undoGroup, SIGNAL(cleanChanged(bool)), this, SLOT(updateWindowTitle()));

	// Create new document
	on_NewPNDVAction_triggered();

	//Load settings
	loadSettings();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e){
	saveSettings();
	bool clean = true;
	for(int i = 0; i < ui->tabWidget->count(); i++){
		QGraphicsView* view = qobject_cast<QGraphicsView*>(ui->tabWidget->widget(i));
		Q_ASSERT(view);
		if(view){
			PetriNetScene* scene = qobject_cast<PetriNetScene*>(view->scene());
			Q_ASSERT(scene);
			if(scene)
				clean &= scene->undoStack()->isClean();
		}
	}
	if(!clean){
		QMessageBox::StandardButtons retval;
		retval = QMessageBox::question(this,
									tr("You have unsaved changed"),
									tr("Do you wish to discard unsaved changes?"),
									QMessageBox::Cancel | QMessageBox::Discard,
									QMessageBox::Cancel);
		if(retval == QMessageBox::Cancel){
			e->ignore();
			return;
		}
	}
	// Accept if state was clean or changes were discarded
	e->accept();
}

/** Create new document-tab */
void MainWindow::on_NewPNDVAction_triggered(){
	PetriNetView* view = new PetriNetView();
	PetriNetScene* scene = new PetriNetScene(&undoGroup, view);
	ui->variableView->setModel(scene->variables());
	view->setScene(scene);
	int index = ui->tabWidget->addTab(view, tr("Untitled PNDV"));
	ui->tabWidget->setCurrentIndex(index);
}

/** Close document-tab */
void MainWindow::on_tabWidget_tabCloseRequested(int index){
	QGraphicsView* view = qobject_cast<QGraphicsView*>(ui->tabWidget->widget(index));
	if(view){
		PetriNetScene* scene = qobject_cast<PetriNetScene*>(view->scene());
		Q_ASSERT(scene != NULL);
		if(!scene->undoStack()->isClean()){
			QMessageBox::StandardButtons retval;
			retval = QMessageBox::question(this,
										tr("You have unsaved changed"),
										tr("Do you wish to discard unsaved changes?"),
										QMessageBox::Cancel | QMessageBox::Discard,
										QMessageBox::Cancel);
			if(retval == QMessageBox::Cancel)
				return;
		}
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
		PetriNetScene* scene = builder.makeScene();
		scene->setFilename(fname);
		scene->undoStack()->setClean();
		view->setScene(scene);
		int index = ui->tabWidget->addTab(view, "");
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
	PetriNetView* view = qobject_cast<PetriNetView*>(ui->tabWidget->widget(index));
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

		disconnect(previousScene->view(), SIGNAL(zoomChanged(double)), zoomToolSpinBox, SLOT(setValue(double)));
		disconnect(zoomToolSpinBox, SIGNAL(valueChanged(double)), previousScene->view(), SLOT(setZoom(double)));

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

		this->zoomToolSpinBox->setValue(currentScene->view()->currentScale() * 100);
		connect(currentScene->view(), SIGNAL(zoomChanged(double)), zoomToolSpinBox, SLOT(setValue(double)));
		connect(zoomToolSpinBox, SIGNAL(valueChanged(double)), currentScene->view(), SLOT(setZoom(double)));

		this->currentScene_modeChanged(this->currentScene->mode());
	}
	updateWindowTitle();
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
	// Save as, if there's no filename
	if(currentScene->filename().isEmpty())
		on_saveAsAction_triggered();
	else{
		// Open file and attempt to save
		QString fname = currentScene->filename();
		QFile file(fname);
		if(!file.open(QIODevice::WriteOnly)){
			// Warn and do save as if save failed... (this is a loop untill user cancels).
			QMessageBox::critical(this, tr("Failed to save!"), tr("Couldn't open \"%1\" in write-mode.").arg(fname));
			on_saveAsAction_triggered();
			return;
		}
		PNMLBuilder builder(&file);
		currentScene->produce(&builder);
		builder.makePNMLFile();
		file.close();
		// Mark this state clean
		currentScene->undoStack()->setClean();
	}
}

void MainWindow::on_saveAsAction_triggered(){
	if(!currentScene)
		return;
	// Select current file if there's one
	QString path = lastLoadSavePath;
	if(!currentScene->filename().isEmpty())
		path = currentScene->filename();
	// Show save file dialog
	QString fname = QFileDialog::getSaveFileName(this, "Save Petri Net as PNML", path);
	if(fname != ""){
		// Set the current filename
		currentScene->setFilename(fname);
		updateWindowTitle();
		// Update settings
		lastLoadSavePath = QFileInfo(fname).absoluteDir().absolutePath();
		// Save the file
		on_SaveAction_triggered();
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
	ui->menuView->addAction(ui->fileToolBar->toggleViewAction());
	ui->menuView->addAction(ui->editingToolBar->toggleViewAction());
	ui->menuView->addAction(ui->toolsToolBar->toggleViewAction());
}

void MainWindow::createZoomTool(){
	zoomToolSpinBox = new QDoubleSpinBox(this);
	zoomToolSpinBox->setSuffix("%");
	zoomToolSpinBox->setRange(PetriNetView::minScale * 100, PetriNetView::maxScale * 100);
	zoomToolSpinBox->setSingleStep(5.0);
	zoomToolSpinBox->setDecimals(0);
	zoomToolSpinBox->setValue(100);
	zoomToolSpinBox->setToolTip(tr("Change current zoom"));

	ui->toolsToolBar->addSeparator();
	ui->toolsToolBar->addWidget(zoomToolSpinBox);
}

void MainWindow::on_autoArrangeAction_triggered(){
	if(currentScene)
		currentScene->autoArrange();
}

/** Update the window title */
void MainWindow::updateWindowTitle(){
	if(!currentScene)
		this->setWindowTitle("PeTe");
	else{
		QString fname;
		QString base;
		if(currentScene->filename().isEmpty()){
			fname = tr("Untitled PNDV");
			base = fname;
		}else{
			fname = QFileInfo(currentScene->filename()).fileName();
			base = QFileInfo(fname).baseName();
		}
		QString star;
		if(!undoGroup.isClean())
			star = "*";
		this->setWindowTitle(fname + star + " - PeTe ");
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), base + star);
	}
}


/** Import and convert a DTAPN */
void MainWindow::on_translateDTAPNAction_triggered(){
	ImportDTAPNDialog d(this, lastImportPath);
	if(d.exec() == QDialog::Accepted){
		QString fname = d.filePath();

		QFile file(fname);
		if(!file.open(QIODevice::ReadOnly))
			return;
		lastImportPath = QFileInfo(fname).absoluteDir().absolutePath();

		// Create translator
		PetriEngine::DTAPN::DTAPNTranslator translator(d.bound());

		// Parse file to translator
		DTAPNParser p;
		p.parse(&file, &translator);
		file.close();
		PetriEngine::LayoutBuilder l;
		translator.makePNDV(&l);

		// Print parse stuff, for debugging
		//file.open(QIODevice::ReadOnly);
		//PetriEngine::DTAPN::DTAPNPrinter printer;
		//p.parse(&file, &printer);

		// Build scene and translation
		PetriNetView* view = new PetriNetView();
		PetriNetSceneBuilder builder(&undoGroup, view);
		l.produce(&builder);
		PetriNetScene* scene = builder.makeScene();

		foreach(const DTAPNParser::Query& q, p.getQueries()){
			QueryModel::Query nq;
			nq.name = q.name;
			nq.query = PetriEngine::DTAPN::DTAPNTranslator::translateQuery(q.query.toStdString()).c_str();
			nq.jit = true;
			nq.strategy = "";
			scene->queries()->insertQuery(nq);
		}

		// Configure view and stuff
		view->setScene(scene);
		int index = ui->tabWidget->addTab(view, "");
		ui->tabWidget->setCurrentIndex(index);
	}
}

/******************** Undo/Redo Handling ********************/

void MainWindow::createUndoActions(){
	QAction* undo = undoGroup.createUndoAction(this, tr("Undo"));
	QAction* redo = undoGroup.createRedoAction(this, tr("Redo"));
	undo->setIcon(QIcon(":/Icons/24x24/undo.png"));
	undo->setShortcut(QKeySequence::Undo);
	redo->setIcon(QIcon(":/Icons/24x24/redo.png"));
	redo->setShortcut(QKeySequence::Redo);
	ui->editingToolBar->addSeparator();
	ui->editingToolBar->addAction(undo);
	ui->editingToolBar->addAction(redo);
	ui->menuEdit->addSeparator();
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

/******************** Action handling ********************/


void MainWindow::on_alignHorizontalAction_triggered(){
	if(currentScene)
		currentScene->alignSelectItems(Qt::Horizontal);
}

void MainWindow::on_alignVerticalAction_triggered(){
	if(currentScene)
		currentScene->alignSelectItems(Qt::Vertical);
}

#include <QPrintDialog>
#include <QPrinter>

void MainWindow::on_printAction_triggered(){
	if(currentScene){
		QPrinter p;
		QPrintDialog d(&p, this);
		if(d.exec() == QDialog::Accepted){
			QPainter painter;
			painter.begin(&p);
			currentScene->render(&painter);
			painter.end();
		}
	}
}
