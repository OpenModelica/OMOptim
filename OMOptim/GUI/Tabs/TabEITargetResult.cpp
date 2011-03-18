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

 	@file TabEITargetResult.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "TabEITargetResult.h"
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"
#include "SimpleMilpTarget.h"


TabEITargetResult::TabEITargetResult(Project *project_,ProblemTarget *problem_, QWidget *parent) :
MO2ColTab(problem_,false,parent)
{
	type = TABSOLVEDPROBLEM;

	project = project_;
	problem = problem_;

	// Variables
	widgetEIInputVars = new WidgetEIInputVars(project_,problem->inputVars());
	widgetTreeStreams = new WidgetTreeStreams(problem->rootEI,true,true,project->eiReader(),problem->inputVars(),this);
	MERResult* merResult = problem->getMERResult();
	widgetCCPlot = new WidgetCCPlot(merResult,this);
	widgetSelPointScan = new WidgetSelPointScan(problem->inputVars(),this);
	widgetTableConnConstr = new WidgetTableConnConstr(
		problem->connConstrs(),
		problem->rootEI,
		project->eiReader(),
		true,
		this);
	widgetTableEIGroups = new WidgetTableEIGroups(problem->rootEI,false,project->eiReader(),this);

	addDockWidget("Loaded variables",widgetEIInputVars);
	addDockWidget("Points and Scans",widgetSelPointScan,widgetEIInputVars);
	addDockWidget("EI items",widgetTreeStreams,widgetEIInputVars);
	addDockWidget("EI Groups",widgetTableEIGroups,widgetEIInputVars);
	addDockWidget("Composites",widgetCCPlot);
	addDockWidget("Connections",widgetTableConnConstr,widgetEIInputVars);

	connect(problem,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));
	connect(widgetEIInputVars,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));
	connect(widgetTreeStreams,SIGNAL(targetAsked()),this,SLOT(onTargetAsked()));
	connect(widgetTreeStreams,SIGNAL(MERAsked(bool)),this,SLOT(onMERAsked(bool)));
	
	updateSelPointScan();
}

TabEITargetResult::~TabEITargetResult()
{
	
}

void TabEITargetResult::actualize()
{
	widgetTreeStreams->refreshTree();
	widgetCCPlot->actualizeGui();
}

void TabEITargetResult::updateSelPointScan()
{
	bool show = problem->inputVars()->getUsePoints()||problem->inputVars()->getUseScan();
	setWidgetVisible(widgetSelPointScan,show);
}

void TabEITargetResult::onInputVarsModified()
{
	widgetTreeStreams->setInputVars(problem->inputVars());
	updateSelPointScan();
}

void TabEITargetResult::onTargetAsked()
{
	QString tempDir = project->tempPath();
	ProblemConfig cfg;
	cfg.tempDir = tempDir;
	problem->launch(cfg);
}

void TabEITargetResult::onMERAsked(bool includeUtilities)
{
	QString tempDir = project->tempPath();
	ProblemConfig cfg;
	cfg.tempDir = tempDir;
	problem->launchMER(cfg,includeUtilities);
}
