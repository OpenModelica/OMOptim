// $Id$
/**
  @file MainWindow.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include <iostream>

#include "MainWindow.h"
#include "MOThreads.h"
#include "newprojectform.h"
#include "ListInfo.h"
#include "MOVector.h"
#include "Variable.h"
#include "TabOMC.h"
#include "Tabs/MOTab.h"
#include "OMOptimGuiTools.h"
#include "GuiTools.h"
#include "Problems.h"
#include "Results.h"
#include "ProblemInterfaces.h"
#include "widgetloadexemodel.h"
#include "scriptparseromoptim.h"
#include "Dialogs/HelpDlg.h"
#include "ScriptTextDlg.h"
#include "OpenModelica.h"

namespace Ui
{
class MainWindowClass;
}


MainWindow::MainWindow(Project* project,QWidget *parent)
    : QMainWindow(parent), _ui(new Ui::MainWindowClass)
{

    this->resize(800, 600);
    this->setStyleSheet("QMainWindow::separator {height:0;width:0}");

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

    addDockWidget(Qt::BottomDockWidgetArea,_ui->dockLog,Qt::Vertical);

    // Actions
    QList<QAction*> dispActions;
    dispActions << _ui->dockLog->toggleViewAction();
    _ui->menuDisplay->addActions(dispActions);



    // Trees
    _casesTree = new OMCasesCombiner(_project->problems(),_project->results(),this);
    _casesTreeView = new OMCasesTreeView(_project,_casesTree,this);
    _ui->dockOMCases->setWidget(_casesTreeView);

    _ui->treeModItem->setModel(_project->modItemsTree());
    _ui->treeModItem->setContextMenuPolicy(Qt::CustomContextMenu);
    _ui->treeModItem->setDragEnabled(true);
    _ui->treeModItem->setDragDropMode(QAbstractItemView::DragDrop);

    // progress dock
    // hide title
    QWidget* taskTitleWidget = new QWidget(this); /* where this a QMainWindow object */
    _ui->dockProgress->setTitleBarWidget( taskTitleWidget );


    // text log
    _textLog=new MyTextLog(this);
    _ui->layoutTextLog->addWidget(_textLog);
    _textLog->setOpenExternalLinks(true);

    // limit the size of omc and debug log
    _ui->textOMC->document()->setMaximumBlockCount(1000);
    _ui->textDebug->document()->setMaximumBlockCount(1000);
    _textLog->document()->setMaximumBlockCount(1000);

    //Error, msg and progress
    _widgetProgress = new WidgetProgress(this);
    _ui->layoutProgress->addWidget(_widgetProgress);
    displayProgress(0);



    //Create the Statusbar
    _statusBar = new QStatusBar();
    _statusBar->setObjectName("statusBar");
    _statusBar->setContentsMargins(0, 0, 1, 0);
    _stProgressBar = new QProgressBar;
    _stProgressBar->setMaximumWidth(300);
    _stProgressBar->setTextVisible(false);
    //_statusBar->addPermanentWidget(_stProgressBar);
    _statusBar->setVisible(false);
    this->setStatusBar(_statusBar);


    connectProject();
    actualizeGuiFromProject();

    //*********************************
    // Signals for gui
    //*********************************
    connect(_casesTreeView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(enableOMCaseTab(QModelIndex)));
    connect(_ui->treeModItem, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelectedModItem(QModelIndex)));
    connect(this, SIGNAL(sendInfo(Info)),this, SLOT( displayInfo(Info)));
    connect (_ui->treeModItem,SIGNAL(customContextMenuRequested(const QPoint &)),
             this,SLOT(showModItemsTreePopup(const QPoint &)));
    connect (_tabMain,SIGNAL(customContextMenuRequested(const QPoint &)),
             this,SLOT(showTabTitlePopup(const QPoint &)));
    connect(_tabProject,SIGNAL(newProject()),this,SLOT(newProject()));
    connect(_tabProject,SIGNAL(loadProject()),this,SLOT(loadProject()));


    // Menus
    connect( _ui->actionNewProject, SIGNAL( triggered() ),this, SLOT( newProject()));
    connect( _ui->actionSaveProject, SIGNAL( triggered() ),this, SLOT( saveProject()));
    connect( _ui->actionLoadProject, SIGNAL( triggered() ),this, SLOT( loadProject()));
    connect( _ui->actionQuit, SIGNAL( triggered() ),this, SLOT( quit()));
    connect( _ui->actionOMCShell,SIGNAL( triggered() ),this, SLOT( newOMCShell()));
    connect( _ui->actionOMCClear,SIGNAL( triggered() ),this, SLOT( OMCClear()));
    connect( _ui->actionSettings,SIGNAL( triggered() ),this, SLOT(openSettings()));
    connect( _ui->actionClearRecent, SIGNAL(triggered()), this, SLOT(clearRecentFilesMenu()));
    connect( _ui->actionLoadMoFile,  SIGNAL(triggered()), this, SLOT(loadMoFile()));
    connect( _ui->actionLoadModelicaLibrary,  SIGNAL(triggered()), this, SLOT(loadModelicaLibrary()));
    connect( _ui->actionLoadExecutableModel,  SIGNAL(triggered()), this, SLOT(loadExecutableModel()));
    connect( _ui->actionRefreshModTree, SIGNAL(triggered()),this,SLOT(refreshModelTree()));
    connect( _ui->actionHelp, SIGNAL(triggered()),this, SLOT(openUserManual()));
    connect( _ui->actionStartOmc,SIGNAL(triggered()),_project->moomc(),SLOT(startServer()));
    connect( _ui->actionClearLog,SIGNAL(triggered()),this,SLOT(clearLog()));
    connect( _ui->actionLoadCases,SIGNAL(triggered()),this,SLOT(loadCases()));
    connect( _ui->actionLaunchScript2,SIGNAL(triggered()),this,SLOT(launchScript()));
    connect( _ui->actionDispScriptFunctions,SIGNAL(triggered()),this,SLOT(dispScriptFunctions()));
    connect( _ui->actionLaunchScriptText,SIGNAL(triggered()),this,SLOT(launchScriptText()));
    connect( _ui->actionExportProject,SIGNAL(triggered()),this,SLOT(exportFolder()));

    //*********************************
    // Signals for informations
    //*********************************
    connect(InfoSender::instance(),SIGNAL(setCurrentTask(QString)),this,SLOT(setStatusBarText(QString)));
    connect(InfoSender::instance(),SIGNAL(increaseTaskProgress()),this,SLOT(increaseStProgressBar()));
    connect(InfoSender::instance(),SIGNAL(noCurrentTask()),this,SLOT(eraseStatusBarText()));
    connect(InfoSender::instance(), SIGNAL(sent(Info)),this, SLOT( displayInfo(Info)));


    //*********************************
    // Signals for ModItem
    //*********************************
    connect(_ui->treeModItem, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelectedModItem(QModelIndex)));
    connect( _ui->pushLoadMoFile,  SIGNAL(clicked()), this, SLOT(loadMoFile()));

    //*********************************
    // Signals for About dialogs
    //*********************************
    connect(_ui->actionAboutQt, SIGNAL(triggered()),this, SLOT(dispAboutQt()));
    connect( _ui->actionAboutOMOptim,  SIGNAL(triggered()), this, SLOT(dispAboutOMOptim()));




    //*********************************
    // Signals for plugins
    //*********************************
    connect(_ui->actionLoadPlugins,SIGNAL(triggered()),this,SLOT(loadPlugins()));


    //*********************************
    // Reload gui configuration
    //*********************************
    readSettings();

    //*********************************
    // Configuration
    //*********************************

    updateProblemsMenu();
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::clearLog()
{
    _textLog->clear();
    _ui->textDebug->clear();
    _ui->textOMC->clear();
}

