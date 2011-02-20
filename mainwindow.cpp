#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "netitems/petrinetscene.h"
#include "netitems/petrinetview.h"

#include <QGraphicsView>
#include <QUndoView>
#include <QtGlobal>

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
	ui->tabWidget->addTab(view, "New TAPN");
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

/** Propogate mode change to document */
void MainWindow::modeActionGroup_triggered(QAction *action){
	QVariant m = action->property("Mode");
	Q_ASSERT(m.isValid() && m.type() == QVariant::Int);
	if(currentScene)
		currentScene->setMode((PetriNetScene::Mode)m.toInt());
}
