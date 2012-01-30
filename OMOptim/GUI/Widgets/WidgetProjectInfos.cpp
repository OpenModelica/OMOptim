// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

        @file WidgetProjectInfos.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetProjectInfos.h"



namespace Ui
{
class WidgetProjectInfos_Class;
}

WidgetProjectInfos::WidgetProjectInfos(Project *project_,QList<QAction*> recentProjActions,QWidget *parent) :
    QWidget(parent), ui(new Ui::WidgetProjectInfos_Class)
{
    ui->setupUi(this);

    QPalette p = ui->textProject->palette();
    p.setColor(QPalette::Base,this->palette().color(QPalette::Background));
    ui->textProject->setPalette(p);


    project = project_;
    connect(ui->pushNewProject, SIGNAL(clicked()), this, SIGNAL(newProject()));
    connect(ui->pushLoadProject, SIGNAL(clicked()), this, SIGNAL(loadProject()));
    connect(ui->pushLoadPlugin,SIGNAL(clicked()),this,SIGNAL(loadPlugin()));
    //  connect(ui->pushOpenFolder,SIGNAL(clicked()),this,SLOT(openFolder()));

    //    ui->layoutRecents->setAlignment(Qt::AlignLeft);
    //    for(int i=0;i<recentProjActions.size();i++)
    //    {

    //        QWidget* test = new QWidget(this);
    //        test->addAction(recentProjActions.at(i));
    //        ui->layoutRecents->addWidget(test);
    //    }

    //    QToolBar* toolBar = new QToolBar(this);
    //    toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    //    toolBar->setOrientation(Qt::Vertical);
    //    toolBar->addActions(recentProjActions);
    //    toolBar->setStyleSheet("QToolBar { border: 0px }");
    //    ui->layoutRecents->addWidget(toolBar);

}

WidgetProjectInfos::~WidgetProjectInfos()
{
    delete ui;
}

void WidgetProjectInfos::actualizeGuiFromProject()
{

    // File names
    if(project->isDefined())
    {

        QString msg;
        msg = "<b>Project name</b> : " + project->name() +" <br> ";
        msg += "<b>Project file</b> : " + project->filePath() +" <br> ";
        msg += "<b>Project folder</b> : " + project->folder() + " <br> ";
        msg += "<b>ModelFiles</b> <br> ";

        QStringList listMO;
        for(int i=0;i<project->moFiles().size();i++)
        {
            listMO.push_back(project->moFiles().at(i));
        }
        msg += listMO.join("<br>");

        msg += "<b>Plugins</b> <br> ";
        QStringList listPlugins;
        QStringList pluginsNames(project->pluginsLoaded().keys());
        for(int i=0;i<pluginsNames.size();i++)
        {
            listPlugins.push_back(pluginsNames.at(i)+"\t"+project->pluginsLoaded().values().at(i));
        }
        msg += listPlugins.join("<br>");

        ui->textProject->setText(msg);
        ui->widgetInfos->show();
        ui->widgetBeginning->hide();
    }
    else
    {
        ui->widgetInfos->hide();
        ui->widgetBeginning->show();
    }
}

void WidgetProjectInfos::openFolder()
{
    QFileInfo fileInfo(project->filePath());
    LowTools::openFolder(fileInfo.absolutePath());
}
