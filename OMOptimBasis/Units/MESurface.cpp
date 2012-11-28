// $Id: MESurface.cpp 9551 2011-07-28 16:56:59Z hubert.thieriot $
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

     @file MESurface.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
        @version

  */
#include "MESurface.h"

MESurface::MESurface():
MEDimValue()
{
}


MESurface::MESurface(double value,int unit):
MEDimValue(value,unit,true)
{
}

MESurface::MESurface(const MESurface& temp):
MEDimValue(temp._value,temp._unit,temp.isValid())
{
}

MESurface::~MESurface(void)
{
}

QString MESurface::unit(int iUnit) const
{
    switch(iUnit)
    {
        case M2 :
                return "m2";
        case CM2 :
                return "cm2";
    default :
        return "-";
    }
}

QString MESurface::unit() const
{
        return unit(_unit);
}

unsigned MESurface::nbUnits() const
{
    return 2;
}

double MESurface::convert(double value,int orgUnit,int dstUnit) const
{
    double result=value;
        // convert to m2
    switch(orgUnit)
    {
        case CM2 :
                result=result/1E4;
        break;
    default :
        break;
    }

    // convert to dstUnit
    switch(dstUnit)
    {
        case CM2 :
                result=result*1E4;
        break;
    default :
        break;
    }
    return result;
}


MESurface& MESurface::operator+=(const MESurface& b)
{
    setValue(value(_unit) + b.value(_unit),_unit);
    if(!b.isValid())
        this->invalidate();
    return *this;
}

MESurface& MESurface::operator-=(const MESurface& b)
{
    setValue(value(_unit) - b.value(_unit),_unit);
    if(!b.isValid())
        this->invalidate();
    return *this;
}

MESurface MESurface::operator-(const MESurface& b) const
{
    MESurface result(value(_unit) - b.value(_unit),_unit);
    if(!b.isValid())
        result.invalidate();

    return result;
}

MESurface MESurface::operator+(const MESurface& b) const
{
    MESurface result(value(_unit) + b.value(_unit),_unit);
    if(!b.isValid())
        result.invalidate();

    return result;
}

