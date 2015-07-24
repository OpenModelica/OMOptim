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

     @file tabOptimization.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "TabOptimization.h"

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QSpinBox>
#include <QtCore/QObject>
#include <QtCore/QObjectList>
#include <QtGui/QMessageBox>
#endif

#include "Optimization.h"
#include "Project.h"
#include "Widgets/WidgetOptParameters.h"
#include "Widgets/WidgetSelectOptVars.h"
//#include "Widgets/WidgetSelectComponents.h"
#include "Widgets/WidgetFilesList.h"
#include "Widgets/WidgetProblemActions.h"
#include "Widgets/WidgetCtrlParameters.h"
#include "Widgets/WidgetModelsList.h"
//#include "BlockSubstituteConnDlg.h"
#include "Tools/GuiTools.h"


TabOptimization::TabOptimization(Optimization *problem, QWidget *parent) :
    MO2ColTab(problem->project()->name(),problem,false,parent)
{
    _project = dynamic_cast<Project*>(problem->project());
    _problem = problem;

    _widgetSelectOptVars = new WidgetSelectOptVars(_problem,true,this);
    addDockWidget("Variables",_widgetSelectOptVars);

    //#ifdef USEBLOCKSUB
    //        _widgetSelectComponents = new WidgetSelectComponents(_project,_problem,false,this);
    //        addDockWidget("Components",_widgetSelectComponents,_widgetSelectOptVars);
    //#endif

    _widgetOptParameters = new WidgetOptParameters(_project,_problem,false,this);
    addDockWidget("Parameters",_widgetOptParameters,_widgetSelectOptVars);

    _widgetFilesList = new WidgetFilesList(_problem->_filesToCopy,this);
    _widgetFilesList->setInfos("Select here files needed for simulation to perform. \nThose would be copied in temporary directory along with simulation executable.");
    addDockWidget("Files",_widgetFilesList,_widgetSelectOptVars);


    _widgetCtrl = new WidgetCtrlParameters(_project,_problem->_filesToCopy,_problem->ctrls(),false,this);

    addDockWidget("Simulator",_widgetCtrl,_widgetSelectOptVars);

    _widgetModels = new WidgetModelsList(_project,_problem,this);
    addDockWidget("Models",_widgetModels,_widgetSelectOptVars);

    _widgetOptimActions = new WidgetProblemActions(_project,_problem,false,NULL,this);
    addFixedWidget("Launch",_widgetOptimActions,Qt::BottomDockWidgetArea,Qt::Vertical,false);



    actualizeGui();


    // connect fow widget ctrls to be updated
    connect(_problem,SIGNAL(removedModel(QString)),this,SLOT(onModelsChanged()));
    connect(_problem,SIGNAL(addedModel(QString)),this,SLOT(onModelsChanged()));

    //change view to show variables
    mapDockWidgets.key(_widgetSelectOptVars)->raise();
}

TabOptimization::~TabOptimization()
{

}


void TabOptimization::actualizeGui()
{
    _widgetOptParameters->actualizeGui();
    _widgetSelectOptVars->actualizeGui();

    //#ifdef USEBLOCKSUB
    //        _widgetSelectComponents->actualizeGui();
    //#endif
}

void TabOptimization::onModelsChanged()
{
    _widgetCtrl->update(_problem->ctrls());
    _widgetModels->refreshList();
}


