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

 	@file MERefValue.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if defined(_MEREFVALUE_H)

#include "MERefValue.h"

template<class DimValue>
MERefValue<DimValue>::MERefValue(QVariant value,int unit)
{
	setValue(value,unit);
	dimValue = new DimValue();
}

template<class DimValue>
MERefValue<DimValue>::~MERefValue(void)
{
}

template<class DimValue>
QVariant MERefValue<DimValue>::value() const
{
	return _value;
}

template<class DimValue>
void MERefValue<DimValue>::setValue(QVariant value,int iUnit)
{
	_value = value; 
	if(iUnit>-1)
		_unit = iUnit;
}

template<class DimValue>
bool MERefValue<DimValue>::setValue(QVariant value,QString unit)
{
	int iUnit = units().indexOf(unit);
	if(iUnit==-1)
		return false;
	else
		setValue(value,iUnit);
	return true;
}

template<class DimValue>
bool MERefValue<DimValue>::setUnit(QString unit)
{
	int iUnit = units().indexOf(unit);
	if(iUnit==-1)
		return false;
	else
		setUnit(iUnit);
	return true;
	
}

template<class DimValue>
void MERefValue<DimValue>::setUnit(int iUnit)
{
	_unit = iUnit;
}

template<class DimValue>
QStringList MERefValue<DimValue>::units() const
{
	return dimValue->units();
}

template<class DimValue>
QString MERefValue<DimValue>::unit() const
{
	return units().at(_unit);
}

template<class DimValue>
int MERefValue<DimValue>::iUnit() const
{
	return _unit;
}

template<class DimValue>
QString MERefValue<DimValue>::unit(int iUnit) const
{
	return units().at(iUnit);
}

template<class DimValue>
unsigned MERefValue<DimValue>::nbUnits() const
{
	return dimValue->nbUnits();
}

template<class DimValue>
double MERefValue<DimValue>::getNumValue(MOOptVector *variables,int iUnit,bool &ok)
{
	bool isNum;
	double result = _value.toDouble(&isNum);
	if(isNum)
	{
		dimValue->setValue(result,_unit);
		ok=true;
		return dimValue->value(iUnit);
	}
	else
	{
		// looking in variables
		int iVar = variables->findItem(_value.toString());
		if(iVar==-1)
		{
			ok = false;
			return -1;
		}
		else
		{
			ok =  true;
			result = variables->items.at(iVar)->finalValue(variables->curScan(),variables->curPoint());
			result = dimValue->convert(result,this->iUnit(),iUnit);
			return result;
		}
	}
}

#endif
