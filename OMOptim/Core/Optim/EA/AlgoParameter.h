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

     @file AlgoParameter.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#if !defined(_ALGOPARAMETER_H)
#define _ALGOPARAMETER_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#endif

#include "MOItem.h"
#include "MOVector.h"

class AlgoParameter : public MOItem
{
public:

    enum Field
    {
        //Modelica fields
        NAME,
        VALUE,
        DEFAULTVALUE,
        TYPE,
        MIN,
        MAX,
        DESCRIPTION
    };



    AlgoParameter();
    AlgoParameter(const AlgoParameter &);
    AlgoParameter(QString _name,QString _desc,QVariant _defaultValue, QString _type, QVariant _minValue=0, QVariant _maxValue=1);
    AlgoParameter(QString);
    AlgoParameter(QDomElement & domEl);
    ~AlgoParameter(void);

    virtual QString getClassName() const {return "AlgoParameter";};

    static const int nbFields = 7;
    virtual unsigned getNbFields( ) const {return nbFields;};


    bool setFieldValue(int ,QVariant );
    QVariant getFieldValue(int ifield, int role = Qt::UserRole) const;
    static QString sFieldName(int ifield, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole) const {return AlgoParameter::sFieldName(i,role);};

    AlgoParameter* clone() const;

private :
    QString _description;
    QVariant _value;
    QVariant _defaultValue;
    QString _type;
    QVariant _min;
    QVariant _max;


};






#endif
