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

 	@file ModModelParametersDlg.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "ModModelParametersDlg.h"
#include "MOTableView.h"
#include <QtGui/QScrollBar>

ModModelParametersDlg::ModModelParametersDlg(MOVector<ModModelParameter> *_parameters)
{
	orgParameters = _parameters;
	localParameters = _parameters->clone();
	QGridLayout *newLayout = buildLayoutFromEaConfig();
	this->setLayout(newLayout);
}

QGridLayout* ModModelParametersDlg::buildLayoutFromEaConfig()
{
	//Adding Layout
	QGridLayout *newLayout = new QGridLayout(this);

	MOTableView *table = new MOTableView(this);
	newLayout->addWidget(table,0,0,1,3);

	GuiTools::ModelToView(localParameters,table);

	// resizing window
	int count = localParameters->columnCount(QModelIndex());
	int w=0;
    for (int i = 0; i < count; i++)
        w += table->columnWidth(i);
        
    int maxW = 
        (w +                                                      // total column width
        count +                                                  // to account for the pixel(s) used in the grid
        table->verticalHeader()->width() + 
		table->verticalScrollBar()->width());   // Need room for the vertical scrollbar


	count = localParameters->rowCount(QModelIndex());
	int h=0;
    for (int i = 0; i < count; i++)
		h += table->rowHeight(i);
        
    int maxH = 
        (h +                                                      // total column width
        count +                                                  // to account for the pixel(s) used in the grid
		table->horizontalHeader()->height() + 
		table->horizontalScrollBar()->height());   // Need room for the vertical scrollbar

  
	resize(maxW+2,maxH+40);

	newLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum),1,0);

	QPushButton *pushCancel = new QPushButton("Cancel",this);
	QPushButton *pushOk = new QPushButton("Ok",this);

	newLayout->addWidget(pushCancel,1,1);
	newLayout->addWidget(pushOk,1,2);

	connect(pushCancel,SIGNAL(clicked()),this,SLOT(pushedCancel()));
	connect(pushOk,SIGNAL(clicked()),this,SLOT(pushedOk()));

	pushOk->setDefault(true);

	return newLayout;
}






void ModModelParametersDlg::pushedOk()
{

	orgParameters->cloneFromOtherVector(localParameters);
	/*int nbParameters = ListEA::EAs[iEA].nbParameters;
	QVariant value;
	ListEA::AlgoParameter curParam;
	QWidget *widget;


	for(int i=0;i<nbParameters;i++)
	{
		curParam = ListEA::EAs[iEA].parameters[i];
		widget = ((QGridLayout*)layout())->itemAtPosition(i,1)->widget();

		if(!QString::compare(curParam.type,"int",Qt::CaseInsensitive))
		{
			value = ((QSpinBox*)widget)->value();
		}
		else
		{
			if(!QString::compare(curParam.type,"double",Qt::CaseInsensitive))
			{
				value = ((QDoubleSpinBox*)widget)->value();
			}
		}
		EAUtils::setEAConfigFieldValue(eaConfig,iEA,i,value);

	}*/

	accept();
}

void ModModelParametersDlg::pushedCancel()
{
	delete localParameters;
	reject();
}
