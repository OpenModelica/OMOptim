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


#include "widgetloadexemodel.h"
#include "ui_widgetloadexemodel.h"

WidgetLoadExeModel::WidgetLoadExeModel(Project* project,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WidgetLoadExeModel)
{
    ui->setupUi(this);
    _project = project;

    connect(ui->pushOpenExe,SIGNAL(clicked()),this,SLOT(openExe()));
    connect(ui->pushOpenInput,SIGNAL(clicked()),this,SLOT(openInput()));
    connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(validate()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

WidgetLoadExeModel::~WidgetLoadExeModel()
{
    delete ui;
}

QString WidgetLoadExeModel::name()
{
    return ui->lineModelName->text();
}

QFileInfo WidgetLoadExeModel::exeFileInfo()
{
    return QFileInfo(ui->lineExe->text());
}

QFileInfo WidgetLoadExeModel::inputFileInfo()
{
    return QFileInfo(ui->lineInput->text());
}


void WidgetLoadExeModel::openExe()
{
    // get starting dir
    QDir startDir = QFileInfo(ui->lineExe->text()).absoluteDir();

    QString filename = QFileDialog::getOpenFileName(this,"Executable",startDir.absolutePath());
    if(!filename.isEmpty())
        ui->lineExe->setText(filename);
}

void WidgetLoadExeModel::openInput()
{
    // get starting dir
    QDir startDir;
    if(!ui->lineInput->text().isEmpty())
        startDir.setPath(QFileInfo(ui->lineInput->text()).absolutePath());
    else
        startDir.setPath(QFileInfo(ui->lineExe->text()).absolutePath());

    QString filename = QFileDialog::getOpenFileName(this,"Input file",startDir.absolutePath());
    if(!filename.isEmpty())
        ui->lineInput->setText(filename);
}

void WidgetLoadExeModel::validate()
{
    bool ok = true;
    QString msg;
    // check
    if(!exeFileInfo().exists())
    {
        ok = false;
        msg+= "Exe file does not exist. \n";
    }

    if(!inputFileInfo().exists())
    {
        ok = false;
        msg+= "Input file does not exist. \n";
    }

    if(name().isEmpty())
    {
        ok = false;
        msg+= "Model name is empty. \n";
    }
    if(_project->findModItem(name()))
    {
        ok = false;
        msg+= "A similar named model already exists in project. \n";
    }
    if(ok)
        accept();
    else
    {
        QMessageBox msgBox;
        msgBox.setText(msg);
        msgBox.exec();
    }
}

