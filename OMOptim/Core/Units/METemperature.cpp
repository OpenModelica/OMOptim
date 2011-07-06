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

 	@file METemperature.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "METemperature.h"

METemperature::METemperature(double value,int unit):
MEDimValue(value,unit)
{
}

METemperature::METemperature(const METemperature& temp):
MEDimValue(temp._value,temp._unit)
{
}

METemperature::~METemperature(void)
{
}

QString METemperature::unit(int iUnit) const
{
	switch(iUnit)
	{
	case K :
		return "K";
	case C :
		return "C";
	default :
		return "-";
	}
}

QString METemperature::unit() const
{
        return unit(_unit);
}

unsigned METemperature::nbUnits() const
{
	return 2;
}

double METemperature::convert(double value,int orgUnit,int dstUnit) const
{
	double result=value;
	// convert to K
	switch(orgUnit)
	{
	case C :
		result=result+273.15;
		break;
	default :
		break;
	}

	// convert to dstUnit
	switch(dstUnit)
	{
	case C :
		result=result-273.15;
		break;
	default :
		break;
	}
	return result;
}


double METemperature::operator-(const METemperature& b) const
{
    double resValue = value(_unit)-b.value(_unit);
    return resValue;
}

bool METemperature::operator==(const METemperature& b) const
{
    return (value(_unit)==b.value(_unit));
}

bool METemperature::operator!=(const METemperature& b) const
{
    return !(*this==b);
}

METemperature& METemperature::operator+=(const double& diffTemp)
{
    _value += diffTemp;
    return *this;
}

METemperature& METemperature::operator-=(const double& diffTemp)
{
    _value -= diffTemp;
    return *this;
}

bool METemperature::operator<(const METemperature& b) const
{
    return value(_unit)<b.value(_unit);
}
bool METemperature::operator>(const METemperature& b) const
{
    return value(_unit)>b.value(_unit);
}

bool METemperature::operator<=(const METemperature& b) const
{
    return value(_unit)<=b.value(_unit);
}
bool METemperature::operator>=(const METemperature& b) const
{
    return value(_unit)>=b.value(_unit);
}
