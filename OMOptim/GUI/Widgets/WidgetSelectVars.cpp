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
 	@version 0.9 
*/

#include "WidgetSelectVars.h"
#include "ui_WidgetSelectVars.h"
#include <QtGui/QErrorMessage>


WidgetSelectVars::WidgetSelectVars(MOVector<Variable> *_variables,QWidget *parent,MOVector<Variable> *_selectedVariables):
    QWidget(parent),
    ui(new Ui::WidgetSelectVarsClass)
{
    ui->setupUi(this);
	
	variables = _variables;

	if(_selectedVariables)
		selectedVariables = _selectedVariables->clone();
	else
		selectedVariables = new MOVector<Variable>();

	variableProxyModel = GuiTools::ModelToViewWithFilter(variables,ui->listVars,ui->lineVariableFilter);
	
	ui->listSelectedVars->setModel(selectedVariables);

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
	Variable *newVar;
	
	// Adding selected variables in overwritedVariables
	bool alreadyIn;
	
	foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
	{
		curSourceIndex = variableProxyModel->mapToSource(curProxyIndex);
		varSelected=variables->items.at(curSourceIndex.row());
		
		alreadyIn = selectedVariables->alreadyIn(varSelected->name());
				
		if (!alreadyIn)
		{
			newVar = new Variable(*varSelected);
			selectedVariables->addItem(newVar);;
		}			
	}
}

void WidgetSelectVars::removeVariables()
{
	QModelIndexList indexList = ui->listSelectedVars->selectionModel()->selectedIndexes();

	for(int i=0;i<indexList.size();i++)
	{
		int iVar = selectedVariables->items.indexOf((Variable*)indexList.at(i).internalPointer());
		if(iVar>-1)
			selectedVariables->removeRow(iVar);
	}
}

MOVector<Variable>* WidgetSelectVars::getSelectedVars()
{
	return selectedVariables;
}
