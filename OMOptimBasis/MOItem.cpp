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
MOItem::MOItem(QObject* parent)
   // :QObject(parent) // creates a lot of problems cause of threads. To fix...
{
}

MOItem::MOItem(const MOItem &item)
{
    _name = item._name;
    _filledFields = item._filledFields;
    _protectedFields = item._protectedFields;
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

QString MOItem::getFieldDescription(int iField) const
{
    return "-";
}

MOItem::FieldType MOItem::getFieldType(int iField) const
{
    return GENERIC;
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

void MOItem::setProtectedFields(QList<int> protectedFields)
{
    _protectedFields = protectedFields;
}
void MOItem::setEditableFields(QList<int> editableFields)
{
    _protectedFields.clear();
    for(int i=0;i<getNbFields();i++)
  if(!editableFields.contains(i))
      _protectedFields.push_back(i);
}
void MOItem::setIsProtectedField(int iField, bool isProtected)
{
    if(isProtected)
    {
  if(!_protectedFields.contains(iField))
      _protectedFields.push_back(iField);
    }
    else
    {
  _protectedFields.removeAll(iField);
    }
}
bool MOItem::isProtectedField(int iField)
{
    return(_protectedFields.contains(iField));
}

bool MOItem::protectAllFields()
{
    for(int i=0;i<getNbFields();i++)
  if(!_protectedFields.contains(i))
      _protectedFields.push_back(i);
}


/**
* Creates a string with all field values consecutively.
*/
QString MOItem::toSavingString()
{
    QString savingString;
    for(int iF=0;iF<getNbFields();iF++)
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

/**
* Returns string with all field values
*/
QString MOItem::toCSV() const
{
    QString result;
    QString separator = "\t";
    for(int iF=0;iF<getNbFields();iF++)
    {
  result.push_back(getFieldValue(iF,Qt::DisplayRole).toString());
  result.push_back(separator);
    }
    if(result.size()>0)
  result.remove(result.size()-separator.size(),separator.size());
    return result;
}

/**
* Returns string that should be displayed as a tooltip
* By default, it is the name.
*/
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