void MainWindow::displayInfo(Info i)
{
    if(i.infoMsg.isEmpty() && i.infoType!=ListInfo::TASK)
        return ;

    QTextCharFormat infoFormat;

    // stylize message
    QString prefix;
    QString suffix;

    switch(i.infoType)
    {
    case ListInfo::NORMAL2 :
    case ListInfo::TASK :
        prefix = "";
        suffix = "";
        infoFormat.setForeground(Qt::black);
        infoFormat.setFontWeight(QFont::Normal);
        break;
    case ListInfo::WARNING2 :
        prefix = "<b><font color='#FF7700'>Warning : ";
        suffix = "</font></b>";
        infoFormat.setForeground(QBrush(QColor(255,119,0)));
        infoFormat.setFontWeight(QFont::Bold);
        break;
    case ListInfo::ERROR2 :
        prefix = "<b><font color='red'>Error : ";
        suffix = "</font></b>";
        infoFormat.setForeground(Qt::red);
        infoFormat.setFontWeight(QFont::Bold);
        break;

    case ListInfo::OMCNORMAL2 :
        prefix = "OMCNormal :";
        suffix = "";
        infoFormat.setForeground(Qt::black);
        infoFormat.setFontWeight(QFont::Normal);
        break;
    case ListInfo::OMCWARNING2 :
        prefix = "OMCWarning :";
        suffix = "";
        infoFormat.setForeground(Qt::darkYellow);
        infoFormat.setFontWeight(QFont::Bold);
        break;
    case ListInfo::OMCERROR2 :
        prefix = "<b><font color='red'>OMCError : ";
        suffix = "</font></b>";
        infoFormat.setForeground(Qt::red);
        infoFormat.setFontWeight(QFont::Bold);
        break;
    case ListInfo::INFODEBUG :
        prefix = "<b><font color='blue'>Debug : ";
        suffix = "</font></b>";
        infoFormat.setForeground(Qt::blue);
        infoFormat.setFontWeight(QFont::Bold);
        break;
    }

    // QString msg = prefix + i.infoMsg + suffix;
    QString timePrefix = "["+QTime::currentTime().toString()+"] ";
    QString msg = timePrefix + i.infoMsg;
    // display
    switch(i.infoType)
    {
    case ListInfo::NORMAL2 :
    case ListInfo::WARNING2 :
    case ListInfo::ERROR2 :
    case ListInfo::TASK :
        if(!i.infoMsg.isEmpty())
        {
            _textLog->setCurrentCharFormat(infoFormat);
            _textLog->append(msg);
        }
        break;
    case ListInfo::OMCNORMAL2 :
    case ListInfo::OMCWARNING2 :
    case ListInfo::OMCERROR2 :
        _ui->textOMC->setCurrentCharFormat(infoFormat);
        _ui->textOMC->append(msg);
        break;
    case ListInfo::INFODEBUG :
        _ui->textDebug->setCurrentCharFormat(infoFormat);
        _ui->textDebug->append(msg);
        break;
    }

    if(i.infoType==ListInfo::TASK)
    {
        setStatusBarText(msg);
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

        // actualize gui
        actualizeGuiFromProject();
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
            _project->save(true);
        }
        else
        {
            _filePath = QFileDialog::getSaveFileName(
                        this,
                        "MO - Save Project",
                        _project->name()+".min",
                        "MO project (*.min)" );
            if(!_filePath.isNull())
            {
                _project->setFilePath(_filePath);
                _project->save(true);
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
                qAppName()+" - Open Project",
                getLastProjectFolder(),
                "project (*.min)" );

    if (!filename.isNull())
    {
        OMOptimGuiTools::consolidateModelsPath(filename,this);
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

    refreshModelTreeView();
}

void MainWindow::loadProject(QString filename)
{
    OMOptimGuiTools::consolidateModelsPath(filename,this);
    bool loaded = _project->load(filename);
    if(loaded)
    {
        updateRecentFilesList(filename);
        QFileInfo info = QFileInfo(filename);
        setLastProjectFolder(info.absolutePath());
    }

    //update GUI
    actualizeGuiFromProject();

    refreshModelTreeView();
}

void MainWindow::launchScript()
{


    QString filename = QFileDialog::getOpenFileName(
                this,
                qAppName()+" - Open script",
                getLastProjectFolder(),
                "All files (*.*)" );

    if(!filename.isNull())
        _project->launchScript(QFileInfo(filename));
}

void MainWindow::dispScriptFunctions()
{
    TextDlg dlg(_project->scriptParser()->helpText(),"Script functions",this);
    dlg.exec();
}

void MainWindow::launchScriptText()
{
    ScriptTextDlg dlg(_project->scriptParser(),_project,this);
    dlg.exec();
}

void MainWindow::exportFolder()
{
    QString folderPath = QFileDialog::getExistingDirectory(
                this,
                "OMOptim - Export project folder",
                _project->folder().absolutePath());

    if(!folderPath.isEmpty())
    {
        QDir folder(folderPath);
        if(!folder.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files).isEmpty())
        {
            QMessageBox msgbox(QMessageBox::Warning,"Consolidate folder","Please select an empty folder",
                               QMessageBox::Cancel|QMessageBox::Retry,this);

            int ret = msgbox.exec();
            if(ret == QMessageBox::Retry)
                this->exportFolder();
            return;
        }
        bool includeMSL;
        if(QMessageBox::Yes == QMessageBox::question(this,"Export folder","Includes Modelica Standard Library within .mo file ?",QMessageBox::Yes | QMessageBox::No,QMessageBox::No))
            includeMSL = true;
        else
            includeMSL = false;
        _project->exportProjectFolder(folder,includeMSL);
    }
}


void MainWindow::loadPlugins()
{
    // get last plugin folder
    QSettings settings("MO", "Settings");
    QString folder = settings.value("MO/recentPluginsFolder").toString();

    QString filters = "All files (*.*)";
#ifdef WIN32
    filters = "OMOptim plugins (*.dll);; All files (*.*)";
#endif
    QStringList filenames = QFileDialog::getOpenFileNames(
                this,
                "OMOptim - Load plugins",
                folder,
                filters );

    foreach (QString fileName, filenames)
    {
        _project->loadPlugin(fileName,true,true);
    }

    if(filenames.size()>0)
    {
        QDir dir(filenames.at(0));
        settings.setValue("MO/recentPluginsFolder",dir.absolutePath());
    }
}

void MainWindow::loadCases()
{
    // start with project folder
    QString folder = _project->folder().absolutePath();

    QString problemsFolder = QFileDialog::getExistingDirectory(
                this,
                "OMOptim - Load problems",
                folder);

    _project->addOMCases(problemsFolder);
}

void MainWindow::connectProject()
{

    // first disconnect existing signal-slots
    _project->disconnect(this);
    _project->_mofilesWatcher.disconnect(this);


    connect(_project, SIGNAL(projectAboutToBeReset()),this, SLOT(onProjectAboutToBeReset()));
    connect(_project, SIGNAL(projectChanged()),this, SLOT( actualizeGuiFromProject()));
    connect(_project, SIGNAL(sendProgress(float)), this, SLOT(displayProgress(float)));
    connect(_project, SIGNAL(sendProgress(float,int,int)), this, SLOT(displayProgress(float,int,int)));


    //*********************************
    // Signals for problems
    //*********************************
    connect( _project, SIGNAL(addedProblem(Problem*)),this, SLOT(onAddedProblem(Problem*)));
    connect( _project, SIGNAL(beforeRemoveProblem(Problem*)),this, SLOT(removeProblemTab(Problem*)));
    connect( _project, SIGNAL(problemBegun(Problem*)),this,SLOT(onProblemBegun(Problem*)));
    connect( _project, SIGNAL(problemFinished(Problem*,Result*)),this,SLOT(onProblemFinished(Problem*,Result*)));
    connect( _project, SIGNAL(newProblemProgress(float)),this,SLOT(onNewProblemProgress(float)));
    connect( _project, SIGNAL(newProblemProgress(float,int,int)),this,SLOT(onNewProblemProgress(float,int,int)));


    _widgetProgress->disconnect(SIGNAL(askProblemStop(Problem*)));
    connect( _widgetProgress, SIGNAL(askProblemStop(Problem*)),_project,SLOT(onProblemStopAsked(Problem*)));


    //*********************************
    // Signals for problem interfaces
    //*********************************
    connect(_project,SIGNAL(interfacesModified()),this,SLOT(updateProblemsMenu()));

    //*********************************
    // Signals for solved problems
    //*********************************
    connect(_project, SIGNAL(beforeRemoveResult(Result*)),this, SLOT(removeResultTab(Result*)));
    connect(_project, SIGNAL(addedResult(Result*)),this, SLOT(onAddedResult(Result*)));

    //*********************************
    // Signals for model
    //*********************************
    connect(&_project->_mofilesWatcher,SIGNAL(fileChanged(const QString&)),this,SLOT(onMoFileChanged(const QString&)));
    connect(_project,SIGNAL(modItemsTreeRefreshed()),this,SLOT(refreshModelTreeView()));

    //*********************************
    // Signals for MOomc
    //*********************************
    connect(_project->moomc(),SIGNAL(startOMCThread(QString)),this,SLOT(onStartedOMCThread(QString)));
    connect(_project->moomc(),SIGNAL(finishOMCThread(QString)),this,SLOT(onFinishedOMCThread(QString)));


}


void MainWindow::actualizeGuiFromProject()
{
    _tabProject->actualizeGuiFromProject();

    // Menus and buttons
    _ui->actionSaveProject->setEnabled(_project->isDefined());
    _ui->menuProblems->setEnabled(_project->isDefined());
    _ui->actionDatabases->setEnabled(_project->isDefined());
    _ui->actionOMCShell->setEnabled(_project->isDefined());
    _ui->actionOMCClear->setEnabled(_project->isDefined());
    _ui->actionLoadMoFile->setEnabled(_project->isDefined());
    _ui->actionLoadModelicaLibrary->setEnabled(_project->isDefined());

    QString windowTitle = "OMOptim - "+_project->name();
    if(!_project->isSaved()&& _project->isDefined())
        windowTitle.append("*");
    this->setWindowTitle(windowTitle);
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
    DlgSettings *dlg = new DlgSettings(true);
    dlg->exec();
}

void MainWindow::OMCClear()
{
    if(_project->isDefined())
    {
        _project->moomc()->clear();
    }
}


void MainWindow::enableOMCaseTab(QModelIndex index)
{
    if(index.isValid())
    {
        OMCase* selectedCase = _casesTree->item(index);
        _tabMain->enableCaseTab(selectedCase);
    }
}

void MainWindow::quit()
{
    _project->terminateOms();

    if(!_project->isSaved() && _project->isDefined())
    {
        QMessageBox msgBox;
        msgBox.setText("The project has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Save:
            _project->save(true);
            qApp->quit();
            break;
        case QMessageBox::Discard:
            qApp->quit();
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
        }
    }
    // if the slot is activated by the File->Quit menuitem.
    QAction *pActionQuit = qobject_cast<QAction*>(const_cast<QObject*>(sender()));
    if (pActionQuit)
        qApp->quit();
}
void MainWindow::onProjectAboutToBeReset()
{
    // deleting tabs
    while(_tabMain->count()>1)
    {
        _tabMain->removeTab(1);
    }

    _casesTreeView->reset();
    _ui->treeModItem->reset();

    actualizeGuiFromProject();
}

void MainWindow::onAddedProblem(Problem* newProblem)
{
    // Creating problem tab
    ProblemInterface* itf = (_project->problemsInterfaces()).interfaceOf(newProblem);

    QTime elapsed;
    if(itf)
    {
        elapsed.restart();
        _tabMain->addProblemTab(newProblem,itf->createProblemTab(newProblem,this));
    }

    _tabMain->enableCaseTab(newProblem);
}

void MainWindow::onAddedResult(Result* newResult)
{
    // Creating problem tab
    ProblemInterface* itf = _project->problemsInterfaces().interfaceOf(newResult);

    QTime elapsed;
    if(itf)
    {
        elapsed.restart();
        _tabMain->addResultTab(newResult,itf->createResultTab(newResult,this));
    }

    _tabMain->enableCaseTab(newResult);
}

void MainWindow::removeResult()
{
    // SLOT : sender is menu, data is containing problem number
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        int iPb = action->data().toInt();
        if((iPb>-1) && (iPb<_project->results()->rowCount()))
            removeResult(_project->results()->at(iPb));
    }
}


