// $Id: ConsoleDlg.cpp 10834 2011-12-26 12:23:29Z hubert.thieriot $
/**
   @file ConsoleDlg.cpp
   @brief Comments for file documentation.
   @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
   Company : CEP - ARMINES (France)
   http://www-cep.ensmp.fr/english/
   @version

    Main contributor 2010, Hubert Thierot, CEP - ARMINES, Paris, France
  Main contributor 2009, Hubert Thierot, CEP - ARMINES, Paris, France

    Copyright:   Copyright (C) 2010 CEP - ARMINES. All rights reserved.

  This file is part of OpenModelica.

  Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
  c/o Linköpings universitet, Department of Computer and Information Science,
  SE-58183 Linköping, Sweden.
  All rights reserved.

  THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
  THIS OSMC PUBLIC LICENSE (OSMC-PL).
  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
  OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.

  The OpenModelica software and the Open Source Modelica
  Consortium (OSMC) Public License (OSMC-PL) are obtained
  from OSMC, either from the above address,
  from the URLs: http://www.ida.liu.se/projects/OpenModelica or
  http://www.openmodelica.org, and in the OpenModelica distribution.
  GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.

  This program is distributed WITHOUT ANY WARRANTY; without
  even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
  IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.

  See the full OSMC Public License conditions for more details.

  For more information about the Qt-library visit TrollTech's webpage
  regarding the Qt licence: http://www.trolltech.com/products/qt/licensing.html

**/

#include "ConsoleDlg.h"
#include "ui_ConsoleDlg.h"
#include <QtGui/QErrorMessage>


#include <QtGui/QListWidget>
#include "Dialogs/HelpDlg.h"
#include "ProjectBase.h"
#include "InfoSender.h"
#include "GuiTools.h"


ConsoleDlg::ConsoleDlg(QApplication* application, ProjectBase* project,QStringList commands,QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConsoleDlgClass)
{
    ui->setupUi(this);
    _project = project;
    _commands = commands;

    _result = -1;

    connect(this,SIGNAL(finished(int)),application,SLOT(quit()));

    initLogStreams();
}

ConsoleDlg::~ConsoleDlg()
{
    delete ui;
}

QTextBrowser* ConsoleDlg::logTextEdit()
{
    return ui->textLog;
}

QTextBrowser* ConsoleDlg::scriptTextEdit()
{
    return ui->textScript;
}

void ConsoleDlg::launch()
{
    bool scriptOk = _project->scriptParser()->executeCommands(_commands);
    if(scriptOk)
        _result = 0;
    else
        _result = -1;

}

void ConsoleDlg::initLogStreams()
{
    TextEditIoDevice* logIoDevice = new TextEditIoDevice(this->logTextEdit(),this);
    QTextStream* logStream = new QTextStream(logIoDevice);

    TextEditIoDevice* scriptIoDevice = new TextEditIoDevice(this->scriptTextEdit(),this);
    QTextStream* scriptStream = new QTextStream(scriptIoDevice);

    // set output to terminal
    QList<ListInfo::InfoType> logInfoTypes;
    logInfoTypes  << ListInfo::NORMAL2 << ListInfo::WARNING2<<
                     ListInfo::ERROR2<< ListInfo::TASK;
    InfoSender::instance()->setLogStream(logStream,logInfoTypes);

    logInfoTypes.clear();
    logInfoTypes  << ListInfo::SCRIPT;
    InfoSender::instance()->setLogStream(scriptStream,logInfoTypes);

}

void ConsoleDlg::onQuitAsked()
{
    this->done(_result);
}



