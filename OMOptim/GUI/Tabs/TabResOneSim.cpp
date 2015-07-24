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

  @file tabResOneSim.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "TabResOneSim.h"
#include "Plots/MOOptPlot.h"


TabResOneSim::TabResOneSim(OneSimResult *result, QWidget *parent) :
    MO2ColTab(result->project()->name(),result,false,parent)
{
    this->setLocale(QLocale::C);

    _project = dynamic_cast<Project*>(result->project());
    _result = result;

    OneSimulation* problem = dynamic_cast<OneSimulation*>(_result->problem());

    // table widgets
  //  _inputTableVars = new WidgetTableVar(_result->inputVariables(),this);
    _finalTableVars = new WidgetTableOptVar(_result->finalVariables(),this);

 //   addDockWidget("Input Variables",_inputTableVars );
    addDockWidget("Final Variables",_finalTableVars,_inputTableVars);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(100);
    _finalTableVars->setSizePolicy(sizePolicy);
    mapDockWidgets.key(_finalTableVars)->setSizePolicy(sizePolicy);


//    _widgetCtrl = new WidgetCtrlParameters(_project,problem->model(),problem->ctrls(),true,this);
//    addDockWidget("Simulator",_widgetCtrl,_finalTableVars);


    mapDockWidgets.key(_finalTableVars)->raise();



    // scan
    if(_result->finalVariables()->nbScans()>1)
    {
        _widgetSelPointScan = new WidgetSelPointScan(_result->finalVariables(),this);
        addFixedWidget("Scans",_widgetSelPointScan,Qt::BottomDockWidgetArea,Qt::Vertical,false);
        _widgetSelPointScan->resize(_widgetSelPointScan->minimumSize());
        QSize test = _widgetSelPointScan->minimumSize();
        test = _widgetSelPointScan->size();
        test = mapDockWidgets.key(_widgetSelPointScan)->size();
        test = mapDockWidgets.key(_widgetSelPointScan)->minimumSize();
        mapDockWidgets.key(_widgetSelPointScan)->resize(mapDockWidgets.key(_widgetSelPointScan)->minimumSize());
    }
}

TabResOneSim::~TabResOneSim()
{
    //delete ui;
}
//
//void TabResOneSim::updateView(int newCurView)
//{
//    if (newCurView != curView)
//    {
//        curView = newCurView;
//        if (viewList[newCurView] == "Problem")
//        {
//
//            tabProblem->show();
//            problemToolBar->show();
//
//            tabComposites->hide();
//            compositesToolBar->hide();
//
//            tabVariables->hide();
//            variablesToolBar->hide();
//
//
//        }
//
//        if (viewList[newCurView] == "Variables")
//        {
//
//            tabVariables->show();
//            variablesToolBar->show();
//
//            tabProblem->hide();
//            problemToolBar->hide();
//
//            tabComposites->hide();
//            compositesToolBar->hide();
//        }
//        if (viewList[newCurView] == "Composite curves")
//        {
//            tabVariables->hide();
//            variablesToolBar->hide();
//
//            tabComposites->show();
//            compositesToolBar->show();
//
//            tabProblem->hide();
//            problemToolBar->hide();
//        }
//    }
//}
//
//void TabResOneSim::actualizeGui()
//{
//
//}
