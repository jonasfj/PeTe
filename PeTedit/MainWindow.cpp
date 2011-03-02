#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "NetItems/PetriNetScene.h"
#include "NetItems/PetriNetView.h"

#include "PetriNetFactory.h"
#include "DepthFirstReachabilitySearch.h"
#include "CoverabilityTreeNode.h"

#include "Dialogs/QueryDialog.h"

#include "DataFormats/PNMLParser.h"
#include "DataFormats/PNMLFactory.h"
#include "NetItems/PetriNetSceneFactory.h"
#include "CTL/CTLParser.h"

#include <QGraphicsView>
#include <QUndoView>
#include <QtGlobal>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
	undoGroup = new QUndoGroup(this);
	currentScene = NULL;

	//Add undoview to panel... We Should probably do a nicer panel
	QLayout* layout = new QHBoxLayout(ui->panel);
	layout->addWidget(new QUndoView(undoGroup, this));

	//Action group for editing mode
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

	//Create new document
	ui->NewTapnAction->trigger();
}

MainWindow::~MainWindow(){
    delete ui;
}

/** Create new document-tab */
void MainWindow::on_NewTapnAction_triggered(){
	QGraphicsView* view = new PetriNetView();
	PetriNetScene* scene = new PetriNetScene(this->undoGroup, view);
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
		PetriNetSceneFactory fac(this->undoGroup, view);
		PNMLParser p;
		p.parse(&file, &fac);
		file.close();
		view->setScene(fac.makeScene());
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
		disconnect(previousScene, SIGNAL(modeChanged(PetriNetScene::Mode)),
				   this,  SLOT(currentScene_modeChanged(PetriNetScene::Mode)));
	}

	if(currentScene){
		currentScene->setActive();
		connect(this->currentScene, SIGNAL(modeChanged(PetriNetScene::Mode)),
				this, SLOT(currentScene_modeChanged(PetriNetScene::Mode)));
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


void dumpTree(const PetriEngine::CoverabilityTreeNode* tree, PetriEngine::PetriNet* net){

	if(tree != NULL){
		qDebug() << "transition number:" << QString::number( tree->transition());
		for(int i = 0; i < net->nPlaces(); i++)
				qDebug() << "\t m(P" << i << ")=" << QString::number( tree->marking()[i]);

		for(size_t i = 0; i < tree->childNodes().size(); i++){
			PetriEngine::CoverabilityTreeNode* c = tree->childNodes().at(i);
			dumpTree(c,net);
		}
	}
}


/** Open the query editor window */
void MainWindow::on_NewQueryAction_triggered()
{
	QueryDialog* dlg = new QueryDialog(this);

	if(dlg->exec() == QDialog::Accepted){

		// Get raw query text.
		QString queryText = dlg->query();

		PetriEngine::PetriNetFactory* fac = new PetriEngine::PetriNetFactory();
		this->currentScene->produce(fac);
		PetriEngine::PetriNet* net = fac->makePetriNet();

		PetriEngine::CTL::CTLParser parser(&(*net));
		PetriEngine::CTL::CTLExpr* exp = parser.parse(queryText.toStdString());

		PetriEngine::DepthFirstReachabilitySearch dfs;
		dfs.reachable(*net,fac->makeInitialMarking(),exp);

		//NOTE: it helped making this const...
		const PetriEngine::CoverabilityTreeNode* tree = dfs.coverabilityTree();
		dumpTree(tree,net);
	}

	dlg->deleteLater();
}

void MainWindow::on_SaveAction_triggered()
{
	if(!currentScene)
		return;
	QString fname = QFileDialog::getSaveFileName(this, "Save Petri Net");
	if(fname != ""){
		QFile file(fname);
		if(!file.open(QIODevice::WriteOnly))
			return;
		PNMLFactory fac(&file);
		currentScene->produce(&fac);
		fac.makePNMLFile();
		file.close();
	}
}
