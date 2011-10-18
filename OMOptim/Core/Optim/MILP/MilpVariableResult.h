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

        @file MilpVariableResult.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */



#ifndef MilpVariableResult_H
#define MilpVariableResult_H


#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include "MilpTools.h"


class MilpVariableResult
{
public:
    MilpVariableResult(QString name);

    //virtual void fill(glp_prob * glpProblem,double defaultValue,QStringList colNames = QStringList()) = 0;
    QString name(){return _name;};


protected :
        QString _name;

};


class MilpVariableResult0D : public MilpVariableResult
{
public:
    MilpVariableResult0D(QString name);

    double value() const{return _value;};
    void setValue(double value){_value = value;};

private :
    double _value;
};


class MilpVariableResult1D : public MilpVariableResult
{
public:
    MilpVariableResult1D(QString name);


    QMap<QString,double> values() const{return _values;};
    void clear(){_values.clear();};
    void insertValue(QString key,double value){_values.insert(key,value);};




private :
  QMap<QString,double> _values;
};



class MilpVariableResult2D : public MilpVariableResult
{
public:
    MilpVariableResult2D(QString name);

    void clear(){_values.clear();};
    void insertValue(MilpKey2D key,double value){_values.insert(key,value);};
    QMap<MilpKey2D,double> values()const {return _values;};



private :
    QMap<MilpKey2D,double> _values;
};


class MilpVariableResult3D : public MilpVariableResult
{
public:
    MilpVariableResult3D(QString name);

    void clear(){_values.clear();}
    void insertValue(MilpKey3D key,double value){_values.insert(key,value);}
    QMap<MilpKey3D,double> values() const{return _values;}



private :

    QMap<MilpKey3D,double> _values;
};

class MilpVariableResult4D : public MilpVariableResult
{
public:
    MilpVariableResult4D(QString name);

    void clear(){_values.clear();}
    void insertValue(MilpKey4D key,double value){_values.insert(key,value);}
    QMap<MilpKey4D,double> values()const {return _values;}

    QString toString() const;

private :

    QMap<MilpKey4D,double> _values;

};

#endif // MilpVariableResult_H
