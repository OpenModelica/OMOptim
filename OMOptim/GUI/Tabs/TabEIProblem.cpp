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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

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

TabEIProblem::TabEIProblem(EIProblem *problem, QWidget *parent) :
    MO2ColTab(problem->project()->name(),problem,false,parent)
{

    _project = problem->project();
    _problem = problem;

    //problem given in argument could be either a EITarget,EIMERProblem or EIHEN
    // create new ProblemMER or copy from problem
    if(dynamic_cast<EITarget*>(problem))
        _problemTarget = new EITarget(*dynamic_cast<EITarget*>(problem));
    else
        _problemTarget = new EITarget(_project);

     // create new ProblemMER or copy from problem
    if(dynamic_cast<EIMERProblem*>(problem))
        _problemMER = new EIMERProblem(*dynamic_cast<EIMERProblem*>(problem));
    else
        _problemMER = new EIMERProblem(_project);

    // create new ProblemHEN or copy from problem
    if(dynamic_cast<EIHEN1Problem*>(problem))
        _problemHEN = new EIHEN1Problem(*dynamic_cast<EIHEN1Problem*>(problem));
    else
        _problemHEN = new EIHEN1Problem(_project);




    _resultMER = new EIMERResult();

    // Variables
    _widgetTreeStreams = new WidgetTreeStreams(_problem->eiTree(), true,true,_project->modClassTree(),_project->moomc(),this);

    _widgetCCPlot = new WidgetCCPlot(_resultMER,this);
    _widgetTableConnConstr = new WidgetTableConnConstr(
            _problem->connConstrs(),
            _problem->eiTree(),
            true,
            this);
    _widgetCtrl = new WidgetCtrlParameters(project,_problem->modelsReferedCtrls(),false,this);


    _widgetLaunchEI = new WidgetLaunchEI(_problem,this);



  //  addDockWidget("Loaded variables",_widgetEIInputVars);
    addDockWidget("EI Streams",_widgetTreeStreams);
    addDockWidget("Composites, MER",_widgetCCPlot,_widgetTreeStreams);
    addDockWidget("Connections",_widgetTableConnConstr,_widgetTreeStreams);
    addDockWidget("Simulator",_widgetCtrl,_widgetTreeStreams);
    addFixedWidget("Launch",_widgetLaunchEI,Qt::BottomDockWidgetArea,Qt::Vertical,false);

    connect(_widgetLaunchEI,SIGNAL(targetAsked()),this,SLOT(onTargetAsked()));
    connect(_widgetLaunchEI,SIGNAL(MERAsked()),this,SLOT(onMERAsked()));
    connect(_widgetLaunchEI,SIGNAL(HENAsked()),this,SLOT(onHENAsked()));
    connect(_widgetTreeStreams,SIGNAL(EILoadModelAsked()),this,SLOT(onEILoadModelAsked()));


    // ctrl update
    connect(_problem,SIGNAL(addedModelRef(ModModelPlus*)),this,SLOT(onAddedModelRef(ModModelPlus*)));
    connect(_problem,SIGNAL(removedModelRef(ModModelPlus*)),this,SLOT(onRemovedModelRef(ModModelPlus*)));

    mapDockWidgets.key(_widgetTreeStreams)->raise();
    _widgetTreeStreams->raise();

    readGUIState();
}

TabEIProblem::~TabEIProblem()
{
    // first delete widgets
    delete _widgetTreeStreams;
    delete _widgetCCPlot;
    delete _widgetTableConnConstr;
    delete _widgetLaunchEI;

    // then deletes problems and result
    delete _problemMER;
    if(_resultMER)
        delete _resultMER;

    delete _problemTarget;
    delete _problemHEN;
}

void TabEIProblem::actualize()
{
    _widgetTreeStreams->refreshTree();
    _widgetCCPlot->actualizeGui();
}

void TabEIProblem::onTargetAsked()
{
    MOParametersDlg dlg(_problemTarget->parameters());
    if(dlg.exec()==QDialog::Accepted)
    {

        // Fill EITree First
        //replace ei references by values
        EITree* filledEI = EIValueFiller::getFilledEI(_problem->eiTree(),_problem->storedResults(),_project);

        // Launch problem MER
        _problemTarget->setEITree(*filledEI);
    _problemTarget->setConnConstrs(new EIConnConstrs(*_problem->connConstrs()));

        _widgetCCPlot->setMERResult(NULL); // otherwise, segmentation fault when detaching curves
        _project->launchProblem(_problemTarget);
    }
}

void TabEIProblem::onMERAsked()
{
    MOParametersDlg dlg(_problemMER->parameters());

    if(dlg.exec()==QDialog::Accepted)
    {

    ProblemConfig cfg;

        // Fill EITree First
        //replace ei references by values
        EITree* filledEI = EIValueFiller::getFilledEI(_problem->eiTree(),_problem->storedResults(),_project);
        _problemMER->setEITree(*filledEI);

        // check
        QString error;
        bool ok = _problemMER->checkBeforeComp(error);
        if(!ok)
        {
            infoSender.send(Info(error,ListInfo::WARNING2));
            QMessageBox::warning(QApplication::activeWindow(), "Error",error,QMessageBox::Ok,QMessageBox::Ok);
        }
        else
        {

        _widgetCCPlot->setMERResult(NULL); // otherwise, segmentation fault when detaching curves

        if(_resultMER)
            delete _resultMER;

        _resultMER = dynamic_cast<EIMERResult*>(_problemMER->launch(cfg));

        // update target inputvars
        // (useful if MER has launched some simulation -> keep results and avoid simulating several times)
            //_problemTarget->updateInputVars(_problemMER->inputVars());

        //change view to show composites
        _widgetCCPlot->raise();
        mapDockWidgets.key(_widgetCCPlot)->raise();


        // set result to composites
        _widgetCCPlot->setMERResult(_resultMER);

    }
    }
}

void TabEIProblem::onHENAsked()
{
    QString tempDir = _project->tempPath();
    ProblemConfig cfg;

    // Fill EITree First
    //replace ei references by values
    EITree* filledEI = EIValueFiller::getFilledEI(_problem->eiTree(),_problem->storedResults(),_project);

    // Launch problem MER
    _problemHEN->setEITree(*filledEI);

    _problemHEN->setConnConstrs(new EIConnConstrs(*_problem->connConstrs()));

    MOParametersDlg dlg(_problemHEN->parameters());
    if(dlg.exec()==QDialog::Accepted)
    {
        _widgetCCPlot->setMERResult(NULL);
         _project->launchProblem(_problemHEN);
    }
}

void TabEIProblem::onEILoadModelAsked()
{
    WidgetSelectModModel* widgetSelect = new WidgetSelectModModel(_project->modClassTree(),this);
    if(widgetSelect->exec()==QDialog::Accepted)
    {
        ModModel* curModel = widgetSelect->selectedModel;
        _problem->loadModel(curModel);
    }
    _widgetTreeStreams->_treeView->expandAll();
    _widgetTreeStreams->_treeView->resizeColumnToContents(0);
}

void TabEIProblem::onAddedModelRef(ModModelPlus*)
{
    _widgetCtrl->update(_problem->modelsReferedCtrls());
}

void TabEIProblem::onRemovedModelRef(ModModelPlus*)
{
    _widgetCtrl->update(_problem->modelsReferedCtrls());
}
