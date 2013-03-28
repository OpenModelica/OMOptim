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

  @file WidgetSelectOptVars.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetSelectOptVars.h"
#include "ui_WidgetSelectOptVars.h"
#include <QtGui/QErrorMessage>
#include "Optimization.h"
#include "Project.h"
#include "ModExePlus.h"
#include "ModModelPlus.h"

WidgetSelectOptVars::WidgetSelectOptVars(Optimization* problem,bool isEditable,QWidget *parent):
    QWidget(parent),
    _ui(new Ui::WidgetSelectOptVarsClass)
{
    _ui->setupUi(this);
    _problem = problem;
    _project = dynamic_cast<Project*>(problem->project());

    _isEditable = isEditable;

    // concatenate model variables
    _allModelsVars = new Variables(true);
    for(int i=0;i<_problem->models().size();i++)
    {
        ModelPlus* modelPlus =  _project->modelPlus(_problem->models().at(i));
        if(modelPlus)
        {
            Variables* modelVars = modelPlus->variables();
            connect(modelVars,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(refreshAllModelsVars()));
        }
    }

    // create permanent vars vector
    _permanentVars = new Variables(true);

    // create tables
    _tableVariables = new MOTableView(this);
    _tableScannedVars = new MOTableView(this);
    _tableOptimizedVars = new MOTableView(this);
    _tableObjectives = new MOTableView(this);
    _tableOverVars = new MOTableView(this);

    _tableVariables->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableVariables->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _tableScannedVars->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableOptimizedVars->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableObjectives->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableOverVars->setSelectionBehavior(QAbstractItemView::SelectRows);

    _ui->layoutObjectives->addWidget(_tableObjectives);
    _ui->layoutScannedVars->addWidget(_tableScannedVars);
    _ui->layoutOptimizedVars->addWidget(_tableOptimizedVars);
    _ui->layoutTableVariables->addWidget(_tableVariables);
    _ui->layoutOvervars->addWidget(_tableOverVars);

    // tables' model
    _optVariableProxyModel = GuiTools::ModelToViewWithFilter(_problem->optimizedVariables(),
                                                             _tableOptimizedVars,NULL);


    _variableProxyModel = GuiTools::ModelToViewWithFilter(_allModelsVars,
                                                          _tableVariables,_ui->lineVariableFilter);

    _scannedProxyModel = GuiTools::ModelToViewWithFilter(_problem->scannedVariables(),
                                                         _tableScannedVars,NULL);

    _objectiveProxyModel = GuiTools::ModelToViewWithFilter(_problem->objectives(),
                                                           _tableObjectives,NULL);

    _overVarsProxyModel = GuiTools::ModelToViewWithFilter(_problem->overwritedVariables(),
                                                          _tableOverVars,NULL);



    setShownColumns();
    setDelegates();
    refreshAllModelsVars();

    //buttons
    connect(_ui->pushAddVariables, SIGNAL(clicked()), this, SLOT(addOptVariables()));
    connect(_ui->pushRemoveVariables, SIGNAL(clicked()), this, SLOT(deleteOptVariables()));
    connect(_ui->pushAddObjectives, SIGNAL(clicked()), this, SLOT(addOptObjectives()));
    connect(_ui->pushRemoveObjectives, SIGNAL(clicked()), this, SLOT(deleteOptObjectives()));
    connect(_ui->pushAddScanned, SIGNAL(clicked()), this, SLOT(addScannedVariables()));
    connect(_ui->pushRemoveScanned, SIGNAL(clicked()), this, SLOT(deleteScannedVariables()));
    connect(_ui->pushAddParam, SIGNAL(clicked()), this, SLOT(addOverVariables()));
    connect(_ui->pushRemoveParam, SIGNAL(clicked()), this, SLOT(deleteOverVariables()));

    connect(_ui->pushReadVariables,SIGNAL(clicked()),this,SLOT(readVariables()));


    //size tables' columns
    _tableScannedVars->resizeColumnsToContents();
    _tableOptimizedVars->resizeColumnsToContents();
    _tableObjectives->resizeColumnsToContents();
    _tableVariables->resizeColumnsToContents();

}

