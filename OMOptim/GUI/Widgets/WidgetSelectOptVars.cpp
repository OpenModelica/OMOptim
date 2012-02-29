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

WidgetSelectOptVars::WidgetSelectOptVars(Optimization* problem,bool isEditable,QWidget *parent):
    QWidget(parent),
    _ui(new Ui::WidgetSelectOptVarsClass)
{
    _ui->setupUi(this);
    _problem = problem;
    _isEditable = isEditable;


    // tables' model
    _optVariableProxyModel = GuiTools::ModelToViewWithFilter(_problem->optimizedVariables(),
                                                             _ui->tableOptimizedVariables,NULL);

    _variableProxyModel = GuiTools::ModelToViewWithFilter(_problem->modModelPlus()->variables(),
                                                          _ui->tableVariables,_ui->lineVariableFilter);

    _scannedProxyModel = GuiTools::ModelToViewWithFilter(_problem->scannedVariables(),
                                                         _ui->tableScannedVariables,NULL);

    _objectiveProxyModel = GuiTools::ModelToViewWithFilter(_problem->objectives(),
                                                           _ui->tableObjectives,NULL);


    // tables'gui
    _ui->tableObjectives->setSelectionBehavior(QAbstractItemView::SelectRows);


    // Hide columns
    QList<int> varsColsToHide;
    //varsColsToHide;
    for(int i=0;i<varsColsToHide.size();i++)
        _ui->tableVariables->setColumnHidden(varsColsToHide.at(i),true);

    QList<int> optColsToHide;
    optColsToHide << OptVariable::DESCRIPTION;
    for(int i=0;i<optColsToHide.size();i++)
        _ui->tableOptimizedVariables->setColumnHidden(optColsToHide.at(i),true);

    QList<int> scannedColsToHide;
    scannedColsToHide << ScannedVariable::DESCRIPTION;
    for(int i=0;i<scannedColsToHide.size();i++)
        _ui->tableScannedVariables->setColumnHidden(scannedColsToHide.at(i),true);

    QList<int> objColsToHide;
    objColsToHide << OptObjective::DESCRIPTION;
    for(int i=0;i<objColsToHide.size();i++)
        _ui->tableObjectives->setColumnHidden(objColsToHide.at(i),true);

    //tables' delegates
    QList<int> values;
    QStringList titles;
    values << OptObjective::MINIMIZE	<< OptObjective::MAXIMIZE;
    titles << "Minimize"				<< "Maximize";
    GenericDelegate *directionDelegate = new GenericDelegate(values,titles,this);
    _ui->tableObjectives->setItemDelegateForColumn(OptObjective::DIRECTION,directionDelegate);

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
    _ui->tableObjectives->setItemDelegateForColumn(OptObjective::SCANFUNCTION,scanFunctionDelegate);

    values.clear();
    titles.clear();
    values << OMREAL << OMINTEGER << OMBOOLEAN << OMSTRING ;
    titles << "Real" << "Integer" << "Boolean" << "String";
    GenericDelegate *dataTypeDelegate = new GenericDelegate(values,titles,this);
    _ui->tableOptimizedVariables->setItemDelegateForColumn(OptVariable::DATATYPE,dataTypeDelegate);


    DoubleSpinBoxDelegate* minDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* maxDelegate = new DoubleSpinBoxDelegate(this,30);
    _ui->tableOptimizedVariables->setItemDelegateForColumn(OptVariable::OPTMIN,minDelegate);
    _ui->tableOptimizedVariables->setItemDelegateForColumn(OptVariable::OPTMAX,maxDelegate);

    DoubleSpinBoxDelegate* minObjDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* maxObjDelegate = new DoubleSpinBoxDelegate(this,30);
    _ui->tableObjectives->setItemDelegateForColumn(OptObjective::MIN,minObjDelegate);
    _ui->tableObjectives->setItemDelegateForColumn(OptObjective::MAX,maxObjDelegate);

    DoubleSpinBoxDelegate* minScanDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* maxScanDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* valueScanDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* stepScanDelegate = new DoubleSpinBoxDelegate(this,30);
    _ui->tableScannedVariables->setItemDelegateForColumn(ScannedVariable::SCANMIN,minScanDelegate);
    _ui->tableScannedVariables->setItemDelegateForColumn(ScannedVariable::SCANMAX,maxScanDelegate);
    _ui->tableScannedVariables->setItemDelegateForColumn(ScannedVariable::VALUE,valueScanDelegate);
    _ui->tableScannedVariables->setItemDelegateForColumn(ScannedVariable::SCANSTEP,stepScanDelegate);

    //buttons
    connect(_ui->pushAddVariables, SIGNAL(clicked()), this, SLOT(addOptVariables()));
    connect(_ui->pushRemoveVariables, SIGNAL(clicked()), this, SLOT(deleteOptVariables()));
    connect(_ui->pushAddObjectives, SIGNAL(clicked()), this, SLOT(addOptObjectives()));
    connect(_ui->pushRemoveObjectives, SIGNAL(clicked()), this, SLOT(deleteOptObjectives()));
    connect(_ui->pushAddScanned, SIGNAL(clicked()), this, SLOT(addScannedVariables()));
    connect(_ui->pushRemoveScanned, SIGNAL(clicked()), this, SLOT(deleteScannedVariables()));
    connect(_ui->pushReadVariables,SIGNAL(clicked()),this,SLOT(readVariables()));


    //size tables' columns
    _ui->tableScannedVariables->resizeColumnsToContents();
    _ui->tableOptimizedVariables->resizeColumnsToContents();
    _ui->tableObjectives->resizeColumnsToContents();
    _ui->tableVariables->resizeColumnsToContents();

}

