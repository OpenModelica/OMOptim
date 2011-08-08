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

        @file tabEITargetResult.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9
*/

#include "tabEITargetResult.h"
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"
#include "SimpleMilpTarget.h"


TabEITargetResult::TabEITargetResult(Project *project,EITargetResult  *targetResult, QWidget *parent) :
        MO2ColTab(project->name(),targetResult,false,parent)
{
    _project = project;
    _targetResult = targetResult;

    // create new EIMER and launch it
    _problemMER = new EIMER(project,_project->modClassTree(),_project->moomc());
    _problemMER->setEITree(*_targetResult->eiTree());
    _merResult = dynamic_cast<EIMERResult*>(_problemMER->launch(ProblemConfig()));

    // Variables

    _widgetTreeStreams = new WidgetTreeStreams(_targetResult->eiTree(),true,false,
                                               _project->modClassTree(),_project->moomc(),this,((EIProblem*)_targetResult->problem())->inputVars());


    _widgetCCPlot = new WidgetCCPlot(_merResult,this);
    //_widgetSelPointScan = new WidgetSelPointScan(((EIProblem*)_targetResult->problem())->inputVars(),this);
    _widgetTableConnConstr = new WidgetTableConnConstr(
            ((EIProblem*)_targetResult->problem())->connConstrs(),
            _targetResult->eiTree(),
            true,
            this);
    _widgetTableEIGroups = new WidgetTableEIGroups(_targetResult->eiTree()->rootElement(),false,this);
    _widgetEIConns = new WidgetEIConns(_targetResult->eiConns(),this);
    _widgetEITargetResult = new WidgetEITargetResult(_targetResult,this);

    addDockWidget("EI items",_widgetTreeStreams);
    //addDockWidget("Points and Scans",_widgetSelPointScan,_widgetTreeStreams);
    addDockWidget("EI Groups",_widgetTableEIGroups,_widgetTreeStreams);
    addDockWidget("Composites",_widgetCCPlot,_widgetTreeStreams);
    addDockWidget("Connections constraints",_widgetTableConnConstr,_widgetTreeStreams);
    addDockWidget("Connections",_widgetEIConns,_widgetTreeStreams);
    addFixedWidget("Result",_widgetEITargetResult,Qt::BottomDockWidgetArea);

    //connect(_problemTarget,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));

    mapDockWidgets.key(_widgetCCPlot)->raise();
    _widgetCCPlot->raise();

    updateSelPointScan();

    readGUIState();
}

TabEITargetResult::~TabEITargetResult()
{

}

void TabEITargetResult::actualize()
{
    _widgetTreeStreams->refreshTree();
    _widgetCCPlot->actualizeGui();
}

void TabEITargetResult::updateSelPointScan()
{
    bool show = ((EIProblem*)_targetResult->problem())->inputVars()->getUsePoints()||((EIProblem*)_targetResult->problem())->inputVars()->getUseScan();
    setWidgetVisible(_widgetSelPointScan,show);
}

