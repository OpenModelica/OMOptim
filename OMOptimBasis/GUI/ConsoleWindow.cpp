// $Id: ConsoleWindow.cpp 16221 2013-06-05 08:47:46Z hubert.thieriot $
/**
  @file ConsoleWindow.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include <iostream>

#include "ConsoleWindow.h"
#include "MOThreads.h"
#include "ListInfo.h"
#include "GuiTools.h"
#include "scriptparser.h"
#include "Dialogs/HelpDlg.h"
#include "ScriptTextDlg.h"

namespace Ui
{
class ConsoleWindowClass;
}


ConsoleWindow::ConsoleWindow(ProjectBase* project,QStringList commands,QWidget *parent)
    : QMainWindow(parent), _ui(new Ui::ConsoleWindowClass)
{

    this->resize(800, 600);
    this->setStyleSheet("QMainWindow::separator {height:0;width:0}");

    //************************
    // Basic initialization
    //************************
    _ui->setupUi(this);
    _project = project;
    _commands = commands;

    actualizeGuiFromProject();



    initLog();
}

ConsoleWindow::~ConsoleWindow()
{
    delete _ui;
    delete _logProxy;
}


void ConsoleWindow::initLog()
{
    // text log
    _logProxy = new QSortFilterProxyModel();
    _logProxy->setSourceModel(InfoSender::instance()->infosNormal());
    _ui->listLog->setModel(_logProxy);
    //_ui->listDebug->setModel(InfoSender::instance()->infosDebug());
    //_ui->listOM->setModel(InfoSender::instance()->infosOM());
    _ui->listLog->setColumnWidth(0,60);
    //_ui->listDebug->setColumnWidth(0,60);
    //_ui->listOM->setColumnWidth(0,60);

    _ui->listScript->setModel(InfoSender::instance()->infosScript());
    _ui->listScript->setColumnWidth(0,60);

//    // log buttons
//    connect(_ui->pushDispWarning,SIGNAL(toggled(bool)),this,SLOT(dispOnlyWarnings(bool)));
//    connect(_ui->pushStayInLine,SIGNAL(toggled(bool)),this,SLOT(stayInLine(bool)));
//    _ui->pushStayInLine->toggle();

}

//void ConsoleWindow::displayInfo(const Info & i)
//{
//    if(i.infoType==ListInfo::TASK)
//    {
//        setStatusBarText(i.infoMsg);
//    }
//}


void ConsoleWindow::actualizeGuiFromProject()
{
    QString windowTitle = "OMOptim - "+_project->name();
    if(!_project->isSaved()&& _project->isDefined())
        windowTitle.append("*");
    this->setWindowTitle(windowTitle);
}

int ConsoleWindow::launch()
{
    connect(this,SIGNAL(destroyed()),_project->scriptParser(),SLOT(stop()));

    bool scriptOk = _project->scriptParser()->executeCommands(_commands);
    if(scriptOk)
        _result = 0;
    else
        _result = -1;

    return _result;
}

void ConsoleWindow::closeEvent(QCloseEvent *event)
{
    qDebug("ConsoleWindow close event");
   _project->scriptParser()->stop();
   QMainWindow::closeEvent(event);
}
