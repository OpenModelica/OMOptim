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

    @file ModItem.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#include "ModItem.h"
#include "MOomc.h"
#include "LowTools.h"

ModItem::ModItem(MOomc* moomc)
{
    _moomc = moomc;
    _childrenReaden = false;
    _parent = NULL;
}


ModItem::ModItem(MOomc* moomc,ModItem* parent,QString fullname,QFileInfo file)
{
    _moomc = moomc;
    _parent = parent;
    _name = fullname;
    _file = file;
    _childrenReaden = false;
}

ModItem* ModItem::clone() const
{
    ModItem* newModItem = new ModItem(_moomc,_parent,_name,_file);
    newModItem->_childrenReaden = _childrenReaden;

    for(int i=0;i<_children.size();i++)
    {
  newModItem->addChild(_children.at(i)->clone());
    }
    return newModItem;
}

ModItem::~ModItem(void)
{
    clearDescendants();
}

QVariant ModItem::getFieldValue(int iField, int role) const
{
    switch(iField)
    {
    case NAME:
  return _name;
  break;
    case FILEPATH:
  return _file.absoluteFilePath();
  break;
    default :
  return QVariant();
  break;
    }
}

bool ModItem::setFieldValue(int iField, QVariant value)
{
    try{
  switch(iField)
  {
  case NAME:
      _name = value.toString();
      break;
  case FILEPATH:
      _file.setFile(value.toString());
      break;
  }
  emit modified();
  return true;
    }
    catch(std::exception )
    {
  return false;
    }
}

QString ModItem::sFieldName(int iField, int role)
{
    switch(iField)
    {
    case NAME:
  return "Name";
    case FILEPATH:
  return "FilePath";
    default :
  return "-";
    }
}

ModItem* ModItem::parentItem()
{
    return _parent;
}

int ModItem::childCount() const
{
    return _children.size();
}

QStringList ModItem::getChildrenNames()
{
    QStringList result;
    for(int i=0;i<this->childCount();i++)
    {
  result.push_back(this->child(i)->name());
    }
    return result;
}

bool ModItem::childrenReaden()
{
    return _childrenReaden;
}

void ModItem::setChildrenReaden(bool childrenReaden)
{
    _childrenReaden = childrenReaden;
}

int ModItem::compChildCount()
{
    int nbComp=0;
    for(int i=0;i<_children.size();i++)
  if(_children.at(i)->getClassRestr()==Modelica::COMPONENT)
      nbComp++;

    return nbComp;
}

int ModItem::modelChildCount()
{
    int nbModel=0;
    for(int i=0;i<_children.size();i++)
  if(_children.at(i)->getClassRestr()==Modelica::MODEL)
      nbModel++;

    return nbModel;
}

int ModItem::packageChildCount()
{
    int nbPackage=0;
    for(int i=0;i<_children.size();i++)
  if(_children.at(i)->getClassRestr()==Modelica::PACKAGE)
      nbPackage++;

    return nbPackage;
}

int ModItem::recordChildCount()
{
    int nbRecords=0;
    for(int i=0;i<_children.size();i++)
  if(_children.at(i)->getClassRestr()==Modelica::RECORD)
      nbRecords++;

    return nbRecords;
}

ModItem* ModItem::child(int nRow) const
{
    if((nRow>-1)&&(nRow<_children.count()))
  return _children.at(nRow);
    else
  return NULL;
}

ModItem* ModItem::compChild(int nRow) const
{
    int iCurComp=-1;
    int curIndex=0;
    while((curIndex<_children.size())&&(iCurComp<nRow))
    {
  if(_children.at(curIndex)->getClassRestr()==Modelica::COMPONENT)
      iCurComp++;

  curIndex++;
    }

    if(iCurComp==nRow)
  return _children.at(curIndex-1);
    else
  return NULL;
}

ModItem* ModItem::packageChild(int nRow) const
{
    int iCurPackage=-1;
    int curIndex=0;
    while((curIndex<_children.size())&&(iCurPackage<nRow))
    {
  if(_children.at(curIndex)->getClassRestr()==Modelica::PACKAGE)
      iCurPackage++;

  curIndex++;
    }

    if(iCurPackage==nRow)
  return _children.at(curIndex-1);
    else
  return NULL;
}

