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

 	@file tabModelVariables.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "tabModelVariables.h"
#include <QtGui/QSortFilterProxyModel>
#include "GuiTools.h"


namespace Ui
{
	class TabModelVariables_Class;
}

TabModelVariables::TabModelVariables(Project *project_, QWidget *parent) :
    QWidget(parent), ui(new Ui::TabModelVariables_Class)
{
    ui->setupUi(this);
	tableVariables = new MOTableView(this);
	ui->mainLayout->addWidget(tableVariables);

	project = project_;

	// connect
	connect(ui->pushActualize,SIGNAL(clicked()),this,SLOT(readVariables()));
	connect(project,SIGNAL(curModModelChanged(ModModel*)),this,SLOT(onCurModModelChanged(ModModel*)));
}

TabModelVariables::~TabModelVariables()
{
    delete ui;
}


void TabModelVariables::actualizeGuiFromProject()
{
	updateVariables(false);
}

void TabModelVariables::readVariables()
{
	// readVariables
	ModModel* curModModel = project->curModModel();
	if(curModModel)
	{
		ModModelPlus* curModModelPlus = project->modModelPlus(curModModel);
		curModModelPlus->readVariables();
	}
	updateVariables(true);
}

void TabModelVariables::onCurModModelChanged(ModModel* _model)
{
	MOVector<Variable>* variables = NULL;
	if(_model)
	{
		ModModelPlus* curModModelPlus = project->modModelPlus(_model);

		if(curModModelPlus)
			variables = curModModelPlus->variables();

	}
	GuiTools::ModelToView(variables,tableVariables);
}

void TabModelVariables::updateVariables(bool forceRecompile)
{

	ModModel* curModModel = project->curModModel();
	// refresh gui
	onCurModModelChanged(curModModel);
}
