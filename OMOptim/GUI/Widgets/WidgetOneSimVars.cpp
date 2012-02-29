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

  @file WidgetOneSimVars.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetOneSimVars.h"
#include <QtGui/QSortFilterProxyModel>
#include "Tools/GuiTools.h"
#include "MyDelegates.h"

namespace Ui
{
class WidgetOneSimVarsClass;
}

WidgetOneSimVars::WidgetOneSimVars(ProjectBase *project,OneSimulation *problem, QWidget *parent)
    : QWidget(parent),
      _ui(new Ui::WidgetOneSimVarsClass)
{
    this->setLocale(QLocale::C);

    _ui->setupUi(this);

    _project = project;
    _problem = problem;

    // fill variables' table
    ModModelPlus* modModelPlus = _problem->modModelPlus();
    _variables = modModelPlus->variables();

    _variableProxyModel = GuiTools::ModelToViewWithFilter(_variables,_ui->tableVariables,_ui->lineVariableFilter);
    _overVariableProxyModel = GuiTools::ModelToViewWithFilter(_problem->overwritedVariables(),_ui->tableOverwritedVariables,NULL);
    _scannedVariableProxyModel = GuiTools::ModelToViewWithFilter(_problem->scannedVariables(),_ui->tableScannedVariables,NULL);

    // Hide columns
    QList<int> varsColsToHide;
    //varsColsToHide << Variable::MIN<< Variable::MAX<< Variable::TYPE<< Variable::CATEGORY<< Variable::DATATYPE;
    for(int i=0;i<varsColsToHide.size();i++)
        _ui->tableVariables->setColumnHidden(varsColsToHide.at(i),true);
    _ui->tableVariables->resizeColumnsToContents();

    QList<int> scannedColsToHide;
    //scannedColsToHide << ScannedVariable::VALUE<< ScannedVariable::MIN<< ScannedVariable::MAX<< ScannedVariable::TYPE<< ScannedVariable::CATEGORY<< ScannedVariable::DATATYPE;
    for(int i=0;i<scannedColsToHide.size();i++)
        _ui->tableScannedVariables->setColumnHidden(scannedColsToHide.at(i),true);

    _ui->tableScannedVariables->resizeColumnsToContents();


    //delegates
    DoubleSpinBoxDelegate* valueDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* scanValueDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* scanMinDelegate = new DoubleSpinBoxDelegate(this,30);
    DoubleSpinBoxDelegate* scanMaxDelegate = new DoubleSpinBoxDelegate(this,30);
    _ui->tableOverwritedVariables->setItemDelegateForColumn(Variable::VALUE,valueDelegate);
    _ui->tableScannedVariables->setItemDelegateForColumn(ScannedVariable::VALUE,scanValueDelegate);
    _ui->tableScannedVariables->setItemDelegateForColumn(ScannedVariable::SCANMIN,scanMinDelegate);
    _ui->tableScannedVariables->setItemDelegateForColumn(ScannedVariable::SCANMAX,scanMaxDelegate);

    //buttons
    connect(_ui->pushAddOver, SIGNAL(clicked()), this, SLOT(addModVariables()));
    connect(_ui->pushRemoveOver, SIGNAL(clicked()), this, SLOT(deleteModVariables()));
    connect(_ui->pushAddScanned, SIGNAL(clicked()), this, SLOT(addScannedVariables()));
    connect(_ui->pushRemoveScanned, SIGNAL(clicked()), this, SLOT(deleteScannedVariables()));
    connect(_ui->pushSimulate, SIGNAL(clicked()), this, SLOT(simulate()));
    connect(_ui->pushReadVariables, SIGNAL(clicked()), this, SLOT(readVariables()));

}


WidgetOneSimVars::~WidgetOneSimVars()
{
    delete _ui;
}

void WidgetOneSimVars::addModVariables()
{
    QModelIndexList proxyIndexes = _ui->tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* varProv;
    Variable *newVarProv;

    // Adding selected variables in overwritedVariables
    bool alreadyIn;

    foreach(curProxyIndex, proxyIndexes)   // loop through
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);
        varProv = _variables->at(curSourceIndex.row());

        alreadyIn = _problem->overwritedVariables()->alreadyIn(varProv->name());
        if (!alreadyIn)
        {
            // set value editable
            newVarProv = new Variable(*varProv);
            newVarProv->setIsEditableField(Variable::VALUE,true);
            _problem->overwritedVariables()->addItem(newVarProv);
        }
    }
}

void WidgetOneSimVars::deleteModVariables()
{
    QModelIndexList indexList = _ui->tableOverwritedVariables->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _overVariableProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->overwritedVariables()->removeRows(rows);
}

void WidgetOneSimVars::addScannedVariables()
{
    QModelIndexList proxyIndexes = _ui->tableVariables->selectionModel()->selectedRows();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* varProv;
    ScannedVariable* newVarProv;

    // Adding selected variables in scannedVariables
    bool alreadyIn;

    foreach(curProxyIndex, proxyIndexes)   // loop through
    {
        curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);
        varProv = _variables->at(curSourceIndex.row());

        alreadyIn = _problem->scannedVariables()->alreadyIn(varProv->name());

        if (!alreadyIn)
        {
            newVarProv = new ScannedVariable(*varProv);
            _problem->scannedVariables()->addItem(newVarProv);
        }
    }
}

void WidgetOneSimVars::deleteScannedVariables()
{
    QModelIndexList indexList = _ui->tableScannedVariables->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _scannedVariableProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    _problem->scannedVariables()->removeRows(rows);
}




void WidgetOneSimVars::simulate()
{
    _project->launchProblem(_problem);
}

void WidgetOneSimVars::readVariables()
{
    ModModelPlus* modModelPlus = _problem->modModelPlus();
    modModelPlus->readVariables(_problem->ctrl());
}
