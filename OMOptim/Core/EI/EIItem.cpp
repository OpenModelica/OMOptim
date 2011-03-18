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

 	@file EIItem.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIItem.h"

EIItem::EIItem()
{
	parent = NULL;
}


EIItem::EIItem(EIItem* _parent,QString _name)
{
	parent = _parent;
	setName(_name);
	checked = true;
}

EIItem::EIItem(const EIItem & _item):MOItem(_item)
{
	parent = _item.parent;
	checked = _item.checked;
	
	for(int i=0;i<_item.childCount();i++)
		addChild(_item.child(i)->clone());
}


EIItem::~EIItem(void)
{
	clear();
}

EIItem* EIItem::getParent()
{
		return parent;
}

EIItem* EIItem::clone()
{
	EIItem* newItem = new EIItem(*this);
	return newItem;
}

//*****************************
//Data managment
//*****************************

QVariant EIItem::getFieldValue(int iField, int role) const
{
	switch(iField)
	{
	case NAME:
		return _name;
		break;
	case CHECKED:
		return checked;
		break;
	default :
		return QVariant();
		break;
	}
}

bool EIItem::setFieldValue(int iField, QVariant value)
{
	switch(iField)
	{
	case NAME:
		_name = value.toString();
		break;
	case CHECKED:
		checked = value.toBool();
		break;
	default :
		return false;
	}
	emit modified();
	return true;
}

QString EIItem::sFieldName(int iField, int role)
{
	switch(iField)
	{
	case NAME:
		return "Name";
	case CHECKED:
		return "CHECKED";
	default :
		return "-";
	}
}


//*****************************
//Children
//*****************************
int EIItem::childCount() const
{
	return children.size();
}
int EIItem::streamChildCount()
{
	int nbComp=0;
	for(int i=0;i<children.size();i++)
		if(children.at(i)->getEIType()==EI::STREAM)
			nbComp++;

	return nbComp;
}

int EIItem::groupChildCount()
{
	int nbModel=0;
	for(int i=0;i<children.size();i++)
		if(children.at(i)->getEIType()==EI::GROUP)
			nbModel++;

	return nbModel;
}

EIItem* EIItem::child(int nRow) const
{
	if((nRow>-1)&&(nRow<children.count()))
		return children.at(nRow);
	else
		return NULL;
}

EIItem* EIItem::streamChild(int nRow)
{
	int iCurStream=-1;
	int curIndex=0;
	while((curIndex<children.size())&&(iCurStream<nRow))
	{
		if(children.at(curIndex)->getEIType()==EI::STREAM)
			iCurStream++;
	
		curIndex++;
	}

	if(iCurStream==nRow)
		return children.at(curIndex-1);
	else
		return NULL;
}

EIItem* EIItem::groupChild(int nRow)
{
	int iCurGroup=-1;
	int curIndex=0;
	while((curIndex<children.size())&&(iCurGroup<nRow))
	{
		if(children.at(curIndex)->getEIType()==EI::GROUP)
			iCurGroup++;
	
		curIndex++;
	}

	if(iCurGroup==nRow)
		return children.at(curIndex-1);
	else
		return NULL;
}

void EIItem::removeChild(int i)
{
	if((i>-1)&&(i<childCount()))
	{
		delete child(i);
		children.removeAt(i);
	}
}
void EIItem::removeChild(EIItem * _item)
{
	int i=children.indexOf(_item);
	removeChild(i);
}


void EIItem::emitModified()
{
	emit modified();
}

int EIItem::depth()
{
	QString fullName=_name;
	EIItem *curParent = getParent();

	if(curParent==NULL)
		return  0;
	else
		return curParent->depth()+1;
}


void EIItem::clear()
{
	clearDescendants();
	emit modified();
	parent = NULL;
	_name = QString();
	emit cleared();
}

void EIItem::clearDescendants()
{
	while(children.size()>0)
	{
		delete children.at(0);
		children.removeAt(0);
	}
	 emit modified();
}

bool EIItem::addChild(EIItem *child)
{

	bool ok=false;
	if(child && (this->getEIType()!=EI::STREAM))
	{
		child->setParent(this);
		// verify its name is unique compared to brother
		QStringList brothersNames;
		for(int i=0;i<this->children.size();i++)
			brothersNames.push_back(this->children.at(i)->name());

		child->checkUniqueItemName(brothersNames);
		children.push_back(child);
		
		ok = true;

		emit addedChild(child);
		connect(child,SIGNAL(modified()),this,SIGNAL(modified()));
	}
	if(ok)
		emit modified();

	return ok;
}


void EIItem::setParent(EIItem *_parent)
{
	if(parent != _parent)
	{
		parent = _parent;
		emit modified();
	}
}

QString EIItem::getStrToolTip()
{
	QString toolTip;
	toolTip += ("EI item : " + _name);
	return toolTip;
}

bool EIItem::isChecked()
{
	return checked;
}

void EIItem::setChecked(bool _checked)
{
	if(_checked!=checked)
	{
		setFieldValue(CHECKED,_checked);

		// if false, uncheck children
		if(!_checked)
		{
		for(int i=0;i<childCount();i++)
			child(i)->setChecked(false);
		}
		
		// if true, check parent
		if(parent && _checked)
			parent->setChecked(true),
		
		emit modified();
	}
}

QDomElement EIItem::toXmlData(QDomDocument & doc)
{
	QDomElement result = MOItem::toXmlData(doc);
	for(int i=0;i<childCount();i++)
		result.appendChild(child(i)->toXmlData(doc));

	return result;
}

QString EIItem::name(EI::NameFormat type)
{
	if(type == EI::SHORT)
		return _name;
	else
	{
		QString fullName=_name;
		EIItem *curParent = getParent();

		while((curParent!=NULL)&&(curParent->parent!=NULL))
		{
			fullName.insert(0,curParent->name(EI::SHORT)+".");
			curParent = curParent->parent;
		}
		return fullName;
	}
}
