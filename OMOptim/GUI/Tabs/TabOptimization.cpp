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
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QSpinBox>
#include <QtCore/QObject>
#include <QtCore/QObjectList>
#include <QtGui/QMessageBox>


#include "BlockSubstituteConnDlg.h"
#include "GuiTools.h"


TabOptimization::TabOptimization(Project *project_,Optimization *problem_, QWidget *parent) :
MO2ColTab(project_->name(),problem_,false,parent)
{
	project = project_;
	problem = problem_;

	
        widgetSelectOptVars = new WidgetSelectOptVars(problem,true,this);
	addDockWidget("Variables",widgetSelectOptVars);

#ifdef USEBLOCKSUB
        widgetSelectComponents = new WidgetSelectComponents(project,problem,false,this);
	addDockWidget("Components",widgetSelectComponents,widgetSelectOptVars);
#endif

        widgetOptParameters = new WidgetOptParameters(project,problem,false,this);
	addDockWidget("Optimization",widgetOptParameters,widgetSelectOptVars);
	
        widgetFilesList = new WidgetFilesList(problem->_filesToCopy,this);
        addDockWidget("Files",widgetFilesList,widgetSelectOptVars);

        widgetOptimActions = new WidgetOptimActions(project,problem,false,NULL,this);
        addFixedWidget("Launch",widgetOptimActions,Qt::BottomDockWidgetArea,Qt::Vertical,false);

	actualizeGui();

        //change view to show variables
        mapDockWidgets.key(widgetSelectOptVars)->raise();
}

TabOptimization::~TabOptimization()
{

}


void TabOptimization::actualizeGui()
{
	widgetOptParameters->actualizeGui();
	widgetSelectOptVars->actualizeGui();

#ifdef USEBLOCKSUB
	widgetSelectComponents->actualizeGui();
#endif
}


