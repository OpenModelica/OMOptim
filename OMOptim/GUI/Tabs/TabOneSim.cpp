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

     @file tabOneSim.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "TabOneSim.h"
#include "Plots/MOOptPlot.h"


TabOneSim::TabOneSim(OneSimulation *problem, QWidget *parent) :
MO2ColTab(problem->project()->name(),problem,false,parent)
{

    _project = dynamic_cast<Project*>(problem->project());
    _problem = problem;

    // Variables
    _widgetOneSimVars = new WidgetOneSimVars(_problem,true,this);
    addDockWidget("Variables",_widgetOneSimVars);

    _widgetFilesList = new WidgetFilesList(_problem->_filesToCopy,this);
    addDockWidget("Files",_widgetFilesList,_widgetOneSimVars);


    _widgetCtrl = new WidgetCtrlParameters(_project,_problem->_filesToCopy,_problem->model(),_problem->ctrls(),false,this);

    addDockWidget("Simulator",_widgetCtrl,_widgetOneSimVars);

    _widgetActions = new WidgetProblemActions(_project,_problem,false,NULL,this);
    addFixedWidget("Launch",_widgetActions,Qt::BottomDockWidgetArea,Qt::Vertical,false);


    mapDockWidgets.key(_widgetOneSimVars)->raise();
}

TabOneSim::~TabOneSim()
{

}
