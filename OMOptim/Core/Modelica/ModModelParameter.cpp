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

 	@file ModModelParameter.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ModModelParameter.h"


ModModelParameter::ModModelParameter(){
	_editableFields.clear();
	_editableFields << ModModelParameter::VALUE;
}

ModModelParameter::ModModelParameter(const ModModelParameter & param):MOItem(param)
{
	_description = param._description;
	_value = param._value;
	_defaultValue = param._defaultValue;
	_type = param._type;
	_min = param._min;
	_max = param._max;
	_index = param._index;
}

ModModelParameter::ModModelParameter(int index,QString name,QString description, QVariant defaultValue, QString type, QVariant minValue, QVariant maxValue):
_index(index),_description(description),_defaultValue(defaultValue),_type(type),_min(minValue),_max(maxValue)
{
	_name = name;
	_value = _defaultValue;

	_filledFields.push_back(ModModelParameter::DEFAULTVALUE);
	_filledFields.push_back(ModModelParameter::VALUE);
	_filledFields.push_back(ModModelParameter::NAME);
	_filledFields.push_back(ModModelParameter::MIN);
	_filledFields.push_back(ModModelParameter::MAX);
	_filledFields.push_back(ModModelParameter::TYPE);
	_filledFields.push_back(ModModelParameter::DESCRIPTION);
	_filledFields.push_back(ModModelParameter::INDEX);

	_editableFields.clear();
	_editableFields << ModModelParameter::VALUE;
}

ModModelParameter::ModModelParameter(QString savedString)
{
	QStringList fields = savedString.split(" ",QString::SkipEmptyParts);
	if(fields.size()!=nbFields)
	{
		emit sendInfo(Info(ListInfo::PROBLEMREADINGLINE,savedString));
	}
	else
	{
		for(int iF=0;iF<fields.size();iF++)
		{
			QString curField = fields.at(iF);
			curField.remove(" ");
			curField.remove("\t");
			setFieldValue(iF,QVariant(curField));
		}
	}
	
	_editableFields.clear();
	_editableFields << ModModelParameter::VALUE;
}

ModModelParameter::ModModelParameter(QDomElement & domEl)
{
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
}

ModModelParameter::~ModModelParameter(void)
{
}


bool ModModelParameter::setFieldValue(int ifield,QVariant value)
{
	try{
	switch (ifield)
	{
		case NAME :
			_name=value.toString();
			break;
		case DESCRIPTION :
			_description=value.toString();
			break;
		case VALUE :
			_value=value;
			break;
		case DEFAULTVALUE :
			_defaultValue = value;
			break;
		case TYPE :
			_type=value.toString();
			break;
		case MIN :
			_min=value;
			break;
		case MAX :
			_max=value;
			break;
		case INDEX:
			_index=value.toInt();
			break;
	}
	if(!_filledFields.contains(ifield))
		_filledFields.push_back(ifield);
	return true;
	}
	catch(std::exception)
	{
		return false;
	}
}


QVariant ModModelParameter::getFieldValue(int ifield, int role) const
{
	if (!_filledFields.contains(ifield))
		return QString("-");
	else
	{
		switch (ifield)
		{
		case NAME :
			return _name;
		case DESCRIPTION :
			return _description;
		case VALUE :
			return _value;
		case MIN :
			return _min;
		case MAX :
			return _max;
		case TYPE :
			return _type;
		case DEFAULTVALUE :
			return _defaultValue;
		case INDEX :
			return _index;
		default :
			return "unknown field";
		}
	}
}

ModModelParameter* ModModelParameter::clone()
{
	ModModelParameter* newParam = new ModModelParameter();

	for(int i=0;i<nbFields;i++)
	{
		newParam->setFieldValue(i,getFieldValue(i));
	}
	newParam->_filledFields = _filledFields;
	newParam->_editableFields = _editableFields;

	return newParam;

}

QString ModModelParameter::sFieldName(int iField, int role)
{
	switch (iField)
	{
	case NAME :
		return "Name";
	case DESCRIPTION :
		return "Description";
	case VALUE :
		return "Value";
	case MIN :
		return "Min";
	case MAX :
		return "Max";
	case TYPE :
		return "Type";
	case INDEX :
		return "Index";
	case DEFAULTVALUE :
		return "DefaultValue";
	default :
		return "unknown field";
	}
}
