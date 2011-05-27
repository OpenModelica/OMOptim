// $Id$
/**
 	@file MainWindow.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "MainWindow.h"
#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QInputDialog>
#include <QtGui/QTabBar>
#include <QtGui/QDockWidget>
#include <QSettings>
#include "MOThreads.h"
#include "newprojectform.h"
#include "Software.h"
#include "ListInfo.h"
#include <iostream>
#include "OneSimulation.h"
#include "tabOneSim.h"
#include "MOGuiTools.h"
#include "MOVector.h"
#include "Variable.h"
#include "tabOMC.h"
#include "MOTab.h"
#include <QDebug>



namespace Ui
{
	class MainWindowClass;
}


MainWindow::MainWindow(Project* project,QWidget *parent)
: QMainWindow(parent), _ui(new Ui::MainWindowClass)
{
	//************************
	// Basic initialization	
	//************************
	_ui->setupUi(this);
	_project = project;

	//***********************
	// Main window gui
	//***********************
	// Tabs
	_tabMain = new MOMainTab(this,_project);
	_tabProject = new TabProject(_project);
	_tabProject->setWindowTitle("Project");
	_tabProject->setBackgroundRole(QPalette::Window);
	_tabProject->setAutoFillBackground(true);
	_tabMain->addTab(_tabProject,"Project");
	_tabMain->setCurrentWidget(_tabProject);
	
	// Side part
	_ui->splitterH->insertWidget(1,_tabMain);
	_ui->splitterH->setStretchFactor(1,10);
	delete _ui->widgetToDelete;
	_ui->treeProblems->header()->setResizeMode(QHeaderView::Stretch);
	_ui->treeSolvedProblems->header()->setResizeMode(QHeaderView::Stretch);
	addDockWidget(Qt::BottomDockWidgetArea,_ui->dockLog);
	
	// Actions
	QList<QAction*> dispActions;
	dispActions << _ui->dockLog->toggleViewAction();
	_ui->menuDisplay->addActions(dispActions);

	// Trees
	_ui->treeProblems->setModel(_project->problems());
	_ui->treeSolvedProblems->setModel(_project->solvedProblems());
	ModClassTree* modClassTree = new ModClassTree(_project->modReader(),_project->rootModClass(),this);
	GuiTools::ModClassToTreeView(_project->modReader(),_project->rootModClass(),_ui->treeModClass,modClassTree);
	_ui->treeProblems->setContextMenuPolicy(Qt::CustomContextMenu);
	_ui->treeSolvedProblems->setContextMenuPolicy(Qt::CustomContextMenu);
	_ui->treeModClass->setContextMenuPolicy(Qt::CustomContextMenu);

	// Recent Files
	createRecentFilesMenu();
	updateRecentFilesMenu();

	// text log
	_textLog=new MyTextLog();
	_ui->layoutTextLog->addWidget(_textLog);
        _textLog->setOpenExternalLinks(true);
	
	//Error, msg and progress
	_widgetProgress = new WidgetProgress(this);
	_ui->layoutProgress->addWidget(_widgetProgress);
	displayProgress(0);

	actualizeGuiFromProject();

	//*********************************
	// Signals for gui
	//*********************************
	connect(_ui->treeProblems, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(enableProblemTab(QModelIndex)));
	connect(_ui->treeSolvedProblems, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(enableSolvedProblemTab(QModelIndex)));
	connect(_ui->treeModClass, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelectedModClass(QModelIndex)));
	connect(this, SIGNAL(sendInfo(Info)),this, SLOT( displayInfo(Info)));
	connect (_ui->treeSolvedProblems,SIGNAL(customContextMenuRequested(const QPoint &)),
	this,SLOT(showSolvedProblemPopup(const QPoint &)));
	connect (_ui->treeProblems,SIGNAL(customContextMenuRequested(const QPoint &)),
	this,SLOT(showProblemPopup(const QPoint &)));
	connect (_ui->treeModClass,SIGNAL(customContextMenuRequested(const QPoint &)),
	this,SLOT(showModClassTreePopup(const QPoint &)));
	connect (_tabMain,SIGNAL(customContextMenuRequested(const QPoint &)),
	this,SLOT(showTabTitlePopup(const QPoint &)));

	// Menus
	connect( _ui->actionNewProject, SIGNAL( triggered() ),this, SLOT( newProject()));
	connect( _ui->actionSaveProject, SIGNAL( triggered() ),this, SLOT( saveProject()));
	connect( _ui->actionLoadProject, SIGNAL( triggered() ),this, SLOT( loadProject()));	
	connect( _ui->actionNewOneSimulation, SIGNAL( triggered() ),this, SLOT( newOneSimulation()));
	connect( _ui->actionNewOptimization, SIGNAL( triggered() ),this, SLOT( newOptimization()));
	connect( _ui->actionNewProblemEI, SIGNAL( triggered() ),this, SLOT( newProblemEI()));
	connect( _ui->actionQuit, SIGNAL( triggered() ),this, SLOT( quit()));
	connect( _ui->actionOMCShell,SIGNAL( triggered() ),this, SLOT( newOMCShell()));
	connect( _ui->actionOMCClear,SIGNAL( triggered() ),this, SLOT( OMCClear()));
	connect( _ui->actionSettings,SIGNAL( triggered() ),this, SLOT(openSettings()));
	connect( _ui->actionClearRecent, SIGNAL(triggered()), this, SLOT(clearRecentFilesMenu()));
	connect( _ui->actionLoadMoFile,  SIGNAL(triggered()), this, SLOT(loadMoFile()));
	connect( _ui->actionLoadModelicaLibrary,  SIGNAL(triggered()), this, SLOT(loadModelicaLibrary()));
        connect( _ui->actionHelp, SIGNAL(triggered()),this, SLOT(openUserManual()));
	
	//*********************************
	// Signals for project, infos
	//*********************************
	connect(_project, SIGNAL(projectReset()),this, SLOT(projectReset()));
	connect(_project, SIGNAL(projectChanged()),this, SLOT( actualizeGuiFromProject()));
	connect(_project, SIGNAL(sendProgress(float)), this, SLOT(displayProgress(float)));
	connect(_project, SIGNAL(sendProgress(float,int,int)), this, SLOT(displayProgress(float,int,int)));
	connect(&infoSender, SIGNAL(sent(Info)),this, SLOT( displayInfo(Info)));

	//*********************************
	// Signals for problems
	//*********************************
	connect( _project, SIGNAL(addedProblem(Problem*)),this, SLOT(onAddedProblem(Problem*)));
	connect( _project, SIGNAL(beforeRemoveProblem(int)),this, SLOT(removeProblemTab(int)));
	connect( _project, SIGNAL(problemBegun(Problem*)),this,SLOT(onProblemBegun(Problem*)));
	connect( _project, SIGNAL(problemFinished(Problem*)),this,SLOT(onProblemFinished(Problem*)));
	connect( _project, SIGNAL(newProblemProgress(float)),this,SLOT(onNewProblemProgress(float)));
	connect( _project, SIGNAL(newProblemProgress(float,int,int)),this,SLOT(onNewProblemProgress(float,int,int)));
	connect( _widgetProgress, SIGNAL(pushedStop()),this,SLOT(onProblemStopAsked()));

	//*********************************
	// Signals for ModClass
	//*********************************
	connect(_ui->treeModClass, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelectedModClass(QModelIndex)));
	connect( _ui->pushLoadMoFile,  SIGNAL(clicked()), this, SLOT(loadMoFile()));
	
	//*********************************
	// Signals for About dialogs
	//*********************************
	connect(_ui->actionAboutQt, SIGNAL(triggered()),this, SLOT(dispAboutQt()));
	connect( _ui->actionAboutOMOptim,  SIGNAL(triggered()), this, SLOT(dispAboutOMOptim()));
	
	
	
	//*********************************
	// Signals for solved problems
	//*********************************
	connect(_project, SIGNAL(beforeRemoveSolvedProblem(int)),this, SLOT(removeSolvedProblemTab(int)));
	connect(_project, SIGNAL(addedSolvedProblem(Problem*)),this, SLOT(onAddedSolvedProblem(Problem*)));

	//*********************************
	// Signals for model
	//*********************************
	connect(_project,SIGNAL(connectionsUpdated()),this,SLOT(onConnectionsUpdated()));
	connect(_project,SIGNAL(componentsUpdated()),this,SLOT(onComponentsUpdated()));
	connect(_project,SIGNAL(modifiersUpdated()),this,SLOT(onModifiersUpdated()));
	
	//*********************************
	// Signals for MOomc
	//*********************************
	connect(_project->moomc(),SIGNAL(startOMCThread(QString)),this,SLOT(onStartedOMCThread(QString)));
	connect(_project->moomc(),SIGNAL(finishOMCThread(QString)),this,SLOT(onFinishedOMCThread(QString)));

	//*********************************
	// Reload gui configuration
	//*********************************
	readSettings();

	//*********************************
	// Configuration
	//*********************************
#ifndef USEEI
	_ui->actionNewProblemEI->setVisible(false);
#endif


}

MainWindow::~MainWindow()
{
	delete _ui;
	delete _project;
}


void MainWindow::displayInfo(Info i)
{
	QString msg = i.infoMsg;
	switch(i.infoType)
	{
	case ListInfo::ERROR2 :
	case ListInfo::OMCERROR2 :
		msg.insert(0,"<b><font color='red'>");
		msg.append("</font></b>");
		break;
	case ListInfo::WARNING2 :
	case ListInfo::OMCWARNING2 :
		msg.insert(0,"<b><font color='#FF7700'>");
		msg.append("</font></b>");
		break;
	}


	switch(i.infoType)
	{
	case ListInfo::NORMAL2 :
	case ListInfo::WARNING2 :
	case ListInfo::ERROR2 :
		_textLog->append(msg);
		break;
	case ListInfo::OMCNORMAL2 :
	case ListInfo::OMCWARNING2 :
	case ListInfo::OMCERROR2 :
		_ui->textOMC->append(msg);
		break;
	case ListInfo::DEBUG :
		_ui->textDebug->append(msg);
		break;
	}
}


void MainWindow::onStartedOMCThread(QString _name)
{
	QString msg;
	msg.sprintf("%s started",_name.toLatin1().data());
	_ui->textOMC->append(msg);
}
void MainWindow::onFinishedOMCThread(QString _name)
{
	QString msg;
        msg.sprintf("%s finished",_name.utf16());
	_ui->textOMC->append(msg);
}
void MainWindow::onTerminatingOMCThread(QString _name)
{
	QString msg;
        msg.sprintf("%s terminated",_name.utf16());
	_ui->textOMC->append(msg);
}



void MainWindow::newProject()
{
	// Opening Form
	newProjectForm *form = new newProjectForm();
	form->show();

	//Saving form information
	if (form->exec() == QDialog::Accepted) {

		// project information
		_project->clear();
		_project->setName(form->projectName);
		_project->setIsDefined(true);
	
		// save it (as it is first save, it will ask for path)
		saveProject();
	}
}

void MainWindow::saveProject()
{
	if (!_project->isDefined())
	{
		// project does not exist
	}
	else
	{
		QString _filePath = _project->filePath() ;
		if(_filePath.length())
		{
			_project->save();
		}
		else
		{
			_filePath = QFileDialog::getSaveFileName(
				this,
				"MO - Save Project",
                                QString::null,
				"MO project (*.min)" );
			if(!_filePath.isNull())
			{
				_project->setFilePath(_filePath);
				_project->save();
			}
		}

		updateRecentFilesList(_filePath);

		actualizeGuiFromProject();
	}


}
void MainWindow::loadProject()
{
	

	QString filename = QFileDialog::getOpenFileName(
		this,
		"MO - Open Project",
                getLastProjectFolder(),
                "MO project (*.min)" );

	if (!filename.isNull())
	{
		QMessageBox msgBox;
		msgBox.setText("Project will be loaded. It may take a long time while application will seem freezed.");
                msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();

		msgBox.hide();
		bool loaded = _project->load(filename);

		if(loaded)
		{
			updateRecentFilesList(filename);
			QFileInfo info = QFileInfo(filename);
			setLastProjectFolder(info.absolutePath());
		}

	}

	//update GUI
	actualizeGuiFromProject();
}

void MainWindow::loadProject(QString _fileName)
{
	QMessageBox msgBox;
	msgBox.setText("Project will be loaded. It may take a long time while application will seem freezed.");
	msgBox.exec();


	bool loaded = _project->load(_fileName);
	if(loaded)
	{
		updateRecentFilesList(_fileName);
		QFileInfo info = QFileInfo(_fileName);
		setLastProjectFolder(info.absolutePath());
	}
	//update GUI
	actualizeGuiFromProject();
}

void MainWindow::actualizeGuiFromProject()
{
	_tabProject->actualizeGuiFromProject();


	// Menus and buttons
	_ui->actionSaveProject->setEnabled(_project->isDefined());
	_ui->menuAddProblem->setEnabled(_project->isDefined());
	_ui->actionDatabases->setEnabled(_project->isDefined());
        _ui->actionOMCShell->setEnabled(_project->isDefined());
        _ui->actionOMCClear->setEnabled(_project->isDefined());


	if(_project->isDefined())
	{
		
		updateRecentFilesMenu();
	}
	else
	{
	}
}


void MainWindow::newOneSimulation()
{
	if(_project->isDefined())
	{
		// Creating Problem
		WidgetSelectModModel* widgetSelect = new WidgetSelectModModel(_project->modReader(),_project->rootModClass(),this);
		if(widgetSelect->exec()==QDialog::Accepted)
		{
			ModModel* curModel = widgetSelect->selectedModel;
			_project->addNewProblem(Problem::ONESIMULATION,curModel);
		}
	}
}
void MainWindow::newOptimization(){

	if(_project->isDefined())
	{
		// Creating Problem
		WidgetSelectModModel* widgetSelect = new WidgetSelectModModel(_project->modReader(),_project->rootModClass(),this);
		if(widgetSelect->exec()==QDialog::Accepted)
		{
			ModModel* curModel = widgetSelect->selectedModel;
			_project->addNewProblem(Problem::OPTIMIZATION,curModel);
		}
	}
}
void MainWindow::newProblemEI()
{
	if(_project->isDefined())
	{
                _project->addNewProblem(Problem::EIPROBLEM,NULL);
	}
}
void MainWindow::newOMCShell()
{
	if(_project->isDefined())
	{
		//adding tab
		TabOMC* newTab = new TabOMC(_project);
		newTab->setBackgroundRole(QPalette::Window);
		newTab->setAutoFillBackground(true);
		newTab->setWindowTitle("OMC");

		//Adding tab
		_tabMain->addTab(newTab,"OMC");
		_tabMain->setCurrentWidget(newTab);
	}
}

void MainWindow::openSettings()
{
	DlgSettings *dlg = new DlgSettings(MOSettings());
	dlg->exec();
}

void MainWindow::OMCClear()
{
	if(_project->isDefined())
	{
		_project->moomc()->clear();
	}
}

void MainWindow::enableProblemTab(QModelIndex index)
{
	int i=0;
	if(_project->isDefined() && index.isValid())
	{
		if (index.row()<_project->problems()->items.size())
		{
			QString name = _project->problems()->items.at(index.row())->name();

			bool found = false;
			while(i<_tabMain->count() && !found)
			{
                                if((_tabMain->tabText(i)==name)&&(((MOTabBase*)_tabMain->widget(i))->tabType()==MOTabBase::TABPROBLEM))
					found =true;
				else
					i++;
			}
			if (found)
				_tabMain->setCurrentIndex(i);
		}
	}
}




void MainWindow::enableSolvedProblemTab(QModelIndex index)
{
	int i=0;
	if(_project->isDefined() && index.isValid())
	{
		if (index.row()<_project->solvedProblems()->items.size())
		{
			QString name = _project->solvedProblems()->items.at(index.row())->name();

			bool found = false;
			while(i<_tabMain->count() && !found)
			{
                                if((_tabMain->tabText(i)==name)&&(((MOTabBase*)_tabMain->widget(i))->tabType()==MOTabBase::TABSOLVEDPROBLEM))
					found =true;
				else
					i++;
			}
			if (found)
				_tabMain->setCurrentIndex(i);
		}
	}
}



void MainWindow::quit()
{
	_project->terminateOmsThreads();
	
	qApp->quit();
}
void MainWindow::projectReset()
{
	// deleting tabs
	while(_tabMain->count()>1)
	{
		_tabMain->removeTab(1);
	}
	_ui->treeProblems->reset();
	_ui->treeSolvedProblems->reset();
	actualizeGuiFromProject();
}
void MainWindow::onAddedProblem(Problem* newProblem)
{
	// Creating problem tab
	_tabMain->addProblemTab(_project,newProblem);
	emit sendInfo(Info(ListInfo::ADDEDPROBLEM,newProblem->name()));
}
void MainWindow::onAddedSolvedProblem(Problem* newProblem)
{
	// Creating problem tab
	_tabMain->addSolvedProblemTab(_project,newProblem);
}

void MainWindow::onComponentsUpdated()
{
	// updating gui of components (in project tab)
	//_tabProject->tabComponents->actualizeComponentTree();
}

void MainWindow::onModifiersUpdated()
{
	// updating gui of components (in project tab)
	//_tabProject->tabComponents->actualizeModifiersTable();
}

void MainWindow::onConnectionsUpdated()
{
	// updating gui of components (in project tab)
	//_tabProject->tabComponents->actualizeConnectionsTable();
}

void MainWindow::removeSolvedProblem()
{
	// SLOT : sender is menu, data is containing problem number
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		int iPb = action->data().toInt();
		if((iPb>-1) && (iPb<_project->solvedProblems()->rowCount()))
			removeSolvedProblem(iPb);
	}
}


void MainWindow::removeSolvedProblem(int num)
{
	QMessageBox msgBox;
	msgBox.setText("Removing solved problem cannot be undone.");
	QString msg;
	msg.sprintf("Are you sure you want to remove %s ?",_project->solvedProblems()->items.at(num)->name().toLatin1().data());
	msgBox.setInformativeText(msg);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Yes);

	if(msgBox.exec() == QMessageBox::Yes)
	{
		_project->removeSolvedProblem(num);
	}
}

void MainWindow::removeSolvedProblemTab(int num)
{
	//remove tab
	_tabMain->removeTab(MOTabBase::TABSOLVEDPROBLEM,_project->solvedProblems()->items.at(num)->name());
}

void MainWindow::removeProblem()
{
	// SLOT : sender is menu, data is containing problem number
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		int iPb = action->data().toInt();
		if((iPb>-1) && (iPb<_project->problems()->rowCount()))
			removeProblem(iPb);
	}
}


void MainWindow::removeProblem(int num)
{
	
	QMessageBox msgBox;
	msgBox.setText("Removing problem cannot be undone.");
	QString msg;
	msg.sprintf("Are you sure you want to remove %s ?",_project->problems()->items.at(num)->name().toLatin1().data());
	msgBox.setInformativeText(msg);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Yes);

	if(msgBox.exec() == QMessageBox::Yes)
	{
		_project->removeProblem(num);
	}
}

void MainWindow::removeProblemTab(int num)
{
	//remove tab
	_tabMain->removeTab(MOTabBase::TABPROBLEM,_project->problems()->items.at(num)->name());
}

void MainWindow::renameProblem()
{
	// SLOT : sender is menu, data is containing problem number
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		int iPb = action->data().toInt();
		if((iPb>-1) && (iPb<_project->problems()->rowCount()))
			renameProblem(iPb);
	}

}
void MainWindow::renameProblem(int i)
{
	 bool ok;
     QString newName = QInputDialog::getText(this, "Rename...",
		 "New name :", QLineEdit::Normal,_project->problems()->items.at(i)->name(),&ok);
                                          
     if (ok && !newName.isEmpty())
	 {
		 _project->renameProblem(i,newName);
	 }
}


void MainWindow::renameSolvedProblem()
{
	// SLOT : sender is menu, data is containing problem number
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		int iPb = action->data().toInt();
		if((iPb>-1) && (iPb<_project->solvedProblems()->rowCount()))
			renameSolvedProblem(iPb);
	}

}
void MainWindow::renameSolvedProblem(int i)
{
	 bool ok;
     QString newName = QInputDialog::getText(this, "Rename...",
		 "New name :", QLineEdit::Normal,_project->solvedProblems()->items.at(i)->name(),&ok);
                                          
     if (ok && !newName.isEmpty())
	 {
		 _project->renameSolvedProblem(i,newName);
	 }
}





void MainWindow::showSolvedProblemPopup(const QPoint & iPoint)
{
	//Popup on Result Tree
	QModelIndex  index ;
	index = _ui->treeSolvedProblems->indexAt(iPoint);
	if ( index.row() == -1 )
	{
		// no item selected
	}
	else
	{
		_ui->treeSolvedProblems->setCurrentIndex(index);
		Problem *selectedProblem = _project->solvedProblems()->items.at(index.row());
		QMenu *problemMenu = GuiTools::createSolvedProblemPopupMenu(_project,this,_ui->treeSolvedProblems->mapToGlobal(iPoint),selectedProblem,index.row());
		problemMenu->exec(_ui->treeSolvedProblems->mapToGlobal(iPoint));
	}
}



void MainWindow::showProblemPopup(const QPoint & iPoint)
{
	//Popup on Proble Tree
	QModelIndex  index ;
	index = _ui->treeProblems->indexAt(iPoint);
	if ( index.row() == -1 )
	{
		// no item selected
	}
	else
	{
		_ui->treeProblems->setCurrentIndex(index);
		Problem *selectedProblem = _project->problems()->items.at(index.row());
		QMenu * problemMenu = GuiTools::createProblemPopupMenu(_project,this,_ui->treeProblems->mapToGlobal(iPoint),selectedProblem,index.row());
		problemMenu->exec(_ui->treeProblems->mapToGlobal(iPoint));
	}
}

void MainWindow::showModClassTreePopup(const QPoint & iPoint)
{
	// Popup on ModClass Tree
	QModelIndex  index ;
	index = _ui->treeModClass->indexAt(iPoint);
	if ( !index.isValid())
	{
		// no item selected
	}
	else
	{
		ModClass* selectedModClass = static_cast<ModClass*>(index.internalPointer());
		QMenu * modClassMenu= GuiTools::newModClassPopupMenu(_project,_ui->treeProblems->mapToGlobal(iPoint),selectedModClass);
		if(modClassMenu)
			modClassMenu->exec(_ui->treeModClass->mapToGlobal(iPoint));
	}
}


void MainWindow::updateRecentFilesList(QString fileName)
{
	QSettings settings("MO", "Settings");
    QStringList files = settings.value("MO/recentFileList").toStringList();
    
	files.removeAll(fileName);
    files.prepend(fileName);
    
	while (files.size() > MaxRecentFiles)
	{
            files.removeLast();
	}

    settings.setValue("MO/recentFileList", files);
}

void MainWindow::setLastProjectFolder(QString folderName)
{
	QSettings settings("MO", "Settings");
    settings.setValue("MO/recentProjectFolder", folderName);
}

void MainWindow::setLastDatabaseFolder(QString folderName)
{
	QSettings settings("MO", "Settings");
    settings.setValue("MO/recentDatabseFolder", folderName);
}

QString MainWindow::getLastProjectFolder()
{
	QSettings settings("MO", "Settings");
	return settings.value("MO/recentProjectFolder").toString();
}

QString MainWindow::getLastDatabaseFolder()
{
	QSettings settings("MO", "Settings");
	return settings.value("MO/recentDatabaseFolder").toString();
}

void MainWindow::updateRecentFilesMenu()
{
	QSettings settings("MO", "Settings");
    QStringList files = settings.value("MO/recentFileList").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
		_recentFileActs[i]->setText(text);
		_recentFileActs[i]->setData(files[i]);
		_recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
	{
		_recentFileActs[j]->setVisible(false);
	}

	_separatorAct->setVisible(numRecentFiles > 0);

}

void MainWindow::createRecentFilesMenu()
{

	for (int i = 0; i < MaxRecentFiles; ++i)
	{
		_recentFileActs[i] = new QAction(this);
		connect(_recentFileActs[i], SIGNAL(triggered()),
                    this, SLOT(openRecentFile()));
		_ui->menuFile->insertAction(_ui->actionQuit,_recentFileActs[i]);
	}
	_separatorAct = _ui->menuFile->insertSeparator(_ui->actionQuit);
}

void MainWindow::clearRecentFilesMenu()
{
	QSettings settings("MO", "Settings");
    QStringList files;
    settings.setValue("MO/recentFileList", QStringList());

	updateRecentFilesMenu();
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		loadProject(action->data().toString());
	}
}

void MainWindow::displayProgress(float fraction)
{
	if(fraction==0)
	{
		_widgetProgress->hide();
		_widgetProgress->setProgress(0);
	}
	else
	{
		_widgetProgress->show();
		_widgetProgress->setProgress(fraction);
	}
}

void MainWindow::displayProgress(float fraction,int curEval,int totalEval)
{
	if(fraction==0)
	{
		_widgetProgress->hide();
		_widgetProgress->setProgress(0,curEval,totalEval);
	}
	else
	{
		_widgetProgress->show();
		_widgetProgress->setProgress(fraction,curEval,totalEval);
	}
}

void MainWindow::onProblemBegun(Problem* _problem)
{
	_widgetProgress->show();
	_widgetProgress->newTask("Running "+_problem->name()+"...");
}

void MainWindow::onProblemFinished(Problem* _problem)
{
	_widgetProgress->hide();
}


void MainWindow::onNewProblemProgress(float fraction)
{
	_widgetProgress->setProgress(fraction);
}


void MainWindow::onNewProblemProgress(float fraction,int curEval,int totalEval)
{
	_widgetProgress->setProgress(fraction,curEval,totalEval);
}

void MainWindow::onProblemStopAsked()
{
	Problem* curLaunchedProblem = _project->curLaunchedProblem();
	if(curLaunchedProblem)
	{
		_project->onProblemStopAsked(curLaunchedProblem);
	}
}


void MainWindow::loadMoFile()
{
    //get last .mo folder
    QSettings settings("OMOptim", "Settings");
    QString lastMoFolder = settings.value("LastMoFolder").toString();

	QStringList fileNames = QFileDialog::getOpenFileNames(
		this,
		"MO - Add .mo file to project",
            lastMoFolder,
		"Modelica file (*.mo)" );

	for(int i=0;i<fileNames.size();i++)
		_project->loadMoFile(fileNames.at(i));

    // save last .mo folder
    if(fileNames.size())
    {
        QFileInfo loaded(fileNames.at(0));
        lastMoFolder = loaded.absoluteDir().absolutePath();
        settings.setValue("LastMoFolder",lastMoFolder);
}
}

void MainWindow::loadModelicaLibrary()
{
	_project->loadModelicaLibrary();
}

void MainWindow::onSelectedModClass(QModelIndex index)
{
	if(index.isValid())
	{
		ModClass* _modClass = static_cast<ModClass*>(index.internalPointer());
		
		if(_modClass)
			_project->setCurModClass(_modClass);
	}
	else
		_project->setCurModClass(NULL);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
        QSettings settings("MO", "GUI_");
        settings.setValue(QApplication::applicationName()+"/geometry", saveGeometry());
        settings.setValue(QApplication::applicationName()+"/windowState", saveState());
	QMainWindow::closeEvent(event);
}

 void MainWindow::readSettings()
 {
     QSettings settings("MO", "GUI");
     restoreGeometry(settings.value(QApplication::applicationName()+"/geometry").toByteArray());
     restoreState(settings.value(QApplication::applicationName()+"/windowState").toByteArray());
 }

 void MainWindow::dispAboutOMOptim()
 {
	 AboutOMOptim dlg(this);
	 dlg.exec();
 }

 void MainWindow::dispAboutQt()
 {
	QMessageBox::aboutQt(this,"About Qt");
 }

 void MainWindow::openUserManual()
 {
     QUrl userManualPath;
     // since in MAC OS X the url adds extra quotes to it, so we need to handle it differently.
     //#ifdef Q_OS_MAC
     //    userManualPath = QUrl(QString("file:///").append(QString(getenv("OPENMODELICAHOME")))
     //                         .append("/share/doc/omedit/OMEdit-UserManual.pdf"));
     //#else
     userManualPath = QUrl(QString("file:///").append(OpenModelica::home().replace("\\", "/"))
                           .append("/share/doc/omoptim/OMOptim-UsersGuide.pdf"));
     //#endif
     QDesktopServices::openUrl(userManualPath);
 }
