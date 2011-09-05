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

 	@file MOParameter.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#include "MOParameter.h"


MOParameter::MOParameter(){
	_editableFields.clear();
	_editableFields << MOParameter::VALUE;
        _enablingIndex = -1;
        _enablingValue = true;
}

MOParameter::MOParameter(const MOParameter & param):MOItem(param)
{
	_description = param._description;
	_value = param._value;
	_defaultValue = param._defaultValue;
	_type = param._type;
	_min = param._min;
	_max = param._max;
	_index = param._index;
        _enablingIndex = param._enablingIndex;
        _enablingValue = param._enablingValue;
}

MOParameter::MOParameter(int index,QString name,QString description, QVariant defaultValue, Type type, QVariant minValue, QVariant maxValue,int enablingIndex,QVariant enablingValue):
    _index(index),_description(description),_defaultValue(defaultValue),_type(type),_min(minValue),_max(maxValue),_enablingIndex(enablingIndex),_enablingValue(enablingValue)
{
	_name = name;
	_value = _defaultValue;

	_filledFields.push_back(MOParameter::DEFAULTVALUE);
	_filledFields.push_back(MOParameter::VALUE);
	_filledFields.push_back(MOParameter::NAME);
	_filledFields.push_back(MOParameter::MIN);
	_filledFields.push_back(MOParameter::MAX);
	_filledFields.push_back(MOParameter::TYPE);
	_filledFields.push_back(MOParameter::DESCRIPTION);
	_filledFields.push_back(MOParameter::INDEX);

	_editableFields.clear();
	_editableFields << MOParameter::VALUE;
}



MOParameter::MOParameter(QDomElement & domEl)
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

MOParameter::~MOParameter(void)
{
}


bool MOParameter::setFieldValue(int ifield,QVariant value)
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
                        _type=((MOParameter::Type)value.toInt());
			break;
		case MIN :
			_min=value;
			break;
		case MAX :
			_max=value;
			break;
		case INDEX:
			_index=value.toInt();
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


QVariant MOParameter::getFieldValue(int ifield, int role) const
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
        case INDEX :
            return _index;
        default :
                return "unknown field";
    }
    }
}

MOParameter* MOParameter::clone() const
{
        MOParameter* newParam = new MOParameter(*this);
	return newParam;
}

QString MOParameter::sFieldName(int iField, int role)
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
	case INDEX :
		return "Index";
	case DEFAULTVALUE :
		return "DefaultValue";
	default :
		return "unknown field";
	}
}

void MOParameter::setEnablingIndex(int index,QVariant value)
{
    _enablingIndex = index;
    _enablingValue = value;
}

int MOParameter::enablingIndex()
{
    return _enablingIndex;
}

QVariant MOParameter::enablingValue()
{
    return _enablingValue;
}


MOParameterListed::MOParameterListed(){
        _editableFields.clear();
        _editableFields << MOParameter::VALUE;
}

MOParameterListed::MOParameterListed(const MOParameterListed & param):MOParameter(param)
{

        _mapList = param._mapList;
}

MOParameterListed::MOParameterListed(int index,QString name,QString description, QVariant defaultValue, QMap<int,QString> mapList,int enablingIndex,QVariant enablingValue):
    MOParameter(index,name,description,defaultValue,LIST),_mapList(mapList)
{
    setEnablingIndex(enablingIndex,enablingValue);
}



MOParameterListed::MOParameterListed(QDomElement & domEl)
{ QDomNamedNodeMap attributes = domEl.attributes();
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

MOParameterListed::~MOParameterListed(void)
{
}


bool MOParameterListed::setFieldValue(int ifield,QVariant value)
{

    int iValue;
    bool isInt;
        switch (ifield)
        {
                case NAME :
                        _name=value.toString();
                        break;
                case DESCRIPTION :
                        _description=value.toString();
                        break;
                case VALUE :
                        iValue = value.toInt(&isInt);
                        if(isInt)
                            _value = value;
                        else
                        {
                            iValue = _mapList.key(value.toString(),0);
                            _value = iValue;
                        }
                        break;
                case DEFAULTVALUE :
                        _defaultValue = value;
                        break;
                case INDEX:
                        _index=value.toInt();
                        break;
        }
        if(!_filledFields.contains(ifield))
                _filledFields.push_back(ifield);
        return true;
}


QVariant MOParameterListed::getFieldValue(int ifield, int role) const
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
            if(role==Qt::DisplayRole)
                return _mapList.value(_value.toInt(),QString());
            else
                return _value.toInt();
        case TYPE :
            return _type;
        case DEFAULTVALUE :
            return _defaultValue;
        case INDEX :
            return _index;
        default :
                return "unknown field";
        }
    }
}

MOParameterListed* MOParameterListed::clone() const
{
        MOParameterListed* newParam = new MOParameterListed(*this);
        return newParam;
}

QString MOParameterListed::sFieldName(int iField, int role)
{
    return MOParameter::sFieldName(iField,role);
}

QString MOParameterListed::strValue()
{
    return _mapList.value(_value.toInt());
};

QVariant MOParameters::value(int index,QVariant defaultValue)
{
    int iParam = this->findItem(index,MOParameter::INDEX);
   if(iParam>-1)
       return this->at(iParam)->getFieldValue(MOParameter::VALUE);
   else
       return defaultValue;
}

QVariant MOParameters::value(QString name,QVariant defaultValue)
{
    int iParam = this->findItem(name,MOParameter::NAME);
   if(iParam>-1)
       return this->at(iParam)->getFieldValue(MOParameter::VALUE);
   else
   {
       QString msg = "MOParameters : did not find parameter :"+name;
       infoSender.debug(msg);
       return defaultValue;
   }
}

 bool MOParameters::setValue(int index,QVariant value)
 {
     int iParam = this->findItem(index,MOParameter::INDEX);
    if(iParam>-1)
        return this->at(iParam)->setFieldValue(MOParameter::VALUE,value);
    else
        return false;
 }
