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

 	@file WidgetEIInputVars.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetEIInputVars.h"
#include <QtGui/QSortFilterProxyModel>
#include "GuiTools.h"

namespace Ui
{
	class WidgetEIInputVarsClass;
}

WidgetEIInputVars::WidgetEIInputVars(Project *project_,MOOptVector *_inputVars, QWidget *parent)
: QWidget(parent),
 ui(new Ui::WidgetEIInputVarsClass)
{

	ui->setupUi(this);


	project = project_;
	inputVars = _inputVars;
		
	loadedVarsProxyModel = GuiTools::ModelToViewWithFilter(inputVars,ui->tableLoadedVariables,ui->lineVariableFilter);
	
	// Hide columns
	QList<int> varsColsToHide;
	//varsColsToHide << VariableResult::MIN<< VariableResult::MAX<< VariableResult::TYPE<< Variable::CATEGORY<< VariableResult::DATATYPE<<VariableResult::VALUE;

	for(int i=0;i<varsColsToHide.size();i++)
		ui->tableLoadedVariables->setColumnHidden(varsColsToHide.at(i),true);

	//buttons
	connect(ui->pushClear, SIGNAL(clicked()), this, SLOT(clearInputVars()));
	connect(ui->pushLoadVariables, SIGNAL(clicked()), this, SLOT(appendInputVars()));
	connect(ui->pushRefreshList, SIGNAL(clicked()),this,SLOT(fillList()));
	//Fill list
	fillList();
}

WidgetEIInputVars::~WidgetEIInputVars()
{
    delete ui;
}

void WidgetEIInputVars::fillList()
{
	Problem *curProblem;
	Result *curResult;
	QListWidgetItem* item;
	ui->listAvailableVectors->clear();

	for(int i=0;i<project->solvedProblems()->items.size();i++)
	{
		curProblem = project->solvedProblems()->items.at(i);
		curResult = curProblem->result();
		switch(curProblem->type())
		{
		case Problem::OPTIMIZATION :
				item = new QListWidgetItem(project->solvedProblems()->items.at(i)->name());
				map.insert(item,dynamic_cast<OptimResult*>(curResult)->recomputedVariables());
				ui->listAvailableVectors->addItem(item);
				break;
		case Problem::ONESIMULATION :
				item = new QListWidgetItem(project->solvedProblems()->items.at(i)->name());
				map.insert(item,dynamic_cast<OneSimResult*>(curResult)->finalVariables());
				ui->listAvailableVectors->addItem(item);
				break;
		default :
			break;
		}
	}
}

void WidgetEIInputVars::appendInputVars()
{
	QList<QListWidgetItem*> itemList = ui->listAvailableVectors->selectedItems();

	for(int i=0;i<itemList.size();i++)
	{
		if(map.contains(itemList.at(i)))
			inputVars->append(*map.value(itemList.at(i)),true);
	}
	emit inputVarsModified();
}

void WidgetEIInputVars::clearInputVars()
{
	inputVars->clear();
	inputVarsModified();
}
