// $Id: METime.cpp 9551 2011-07-28 16:56:59Z hubert.thieriot $
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

     @file METime.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
        @version

  */
#include "METime.h"
#include "InfoSender.h"

METime::METime():
    MEDimValue()
{
}


METime::METime(double value,int unit):
    MEDimValue(value,unit,true)
{
}

METime::METime(const METime& temp):
    MEDimValue(temp._value,temp._unit,temp.isValid())
{
}

METime::~METime(void)
{
}

QString METime::unit(int iUnit) const
{
    switch(iUnit)
    {
    case SEC :
        return "s";
    default :
        return "-";
    }
}

QString METime::unit() const
{
    return unit(_unit);
}

unsigned METime::nbUnits() const
{
    return 1;
}

double METime::convert(double value,int orgUnit,int dstUnit) const
{
    double result=value;
    // convert to m2
    switch(orgUnit)
    {
    //        case CM2 :
    //                result=result/1E4;
    //        break;
    default :
        break;
    }

    // convert to dstUnit
    switch(dstUnit)
    {
    //        case CM2 :
    //                result=result*1E4;
    //        break;
    default :
        break;
    }
    return result;
}


METime& METime::operator+=(const METime& b)
{
    setValue(value(_unit) + b.value(_unit),_unit);
    return *this;
}

METime& METime::operator-=(const METime& b)
{
    setValue(value(_unit) - b.value(_unit),_unit);
    return *this;
}

METime METime::operator-(const METime& b) const
{
    return METime(value(_unit) - b.value(_unit),_unit);
}

METime METime::operator+(const METime& b) const
{
    return METime(value(_unit) + b.value(_unit),_unit);
}

double METime::operator/(const METime& b) const
{
    double result = value(_unit) / b.value(_unit);
    return result;
}

METime METimes::time(int iP, bool & ok, QString & msg) const
{
    if((iP<0)||(iP>=this->size()))
    {
        ok = false;
        msg = "Not valid period index ("+QString::number(iP)+") : ";
        msg+=" \n EIItem has only "+QString::number(this->size())+" periods";
        return METime();
    }
    else
    {
        ok = true;
        msg.clear();
        return this->at(iP);
    }
}

int METimes::iPeriod(const METime & time, bool lastIsEndingTime) const
{
    if(this->isEmpty() || time<this->at(0))
        return -1;

    if(lastIsEndingTime && (time>=this->last()))
        return -1;

    int i=0;
    while((i<this->size()-1)&&(time>=this->at(i+1)))
        i++;


    return i;
}

METimes &METimes::operator =(const QList<METime> & times)
{
    this->clear();
    for(int i=0;i<times.size();i++)
        this->push_back(times.at(i));
}


MEDurations::MEDurations(QList<METime> times)
{
    METime curDuration;
    for(int i=0;i<times.size()-1;i++)
    {
        curDuration = times.at(i+1)-times.at(i);
        this->push_back(curDuration);
    }
}

METime MEDurations::startingTime(int iP, bool & ok, QString & msg) const
{
    if((iP<0)||(iP>=this->size()))
    {
        ok = false;
        msg = "Not valid period index ("+QString::number(iP)+") : ";
        msg+=" \n EIItem has only "+QString::number(this->size())+" periods";
        return METime();
    }
    else
    {
        ok = true;
        msg.clear();
        METime result(0,METime::SEC);
        for(int i=0;i<iP;i++)
            result += this->at(i);
        return result;
    }
}

int MEDurations::iPeriod(const METime & time) const
{
    if(this->isEmpty() || time.value()<0)
        return -1;

    METime offset = METime(0,0);
    int i=0;
    while((i<this->size())&&(time>=offset+this->at(i))&&(this->at(i)!=METime::inf()))
    {
        offset = offset + this->at(i);
        i++;
    }

    return i;
}

MEDurations &MEDurations::operator =(const QList<METime> & durations)
{
    this->clear();
    for(int i=0;i<durations.size();i++)
        this->push_back(durations.at(i));
}

METimes MEDurations::allTimes() const
{
    int i = 0;
    METimes result;
    METime offset(0,0);
    result.push_back(offset);

    while(i<this->size())
    {
        if(this->at(i)==METime::inf())
        {
            result.push_back(METime::inf());
            i = this->size();//stop here
        }
        else
        {
            offset = offset + this->at(i);
            result.push_back(offset);
            i++;
        }
    }
    return result;
}

METime MEDurations::at(int i) const
{
    return QList<METime>::at(i);
}

void MEDurations::push_back(const METime duration)
{
    if(duration>=treshold())
        QList<METime>::push_back(METime::inf());
    else
        QList<METime>::push_back(duration);
}

void MEDurations::setDuration(const METime duration, int iPeriod)
{
    while(size()<=iPeriod)
    {
        push_back(METime(0,METime::SEC));
    }
    if(duration>=treshold())
        replace(iPeriod,METime::inf());
    else
        replace(iPeriod,duration);

}

void MEDurations::cutTotalDuration(const METime totalDuration)
{
    int i = 0;
    METime soFar(0,0);
    bool reached = false;
    while((i<size()) && (!reached))
    {
        if(soFar+this->at(i)>=totalDuration)
        {
            replace(i,totalDuration-soFar);
            while(size()!=i+1)
                removeAt(i+1);
            reached = true;
        }
        else
        {
            soFar += this->at(i);
            i++;
        }
    }
}
