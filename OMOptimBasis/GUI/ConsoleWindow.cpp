/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

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
