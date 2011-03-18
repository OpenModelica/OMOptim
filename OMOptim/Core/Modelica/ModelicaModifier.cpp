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

 	@file ModelicaModifier.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ModelicaModifier.h"

ModelicaModifier::ModelicaModifier(void)
{
	_description = "_";
	_name = "_";
}

ModelicaModifier::ModelicaModifier(QString componentName,QString modifierName,QString value,QString description,ModClass* component)
{
	_componentName = componentName;
	_name = modifierName;
	_value = value;
	_description = description;
	_component = component;

}

ModelicaModifier::~ModelicaModifier(void)
{
}

ModelicaModifier* ModelicaModifier::clone()
{
	ModelicaModifier* newOne = new ModelicaModifier(_componentName,_name,_value,_description,_component);
	newOne->_filledFields = _filledFields;
	return newOne;
}

QString ModelicaModifier::name()
{
	return _name;
}
QString ModelicaModifier::description()
{
	return _description;
}
QVariant ModelicaModifier::value()
{
	return _value;
}

void ModelicaModifier::setName(QString name)
{
	if(!_filledFields.contains(ModelicaModifier::NAME))
		_filledFields.push_back(ModelicaModifier::NAME);
	_name = name;
}
void ModelicaModifier::setDesc(QString desc)
{
	if(!_filledFields.contains(ModelicaModifier::DESCRIPTION))
		_filledFields.push_back(ModelicaModifier::DESCRIPTION);
	
	_description = desc;
}
void ModelicaModifier::setValue(QString value)
{
	if(!_filledFields.contains(ModelicaModifier::VALUE))
		_filledFields.push_back(ModelicaModifier::VALUE);
	
	_value = value;
}

void ModelicaModifier::setComponent(ModClass *component)
{
	if(!_filledFields.contains(ModelicaModifier::COMPONENT_NAME))
		_filledFields.push_back(ModelicaModifier::COMPONENT_NAME);
	_component = component;
	_componentName = component->name(Modelica::DYMOLA);
}


QVariant ModelicaModifier::getFieldValue(int ifield, int role) const
{
	if (!_filledFields.contains(ifield))
		return QString("-");
	else
	{
		switch (ifield)
		{
		case NAME :
			return _name;
		case VALUE :
			return _value;
		case DESCRIPTION :
			return _description;
		case COMPONENT_NAME :
			return _componentName;
		default :
			return "unknown field";
		}
	}
}

QString ModelicaModifier::sFieldName(int ifield, int role)
{
	switch (ifield)
	{
		case NAME :
			return "Name";
		case VALUE :
			return "Value";
		case DESCRIPTION :
			return "Description";
		case COMPONENT_NAME :
			return "Component";
		default :
			return "unknown field";
	}
}

bool ModelicaModifier::setFieldValue(int ifield,QVariant value)
{
	try{
	switch (ifield)
	{
		case NAME :
			_name=value.toString();
			break;
		case VALUE :
			_value=value.toString();
			break;
		case DESCRIPTION :
			_description = value.toString();
			break;
		case COMPONENT_NAME :
			_componentName = value.toString();
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

