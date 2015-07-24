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

  @file WidgetSelectVars.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetSelectVars.h"
#include "ui_WidgetSelectVars.h"

WidgetSelectVars::WidgetSelectVars(MOVector<Variable> *allVariables,QWidget *parent,MOVector<Variable> *selectedVariables):
    QWidget(parent),
    ui(new Ui::WidgetSelectVarsClass)
{
    ui->setupUi(this);

    _useOpt = false;
    _allVariables = allVariables;
    if(selectedVariables)
        _selectedVariables = selectedVariables->clone();
    else
        _selectedVariables = new MOVector<Variable>(false);

    variableProxyModel = GuiTools::ModelToViewWithFilter(_allVariables,ui->listVars,ui->lineVariableFilter);

    ui->listSelectedVars->setModel(_selectedVariables);

    connect(ui->pushAddVar,SIGNAL(clicked()),this,SLOT(addVariables()));
    connect(ui->pushRemoveVar,SIGNAL(clicked()),this,SLOT(removeVariables()));

    ui->listSelectedVars->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listVars->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

WidgetSelectVars::WidgetSelectVars(MOVector<Variable> *allVariables, QWidget *parent, QList<VariableCausality> causalities,MOVector<Variable> *selectedVariables):
    QWidget(parent),
    ui(new Ui::WidgetSelectVarsClass)
{
    ui->setupUi(this);

    _useOpt = false;
    _allVariables = allVariables;
    if(selectedVariables)
        _selectedVariables = selectedVariables->clone();
    else
        _selectedVariables = new MOVector<Variable>(false);

   // variableProxyModel = GuiTools::ModelToViewWithFilter(_allVariables,ui->listVars,ui->lineVariableFilter);

    variableProxyModel = new VariableSortFilter(causalities,this);
    GuiTools::ModelToViewWithFilter(_allVariables,ui->listVars,ui->lineVariableFilter,variableProxyModel);


    ui->listSelectedVars->setModel(_selectedVariables);

    connect(ui->pushAddVar,SIGNAL(clicked()),this,SLOT(addVariables()));
    connect(ui->pushRemoveVar,SIGNAL(clicked()),this,SLOT(removeVariables()));

    ui->listSelectedVars->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listVars->setSelectionMode(QAbstractItemView::ExtendedSelection);
}


WidgetSelectVars::WidgetSelectVars(MOOptVector *allVariables,QWidget *parent,MOOptVector* selectedVariables):
    QWidget(parent),
    ui(new Ui::WidgetSelectVarsClass)
{
    ui->setupUi(this);

    _useOpt = true;
    _allOptVariables= allVariables;
    if(selectedVariables)
        _selectedOptVariables = selectedVariables->clone();
    else
        _selectedOptVariables = new MOOptVector(false,true,true,this);


    variableProxyModel = GuiTools::ModelToViewWithFilter(_allOptVariables,ui->listVars,ui->lineVariableFilter);

    ui->listSelectedVars->setModel(_selectedOptVariables);

    connect(ui->pushAddVar,SIGNAL(clicked()),this,SLOT(addVariables()));
    connect(ui->pushRemoveVar,SIGNAL(clicked()),this,SLOT(removeVariables()));

    ui->listSelectedVars->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listVars->setSelectionMode(QAbstractItemView::ExtendedSelection);

}

WidgetSelectVars::~WidgetSelectVars()
{
    delete ui;
}



void WidgetSelectVars::addVariables()
{
    QModelIndexList proxyIndexes = ui->listVars->selectionModel()->selectedIndexes();
    QModelIndex curProxyIndex;
    QModelIndex curSourceIndex;
    Variable* varSelected;
    VariableResult* optVarSelected;


    // Adding selected variables in overwritedVariables
    bool alreadyIn;

    foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
    {
        curSourceIndex = variableProxyModel->mapToSource(curProxyIndex);

        if(_useOpt)
        {
            VariableResult *newVar;
            optVarSelected=_allOptVariables->at(curSourceIndex.row());
            qDebug() << optVarSelected;
            qDebug() << curProxyIndex.internalPointer();
            qDebug() << curSourceIndex.internalPointer();
            alreadyIn = _selectedOptVariables->alreadyIn(optVarSelected->name());


            if (!alreadyIn)
            {
                newVar = new VariableResult(*optVarSelected);
                _selectedOptVariables->addItem(newVar);;
            }
        }
        else
        {
            Variable *newVar;
            varSelected=_allVariables->at(curSourceIndex.row());
            alreadyIn = _selectedVariables->alreadyIn(varSelected->name());

            qDebug() << varSelected;
            qDebug() << curProxyIndex.internalPointer();
            qDebug() << curSourceIndex.internalPointer();

            if (!alreadyIn)
            {
                newVar = new Variable(*varSelected);
                _selectedVariables->addItem(newVar);;
            }
        }
    }
}

void WidgetSelectVars::removeVariables()
{
    QModelIndexList indexList = ui->listSelectedVars->selectionModel()->selectedIndexes();

    for(int i=0;i<indexList.size();i++)
    {
        if(_useOpt)
        {
            int iVar = _selectedOptVariables->indexOf((VariableResult*)indexList.at(i).internalPointer());
            if(iVar>-1)
                _selectedOptVariables->removeRow(iVar);
        }
        else
        {
            int iVar = _selectedVariables->indexOf((Variable*)indexList.at(i).internalPointer());
            if(iVar>-1)
                _selectedVariables->removeRow(iVar);
        }
    }
}

MOVector<Variable>* WidgetSelectVars::getSelectedVars()
{
    return _selectedVariables;
}

MOOptVector* WidgetSelectVars::getSelectedOptVars()
{
    return _selectedOptVariables;
}

