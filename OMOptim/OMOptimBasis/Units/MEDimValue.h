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

     @file MEDimValue.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#if !defined(_MEDIMVALUE_H)
#define _MEDIMVALUE_H

#include <QtCore/QObject>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <cmath>

class MEDimValue
{
public:
    MEDimValue();
    MEDimValue(double value,int unit);
    virtual ~MEDimValue(void);

    QStringList units()  const ;
    virtual QString unit() const ;
    int iUnit() const ;
    virtual QString unit(int iUnit) const =0;
    virtual unsigned nbUnits() const =0;
    virtual void setValue(double,int iUnit=-1);
    virtual bool setValue(double,QString unit);
    virtual void setUnit(int iUnit);
    virtual bool setUnit(QString iUnit);
    double value(int iUnit) const;
    double value() const;
    QString strValue(int iUnit) const;
    QString strValue() const;
    
    virtual QString toString() const {return QString::number(value(_unit))+" "+unit();}

    virtual MEDimValue & operator=(const MEDimValue &);
    virtual bool operator<(const MEDimValue &) const;
    virtual bool operator>(const MEDimValue &) const;
    virtual bool operator==(const MEDimValue &) const;
    virtual bool operator!=(const MEDimValue &) const;
    virtual bool operator<=(const MEDimValue &) const;
    virtual bool operator>=(const MEDimValue &) const;
    virtual bool equalsRel(const MEDimValue& b,double maxRelDistance) const;


    bool isValid() const;
    void invalidate();
    void validate();




protected :
    int _unit;
    double _value;
    bool _isValid;

    virtual double convert(double value,int orgUnit,int dstUnit) const =0 ;
};


template<class DimValue>
class MEDimValueTools
{
public :
    /**
      * Convert a list of MEDimValue to a QString
      * Used for GUI or saving/load functions
      */
    static QString listToString(const QList<DimValue> &list, int iUnit)
    {
        QString separator = ";";
        QString result="\[";
        bool allAreId = true; // if all are id then copy only num value
        for(int i=0;i<list.size();i++)
        {
            result+=QString::number(list.at(i).value(iUnit));
            result+=separator;
            allAreId = allAreId && (list.at(i).value(iUnit)==list.at(0).value(iUnit));
        }
        if(allAreId)
            return QString::number(list.at(0).value(iUnit));
        else
        {
            result.remove(result.size()-separator.size(),separator.size());
            result.push_back("\]");
            return result;
        }
    }

    /**
      * Convert a Qstring to a list of DimValue
      */
    static QList<DimValue> stringToList(QString str,int iUnit, bool &ok)
    {
        bool tmpOk;
        QList<DimValue> result;
        QString separator = ";"; // be sure to set the same in listToString function.
        str = str.remove("\[");
        str = str.remove("\]");


        QStringList strList = str.split(separator);
        ok = !strList.isEmpty();

        for(int i=0;i<strList.size();i++)
        {
            DimValue newT(strList.at(i).toDouble(&tmpOk),iUnit);
            // if field was empty, stream not in concerned period
            if(!tmpOk)
            {
                newT.invalidate();
            }
            result.push_back(newT);
        }
        ok = true;
        return result;
    }

};


#endif