void MainWindow::removeResult(Result* result)
{
    QMessageBox msgBox;
    msgBox.setText("Removing solved problem cannot be undone.");
    QString msg;
    msg.sprintf("Are you sure you want to remove %s ?",result->name().toLatin1().data());
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if(msgBox.exec() == QMessageBox::Yes)
    {
        _project->removeResult(result);
    }
}

void MainWindow::removeResultTab(Result* result)
{
    //remove tab
    _tabMain->removeTab(MOTabBase::TABSOLVEDPROBLEM,result->name());
}

void MainWindow::removeProblem()
{
    // SLOT : sender is menu, data is containing problem number
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        int iPb = action->data().toInt();
        if((iPb>-1) && (iPb<_project->problems()->rowCount()))
            removeProblem(_project->problems()->at(iPb));
    }
}


void MainWindow::removeProblem(Problem* problem)
{

    QMessageBox msgBox;
    msgBox.setText("Removing problem cannot be undone.");
    QString msg;
    msg.sprintf("Are you sure you want to remove %s ?",problem->name().toLatin1().data());
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if(msgBox.exec() == QMessageBox::Yes)
    {
        _project->removeProblem(problem);
    }
}

void MainWindow::removeProblemTab(Problem* problem)
{
    //remove tab
    _tabMain->removeTab(MOTabBase::TABPROBLEM,problem->name());
}

