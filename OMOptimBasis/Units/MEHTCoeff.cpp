// $Id: MEHTCoeff.cpp 9551 2011-07-28 16:56:59Z hubert.thieriot $
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

     @file MEHTCoeff.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
        @version

  */
#include "MEHTCoeff.h"

MEHTCoeff::MEHTCoeff():
    MEDimValue()
{
}

MEHTCoeff::MEHTCoeff(double value,int unit):
    MEDimValue(value,unit,true)
{
}

MEHTCoeff::MEHTCoeff(const MEHTCoeff& temp):
    MEDimValue(temp._value,temp._unit,temp.isValid())
{
}

MEHTCoeff::~MEHTCoeff(void)
{
}

QString MEHTCoeff::unit(int iUnit) const
{
    switch(iUnit)
    {
    case W_M2_K :
        return "W/m2.K";
    case KW_M2_K :
        return "kW/m2.K";
    case MW_M2_K :
        return "MW/m2.K";
    default :
        return "-";
    }
}

QString MEHTCoeff::unit() const
{
    return unit(_unit);
}

unsigned MEHTCoeff::nbUnits() const
{
    return 3;
}

double MEHTCoeff::convert(double value,int orgUnit,int dstUnit) const
{
    double result=value;
    // convert to W/m2
    switch(orgUnit)
    {
    case W_M2_K :
        break;
    case KW_M2_K :
        result = result*1E3;
        break;
    case MW_M2_K :
        result = result*1E6;
        break;
    default :
        break;
    }

    // convert to dstUnit
    switch(dstUnit)
    {
    case W_M2_K :
        break;
    case KW_M2_K :
        result = result/1E3;
        break;
    case MW_M2_K :
        result = result/1E6;
        break;
    default :
        break;
    }
    return result;
}


MEHTCoeff& MEHTCoeff::operator+=(const MEHTCoeff& b)
{
    setValue(value(_unit) + b.value(_unit),_unit);
    if(!b.isValid())
        this->invalidate();
    return *this;
}

MEHTCoeff& MEHTCoeff::operator-=(const MEHTCoeff& b)
{
    setValue(value(_unit) - b.value(_unit),_unit);
    if(!b.isValid())
        this->invalidate();
    return *this;
}

MEHTCoeff MEHTCoeff::operator-(const MEHTCoeff& b) const
{
    MEHTCoeff result(value(_unit) - b.value(_unit),_unit);
    if(!b.isValid())
        result.invalidate();

    return result;
}

MEHTCoeff MEHTCoeff::operator+(const MEHTCoeff& b) const
{
    MEHTCoeff result(value(_unit) + b.value(_unit),_unit);
    if(!b.isValid())
        result.invalidate();

    return result;
}
