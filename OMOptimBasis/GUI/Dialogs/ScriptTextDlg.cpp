// $Id: ScriptTextDlg.cpp 10834 2011-12-26 12:23:29Z hubert.thieriot $
 /**
   @file ScriptTextDlg.cpp
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

#include "ScriptTextDlg.h"
#include "ui_ScriptTextDlg.h"
#include "Dialogs/HelpDlg.h"
#include "ProjectBase.h"

ScriptTextDlg::ScriptTextDlg(ScriptParser* parser,ProjectBase* project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptTextDlgClass)
{
    ui->setupUi(this);
    _parser = parser;
    _project = project;

    ui->textHelp->setText(_parser->helpText());

 //   connect(ui->pushHelp,SIGNAL(clicked()),this,SLOT(help()));
    connect(ui->pushLaunch,SIGNAL(clicked()),this,SLOT(launch()));
    connect(ui->pushCancel,SIGNAL(clicked()),this,SLOT(reject()));
}

ScriptTextDlg::~ScriptTextDlg()
{
    delete ui;
}

void ScriptTextDlg::launch()
{
    accept();
    _project->launchScript(ui->textEdit->toPlainText());
}

void ScriptTextDlg::help()
{
    TextDlg dlg(_parser->helpText(),"Script functions",this);
    dlg.exec();
}