void MainWindow::renameProblem()
{
    // SLOT : sender is menu, data is containing problem number
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        int iPb = action->data().toInt();
        if((iPb>-1) && (iPb<_project->problems()->rowCount()))
            renameProblem(_project->problems()->at(iPb));
    }

}

void MainWindow::renameProblem(Problem* problem)
{
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename...",
                                            "New name :", QLineEdit::Normal,problem->name(),&ok);

    if (ok && !newName.isEmpty())
    {
        _project->renameProblem(problem,newName);
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
            renameResult(_project->results()->at(iPb));
    }

}
void MainWindow::renameResult(Result* result)
{
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename...",
                                            "New name :", QLineEdit::Normal,result->name(),&ok);

    if (ok && !newName.isEmpty())
    {
        _project->renameResult(result,newName);
    }
}





void MainWindow::rightClickedOnCase(const QPoint & iPoint)
{
    //Popup on cases tree
    QModelIndex  index ;
    index = _casesTreeView->indexAt(iPoint);
    QModelIndexList indexes = _casesTreeView->selectionModel()->selectedIndexes();

    if ( !index.isValid() == -1 )
    {
        // no item selected
    }
    else
    {
        if(indexes.contains(index))
        {
            QMenu* caseMenu = NULL;
            OMCase* selectedCase = _casesTree->item(index);

            if(_project->problems()->contains(selectedCase))
                caseMenu = GuiTools::createProblemPopupMenu(_project,this,_casesTreeView->mapToGlobal(iPoint),dynamic_cast<Problem*>(selectedCase),index.row());
            if(_project->results()->contains(selectedCase))
                caseMenu = GuiTools::createResultPopupMenu(_project,this,_casesTreeView->mapToGlobal(iPoint),dynamic_cast<Result*>(selectedCase),index.row());

            if(caseMenu)
                caseMenu->exec(_casesTreeView->mapToGlobal(iPoint));
        }
    }
}


