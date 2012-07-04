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
#include "LowTools.h"

namespace Ui
{
class WidgetProjectInfos_Class;
}

WidgetProjectInfos::WidgetProjectInfos(Project *project,QList<QAction*> recentProjActions,QWidget *parent) :
    QWidget(parent), _ui(new Ui::WidgetProjectInfos_Class)
{
    _ui->setupUi(this);

    _myTextBrowser = new MyTextLog(this);
    QPalette p = _myTextBrowser->palette();
    p.setColor(QPalette::Base,this->palette().color(QPalette::Background));
    _myTextBrowser->setPalette(p);

    _ui->widgetInfos->layout()->addWidget(_myTextBrowser);




    _project = project;
    connect(_ui->pushNewProject, SIGNAL(clicked()), this, SIGNAL(newProject()));
    connect(_ui->pushLoadProject, SIGNAL(clicked()), this, SIGNAL(loadProject()));

    connect(_project,SIGNAL(infosChanged()),this,SLOT(actualizeGuiFromProject()));


}

WidgetProjectInfos::~WidgetProjectInfos()
{
    delete _ui;
}

void WidgetProjectInfos::actualizeGuiFromProject()
{

    // File names
    if(_project->isDefined())
    {

        QString msg;
        msg = "<b>Project name</b> : " + _project->name() +" <br> ";
        msg += "<b>Project file</b> : <A href=\"file:///"+ _project->filePath() +"\">"+_project->filePath()+"</A>" +" <br> ";
        msg += "<b>Project folder</b> :  <A href=\"file:///"+ _project->folder().absolutePath() +"\">"+_project->folder().absolutePath()+"</A>" + " <br> ";
        msg += "<b>ModelFiles</b> <br> ";

        QStringList listMO;
        for(int i=0;i<_project->moFiles().size();i++)
        {
            listMO.push_back(_project->moFiles().at(i).absoluteFilePath());
        }
        msg += listMO.join("<br>")+"<br>";

        msg += "<b>Plugins</b> <br> ";
        QStringList listPlugins;
        QStringList pluginsNames(_project->pluginsLoaded().keys());
        for(int i=0;i<pluginsNames.size();i++)
        {
            listPlugins.push_back(pluginsNames.at(i)+"\t"+_project->pluginsLoaded().values().at(i));
        }
        msg += listPlugins.join("<br>");

        _myTextBrowser->setText(msg);
        _myTextBrowser->setOpenExternalLinks(true);
        _myTextBrowser->setOpenLinks(false);
        _ui->widgetInfos->show();
        _ui->widgetBeginning->hide();
    }
    else
    {
        _ui->widgetInfos->hide();
        _ui->widgetBeginning->show();
    }
}

void WidgetProjectInfos::openFolder()
{
    QFileInfo fileInfo(_project->filePath());
    LowTools::openFolder(fileInfo.absolutePath());
}
