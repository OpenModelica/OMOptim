
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

        @file TabEIHEN1Result.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9
*/

#include "tabEIHEN1Result.h"
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"
#include "SimpleMilpTarget.h"


TabEIHEN1Result::TabEIHEN1Result(Project *project,EIHEN1Result *result, QWidget *parent) :
MO2ColTab(project->name(),result,false,parent)
{
        _project = project;
        _result  = result;



        // create new EIMER and launch it
        _problemMER = new EIMER(project,_project->modClassTree(),_project->moomc());
        _problemMER->setEITree(*_result->eiTree());
        _problemMER->parameters()->setValue(EIMERParameters::INCLUDEUTILITIES,true);
         _merResult = dynamic_cast<EIMERResult*>(_problemMER->launch(ProblemConfig()));

          EIHEN1 *problem = dynamic_cast<EIHEN1*>(_result->problem());
        // Variables
            MOOptVector* variables=NULL;
            if(problem)
                variables = problem->inputVars();

        _widgetTreeStreams = new WidgetTreeStreams(_result->eiTree(),true,true,
                                                  _project->modClassTree(),_project->moomc(),this,variables);


        _widgetCCPlot = new WidgetCCPlot(_merResult,this);
        //_widgetSelPointScan = new WidgetSelPointScan(variables,this);
        _widgetTableConnConstr = new WidgetTableConnConstr(
                problem->connConstrs(),
                _result ->eiTree(),
                true,
                this);
        _widgetTableEIGroups = new WidgetTableEIGroups(_result->eiTree()->rootElement(),false,this);
        _widgetEIConns = new WidgetEIConns(_result->eiConns(),this);
        _widgetEIHEN1Result = new WidgetEIHEN1Result(_result,this);

        addDockWidget("EI items",_widgetTreeStreams);
//        addDockWidget("Points and Scans",_widgetSelPointScan,_widgetTreeStreams);
        addDockWidget("EI Groups",_widgetTableEIGroups,_widgetTreeStreams);
        addDockWidget("Composites",_widgetCCPlot,_widgetTreeStreams);
        addDockWidget("Connections constraints",_widgetTableConnConstr,_widgetTreeStreams);
        addDockWidget("Connections",_widgetEIConns,_widgetTreeStreams);
        addFixedWidget("Result",_widgetEIHEN1Result,Qt::BottomDockWidgetArea);

//        connect(_problemTarget,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));

        mapDockWidgets.key(_widgetCCPlot)->raise();


//        updateSelPointScan();
}

TabEIHEN1Result::~TabEIHEN1Result()
{

}

void TabEIHEN1Result::actualize()
{
//        _widgetTreeStreams->refreshTree();
//        _widgetCCPlot->actualizeGui();
}

void TabEIHEN1Result::updateSelPointScan()
{
//        bool show = _problemTarget->inputVars()->getUsePoints()||_problemTarget->inputVars()->getUseScan();
//        setWidgetVisible(_widgetSelPointScan,show);
}

void TabEIHEN1Result::onInputVarsModified()
{
//        _widgetTreeStreams->setInputVars(_problemTarget->inputVars());
//        updateSelPointScan();
}

//void TabEIHEN1Result::onTargetAsked()
//{
//        QString tempDir = _project->tempPath();
//        ProblemConfig cfg;
//        cfg.tempDir = tempDir;
//        _problemTarget->launch(cfg);
//}

//void TabEIHEN1Result::onMERAsked(bool includeUtilities)
//{
//    // create and launch problem
//    QString tempDir = _project->tempPath();
//    ProblemConfig cfg;
//    cfg.tempDir = tempDir;
//    _problemMER->setRootEI(_problemTarget->rootEI()->clone());
//    _problemMER->setIncludeUtilities(includeUtilities);
//    _problemMER->launch(cfg);

//    // set result to composites
//    _widgetCCPlot->setMERResult(_problemMER->result());

//    //change view to show composites
//    mapDockWidgets.key(_widgetCCPlot)->show();
//}
