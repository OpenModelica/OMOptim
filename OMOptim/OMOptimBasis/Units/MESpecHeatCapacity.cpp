// $Id: MESpecHeatCapacity.cpp 9551 2011-07-28 16:56:59Z hubert.thieriot $
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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

  @file MESpecHeatCapacity.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
        @version

  */
#include "MESpecHeatCapacity.h"
MESpecHeatCapacity::MESpecHeatCapacity():
    MEDimValue()
{
}

MESpecHeatCapacity::MESpecHeatCapacity(double value,int unit):
    MEDimValue(value,unit)
{
}

MESpecHeatCapacity::MESpecHeatCapacity(const MESpecHeatCapacity& temp):
    MEDimValue(temp._value,temp._unit)
{
}

MESpecHeatCapacity::~MESpecHeatCapacity(void)
{
}

QString MESpecHeatCapacity::unit(int iUnit) const
{
    switch(iUnit)
    {
    case MJ_KG_K :
        return "MJ/kg.K";
    case KJ_KG_K :
        return "kJ/kg.K";
    case J_KG_K :
        return "J/kg.K";
    default :
        return "-";
    }
}

QString MESpecHeatCapacity::unit() const
{
    return unit(_unit);
}

unsigned MESpecHeatCapacity::nbUnits() const
{
    return 3;
}

double MESpecHeatCapacity::convert(double value,int orgUnit,int dstUnit) const
{
    double result=value;
    // convert to j/kg.K
    switch(orgUnit)
    {
    case KJ_KG_K :
        result = result*1000;
        break;
    case MJ_KG_K :
        result = result*1E6;
        break;
    default :
        break;
    }

    // convert to dstUnit
    switch(dstUnit)
    {
    case KJ_KG_K :
        result = result/1000;
        break;
    case MJ_KG_K :
        result = result/1E6;
        break;
    default :
        break;
    }
    return result;
}



MESpecHeatCapacity& MESpecHeatCapacity::operator+=(const MESpecHeatCapacity& b)
{
    setValue(value(_unit) + b.value(_unit),_unit);
    return *this;
}

MESpecHeatCapacity& MESpecHeatCapacity::operator-=(const MESpecHeatCapacity& b)
{
    setValue(value(_unit) - b.value(_unit),_unit);
    return *this;
}

MESpecHeatCapacity MESpecHeatCapacity::operator-(const MESpecHeatCapacity& b) const
{
    return MESpecHeatCapacity(value(_unit) - b.value(_unit),_unit);
}

MESpecHeatCapacity MESpecHeatCapacity::operator+(const MESpecHeatCapacity& b) const
{
    return MESpecHeatCapacity(value(_unit) + b.value(_unit),_unit);
}


