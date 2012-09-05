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

}

MOParameter::MOParameter(const MOParameter & param):MOItem(param)
{
    _description = param._description;
    _value = param._value;
    _defaultValue = param._defaultValue;
    _type = param._type;
    _min = param._min;
    _max = param._max;
    //_index = param._index;
    _enablingIndexes = param._enablingIndexes;
    _group = param._group;
    _panel = param._panel;
}

MOParameter::MOParameter(QString name,QString description, QVariant defaultValue, Type type, QVariant minValue, QVariant maxValue,QString enablingIndex,QVariant enablingValue,QString group):
    _description(description),_defaultValue(defaultValue),_type(type),_min(minValue),_max(maxValue),_group(group)
{
    _name = name;
    _value = _defaultValue;
    _enablingIndexes.insert(enablingIndex,enablingValue);

    _filledFields.push_back(MOParameter::DEFAULTVALUE);
    _filledFields.push_back(MOParameter::VALUE);
    _filledFields.push_back(MOParameter::NAME);
    _filledFields.push_back(MOParameter::MIN);
    _filledFields.push_back(MOParameter::MAX);
    _filledFields.push_back(MOParameter::TYPE);
    _filledFields.push_back(MOParameter::DESCRIPTION);
    //_filledFields.push_back(MOParameter::INDEX);
    _filledFields.push_back(MOParameter::GROUP);
    _filledFields.push_back(MOParameter::PANEL);

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
//        case INDEX:
//            _index=value.toInt();
//            break;
        case GROUP:
            _group=value.toString();
            break;
        case PANEL:
            _panel=value.toString();
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
//        case INDEX :
//            return _index;
        case GROUP :
            return _group;
        case PANEL :
            return _panel;
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
//    case INDEX :
//        return "Index";
    case DEFAULTVALUE :
        return "DefaultValue";
    case GROUP :
        return "Group";
    case PANEL :
        return "Panel";
    default :
        return "unknown field";
    }
}

void MOParameter::addEnablingIndex(QString name,QVariant value)
{
    _enablingIndexes.insert(name,value);
}

QMap<QString,QVariant> MOParameter::enablingIndexes() const
{
    return _enablingIndexes;
}

/**
* Equivalent to MOItem::toXmlData() but here, we do not save
* index field. Because if index changed (e.g. order is changed between two versions),
* there would be misunderstanding to keep index. Name is the field used to refer.
* @sa MOItem::toXmlData()
*/
QDomElement MOParameter::toXmlData(QDomDocument & doc)
{
    QDomElement cItem = doc.createElement(this->getClassName());
    QString fieldName;
    QString fieldValue;
    for(int iF=0;iF<getNbFields();iF++)
    {
//        if(iF!=MOParameter::INDEX)
//        {
            fieldName = getFieldName(iF,Qt::UserRole);
            fieldName.replace(" ",XMLTools::space());
            fieldValue = getFieldValue(iF).toString();
            fieldValue.replace(" ",XMLTools::space());
            cItem.setAttribute(fieldName,fieldValue);
//        }
    }
    return cItem;
}

/**
* Equivalent to MOItem::update() but here, we do not read field.
* Because if index changed (e.g. order is changed between two versions),
* there would be misunderstanding to keep index. Name is the field used to refer.
* @sa MOParameter::toXmlData()
* @sa MOItem::update(QDomElement & domEl)
*/
void MOParameter::update(QDomElement & domEl)
{
    QDomNamedNodeMap attributes = domEl.attributes();
    QString fieldName;
    QString fieldValue;
    int iField;
    for(int i=0;i<attributes.count();i++)
    {
        fieldName = attributes.item(i).toAttr().name();
        fieldName.replace(XMLTools::space()," ");
        fieldValue = attributes.item(i).toAttr().value();
        fieldValue.replace(XMLTools::space()," ");
        iField = getFieldIndex(fieldName,Qt::UserRole);
        setFieldValue(iField,QVariant(fieldValue));
    }
}

MOParameterListed::MOParameterListed(){
    _editableFields.clear();
    _editableFields << MOParameter::VALUE;
}

MOParameterListed::MOParameterListed(const MOParameterListed & param):MOParameter(param)
{

    _mapList = param._mapList;
}

