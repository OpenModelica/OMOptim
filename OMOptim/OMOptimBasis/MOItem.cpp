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

     @file MOItem.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 

  */
#include "MOItem.h"


// Constructor and destructor
MOItem::MOItem(void)
{
}

MOItem::MOItem(const MOItem &item)
{
    _name = item._name;
    _filledFields = item._filledFields;
    _editableFields = item._editableFields;
}

MOItem::MOItem(QDomElement & domEl)
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

        setFieldValue(fieldName,QVariant(fieldValue));
    }
}

MOItem::~MOItem(void)
{
    emit deleted();
}

void MOItem::update(QDomElement & domEl)
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

        setFieldValue(fieldName,QVariant(fieldValue));
    }
}

MOItem::MOItem(QString savedString,ProjectBase *project)
{
    connect(this,SIGNAL(sendInfo(Info)),(QObject*)project,SIGNAL(sent(Info*)));

    QStringList fields = savedString.split(" ",QString::SkipEmptyParts);
    if(fields.size()!=nbFields)
    {
        emit sendInfo( Info(ListInfo::PROBLEMREADINGLINE,savedString));
    }
    else
    {
        for(int iF=0;iF<fields.size();iF++)
        {
            setFieldValue(iF,QVariant(fields.at(iF)));
        }
    }
}

//Name
void MOItem::setName(QString name)
{
    _name = name;

    if(!_filledFields.contains(MOItem::NAME))
        _filledFields.push_back(MOItem::NAME);
}

QString MOItem::name() const
{
    return _name;
}

//All fields
QVariant MOItem::getFieldValue(int ifield, int role) const
{
 if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
        return QString("-");
    else
    {
                switch (ifield)
        {
        case NAME :
                        return name();
        default :
            return "unknown field";
        }
    }
}

bool MOItem::setFieldValue(int iField, QVariant value)
{
    if(!_filledFields.contains(iField))
        _filledFields.push_back(iField);

    switch (iField)
    {
    case NAME :
        _name=value.toString();
        return true;
    default : 
        return false;
    }

}

bool MOItem::setFieldValue(QString field, QVariant value)
{
        int i = getFieldIndex(field,Qt::UserRole);
    if(i>-1)
    {
        setFieldValue(i,value);
        return true;
    }
    else
        return false;
}

QString MOItem::sFieldName(int iField, int role)
{
    switch (iField)
    {
    case NAME :
        return "Name";
    default :
        return "unknown field";
    }
}

int MOItem::getFieldIndex(QString fieldName,int role)
{
    int index=0;
    while((index<this->getNbFields()) && (this->getFieldName(index,role)!=fieldName))
        index++;

    if(index<getNbFields())
        return index;
    else
        return -1;
}

void MOItem::setEditableFields(QList<int> _editableFields)
{
    _editableFields = _editableFields;
}
void MOItem::setIsEditableField(int iField, bool isEditable)
{
    if(isEditable)
    {
        if(_editableFields.indexOf(iField<0))
            _editableFields.push_back(iField);
    }
    else
    {
        int index = _editableFields.indexOf(iField);
            if(index>-1)
                _editableFields.removeAt(index);    
    }
}
bool MOItem::isEditableField(int iField)
{
    return(_editableFields.indexOf(iField)>-1);
}

QString MOItem::toSavingString()
{
    QString savingString;
    for(int iF=0;iF<nbFields;iF++)
    {
        savingString += getFieldValue(iF).toString() + "\t";
    }
    return savingString;
}

/**
* Virtual function used to check (used by inheriting classes).
* @param error Contains error message
* @return true if there is an error, false otherwise.
*/
bool MOItem::check(QString &error)
{
    error.clear();
    return true;
}

/**
* This function exports item information in XML format.
* @param doc QDomDocument from which XML node should be created.
*/
QDomElement MOItem::toXmlData(QDomDocument & doc)
{
    QDomElement cItem = doc.createElement(this->getClassName());
    QString fieldName;
    QString fieldValue;
    for(int iF=0;iF<getNbFields();iF++)
    {
        fieldName = getFieldName(iF,Qt::UserRole);
        fieldName.replace(" ",XMLTools::space());
        fieldValue = getFieldValue(iF).toString();
        fieldValue.replace(" ",XMLTools::space());
        cItem.setAttribute(fieldName,fieldValue);            
    }
    return cItem;
}

QString MOItem::getStrToolTip()
{
    return _name;
}

void MOItem::checkUniqueItemName( QStringList & list)
{
    QString defaultTitle;
    QString title;
    QString strSuffix;
    bool titleIsUnique=false;
    bool titleIsFound=false;
    int i;
    int iSuffix;
    QString expr = "_%d";

    //set problem title
    defaultTitle=this->name();
    title=defaultTitle;
    iSuffix=2;
    while(!titleIsUnique)
    {
        i=0;
        titleIsFound=false;
        while(i<list.size() && !titleIsFound)
        {
            if (title==list.at(i))
            {
                titleIsFound=true;
                strSuffix.sprintf(expr.toLatin1().data(),iSuffix);
                title=defaultTitle+strSuffix;
                iSuffix++;
            }
            i++;
        }
        if (!titleIsFound)
        {
            titleIsUnique=true;
        }
    }
    this->setName(title);
}
