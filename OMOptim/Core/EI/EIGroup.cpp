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

 	@file EIGroup.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIGroup.h"

EIGroup::EIGroup():EIItem(NULL,"Group")
{
	setFieldValue(COSTFIX,0);
	setFieldValue(COSTMULT,1);
}

EIGroup::EIGroup(EIItem* _parent,QString _name)
:EIItem(_parent,_name)
{
	setFieldValue(COSTFIX,0);
	setFieldValue(COSTMULT,1);
}

EIGroup::~EIGroup()
{
	clearDescendants();
}

EIGroup::EIGroup(const EIGroup & _group):EIItem(_group)
{
	factIsVariable = _group.factIsVariable;
	fact = EIGroupFact(_group.fact);
	costFix = _group.costFix;
	costMult = _group.costMult;
}



EIItem* EIGroup::clone()
{
	return (EIItem*)(new EIGroup(*this));
}

EIGroup::EIGroup(QDomElement & domEl)
{
	setFieldValue(COSTFIX,0);
	setFieldValue(COSTMULT,1);

	QDomNamedNodeMap attributes = domEl.attributes();
	QString fieldName;
	QString fieldValue;

	for(int i=0;i<attributes.count();i++)
	{
		fieldName = attributes.item(i).toAttr().name();
		fieldName.replace(XMLTools::space()," ");
		fieldValue = attributes.item(i).toAttr().value();
		fieldValue.replace(XMLTools::space()," ");
		MOItem::setFieldValue(fieldName,QVariant(fieldValue));
	}

	_editableFields << EIGroup::NAME;
}


QVariant EIGroup::getFieldValue(int ifield, int role) const
{
         if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
		return QString("-");
	else
	{
		switch (ifield)
		{
		case NAME :
			return _name;
		case FACTISVARIABLE :
			return factIsVariable;
		case FACTMIN :
			return fact.min;
		case FACTMAX :
			return fact.max;
		case FACT :
			return fact.value;
		case COSTFIX :
			return costFix;
		case COSTMULT :
			return costMult;
		case CHECKED :
                        return _checked;
		default :
			return "unknown field";
		}
	}
}

QString EIGroup::sFieldName(int ifield, int role)
{
	switch (ifield)
	{
	case NAME :
		return "Name";
	case FACTISVARIABLE :
		return "FactVariable";
	case FACTMIN :
		return "FMin";
	case FACTMAX :
		return "FMax";
	case FACT :
		return "FValue";
	case COSTFIX :
		return "FixCost";
	case COSTMULT :
		return "VarCost";
	case CHECKED :
		return "Checked";
	default :
		return "unknown field";
	}
}



bool EIGroup::setFieldValue(int ifield,QVariant value_)
{
	bool ok=true;

	switch (ifield)
	{
	case NAME :
		_name = value_.toString();
		break;
	case FACTISVARIABLE :
		factIsVariable = value_.toBool();
		break;
	case FACTMIN :
		fact.min = value_.toDouble();
		break;
	case FACTMAX :
		fact.max = value_.toDouble();
		break;
	case FACT :
		fact.value = value_.toDouble();
		break;
	case COSTFIX :
		costFix = value_.toDouble();
		break;
	case COSTMULT :
		costMult = value_.toDouble();
		break;
	case CHECKED :
                _checked =value_.toBool();
		break;
	}
	if(!_filledFields.contains(ifield))
		_filledFields.push_back(ifield);
	
	emit modified();
	return ok;
}


EIGroupFact* EIGroup::getFact()
{
	return &fact;
}

bool EIGroup::isFactVariable()
{
	return factIsVariable;
}