MOParameterListed::MOParameterListed(QString name,QString description, QVariant defaultValue, QMap<int,QString> mapList,QString enablingIndex,QVariant enablingValue):
    MOParameter(name,description,defaultValue,LIST),_mapList(mapList)
{
    addEnablingIndex(enablingIndex,enablingValue);
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
//    case INDEX:
//        _index=value.toInt();
//        break;
    case GROUP:
        _group=value.toString();
        break;
    case PANEL:
        _panel=value.toString();
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
//        case INDEX :
//            return _index;
        case GROUP :
            return _group;
        case PANEL :
            return _panel;
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
}


MOParameters::MOParameters()
    :MOVector<MOParameter>(true)
{
    connect(this,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SIGNAL(modified()));
    qDebug(QString("New MOParameters").toLatin1().data());
}

MOParameters::MOParameters(const MOParameters & parameters)
    :MOVector<MOParameter>(parameters)
{
    qDebug(QString("New MOParameters").toLatin1().data());
}

MOParameters::~MOParameters()
{
    qDebug(QString("Remove MOParameters").toLatin1().data());
}

QVariant MOParameters::value(QString name,QVariant defaultValue) const
{
    int iParam = this->findItem(name,MOParameter::NAME);
    if(iParam>-1)
        return this->at(iParam)->getFieldValue(MOParameter::VALUE);
    else
    {
        QString msg = "MOParameters : did not find parameter :"+name;
        InfoSender::instance()->debug(msg);
        return defaultValue;
    }
}

bool MOParameters::setValue(QString name,QVariant value)
{
    int iParam = this->findItem(name);
    if(iParam>-1)
        return this->at(iParam)->setFieldValue(MOParameter::VALUE,value);
    else
        return false;
}

QMultiMap<QString,MOParameter*> MOParameters::groupmap() const
{
    QMultiMap<QString,MOParameter*> map;
    for(int i=0;i<size();i++)
        map.insert(this->at(i)->getFieldValue(MOParameter::GROUP).toString(),this->at(i));

    return map;
}

void MOParameters::setGroup(QString group,QStringList indexes)
{
    int iParam;
    for(int i=0;i<indexes.size();i++)
    {
        iParam = this->findItem(indexes.at(i));
        if(iParam>-1)
            this->at(iParam)->setFieldValue(MOParameter::GROUP,group);
    }
}

void MOParameters::setPanel(QString panel)
{
    int iParam;
    for(int i=0;i<size();i++)
    {
        iParam = this->at(i)->setFieldValue(MOParameter::PANEL,panel);
    }
}

void MOParameters::addEnablingIndex(QStringList enabledIndexes,QString enablingIndex, QVariant enablingValue)
{
    int iParam;
    for(int i=0;i<enabledIndexes.size();i++)
    {
        iParam = this->findItem(enabledIndexes.at(i));
        if(iParam>-1)
            this->at(iParam)->addEnablingIndex(enablingIndex,enablingValue);
    }
}

/**
  Considering enablingIndex parameters and their values,
  return whether or not parameter indicated by index should be enabled.
  */
bool MOParameters::shouldBeEnabled(QString name)
{
    int iParam = this->findItem(name);
    if(iParam==-1)
        return false;

    QMap<QString,QVariant> enablingIndexes = this->at(iParam)->enablingIndexes();
    bool result = true;
    int i=0;
    QString curKey;
    while(result && (i<enablingIndexes.keys().size()))
    {
        curKey = enablingIndexes.keys().at(i);
        if(!curKey.isEmpty())
            result = result && (value(curKey)==enablingIndexes.value(curKey));
        i++;
    }
    return result;
}

MOParameters* MOParameters::clone() const
{
    MOParameters* newVector = new MOParameters();
    for(int i=0;i<this->size();i++)
        newVector->addItem(this->at(i)->clone());
    return newVector;
}

bool MOParameters::operator==(const MOParameters& b)const
{
    if(this->size()!=b.size())
        return false;

    bool equals = true;
    QString curName;
    int i=0;
    while(equals && (i<size()))
    {
        curName = this->at(i)->name();
        equals = equals && (this->at(i)->value()==b.value(curName));
        i++;
    }
    return equals;
}
