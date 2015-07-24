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

     @file tabResOptimization.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "TabResOptimization.h"
#include "TabOptimization.h"

#include "ui_TabResOptimization.h"

#include "Project.h"
#include "Plots/MOOptPlot.h"
#include "OptimResult.h"
#include "Tabs/MO2ColTab.h"
//#include "Widgets/WidgetBlocks.h"
#include "Widgets/WidgetMooPlot.h"
#include "Widgets/WidgetMooPointsList.h"
#include "Widgets/WidgetOptTable.h"
#include "Widgets/WidgetTableRecVar.h"
#include "Widgets/WidgetToolBar.h"
#include "Widgets/WidgetCalculateMooPoints.h"
#include "Widgets/WidgetProblemActions.h"
#include "Widgets/WidgetResultInfos.h"
#include "Widgets/WidgetCtrlParameters.h"
#include "Widgets/WidgetOptParameters.h"



TabResOptimization::TabResOptimization(OptimResult *result, QWidget *parent) :
MO2ColTab(result->project()->name(),result,false,parent)
{
//        QMainWindow::setDockNestingEnabled(true);
//        QMainWindow::setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
        _project = dynamic_cast<Project*>(result->project());
        _result = result;
        _problem = dynamic_cast<Optimization*>(_result->problem());

        _widgetMooPointsList = new WidgetMooPointsList(_result,this);
        _widgetMooPlot = new WidgetMooPlot(_result,this);
        _widgetTableRecVar = new WidgetTableRecVar(_result,this);

        addDockWidget("Plot",_widgetMooPlot,NULL,Qt::RightDockWidgetArea);
//#ifdef USEBLOCKSUB
//        _widgetBlocks = new WidgetBlocks(_project,_result,this);
//        if(_problem->blockSubstitutions()->size()>0)
//            addDockWidget("Blocks",_widgetBlocks,_widgetMooPlot,Qt::RightDockWidgetArea);
//#endif
        addDockWidget("Variables",_widgetTableRecVar,_widgetMooPlot,Qt::RightDockWidgetArea);

        addFixedWidget("Points",_widgetMooPointsList,Qt::LeftDockWidgetArea,Qt::Horizontal,false);


        _widgetOptimActions = new WidgetProblemActions(_project,_problem,true,_result,this);
        addFixedWidget("Launch",_widgetOptimActions,Qt::BottomDockWidgetArea,Qt::Vertical,false);


        //_widgetCtrl = new WidgetCtrlParameters(_project,_problem->ctrls(),true,this);
        //addDockWidget("Simulator",_widgetCtrl,_widgetTableRecVar);


        _widgetOptParameters = new WidgetOptParameters(_project,_problem,true,this);
        addDockWidget("Parameters",_widgetOptParameters,_widgetTableRecVar);

        _widgetInfos = new WidgetResultInfos(_result,this);
        addDockWidget("Infos",_widgetInfos,_widgetOptParameters);


        // connect signals for selection changed
        connect(_widgetMooPlot,SIGNAL(selectionChanged(QList<int> &)),
                _widgetMooPointsList,SLOT(onExtSelectionChanged(QList<int>&)));
        connect(_widgetMooPlot,SIGNAL(selectionChanged(QList<int>&)),
                _widgetTableRecVar,SLOT(onExtSelectionChanged(QList<int>&)));

        connect(_widgetMooPointsList,SIGNAL(selectionChanged(QList<int>&)),
                _widgetTableRecVar,SLOT(onExtSelectionChanged(QList<int>&)));
        connect(_widgetMooPointsList,SIGNAL(selectionChanged(QList<int>&)),
                _widgetMooPlot,SLOT(onExtSelectionChanged(QList<int>&)));

        // signal for recomputation
        connect(_widgetMooPointsList,SIGNAL(pointsRecomputed()),
                _widgetTableRecVar,SLOT(onPointsRecomputed()));


        // connect signals for shown points changed
        connect(_widgetMooPointsList,SIGNAL(shownPointsChanged(QList<int>&)),
                _widgetMooPlot,SLOT(onExtShownPointsChanged(QList<int>&)));

        // connect signals for cur scan changed
        connect(_result,SIGNAL(curScanChanged(int)),_widgetTableRecVar,SLOT(onCurScanChanged(int )));

        // refresh shown points
         _widgetMooPointsList->setOnlyPareto(_widgetMooPointsList->_ui->pushPareto->isChecked());

        // raise pareto
         _widgetMooPlot->raise();
         mapDockWidgets.key(_widgetMooPlot)->raise();

         // restore position
     //    readGUIState();

//         // select first point
//         if(result->nbPoints()>0)
//             _widgetMooPointsList->_listPoints->setSelectedIndexes(QList<int>()<<0);

}

TabResOptimization::~TabResOptimization()
{
}


