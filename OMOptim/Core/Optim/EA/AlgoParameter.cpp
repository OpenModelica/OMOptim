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

     @file AlgoParameter.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */


#include "AlgoParameter.h"


AlgoParameter::AlgoParameter(){
    setEditableFields(QList<int>()<<  AlgoParameter::VALUE);
}

AlgoParameter::AlgoParameter(const AlgoParameter & param):MOItem(param)
{
    _description = param._description;
    _value = param._value;
    _defaultValue = param._defaultValue;
    _type = param._type;
    _min = param._min;
    _max = param._max;
}

AlgoParameter::AlgoParameter(QString name,QString description, QVariant defaultValue, QString type, QVariant minValue, QVariant maxValue):
_description(description),_defaultValue(defaultValue),_type(type),_min(minValue),_max(maxValue)
{
    _name = name;
    _value = _defaultValue;

    _filledFields.push_back(AlgoParameter::DEFAULTVALUE);
    _filledFields.push_back(AlgoParameter::VALUE);
    _filledFields.push_back(AlgoParameter::NAME);
    _filledFields.push_back(AlgoParameter::MIN);
    _filledFields.push_back(AlgoParameter::MAX);
    _filledFields.push_back(AlgoParameter::TYPE);
    _filledFields.push_back(AlgoParameter::DESCRIPTION);

    setEditableFields(QList<int>()<<  AlgoParameter::VALUE);
}

AlgoParameter::AlgoParameter(QString savedString)
{
    QStringList fields = savedString.split(" ",QString::SkipEmptyParts);
    if(fields.size()!=nbFields)
    {
        emit sendInfo(Info(ListInfo::PROBLEMREADINGLINE,savedString));
    }
    else
    {
        for(int iF=0;iF<fields.size();iF++)
        {
            QString curField = fields.at(iF);
            curField.remove(" ");
            curField.remove("\t");
            setFieldValue(iF,QVariant(curField));
        }
    }

   setEditableFields(QList<int>()<<  AlgoParameter::VALUE);
}

AlgoParameter::AlgoParameter(QDomElement & domEl)
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

AlgoParameter::~AlgoParameter(void)
{
}


bool AlgoParameter::setFieldValue(int ifield,QVariant value)
{
    try{
    switch (ifield)
    {
        case NAME :
            _name=value.toString();
            break;
        case DESCRIPTION :
            _description=value.toString();
            break;
        case VALUE :
            _value=value;
            break;
        case DEFAULTVALUE :
            _defaultValue = value;
            break;
        case TYPE :
            _type=value.toString();
            break;
        case MIN :
            _min=value;
            break;
        case MAX :
            _max=value;
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


QVariant AlgoParameter::getFieldValue(int ifield, int role) const
{
 if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
        return QString("-");
    else
    {
        switch (ifield)
        {
        case NAME :
            return _name;
        case DESCRIPTION :
            return _description;
        case VALUE :
            return _value;
        case MIN :
            return _min;
        case MAX :
            return _max;
        case TYPE :
            return _type;
        case DEFAULTVALUE :
            return _defaultValue;
        default :
            return "unknown field";
        }
    }
}

AlgoParameter* AlgoParameter::clone() const
{
    AlgoParameter* newParam = new AlgoParameter();

    for(int i=0;i<nbFields;i++)
    {
        newParam->setFieldValue(i,getFieldValue(i));
    }
    newParam->_filledFields = _filledFields;
    newParam->_protectedFields = _protectedFields;

    return newParam;

}

QString AlgoParameter::sFieldName(int iField, int role)
{
    switch (iField)
    {
    case NAME :
        return "Name";
    case DESCRIPTION :
        return "Description";
    case VALUE :
        return "Value";
    case MIN :
        return "Min";
    case MAX :
        return "Max";
    case TYPE :
        return "Type";
    case DEFAULTVALUE :
        return "DefaultValue";
    default :
        return "unknown field";
    }
}