WidgetSelectOptVars::~WidgetSelectOptVars()
{
    delete _ui;
}



void WidgetSelectOptVars::addOptVariables()
{
    QModelIndexList proxyIndexes = _ui->tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* selVar;
    OptVariable* optVarProv;
    // Adding selected variables in overwritedVariables
    bool alreadyIn;


    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);
        selVar=_problem->modModelPlus()->variables()->at(curSourceIndex.row());
        alreadyIn = _problem->optimizedVariables()->alreadyIn(selVar->name());
        if (!alreadyIn)
        {
            optVarProv = new OptVariable(*selVar);
            _problem->optimizedVariables()->addItem(optVarProv);
        }
    }

    _ui->tableOptimizedVariables->resizeColumnsToContents();
}

void WidgetSelectOptVars::deleteOptVariables()
{
    QModelIndexList indexList = _ui->tableOptimizedVariables->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _optVariableProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->optimizedVariables()->removeRows(rows);

    _ui->tableOptimizedVariables->resizeColumnsToContents();

}

void WidgetSelectOptVars::addScannedVariables()
{
    QModelIndexList proxyIndexes = _ui->tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* selVar;
    ScannedVariable* scannedVarProv;
    // Adding selected variables in overwritedVariables
    bool alreadyIn;


    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);
        selVar=_problem->modModelPlus()->variables()->at(curSourceIndex.row());
        alreadyIn = _problem->scannedVariables()->alreadyIn(selVar->name());
        if (!alreadyIn)
        {
            scannedVarProv = new ScannedVariable(*selVar);
            _problem->scannedVariables()->addItem(scannedVarProv);
        }
    }

    _ui->tableScannedVariables->resizeColumnsToContents();
}

void WidgetSelectOptVars::deleteScannedVariables()
{
    QModelIndexList indexList = _ui->tableScannedVariables->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _scannedProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->scannedVariables()->removeRows(rows);

    _ui->tableScannedVariables->resizeColumnsToContents();
}



void WidgetSelectOptVars::addOptObjectives()
{
    QModelIndexList proxyIndexes = _ui->tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* selVar;
    OptObjective* newObj;
    bool alreadyIn;
    // Adding selected variables in objectives

    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);
        selVar=_problem->modModelPlus()->variables()->items[curSourceIndex.row()];

        alreadyIn = _problem->objectives()->alreadyIn(selVar->name());
        if (!alreadyIn)
        {
            newObj = new OptObjective(*selVar);
            _problem->objectives()->addItem(newObj);
        }
    }
    _ui->tableObjectives->resizeColumnsToContents();
}

void WidgetSelectOptVars::deleteOptObjectives()
{
    QModelIndexList indexList = _ui->tableObjectives->selectionModel()->selectedRows();
    QList<int> rows;
    QModelIndex curSourceIndex;

    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _objectiveProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->objectives()->removeRows(rows);
    _ui->tableObjectives->resizeColumnsToContents();
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
    tables << _ui->tableObjectives << _ui->tableOptimizedVariables << _ui->tableVariables ;

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
    // is compiled
    bool isCompiled = _problem->modModelPlus()->isCompiled(_problem->ctrl());
    bool shouldForceRecompile = false;
    if(isCompiled)
   {
        // already compiled, ask user if we should only read init file or recompile model
        QMessageBox msgBox;
        msgBox.setText("Model is already compiled.");
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
    }
     _problem->modModelPlus()->readVariables(_problem->ctrl(),shouldForceRecompile);
    _ui->tableVariables->resizeColumnsToContents();
}
