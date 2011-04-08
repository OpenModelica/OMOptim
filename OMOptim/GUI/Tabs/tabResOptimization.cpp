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
 	@version 0.9 
*/

#include "tabResOptimization.h"
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"


namespace Ui
{
	class TabResOptimizationClass;
}

TabResOptimization::TabResOptimization(Project *project_,Optimization *problem_, QWidget *parent) :
MO2ColTab(project_->name(),problem_,false,parent)
{
	type = TABSOLVEDPROBLEM;
        QMainWindow::setDockNestingEnabled(true);
        QMainWindow::setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
	project = project_;
	problem = problem_;
	result = dynamic_cast<OptimResult*>(problem->result());


        widgetMooPointsList = new WidgetMooPointsList(result,this);
        widgetMooPlot = new WidgetMooPlot(result,this);
        widgetTableRecVar = new WidgetTableRecVar(result,this);
        widgetCalculateMooPoints = new WidgetCalculateMooPoints(result,widgetMooPointsList,this);

        // set size policy
        widgetMooPointsList->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
        widgetMooPlot->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
        widgetTableRecVar->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
        widgetCalculateMooPoints->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum);


        addDockWidget("Plot",widgetMooPlot,NULL,Qt::RightDockWidgetArea);
#ifdef USEBLOCKSUB
        addDockWidget("Blocks",new WidgetBlocks(project,result,this),widgetMooPlot,Qt::RightDockWidgetArea);
#endif
        addDockWidget("Recomp. vars",widgetTableRecVar,widgetMooPlot,Qt::RightDockWidgetArea);
        addFixedWidget("Recompute",widgetCalculateMooPoints,Qt::BottomDockWidgetArea);
        addFixedWidget("Points",widgetMooPointsList,Qt::LeftDockWidgetArea,Qt::Horizontal,false);
	

        // connect signals for selection changed
	connect(widgetMooPlot,SIGNAL(selectionChanged(QList<int> &)),
                widgetMooPointsList,SLOT(onExtSelectionChanged(QList<int>&)));
        connect(widgetMooPlot,SIGNAL(selectionChanged(QList<int>&)),
                widgetTableRecVar,SLOT(onExtSelectionChanged(QList<int>&)));

        connect(widgetMooPointsList,SIGNAL(selectionChanged(QList<int>&)),
                widgetTableRecVar,SLOT(onExtSelectionChanged(QList<int>&)));
        connect(widgetMooPointsList,SIGNAL(selectionChanged(QList<int>&)),
                widgetMooPlot,SLOT(onExtSelectionChanged(QList<int>&)));


        // connect signals for shown points changed
        connect(widgetMooPointsList,SIGNAL(shownPointsChanged(QList<int>&)),
                widgetMooPlot,SLOT(onExtShownPointsChanged(QList<int>&)));

        // connect signals for cur scan changed
	connect(result,SIGNAL(curScanChanged(int &)),widgetTableRecVar,SLOT(onCurScanChanged(int &)));

        // refresh shown points
         widgetMooPointsList->setOnlyPareto(widgetMooPointsList->_ui->pushPareto->isChecked());

         // restore position
         readGUIState();
}

TabResOptimization::~TabResOptimization()
{
    int a=2;
}


