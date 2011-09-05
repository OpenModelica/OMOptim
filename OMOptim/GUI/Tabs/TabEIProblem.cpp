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

        @file TabEIProblem.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
        @version
*/

#include "TabEIProblem.h"
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"
#include "SimpleMilpTarget.h"


TabEIProblem::TabEIProblem(Project *project,EIProblem *problem, QWidget *parent) :
        MO2ColTab(project->name(),problem,false,parent)
{

    _project = project;
    _problem = problem;

    //problem given in argument could be either a EITarget,EIMERProblem or EIHEN
    // create new ProblemMER or copy from problem
    if(dynamic_cast<EITarget*>(problem))
        _problemTarget = new EITarget(*dynamic_cast<EITarget*>(problem));
    else
        _problemTarget = new EITarget(_project,_project->modClassTree(),_project->moomc());

     // create new ProblemMER or copy from problem
    if(dynamic_cast<EIMERProblem*>(problem))
        _problemMER = new EIMERProblem(*dynamic_cast<EIMERProblem*>(problem));
    else
        _problemMER = new EIMERProblem(_project,_project->modClassTree(),_project->moomc());

    // create new ProblemHEN or copy from problem
    if(dynamic_cast<EIHEN1Problem*>(problem))
        _problemHEN = new EIHEN1Problem(*dynamic_cast<EIHEN1Problem*>(problem));
    else
        _problemHEN = new EIHEN1Problem(_project,_project->modClassTree(),_project->moomc());




    _resultMER = new EIMERResult();

    // Variables
  //  _widgetEIInputVars = new WidgetEIInputVars(_project,_problem->inputVars(),_problem->eiTree()->rootElement(),this);
    _widgetTreeStreams = new WidgetTreeStreams(_problem->eiTree(), true,true,_project->modClassTree(),_project->moomc(),this,_problem->inputVars());

    _widgetCCPlot = new WidgetCCPlot(_resultMER,this);
    _widgetSelPointScan = new WidgetSelPointScan(problem->inputVars(),this);
    _widgetTableConnConstr = new WidgetTableConnConstr(
            _problem->connConstrs(),
            _problem->eiTree(),
            true,
            this);
    _widgetLaunchEI = new WidgetLaunchEI(_problem,this);



  //  addDockWidget("Loaded variables",_widgetEIInputVars);
    addDockWidget("EI Streams",_widgetTreeStreams);
    addDockWidget("Points and Scans",_widgetSelPointScan,_widgetTreeStreams);
    addDockWidget("Composites, MER",_widgetCCPlot,_widgetTreeStreams);
    addDockWidget("Connections",_widgetTableConnConstr,_widgetTreeStreams);
    addFixedWidget("Launch",_widgetLaunchEI,Qt::BottomDockWidgetArea);

    connect(_problem,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));
  //  connect(_widgetEIInputVars,SIGNAL(inputVarsModified()),this,SLOT(onInputVarsModified()));
    connect(_widgetLaunchEI,SIGNAL(targetAsked()),this,SLOT(onTargetAsked()));
    connect(_widgetLaunchEI,SIGNAL(MERAsked()),this,SLOT(onMERAsked()));
    connect(_widgetLaunchEI,SIGNAL(HENAsked()),this,SLOT(onHENAsked()));
    connect(_widgetTreeStreams,SIGNAL(EILoadModelAsked()),this,SLOT(onEILoadModelAsked()));

    updateSelPointScan();

    mapDockWidgets.key(_widgetTreeStreams)->raise();
    _widgetTreeStreams->raise();

    readGUIState();
}

TabEIProblem::~TabEIProblem()
{
    int a=2;

}

void TabEIProblem::actualize()
{
    _widgetTreeStreams->refreshTree();
    _widgetCCPlot->actualizeGui();
}

void TabEIProblem::updateSelPointScan()
{
    bool show = _problem->inputVars()->getUsePoints()||_problem->inputVars()->getUseScan();
    setWidgetVisible(_widgetSelPointScan,show);
}

void TabEIProblem::onInputVarsModified()
{
    _widgetTreeStreams->setInputVars(_problem->inputVars());
    updateSelPointScan();
}

void TabEIProblem::onTargetAsked()
{
    QString tempDir = _project->tempPath();
    ProblemConfig cfg;
    cfg.tempDir = tempDir;
    _problemTarget->setEITree(*_problem->eiTree());
    _problemTarget->setInputVars(_problem->inputVars()->clone());
    _problemTarget->setConnConstrs(new EIConnConstrs(*_problem->connConstrs()));


    MOParametersDlg dlg(_problemTarget->parameters());
    if(dlg.exec()==QDialog::Accepted)
    {
        _project->launchProblem(_problemTarget);


        //EITargetResult* result = _problemTarget->launch(cfg);


        // update target inputvars
        // (useful if MER or HEN has launched some simulation -> keep results and avoid simulating several times)
        //_problem->updateInputVars(_problemTarget->inputVars());

//        if(result->isSuccess())
//            _project->addResult(result);
    }
}

void TabEIProblem::onMERAsked()
{
    QString tempDir = _project->tempPath();
    ProblemConfig cfg;
    cfg.tempDir = tempDir;
    _problemMER->setEITree(*_problem->eiTree());
    _problemMER->setInputVars(_problem->inputVars()->clone());


    MOParametersDlg dlg(_problemMER->parameters());

    if(dlg.exec()==QDialog::Accepted)
    {
        EIMERResult* oldResult = _resultMER;
        _resultMER = dynamic_cast<EIMERResult*>(_problemMER->launch(cfg));

        // update target inputvars
        // (useful if MER has launched some simulation -> keep results and avoid simulating several times)
        _problemTarget->updateInputVars(_problemMER->inputVars());

        // set result to composites
        _widgetCCPlot->setMERResult(_resultMER);
        delete oldResult;

        //change view to show composites
        _widgetCCPlot->raise();
        mapDockWidgets.key(_widgetCCPlot)->raise();
        //_widgetCCPlot->actualizeGui();
    }
}

void TabEIProblem::onHENAsked()
{
    QString tempDir = _project->tempPath();
    ProblemConfig cfg;
    cfg.tempDir = tempDir;
    _problemHEN->setEITree(*_problem->eiTree());
    _problemHEN->setInputVars(_problem->inputVars()->clone());
    _problemHEN->setConnConstrs(new EIConnConstrs(*_problem->connConstrs()));

    MOParametersDlg dlg(_problemHEN->parameters());
    if(dlg.exec()==QDialog::Accepted)
    {
         _project->launchProblem(_problemHEN);
    }
}

void TabEIProblem::onEILoadModelAsked()
{
    WidgetSelectModModel* widgetSelect = new WidgetSelectModModel(_project->modClassTree(),this);
    if(widgetSelect->exec()==QDialog::Accepted)
    {
        ModModel* curModel = widgetSelect->selectedModel;
        _problem->loadModel(_project->modClassTree(),curModel);
    }
    _widgetTreeStreams->treeView->expandAll();
    _widgetTreeStreams->treeView->resizeColumnToContents(0);
}
