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

  @file WidgetOptimActions.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
  @version
*/


#include "Widgets/WidgetOptimActions.h"
#include "Tools/GuiTools.h"
#include "Tools/MOStyleSheet.h"

namespace Ui
{
class WidgetOptimActionsClass;
}

WidgetOptimActions::WidgetOptimActions(ProjectBase* project,Optimization *problem,bool isResult,Result* result,QWidget *parent)
    : QWidget(parent), _ui(new Ui::WidgetOptimActionsClass)
{
    _ui->setupUi(this);
    _ui->frame->setStyleSheet(MOStyleSheet::launchBarStyleSheet());


    _problem = problem;
    _result = result;
    _project = project;
    _isResult = isResult;

    connect(_ui->pushLaunch,SIGNAL(clicked()),this,SLOT(launch()));
    connect(_ui->pushRestore,SIGNAL(clicked()),this,SLOT(restoreProblem()));

    actualizeGui();
}

WidgetOptimActions::~WidgetOptimActions()
{
    delete _ui;
}

void WidgetOptimActions::launch()
{
    _project->launchProblem(_problem);
}


void WidgetOptimActions::actualizeGui()
{

    // list of widgets to hide when problem is solved
    QWidgetList unsolvedWidgets;
    unsolvedWidgets << _ui->pushLaunch;

    // list of widgets to hide when problem is unsolved
    QWidgetList solvedWidgets;
    solvedWidgets << _ui->pushRestore  ;

    // if problem is solved
    if(_isResult)
    {
  for(int i=0; i < unsolvedWidgets.size(); i++)
      unsolvedWidgets.at(i)->hide();

  for(int i=0; i < solvedWidgets.size(); i++)
      solvedWidgets.at(i)->show();
    }
    else
    {
  for(int i=0; i < unsolvedWidgets.size(); i++)
      unsolvedWidgets.at(i)->show();

  for(int i=0; i < solvedWidgets.size(); i++)
      solvedWidgets.at(i)->hide();
    }
}

void WidgetOptimActions::restoreProblem()
{
    if(_isResult)
  _project->restoreProblemFromResult(_result);
}

void WidgetOptimActions::onShowModelAsked()
{

}

QPushButton*  WidgetOptimActions::addButton(QString text,bool before)
{
    QPushButton* pushButton = new QPushButton(text,this);
    pushButton->setMinimumHeight(_ui->pushLaunch->minimumHeight());
    QHBoxLayout* layout = dynamic_cast<QHBoxLayout*>(_ui->frame->layout());
    if(layout && before)
    {
  layout->insertWidget(1,pushButton);
    }
    else
  _ui->frame->layout()->addWidget(pushButton);
    return pushButton;
}
