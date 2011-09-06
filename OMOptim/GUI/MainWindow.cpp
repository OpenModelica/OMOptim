// $Id$
/**
  @file MainWindow.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
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
#include "TabOneSim.h"
#include "MOGuiTools.h"
#include "MOVector.h"
#include "Variable.h"
#include "TabOMC.h"
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

    // Recent Files
    createRecentFilesMenu();

    //***********************
    // Main window gui
    //***********************
    // Tabs
    _tabMain = new MOMainTab(this,_project);
    _tabProject = new TabProject(_project,_recentFileActs,this);
    _tabProject->setWindowTitle("Project");
    _tabProject->setBackgroundRole(QPalette::Window);
    _tabProject->setAutoFillBackground(true);
    _tabMain->addTab(_tabProject,"Project");
    _tabMain->setCurrentWidget(_tabProject);

    // Side part
    _ui->splitterH->insertWidget(1,_tabMain);
    _ui->splitterH->setStretchFactor(1,10);
    delete _ui->widgetToDelete;
    _ui->treeOMCases->header()->setResizeMode(QHeaderView::Stretch);
    _ui->treeResults->header()->setResizeMode(QHeaderView::Stretch);
    addDockWidget(Qt::BottomDockWidgetArea,_ui->dockLog,Qt::Vertical);

    // Actions
    QList<QAction*> dispActions;
    dispActions << _ui->dockLog->toggleViewAction();
    _ui->menuDisplay->addActions(dispActions);

    // Trees
    _ui->treeOMCases->setModel(_project->problems());
    _ui->treeResults->setModel(_project->results());
    //GuiTools::ModClassToTreeView(_project->modReader(),_project->rootModClass(),_ui->treeModClass,_project->modClassTree());
    _ui->treeModClass->setModel(_project->modClassTree());
    _ui->treeOMCases->setContextMenuPolicy(Qt::CustomContextMenu);
    _ui->treeResults->setContextMenuPolicy(Qt::CustomContextMenu);
    _ui->treeModClass->setContextMenuPolicy(Qt::CustomContextMenu);

    // progress dock
    // hide title
    QWidget* taskTitleWidget = new QWidget(this); /* where this a QMainWindow object */
    _ui->dockProgress->setTitleBarWidget( taskTitleWidget );


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
    connect(_ui->treeOMCases, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(enableProblemTab(QModelIndex)));
    connect(_ui->treeResults, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(enableResultTab(QModelIndex)));
    connect(_ui->treeModClass, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelectedModClass(QModelIndex)));
    connect(this, SIGNAL(sendInfo(Info)),this, SLOT( displayInfo(Info)));
    connect (_ui->treeResults,SIGNAL(customContextMenuRequested(const QPoint &)),
             this,SLOT(showResultPopup(const QPoint &)));
    connect (_ui->treeOMCases,SIGNAL(customContextMenuRequested(const QPoint &)),
             this,SLOT(showProblemPopup(const QPoint &)));
    connect (_ui->treeModClass,SIGNAL(customContextMenuRequested(const QPoint &)),
             this,SLOT(showModClassTreePopup(const QPoint &)));
    connect (_tabMain,SIGNAL(customContextMenuRequested(const QPoint &)),
             this,SLOT(showTabTitlePopup(const QPoint &)));
    connect(_tabProject,SIGNAL(newProject()),this,SLOT(newProject()));
    connect(_tabProject,SIGNAL(loadProject()),this,SLOT(loadProject()));

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
    connect(_project, SIGNAL(projectAboutToBeReset()),this, SLOT(onProjectAboutToBeReset()));
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
    connect( _project, SIGNAL(problemFinished(Problem*,Result*)),this,SLOT(onProblemFinished(Problem*,Result*)));
    connect( _project, SIGNAL(newProblemProgress(float)),this,SLOT(onNewProblemProgress(float)));
    connect( _project, SIGNAL(newProblemProgress(float,int,int)),this,SLOT(onNewProblemProgress(float,int,int)));
    connect( _widgetProgress, SIGNAL(askProblemStop(Problem*)),_project,SLOT(onProblemStopAsked(Problem*)));

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
    connect(_project, SIGNAL(beforeRemoveResult(int)),this, SLOT(removeResultTab(int)));
    connect(_project, SIGNAL(addedResult(Result*)),this, SLOT(onAddedResult(Result*)));

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
    case ListInfo::INFODEBUG :
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
        //        QMessageBox msgBox;
        //        msgBox.setText("Project will be loaded. It may take a long time while application will seem freezed.");
        //        msgBox.setIcon(QMessageBox::Information);
        //        msgBox.exec();

        //        msgBox.hide();
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
    //    QMessageBox msgBox;
    //    msgBox.setText("Project will be loaded. It may take a long time while application will seem freezed.");
    //    msgBox.exec();


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
    _ui->actionLoadMoFile->setEnabled(_project->isDefined());
    _ui->actionLoadModelicaLibrary->setEnabled(_project->isDefined());

}


