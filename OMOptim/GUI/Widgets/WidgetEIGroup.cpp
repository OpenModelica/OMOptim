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

 	@file WidgetEIGroup.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetEIGroup.h"


WidgetEIGroup::WidgetEIGroup(MOItem *_item,QWidget *parent,bool _forceEditable):
WidgetMOItem(_item,parent,_forceEditable)
{
    connect(this,SIGNAL(valueChanged()),this,SLOT(onValueChanged()));
}

WidgetEIGroup::~WidgetEIGroup()
{
}


QWidget* WidgetEIGroup::createEditWidget(MOItem* item, int iField, bool forceEditable)
{
	QDoubleSpinBox *newDblSpin;
	switch (iField)
	{
	case EIGroup::FACTISVARIABLE :
	case EIGroup::CHECKED :
		return new QCheckBox();
	case EIGroup::FACTMIN :
	case EIGroup::FACTMAX :
	case EIGroup::FACT :
	case EIGroup::COSTMULT :
	case EIGroup::COSTFIX :
		newDblSpin = new QDoubleSpinBox();
		newDblSpin->setMinimum(0);
		newDblSpin->setMaximum(1E10);
		return newDblSpin;
	default :
		return NULL;
	}
}

void WidgetEIGroup::onValueChanged()
{

    EIGroup* eiGroup = dynamic_cast<EIGroup*>(item);

    if(eiGroup)
    {
        bool factIsVariable = eiGroup->getFieldValue(EIGroup::FACTISVARIABLE).toBool();

        fieldEditWidget.value(EIGroup::FACT)->setEnabled(factIsVariable);
        fieldEditWidget.value(EIGroup::FACTMAX)->setEnabled(factIsVariable);
        fieldEditWidget.value(EIGroup::FACTMIN)->setEnabled(factIsVariable);
        fieldEditWidget.value(EIGroup::COSTMULT)->setEnabled(factIsVariable);
    }
}
