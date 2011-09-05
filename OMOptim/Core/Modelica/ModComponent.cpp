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

 	@file ModComponent.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#include "ModComponent.h"

ModComponent::ModComponent(MOomc* moomc):ModClass(moomc)
{
}

ModComponent::ModComponent(MOomc* moomc,ModClass* parent,QString name,QString modClassName,QString filePath)
:ModClass(moomc,parent,name,filePath)
{
	_modClassName = modClassName;
}

ModComponent::~ModComponent()
{
	clearDescendants();
}

ModClass* ModComponent::clone() const
{
    ModComponent* newModComp = new ModComponent(_moomc,_parent,_name,_modClassName,_filePath);
    newModComp->_childrenReaden = _childrenReaden;

    for(int i=0;i<children().size();i++)
    {
        newModComp->addChild(child(i)->clone());
    }
    return newModComp;
}


QString ModComponent::getModClassName()
{
	return _modClassName;
}

QVariant ModComponent::getFieldValue(int iField, int role) const
{

	switch(iField)
	{
	case NAME:
		return _name;
	case MODCLASSNAME:
		return _modClassName;
	case FILEPATH:
		return _filePath;
	default :
		return QVariant();
	}
}

bool ModComponent::setFieldValue(int iField, QVariant value)
{
	try{
		switch(iField)
		{
		case NAME:
			_name = value.toString();
			break;
		case MODCLASSNAME:
			_modClassName = value.toString();
		case FILEPATH:
			_filePath = value.toString();
			break;
		}
		return true;
	}
	catch(std::exception)
	{
		return false;
	}
}

QString ModComponent::sFieldName(int iField, int role)
{
	switch(iField)
	{
	case NAME:
		return "Name";
	case MODCLASSNAME:
		return "Class";
	case FILEPATH:
		return "FilePath";
	default:
		return "-";
	}
}

QString ModComponent::getStrToolTip()
{
	QString toolTip;
	toolTip += "Modelica Component \n";
	toolTip += "Name : " + _name + "\n";
	toolTip += "Class: " + _modClassName + "\n";

	return toolTip;
}
