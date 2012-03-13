// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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

     @file MEDimValue.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 

  */
#include "MEDimValue.h"

MEDimValue::MEDimValue()
{
    invalidate();
}

MEDimValue::MEDimValue(double value,int unit)
{
    setValue(value,unit);
}

MEDimValue::~MEDimValue(void)
{
}

double MEDimValue::value(int iUnit) const
{
    return convert(_value,_unit,iUnit);
}

double MEDimValue::value() const
{
        return _value;
}

QString MEDimValue::strValue(int iUnit) const
{
    if(isValid())
        return QString::number(value(iUnit));
    else
        return "-";
}

QString MEDimValue::strValue() const
{
    if(isValid())
        return QString::number(value());
    else
        return "-";
}


void MEDimValue::setValue(double value,int iUnit)
{
    _value = value; 
    if(iUnit>-1)
        _unit = iUnit;

    validate();
}

void MEDimValue::setUnit(int iUnit)
{
    _unit = iUnit;
}

bool MEDimValue::setUnit(QString unit)
{
    int iUnit = units().indexOf(unit);
    if(iUnit==-1)
        return false;
    else
        setUnit(iUnit);
    return true;

}
bool MEDimValue::setValue(double value,QString unit)
{
    int iUnit = units().indexOf(unit);
    if(iUnit==-1)
        return false;
    else
        setValue(value,iUnit);

    validate();

    return true;
}



QStringList MEDimValue::units()  const
{
    QStringList result;
    for(int i=0;i<nbUnits();i++)
        result.push_back(unit(i));

    return result;
}

QString MEDimValue::unit() const
{
    return unit(_unit);
}

int MEDimValue::iUnit() const
{
    return _unit;
}

MEDimValue & MEDimValue::operator=(const MEDimValue &b)
{
    _value = b._value;
    _unit = b._unit;
    _isValid = b._isValid;
}

bool MEDimValue::operator<(const MEDimValue &b) const
{
    return value(_unit)<b.value(_unit);
}

bool MEDimValue::operator>(const MEDimValue &b) const
{
    return value(_unit)>b.value(_unit);
}

bool MEDimValue::operator==(const MEDimValue &b) const
{
    return value(_unit)==b.value(_unit);
}

bool MEDimValue::operator!=(const MEDimValue &b) const
{
    return value(_unit)!=b.value(_unit);
}


bool MEDimValue::operator<=(const MEDimValue &b) const
{
    return value(_unit)<=b.value(_unit);
}

bool MEDimValue::operator>=(const MEDimValue &b) const
{
    return value(_unit)>=b.value(_unit);
}

/**
  * maxRelDistance corresponds to relative distance between this and b.
  * distance is calculated as follows :
  * dist = abs((a-b)/min(a,b))
  */
bool MEDimValue::equalsRel(const MEDimValue& b,double maxRelDistance) const
{
    double dist = fabs((b.value(_unit)-value(_unit))/std::min(b.value(_unit),value(_unit)));
    return (dist<=maxRelDistance);
}

/**
  * Returns true if this value has been initialized
  */
bool MEDimValue::isValid() const
{
    return _isValid;
}

/**
  * Set this value invalid : its value has no more meaning and is considered as uninitialized.
  * @sa isValid()
  */
void MEDimValue::invalidate()
{
    _isValid = false;
}

/**
  * Set this value valid. Called when value is set.
  * @sa isValid()
  */
void MEDimValue::validate()
{
    _isValid = true;
}


