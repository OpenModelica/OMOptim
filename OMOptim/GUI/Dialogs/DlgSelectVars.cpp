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

 	@file DlgSelectVars.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "DlgSelectVars.h"
#include "AlgoParameter.h"
#include "MOTableView.h"

#include <QtGui/QScrollBar>

DlgSelectVars::DlgSelectVars(MOVector<Variable>* _variables,MOVector<Variable>* _alreadySelected)
{
	variables = _variables;
	
	QGridLayout* allLayout = new QGridLayout(this);
	QGridLayout* varLayout = new QGridLayout(this);
	
        widgetSelectVars = new WidgetSelectVars(variables,this,_alreadySelected);
	varLayout->addWidget(widgetSelectVars);

	pushOk = new QPushButton("Ok",this);
	pushCancel = new QPushButton("Cancel",this);

	allLayout->addLayout(varLayout,0,0,1,3);
	allLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum),1,0);
	allLayout->addWidget(pushCancel,1,1,1,1);
	allLayout->addWidget(pushOk,1,2,1,1);
	this->setLayout(allLayout);
	
	connect(pushOk,SIGNAL(clicked()),this,SLOT(pushedOk()));
	connect(pushCancel,SIGNAL(clicked()),this,SLOT(pushedCancel()));
}




void DlgSelectVars::pushedOk()
{

	selectedVars = widgetSelectVars->getSelectedVars();
	accept();
}

void DlgSelectVars::pushedCancel()
{
	reject();
}

MOVector<Variable>* DlgSelectVars::getSelectedVars()
{
	return selectedVars;
}