ModItem* ModItem::recordChild(int nRow) const
{
    int iCurRecord=-1;
    int curIndex=0;
    while((curIndex<_children.size())&&(iCurRecord<nRow))
    {
  if(_children.at(curIndex)->getClassRestr()==Modelica::PACKAGE)
      iCurRecord++;

  curIndex++;
    }

    if(iCurRecord==nRow)
  return _children.at(curIndex-1);
    else
  return NULL;
}


ModItem* ModItem::modelChild(int nRow) const
{
    int iCurModel=-1;
    int curIndex=0;
    while((curIndex<_children.size())&&(iCurModel<nRow))
    {
  if(_children.at(curIndex)->getClassRestr()==Modelica::MODEL)
      iCurModel++;

  curIndex++;
    }

    if(iCurModel==nRow)
  return _children.at(curIndex-1);
    else
  return NULL;
}

int ModItem::indexInParent()
{
    if(parent()==NULL)
  return -1;


    //looking for row number of child in parent
    int nbBrothers = parentItem()->childCount();
    bool found = false;
    int iC=0;

    while(!found && iC<nbBrothers)
    {
  found = (parentItem()->child(iC)==this);
  if(!found)
      iC++;
    }
    if(!found)
  return -1;
    else
  return iC;
}

bool ModItem::containsItemOfClass(QString className) const
{
    if(this->getClassName()==className)
  return true;

    bool found = false;
    int i=0;
    while(!found && i<childCount())
    {
  found = child(i)->containsItemOfClass(className);
  i++;
    }
    return found;
}


QString ModItem::filePath()
{
    ModItem* parent = _parent;
    QFileInfo file = _file;
    while(file.fileName().isEmpty()&&(parent!=NULL))
    {
  file = parent->_file;
  parent = parent->parentItem();
    }

    if(file.fileName().isEmpty() && _moomc)
    {
  file = _moomc->getFileOfClass(name());
    }
    return file.absoluteFilePath();
}

QString ModItem::name(ModItem::NameFormat type)
{
    if(type == ModItem::SHORT)
  return _name.section(".",-1,-1);
    else
    {
  QString fullName = _name;
  //                ModItem *curParent = parent();

  //        while((curParent!=NULL)&&(curParent->name(Modelica::SHORT)!=""))
  //        {
  //            fullName.insert(0,curParent->name(Modelica::SHORT)+".");
  //                        curParent = curParent->parent();
  //        }

  QString middleName;
  switch(type)
  {
  case ModItem::WITHOUTROOT:
      middleName = fullName.section(".",1,fullName.count(".")+1);
      return middleName;
  case ModItem::FULL:
      return fullName;
  default:
      return _name;
  }
    }
}

QString ModItem::getModClassName()
{
    return name(ModItem::FULL);
}

void ModItem::emitModified()
{
    emit modified();
}

int ModItem::depth()
{
    QString fullName=_name;
    ModItem *curParent = parentItem();

    if(curParent==NULL)
  return  0;
    else
  return curParent->depth()+1;
}

QList<ModItem *> ModItem::children() const
{
    return _children;
}


void ModItem::clear()
{
    clearDescendants();
    emit modified();

    _parent = NULL;
    _name = QString();
    _file = QString();
    _childrenReaden = false;

    emit cleared();
}

void ModItem::clearDescendants()
{
    while(_children.size()>0)
    {
  delete _children.at(0);
  _children.removeAt(0);
    }
    _childrenReaden = false;
    emit modified();
}

bool ModItem::addChild(ModItem *child)
{
    bool ok=false;
    if(child)
    {
  child->setParentItem(this);
  _children.push_back(child);
  ok = true;
  emit addedChild(child);
  connect(child,SIGNAL(modified()),this,SIGNAL(modified()));
    }
    if(ok)
  emit modified();

    return ok;
}


void ModItem::setParentItem(ModItem *parent)
{
    if(_parent != parent)
    {
  _parent = parent;
  emit modified();
    }
}



QString ModItem::getStrToolTip()
{
    QString toolTip;
    toolTip += ("Generic Modelica Class : " + _name + "\n");
    toolTip += ("File : " + filePath() + "\n");
    return toolTip;
}

void ModItem::openMoFolder()
{
    QFileInfo fileInfo(filePath());
    LowTools::openFolder(fileInfo.absolutePath());
}

void ModItem::openInEditor()
{
    QUrl fileUrl(QString("file:///").append(filePath()));
    bool ok = QDesktopServices::openUrl(fileUrl);
}
