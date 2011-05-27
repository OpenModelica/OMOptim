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

        @file tabProblemTarget.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "TabEITarget.h"
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"
#include "SimpleMilpTarget.h"


TabEITarget::TabEITarget(Project *project,EITarget *problem, QWidget *parent) :
MO2ColTab(project->name(),problem,false,parent)
{

        _project = project;
        _problemTarget = problem;

        // create new ProblemMER
        _problemMER = new EIMER(_project,_project->modReader(),_project->moomc());

        // create new ProblemHEN
        _problemHEN = new EIHEN1(_project,_project->modReader(),_project->moomc());


	// Variables
        _widgetEIInputVars = new WidgetEIInputVars(_project,_problemTarget->inputVars(),_problemTarget->eiTree()->rootElement());
        _widgetTreeStreams = new WidgetTreeStreams(_problemTarget->eiTree(), true,true,
                                                  _problemTarget->modReader(),_project->rootModClass(),_project->moomc(),_problemTarget->inputVars(),this);

        _widgetCCPlot = new WidgetCCPlot((EIMERResult*)_problemMER->result(),this);
        _widgetSelPointScan = new WidgetSelPointScan(problem->inputVars(),this);
        _widgetTableConnConstr = new WidgetTableConnConstr(
                _problemTarget->connConstrs(),
                _problemTarget->eiTree(),
		true,
		this);
        _widgetLaunchTarget = new WidgetLaunchTarget(_problemTarget,this);



        addDockWidget("Loaded variables",_widgetEIInputVars);
        addDockWidget("Points and Scans",_widgetSelPointScan,_widgetEIInputVars);
        addDockWidget("EI Streams",_widgetTreeStreams,_widgetEIInputVars);
        addDockWidget("Composites, MER",_widgetCCPlot,_widgetEIInputVars);
        addDockWidget("Connections",_widgetTableConnConstr,_widgetEIInputVars);
        addFixedWidget("Launch",_widgetLaunchTarget,Qt::BottomDockWidgetArea);

        connect(_problemTarget,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));
        connect(_widgetEIInputVars,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));
        connect(_widgetLaunchTarget,SIGNAL(targetAsked()),this,SLOT(onTargetAsked()));
        connect(_widgetLaunchTarget,SIGNAL(MERAsked(bool)),this,SLOT(onMERAsked(bool)));
        connect(_widgetLaunchTarget,SIGNAL(HENAsked(bool)),this,SLOT(onHENAsked(bool)));
        connect(_widgetTreeStreams,SIGNAL(EILoadModelAsked()),this,SLOT(onEILoadModelAsked()));
	
	updateSelPointScan();

        mapDockWidgets.key(_widgetTreeStreams)->show();

        readGUIState();
}

TabEITarget::~TabEITarget()
{
	
}

void TabEITarget::actualize()
{
        _widgetTreeStreams->refreshTree();
        _widgetCCPlot->actualizeGui();
}

void TabEITarget::updateSelPointScan()
{
        bool show = _problemTarget->inputVars()->getUsePoints()||_problemTarget->inputVars()->getUseScan();
        setWidgetVisible(_widgetSelPointScan,show);
}

void TabEITarget::onInputVarsModified()
{
        _widgetTreeStreams->setInputVars(_problemTarget->inputVars());
	updateSelPointScan();
}

void TabEITarget::onTargetAsked()
{
	QString error;
        bool ok = _problemTarget->checkBeforeComp(error);
	if(!ok)
                QMessageBox::warning(this, "Error",error,QMessageBox::Ok,QMessageBox::Ok);
	else
                _project->launchProblem(_problemTarget->num());
}

void TabEITarget::onMERAsked(bool includeUtilities)
{
        QString tempDir = _project->tempPath();
	ProblemConfig cfg;
	cfg.tempDir = tempDir;
        _problemMER->setEITree(new EITree(*_problemTarget->eiTree()));
        _problemMER->setIncludeUtilities(includeUtilities);
        _problemMER->setInputVars(_problemTarget->inputVars());
        _problemMER->launch(cfg);

        // update target inputvars
        // (useful if MER has launched some simulation -> keep results and avoid simulating several times)
        _problemTarget->updateInputVars(_problemMER->inputVars());

        // set result to composites
        _widgetCCPlot->setMERResult(_problemMER->result());

        //change view to show composites
        _widgetCCPlot->raise();

}

void TabEITarget::onHENAsked(bool includeUtilities)
{
        QString tempDir = _project->tempPath();
        ProblemConfig cfg;
        cfg.tempDir = tempDir;
        _problemHEN->setEITree(new EITree(*_problemTarget->eiTree()));
        _problemHEN->setIncludeUtilities(includeUtilities);
        _problemHEN->setInputVars(_problemTarget->inputVars());
        _problemHEN->launch(cfg);

        // update target inputvars
        // (useful if MER has launched some simulation -> keep results and avoid simulating several times)
        _problemTarget->updateInputVars(_problemHEN->inputVars());

        //change view to show composites
        _widgetCCPlot->raise();
}

void TabEITarget::onEILoadModelAsked()
{
    WidgetSelectModModel* widgetSelect = new WidgetSelectModModel(_project->modReader(),_project->rootModClass(),this);
    if(widgetSelect->exec()==QDialog::Accepted)
    {
            ModModel* curModel = widgetSelect->selectedModel;
            _problemTarget->loadModel(curModel);
    }
    _widgetTreeStreams->treeView->expandAll();
    _widgetTreeStreams->treeView->resizeColumnToContents(0);
}