/**
  * Used only for inheriting purposes
  */
WidgetSelectOptVars::WidgetSelectOptVars(QWidget *parent):
    QWidget(parent)
{

}

WidgetSelectOptVars::~WidgetSelectOptVars()
{
    delete _ui;
    delete _allModelsVars;
    delete _permanentVars;
}

void WidgetSelectOptVars::addPermanentVars(Variables * vars)
{
    _permanentVars->addItems(vars,true);
}

void WidgetSelectOptVars::addPermanentVar(const Variable & var)
{
    _permanentVars->addItem(var.clone());
}

void WidgetSelectOptVars::removePermanentVars(QString model)
{
    QList<int> toRemove;
    for(int i=0;i<_permanentVars->size();i++)
        if(_permanentVars->at(i)->model()==model)
            toRemove.push_back(i);

    _permanentVars->removeRows(toRemove);
}



void WidgetSelectOptVars::addOptVariables()
{
    QModelIndexList proxyIndexes = _tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* selVar;
    OptVariable* optVarProv;
    // Adding selected variables in overwritedVariables
    bool alreadyIn;


    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);

        selVar=_allModelsVars->at(curSourceIndex.row());

        alreadyIn = _problem->optimizedVariables()->alreadyIn(selVar->name());
        //        if ((!alreadyIn && !selVar->output()))
        if (!alreadyIn)
        {
            optVarProv = new OptVariable(*selVar);
            _problem->optimizedVariables()->addItem(optVarProv);
        }
    }

    _tableOptimizedVars->resizeColumnsToContents();
}

void WidgetSelectOptVars::deleteOptVariables()
{
    QModelIndexList indexList = _tableOptimizedVars->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _optVariableProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->optimizedVariables()->removeRows(rows);

    _tableOptimizedVars->resizeColumnsToContents();

}

void WidgetSelectOptVars::addScannedVariables()
{
    QModelIndexList proxyIndexes = _tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* selVar;
    ScannedVariable* scannedVarProv;
    // Adding selected variables in overwritedVariables
    bool alreadyIn;

    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);

        selVar=_allModelsVars->at(curSourceIndex.row());

        alreadyIn = _problem->scannedVariables()->alreadyIn(selVar->name());
        //        if (!alreadyIn && !selVar->output())
        if (!alreadyIn)
        {
            scannedVarProv = new ScannedVariable(*selVar);
            _problem->scannedVariables()->addItem(scannedVarProv);
        }
    }

    _tableScannedVars->resizeColumnsToContents();
}

void WidgetSelectOptVars::deleteScannedVariables()
{
    QModelIndexList indexList = _tableScannedVars->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _scannedProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->scannedVariables()->removeRows(rows);

    _tableScannedVars->resizeColumnsToContents();
}

void WidgetSelectOptVars::addOverVariables()
{
    QModelIndexList proxyIndexes = _tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* selVar;
    Variable* varProv;
    // Adding selected variables in overwritedVariables
    bool alreadyIn;

    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);
        selVar=_allModelsVars->at(curSourceIndex.row());
        alreadyIn = _problem->overwritedVariables()->alreadyIn(selVar->name());
        //        if (!alreadyIn && !selVar->output())
        if (!alreadyIn)
        {
            varProv = new Variable(*selVar);
            _problem->overwritedVariables()->addItem(varProv);
        }
    }

    _tableOverVars->resizeColumnsToContents();
}

void WidgetSelectOptVars::deleteOverVariables()
{
    QModelIndexList indexList = _tableOverVars->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _overVarsProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->overwritedVariables()->removeRows(rows);

    _tableOverVars->resizeColumnsToContents();
}



void WidgetSelectOptVars::addOptObjectives()
{
    QModelIndexList proxyIndexes = _tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* selVar;
    OptObjective* newObj;
    bool alreadyIn;
    // Adding selected variables in objectives

    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);

        selVar=_allModelsVars->at(curSourceIndex.row());

        alreadyIn = _problem->objectives()->alreadyIn(selVar->name());
        if (!alreadyIn)
            /*&&  (((_problem->ctrlType(_problem->models().at(0)) !=  ModPlusCtrl::EXECUTABLE))
                           || ((_problem->ctrlType(_problem->models().at(0)) == ModPlusCtrl::EXECUTABLE) && selVar->output())) )*/
        {
            newObj = new OptObjective(*selVar);
            _problem->objectives()->addItem(newObj);
        }
    }
    _tableObjectives->resizeColumnsToContents();
}

