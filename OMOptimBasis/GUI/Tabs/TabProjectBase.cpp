// $Id: TabProjectBase.cpp 11039 2012-02-07 12:47:38Z hubert.thieriot $
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

    @file TabProjectBase.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version
*/

#include "TabProjectBase.h"

namespace Ui
{
class TabProjectBase_Class;
}

TabProjectBase::TabProjectBase(ProjectBase *project_, QList<QAction*> recentProjActions,QWidget *parent) :
    MOTabSimple(project_->name(),project_),ui(new Ui::TabProjectBase_Class)
{
    ui->setupUi(this);
    this->setParent(parent);

    project = project_;
    // View widgets
    widgetInfos = new WidgetProjectBaseInfos(project,recentProjActions,this);
    ui->myLayout->addWidget(widgetInfos);

    widgetInfos->show();

    connect(widgetInfos,SIGNAL(newProject()),this,SIGNAL(newProject()));
    connect(widgetInfos,SIGNAL(loadProject()),this,SIGNAL(loadProject()));

}

TabProjectBase::~TabProjectBase()
{
    delete ui;
}


void TabProjectBase::actualizeGuiFromProject()
{
    widgetInfos->actualizeGuiFromProject();
}