void MainWindow::newOneSimulation()
{
    if(_project->isDefined())
    {
        // Creating Problem
        WidgetSelectModModel* widgetSelect = new WidgetSelectModModel(_project->modClassTree(),this);
        if(widgetSelect->exec()==QDialog::Accepted)
        {
            ModModel* curModel = widgetSelect->selectedModel;
            _project->addNewProblem(Problem::ONESIMULATIONTYPE,curModel);
        }
    }
}
void MainWindow::newOptimization(){

    if(_project->isDefined())
    {
        // Creating Problem
        WidgetSelectModModel* widgetSelect = new WidgetSelectModModel(_project->modClassTree(),this);
        if(widgetSelect->exec()==QDialog::Accepted)
        {
            ModModel* curModel = widgetSelect->selectedModel;
            _project->addNewProblem(Problem::OPTIMIZATIONTYPE,curModel);
        }
    }
}
void MainWindow::newProblemEI()
{
    if(_project->isDefined())
    {
        _project->addNewProblem(Problem::EIPROBLEMTYPE,NULL);
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
    MOTabBase* tab;
    if(_project->isDefined() && index.isValid())
    {
        if (index.row()<_project->problems()->size())
        {
            QString name = _project->problems()->at(index.row())->name();

            bool found = false;
            while(i<_tabMain->count() && !found)
            {
                tab = dynamic_cast<MOTabBase*>(_tabMain->widget(i));
                if(tab && (_tabMain->tabText(i)==name)&&(tab->tabType()==MOTabBase::TABPROBLEM))
                    found =true;
                else
                    i++;
            }
            if (found)
                _tabMain->setCurrentIndex(i);
        }
    }
}




void MainWindow::enableResultTab(QModelIndex index)
{
    int i=0;
    MOTabBase* tab;
    if(_project->isDefined() && index.isValid())
    {
        if (index.row()<_project->results()->size())
        {
            QString name = _project->results()->at(index.row())->name();

            bool found = false;
            while(i<_tabMain->count() && !found)
            {
                tab = dynamic_cast<MOTabBase*>(_tabMain->widget(i));
                if(tab && (_tabMain->tabText(i)==name)&&(tab->tabType()==MOTabBase::TABSOLVEDPROBLEM))
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
void MainWindow::onProjectAboutToBeReset()
{
    // deleting tabs
    while(_tabMain->count()>1)
    {
        _tabMain->removeTab(1);
    }
    _ui->treeOMCases->reset();
    _ui->treeResults->reset();
    actualizeGuiFromProject();
}
void MainWindow::onAddedProblem(Problem* newProblem)
{
    // Creating problem tab
    _tabMain->addProblemTab(_project,newProblem);
    emit sendInfo(Info(ListInfo::ADDEDPROBLEM,newProblem->name()));
}
void MainWindow::onAddedResult(Result* newResult)
{
    // Creating problem tab
    _tabMain->addResultTab(_project,newResult);
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

void MainWindow::removeResult()
{
    // SLOT : sender is menu, data is containing problem number
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        int iPb = action->data().toInt();
        if((iPb>-1) && (iPb<_project->results()->rowCount()))
            removeResult(iPb);
    }
}


void MainWindow::removeResult(int num)
{
    QMessageBox msgBox;
    msgBox.setText("Removing solved problem cannot be undone.");
    QString msg;
    msg.sprintf("Are you sure you want to remove %s ?",_project->results()->at(num)->name().toLatin1().data());
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if(msgBox.exec() == QMessageBox::Yes)
    {
        _project->removeResult(num);
    }
}

void MainWindow::removeResultTab(int num)
{
    //remove tab
    _tabMain->removeTab(MOTabBase::TABSOLVEDPROBLEM,_project->results()->at(num)->name());
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
    msg.sprintf("Are you sure you want to remove %s ?",_project->problems()->at(num)->name().toLatin1().data());
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
    _tabMain->removeTab(MOTabBase::TABPROBLEM,_project->problems()->at(num)->name());
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
                                            "New name :", QLineEdit::Normal,_project->problems()->at(i)->name(),&ok);

    if (ok && !newName.isEmpty())
    {
        _project->renameProblem(i,newName);
    }
}


void MainWindow::renameResult()
{
    // SLOT : sender is menu, data is containing problem number
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        int iPb = action->data().toInt();
        if((iPb>-1) && (iPb<_project->results()->rowCount()))
            renameResult(iPb);
    }

}
void MainWindow::renameResult(int i)
{
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename...",
                                            "New name :", QLineEdit::Normal,_project->results()->at(i)->name(),&ok);

    if (ok && !newName.isEmpty())
    {
        _project->renameResult(i,newName);
    }
}





void MainWindow::showResultPopup(const QPoint & iPoint)
{
    //Popup on Result Tree
    QModelIndex  index ;
    index = _ui->treeResults->indexAt(iPoint);
    if ( index.row() == -1 )
    {
        // no item selected
    }
    else
    {
        _ui->treeResults->setCurrentIndex(index);
        Result *selectedResult = _project->results()->at(index.row());
        QMenu *resultMenu = GuiTools::createResultPopupMenu(_project,this,_ui->treeResults->mapToGlobal(iPoint),selectedResult,index.row());
        resultMenu->exec(_ui->treeResults->mapToGlobal(iPoint));
    }
}



void MainWindow::showProblemPopup(const QPoint & iPoint)
{
    //Popup on Proble Tree
    QModelIndex  index ;
    index = _ui->treeOMCases->indexAt(iPoint);
    if ( index.row() == -1 )
    {
        // no item selected
    }
    else
    {
        _ui->treeOMCases->setCurrentIndex(index);
        Problem *selectedProblem = _project->problems()->at(index.row());
        QMenu * problemMenu = GuiTools::createProblemPopupMenu(_project,this,_ui->treeOMCases->mapToGlobal(iPoint),selectedProblem,index.row());
        problemMenu->exec(_ui->treeOMCases->mapToGlobal(iPoint));
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
        QMenu * modClassMenu= GuiTools::newModClassPopupMenu(_project,_ui->treeOMCases->mapToGlobal(iPoint),selectedModClass);
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

void MainWindow::createRecentFilesMenu()
{

    _recentFileActs.clear();
    QAction* newAction;
    QSettings settings("MO", "Settings");
    QStringList files = settings.value("MO/recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        newAction = new QAction(this);
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        newAction->setText(text);
        newAction->setData(files[i]);
        newAction->setVisible(true);
        connect(newAction, SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
        _recentFileActs.push_back(newAction);
        _ui->menuFile->insertAction(_ui->actionQuit,newAction);
    }

    //_separatorAct->setVisible(numRecentFiles > 0);
    _ui->menuFile->insertSeparator(_ui->actionQuit);
}


void MainWindow::clearRecentFilesMenu()
{
    QSettings settings("MO", "Settings");
    QStringList files;
    settings.setValue("MO/recentFileList", QStringList());

    createRecentFilesMenu();
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
        _ui->dockProgress->hide();
        //_widgetProgress->hide();
        _widgetProgress->setProgress(0);
    }
    else
    {
        _ui->dockProgress->show();
        //_widgetProgress->show();
        _widgetProgress->setProgress(fraction);
    }
}

void MainWindow::displayProgress(float fraction,int curEval,int totalEval)
{
    if(fraction==0)
    {
        _ui->dockProgress->hide();
        //_widgetProgress->hide();
        _widgetProgress->setProgress(0,curEval,totalEval);
    }
    else
    {
        _ui->dockProgress->show();
        //_widgetProgress->show();
        _widgetProgress->setProgress(fraction,curEval,totalEval);
    }
}

void MainWindow::onProblemBegun(Problem* problem)
{
    _widgetProgress->setCurProblem(problem);
    _ui->dockProgress->show();
    //_widgetProgress->show();
    _widgetProgress->newTask("Running "+problem->name()+"...");
}

void MainWindow::onProblemFinished(Problem*,Result*)
{
    _ui->dockProgress->hide();
    //_widgetProgress->hide();
}

void MainWindow::onNewProblemProgress(float fraction)
{
    _widgetProgress->setProgress(fraction);
}


void MainWindow::onNewProblemProgress(float fraction,int curEval,int totalEval)
{
    _widgetProgress->setProgress(fraction,curEval,totalEval);
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
    QStringList pathTries;

    pathTries << QApplication::applicationDirPath().replace("\\", "/")+QDir::separator()+"../share/doc/omoptim/OMOptimUsersGuide.pdf";
    pathTries << OpenModelica::home().replace("\\", "/").append("/share/doc/omoptim/OMOptimUsersGuide.pdf");

    int iTry=0;
    bool openOk=false;
    while(iTry<pathTries.size()&&!openOk)
    {
        openOk = QDesktopServices::openUrl(QUrl(QString("file:///").append(pathTries.at(iTry))));
        iTry++;
    }
}