void WidgetSelectOptVars::deleteOptObjectives()
{
    QModelIndexList indexList = _tableObjectives->selectionModel()->selectedRows();
    QList<int> rows;
    QModelIndex curSourceIndex;

    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _objectiveProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->objectives()->removeRows(rows);
    _tableObjectives->resizeColumnsToContents();
}




void WidgetSelectOptVars::actualizeGui()
{
    // list of widgets to hide when problem is solved
    QWidgetList unsolvedWidgets;
    unsolvedWidgets << _ui->pushAddObjectives << _ui->pushAddVariables << _ui->pushAddScanned;
    unsolvedWidgets << _ui->pushRemoveObjectives << _ui->pushRemoveVariables << _ui->pushRemoveScanned;

    // list of widgets to hide when problem is unsolved
    QWidgetList solvedWidgets;

    QList<QTableView*> tables;
    tables << _tableObjectives << _tableOptimizedVars << _tableVariables ;

    // if problem is solved
    if(!_isEditable)
    {
        for(int i=0; i < unsolvedWidgets.size(); i++)
            unsolvedWidgets.at(i)->hide();

        for(int i=0; i < solvedWidgets.size(); i++)
            solvedWidgets.at(i)->show();

        for(int i=0; i< tables.size(); i++)
            tables.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    else
    {
        for(int i=0; i < unsolvedWidgets.size(); i++)
            unsolvedWidgets.at(i)->show();

        for(int i=0; i < solvedWidgets.size(); i++)
            solvedWidgets.at(i)->hide();

        for(int i=0; i< tables.size(); i++)
            tables.at(i)->setEditTriggers(QAbstractItemView::DoubleClicked);
    }
}


void WidgetSelectOptVars::readVariables()
{

    _allModelsVars->clear();

    for(int i=0;i<_problem->models().size();i++)
    {
        ModelPlus* curModelPlus = _project->modelPlus(_problem->models().at(i));

        // is compiled
        bool isCompiled;


        if(curModelPlus->modelType() == ModelPlus::MODELICA)
            isCompiled = ((ModModelPlus *) curModelPlus)->isCompiled(_problem->ctrl(_problem->models().at(i)));
        bool shouldForceRecompile = false;
        if(isCompiled && curModelPlus->modelType() == ModelPlus::MODELICA)
        {
            // already compiled, ask user if we should only read init file or recompile model
            QMessageBox msgBox;
            msgBox.setText("Model "+_problem->models().at(i)+" is already compiled.");
            msgBox.setInformativeText("Would you like to compile it again ?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            switch (ret)
            {
            case QMessageBox::Yes:
                shouldForceRecompile = true;
                break;
            case QMessageBox::No:
                shouldForceRecompile = false;
                break;
            }
            ((ModModelPlus *)curModelPlus)->readVariables(_problem->ctrl(_problem->models().at(i)),_problem->filesToCopy(),shouldForceRecompile);
        }
        if(curModelPlus->modelType() == ModelPlus::EXECUTABLE)
            ((ModExePlus *) curModelPlus)->readVariables(_problem->ctrl(_problem->models().at(i)));
    }

    refreshAllModelsVars();

    _tableVariables->resizeColumnsToContents();
}


void WidgetSelectOptVars::refreshAllModelsVars()
{
    _allModelsVars->clear();
    ModelPlus* curModelPlus;
    Variables* curVariables;
    for(int i=0;i<_problem->models().size();i++)
    {
        curModelPlus = _project->modelPlus(_problem->models().at(i));
        if(curModelPlus)
        {
//            if(compileIfNeeded && !curModelPlus->isCompiled(_problem->ctrl(_problem->models().at(i))))
//                curModelPlus->compile(_problem->ctrl(_problem->models().at(i)));
            curVariables = curModelPlus->variables();
            _allModelsVars->addItems(curVariables,true);
        }
    }

    _allModelsVars->addItems(_permanentVars,true);

}


void WidgetSelectOptVars::setShownColumns()
{
    // Hide columns
    QList<int> varsColsToShow;
    //varsColsToHide;
    varsColsToShow << Variable::NAME << Variable::DESCRIPTION;


    for(int i=0;i<_tableVariables->horizontalHeader()->count();i++)
        _tableVariables->setColumnHidden(i,!varsColsToShow.contains(i));

    //    QList<int> optColsToHide;
    //    optColsToHide << OptVariable::DESCRIPTION;
    //    for(int i=0;i<optColsToHide.size();i++)
    //        _tableOptimizedVars->setColumnHidden(optColsToHide.at(i),true);

    //    QList<int> scannedColsToHide;
    //    scannedColsToHide << ScannedVariable::DESCRIPTION;
    //    for(int i=0;i<scannedColsToHide.size();i++)
    //        _tableScannedVars->setColumnHidden(scannedColsToHide.at(i),true);

    //    QList<int> objColsToHide;
    //    objColsToHide << OptObjective::DESCRIPTION;
    //    for(int i=0;i<objColsToHide.size();i++)
    //        _tableObjectives->setColumnHidden(objColsToHide.at(i),true);

}

void WidgetSelectOptVars::setDelegates()
{
    //tables' delegates
    QList<int> values;
    QStringList titles;
    values << OptObjective::MINIMIZE    << OptObjective::MAXIMIZE;
    titles << "Minimize"                << "Maximize";
    GenericDelegate *directionDelegate = new GenericDelegate(values,titles,this);
    _tableObjectives->setItemDelegateForColumn(OptObjective::DIRECTION,directionDelegate);

    values.clear();
    titles.clear();
    values << OptObjective::NONE
           << OptObjective::SUM
           << OptObjective::AVERAGE
           << OptObjective::DEVIATION
           << OptObjective::MINIMUM
           << OptObjective::MAXIMUM ;

    titles << "None"
           << "Sum"
           << "Average"
           << "Standard deviation"
           << "Minimum"
           << "Maximum";

    GenericDelegate *scanFunctionDelegate = new GenericDelegate(values,titles,this);
    _tableObjectives->setItemDelegateForColumn(OptObjective::SAMPLINGFUNCTION,scanFunctionDelegate);

    values.clear();
    titles.clear();
    values << OMREAL << OMINTEGER << OMBOOLEAN << OMSTRING ;
    titles << "Real" << "Integer" << "Boolean" << "String";
    GenericDelegate *dataTypeDelegate = new GenericDelegate(values,titles,this);
    _tableOptimizedVars->setItemDelegateForColumn(OptVariable::DATATYPE,dataTypeDelegate);


    DoubleSpinBoxDelegate* minDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* maxDelegate = new DoubleSpinBoxDelegate(this,30);
    _tableOptimizedVars->setItemDelegateForColumn(OptVariable::OPTMIN,minDelegate);
    _tableOptimizedVars->setItemDelegateForColumn(OptVariable::OPTMAX,maxDelegate);

    DoubleSpinBoxDelegate* minObjDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* maxObjDelegate = new DoubleSpinBoxDelegate(this,30);
    _tableObjectives->setItemDelegateForColumn(OptObjective::MIN,minObjDelegate);
    _tableObjectives->setItemDelegateForColumn(OptObjective::MAX,maxObjDelegate);

    DoubleSpinBoxDelegate* minScanDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* maxScanDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* valueScanDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* stepScanDelegate = new DoubleSpinBoxDelegate(this,30);
    _tableScannedVars->setItemDelegateForColumn(ScannedVariable::SCANMIN,minScanDelegate);
    _tableScannedVars->setItemDelegateForColumn(ScannedVariable::SCANMAX,maxScanDelegate);
    _tableScannedVars->setItemDelegateForColumn(ScannedVariable::VALUE,valueScanDelegate);
    _tableScannedVars->setItemDelegateForColumn(ScannedVariable::SCANSTEP,stepScanDelegate);
}
