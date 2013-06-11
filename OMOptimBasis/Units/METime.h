// $Id: METime.h 9551 2011-07-28 16:56:59Z hubert.thieriot $
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

        @file METime.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
        @version

  */
#if !defined(_METime_H)
#define _METime_H


#include "MEDimValue.h"
#include <cmath>
#include <limits>

class METime : public MEDimValue
{
public:
    METime();
    METime(double value,int unit); // better not to have default value for unit : forbid unwished conversion from double.
    METime(const METime&);

    ~METime();

    enum Units
    {
        SEC
    };

    static METime inf(){return METime(std::numeric_limits<double>::infinity(),METime::SEC);}

    QString unit(int iUnit)  const;
    QString unit() const;  //should'nt be (should be inherited) but error in compiler otherwise
    unsigned nbUnits() const;
    double convert(double value,int orgUnit,int dstUnit) const;

    METime& operator+=(const METime&);
    METime& operator-=(const METime&);
    METime operator-(const METime&) const;
    METime operator+(const METime&) const;
    double operator/(const METime&) const;
};


class METimes : public QList<METime>
{
public :
    METime time(int iP, bool &ok, QString &msg) const;
    int iPeriod(const METime & time, bool lastIsEndingTime) const;
    METimes& operator=(const QList<METime> &);

};
class MEDurations : public QList<METime>
{
public :
    MEDurations(){};
    MEDurations (QList<METime> times);
    METime startingTime(int iP, bool &ok, QString &msg) const;
    int iPeriod(const METime & time) const;
    MEDurations& operator=(const QList<METime> &);
    METimes allTimes() const;
    METime at(int i) const;
    void push_back(const METime duration);

    void setDuration(const METime duration, int iPeriod);
    void cutTotalDuration(const METime duration);
private:
    static METime treshold(){return METime(1e30,METime::SEC);} // treshold to be consider as infinity
};



#endif