void MainWindow::showModItemsTreePopup(const QPoint & iPoint)
{
    // Popup on ModItem Tree
    QModelIndex  index ;
    index = _ui->treeModItem->indexAt(iPoint);
    if ( !index.isValid())
    {
        // no item selected
    }
    else
    {
        ModItem* selectedModItem = static_cast<ModItem*>(index.internalPointer());
        QMenu * modClassMenu= OMOptimGuiTools::newModItemPopupMenu(_project,_ui->treeModItem->mapToGlobal(iPoint),selectedModItem);
        if(modClassMenu)
            modClassMenu->exec(_ui->treeModItem->mapToGlobal(iPoint));
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

void MainWindow::showModItem(ModItem* modClass)
{
    _ui->treeModItem->expand(_project->modItemsTree()->indexOf(modClass));
}

void MainWindow::onMoFileChanged(const QString &moFile)
{
    // first remove from watcher to avoid multiple asking
    _project->_mofilesWatcher.removePath(moFile);

    QMessageBox msgbox(QMessageBox::Question,"Reload .mo file","Model file has been modified. Do you want to reload it ? \n"+moFile,
                       QMessageBox::No|QMessageBox::Yes,this);
    if(msgbox.exec()==QMessageBox::Yes)
    {
        _project->loadMoFile(moFile,true,true);
        refreshModelTreeView();
    }
    // restore watcher
    _project->_mofilesWatcher.addPath(moFile);
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

    refreshModelTreeView();

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
    refreshModelTreeView();
}

void MainWindow::loadExecutableModel()
{
    WidgetLoadExeModel* widget = new WidgetLoadExeModel(_project,this);
    QString name;
    bool isloaded =false;
    while((name.isEmpty() || _project->modItemsTree()->findInDescendants(name)) && !isloaded )
    {
        if(widget->exec() == QDialog::Accepted)
        {
            name = widget->name();
            if( !widget->name().isEmpty() && !_project->modItemsTree()->findInDescendants(name))
            {
                _project->loadExecutableModel(name, widget->exeFileInfo(),widget->inputFileInfo());
                refreshModelTreeView();
                isloaded =true;
            }
        }
        else
            break;
    }
}

void MainWindow::refreshModelTree()
{
    _project->refreshAllMod();
    refreshModelTreeView();
}

void MainWindow::refreshModelTreeView()
{
    _ui->treeModItem->setModel(NULL);
    _ui->treeModItem->setModel(_project->modItemsTree());
}

void MainWindow::updateProblemsMenu()
{
    _ui->menuProblems->clear();
    ProblemInterface* itf;
    QStringList problemTypes;
    QMenu* curMenu;

    for(int i=0;i<_project->problemsInterfaces().uniqueInterfaces().size();i++)
    {
        itf = _project->problemsInterfaces().uniqueInterfaces().at(i);
        problemTypes = itf->problemTypes();

        if(problemTypes.size()>1)
        {
            curMenu = new QMenu(itf->name(),_ui->menuProblems);
            _ui->menuProblems->addMenu(curMenu);
        }
        else
            curMenu = _ui->menuProblems;

        for(int j=0;j<itf->problemTypes().size();j++)
        {
            QAction *action = new QAction(itf->problemTypes().at(j),this);
            action->setData(itf->problemTypes().at(j));
            connect(action,SIGNAL(triggered()),this,SLOT(onPushedNewProblem()));

            curMenu->addAction(action);
        }
    }
}

void MainWindow::onPushedNewProblem()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QString problemType = action->data().toString();
    ProblemInterface *itf = _project->problemsInterfaces().value(problemType,NULL);
    if(itf)
    {
        bool pursue = true;

        WidgetSelectModModel* widgetSelect;

        QStringList modelsList;

        if(itf)
        {
            switch(itf->modelNeeds(problemType))
            {
            case ProblemInterface::NOMODEL:
                break;
            case ProblemInterface::ONEMODEL:
            case ProblemInterface::SEVERALMODELS:
                widgetSelect = new WidgetSelectModModel(_project->modItemsTree(),itf->modelNeeds(problemType),this);
                if(widgetSelect->exec()==QDialog::Accepted)
                {

                    QList<ModItem*> models = widgetSelect->_selectedModels;
                    for(int i=0;i<models.size();i++)
                        modelsList.push_back(models.at(i)->name());

                }
                else
                    pursue = false;
                delete widgetSelect;
                break;
            }
            if(pursue)
                _project->addNewProblem(itf,modelsList,problemType);
        }

    }
}

void MainWindow::onSelectedModItem(QModelIndex index)
{
    if(index.isValid())
    {
        ModItem* modClass = static_cast<ModItem*>(index.internalPointer());

        if(modClass)
            _project->setCurModItem(modClass);
    }
    else
        _project->setCurModItem(NULL);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
     _project->scriptParser()->stop();
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

    pathTries << QApplication::applicationDirPath().replace("\\", "/")+QDir::separator()+"../share/doc/omc/OpenModelicaUsersGuide.pdf";
    pathTries << OpenModelica::home().replace("\\", "/").append("/share/doc/omc/OpenModelicaUsersGuide.pdf");

    int iTry=0;
    bool openOk=false;
    while(iTry<pathTries.size()&&!openOk)
    {
        openOk = QDesktopServices::openUrl(QUrl(QString("file:///").append(pathTries.at(iTry))));
        iTry++;
    }
}

//! shows the progress bar contained inside the status bar
//! @see hideProgressBar()
void MainWindow::showStatusBar()
{
    _statusBar->setVisible(true);
}

//! hides the progress bar contained inside the status bar
//! @see showProgressBar()
void MainWindow::hideStatusBar()
{
    _statusBar->setVisible(false);
}

//! increases the value of the progress bar contained inside the status bar
void MainWindow::increaseStProgressBar()
{
    _stProgressBar->setValue(_stProgressBar->value()+1);
}


void MainWindow::setStatusBarText(QString text)
{
    _statusBar->setVisible(true);
    _statusBar->showMessage(text);
}

void MainWindow::eraseStatusBarText()
{
    _statusBar->hide();
    _statusBar->clearMessage();
}
