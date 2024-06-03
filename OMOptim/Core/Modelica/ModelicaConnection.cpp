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

     @file ModelicaConnection.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "ModelicaConnection.h"

ModelicaConnection::ModelicaConnection(void)
{

}

ModelicaConnection::ModelicaConnection(QString a,QString b)
{
    _a = a;
    _b = b;
}

ModelicaConnection::ModelicaConnection(QDomElement & domEl)
{
    QDomNamedNodeMap attributes = domEl.attributes();
    QString fieldName;
    QString fieldValue;

    for(int i=0;i<attributes.count();i++)
    {
        fieldName = attributes.item(i).toAttr().name();
        fieldName.replace(XMLTools::space()," ");
        fieldValue = attributes.item(i).toAttr().value();
        fieldValue.replace(XMLTools::space()," ");

        MOItem::setFieldValue(fieldName,QVariant(fieldValue));
    }
}

ModelicaConnection::~ModelicaConnection(void)
{
}

QString ModelicaConnection::getA()
{
    return _a;
}

QString ModelicaConnection::getB()
{
    return _b;
}

void ModelicaConnection::setA(QString a)
{
    _a = a;
}
void ModelicaConnection::setB(QString b)
{
    _b = b;
}


QString ModelicaConnection::sFieldName(int ifield, int role)
{
    switch (ifield)
    {
        case NAME :
            return "Name";
        case A :
            return "Org";
        case B :
            return "Dest";
        default :
            return "unknown field";
    }
}


QVariant ModelicaConnection::getFieldValue(int ifield, int role) const
{
 if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
        return QString("-");
    else
    {
        switch (ifield)
        {
        case NAME :
            return _name;
        case A :
            return _a;
        case B :
            return _b;
        default :
            return "unknown field";
        }
    }
}


bool ModelicaConnection::setFieldValue(int ifield,QVariant value_)
{
    try{
    switch (ifield)
    {
        case NAME :
            _name=value_.toString();
            break;
        case A :
            _a = QString::number(value_.toDouble());
            break;
        case B :
            _b = value_.toString();
            break;
    }
    if(!_filledFields.contains(ifield))
        _filledFields.push_back(ifield);
    return true;
    }
    catch(std::exception)
    {
        return false;
    }
}

