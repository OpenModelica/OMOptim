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
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"


TabResOneSim::TabResOneSim(Project *project,OneSimResult *result, QWidget *parent) :
MO2ColTab(project->name(),result,false,parent)
{
        _project = project;
        _result = result;

        OneSimulation* problem = dynamic_cast<OneSimulation*>(_result->problem());
	
        // table widgets
        _inputTableVars = new WidgetTableVar(_result->inputVariables(),this);
        _finalTableVars = new WidgetTableVar(_result->finalVariables(),this);

        addDockWidget("Input Variables",_inputTableVars );
        addDockWidget("Final Variables",_finalTableVars,_inputTableVars);
		
        _finalTableVars->raise();
		
	//// Adding view list in combo
	//viewList << "Problem" << "Variables" << "Composite curves" ;
	//ui->comboDisp->addItems(viewList);

	////Toolbars
	//problemToolBar = new QToolBar(this);
	//ui->layoutToolBar->addWidget(problemToolBar);
	//
	//compositesToolBar= new QToolBar(this);
	//ui->layoutToolBar->addWidget(compositesToolBar);

	//variablesToolBar = new QToolBar(this);
	//ui->layoutToolBar->addWidget(variablesToolBar);


	//// View widgets
	//tabProblem = new TabOneSim(project,problem);
	//ui->myLayout->addWidget(tabProblem);
	//
	//tabVariables = new TabResOneSim_Variables(project,problem->result());
	//ui->myLayout->addWidget(tabVariables);
	//
	//tabComposites = new TabResOneSim_Composites(project,problem->result(),this,compositesToolBar);
	//ui->myLayout->addWidget(tabComposites);

	//curView = -1; // to force actualization
	//updateView(2);

	//// Connect signals and slots
	//connect(ui->comboDisp,SIGNAL(activated(int)),
	//	this,SLOT(updateView(int)));
}

TabResOneSim::~TabResOneSim()
{
	//delete ui;
}
//
//void TabResOneSim::updateView(int newCurView)
//{
//	if (newCurView != curView)
//	{
//		curView = newCurView;
//		if (viewList[newCurView] == "Problem")
//		{
//
//			tabProblem->show();
//			problemToolBar->show();
//
//			tabComposites->hide();
//			compositesToolBar->hide();
//
//			tabVariables->hide();
//			variablesToolBar->hide();
//
//
//		}
//
//		if (viewList[newCurView] == "Variables")
//		{
//
//			tabVariables->show();
//			variablesToolBar->show();
//
//			tabProblem->hide();
//			problemToolBar->hide();
//
//			tabComposites->hide();
//			compositesToolBar->hide();
//		}
//		if (viewList[newCurView] == "Composite curves")
//		{
//			tabVariables->hide();
//			variablesToolBar->hide();
//			
//			tabComposites->show();		
//			compositesToolBar->show();
//
//			tabProblem->hide();
//			problemToolBar->hide();
//		}
//	}
//}
//
//void TabResOneSim::actualizeGui()
//{
//
//}
