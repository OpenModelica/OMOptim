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
        _parent = NULL;
}


EIItem::EIItem(EIItem* parent,QString name, QString model)
{
        _parent = parent;
        setName(name);
        _checked = true;
        _model = model;
}

EIItem::EIItem(const EIItem & item):MOItem(item)
{
        _parent = item._parent;
        _checked = item._checked;
        _model = item._model;
	
        for(int i=0;i<item.childCount();i++)
                addChild(item.child(i)->clone());
}

EIItem::EIItem(QDomElement & domEl)
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

        _editableFields << EIItem::NAME;
}


EIItem::~EIItem(void)
{
    emit deleted();
	clear();
}

EIItem* EIItem::parent()
{
    return _parent;
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
                return _checked;
		break;
        case MODEL:
                return model();
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
                _checked = value.toBool();
		break;
        case MODEL:
                _model = value.toString();
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
                return "Checked";
        case MODEL:
                return "Model";
	default :
		return "-";
	}
}


//*****************************
//Children
//*****************************
int EIItem::childCount() const
{
        return _children.size();
}

int EIItem::streamChildCount()
{
	int nbComp=0;
        for(int i=0;i<_children.size();i++)
                if(_children.at(i)->getEIType()==EI::STREAM)
			nbComp++;

	return nbComp;
}

int EIItem::groupChildCount()
{
	int nbModel=0;
        for(int i=0;i<_children.size();i++)
                if(_children.at(i)->getEIType()==EI::GROUP)
			nbModel++;

	return nbModel;
}

EIItem* EIItem::child(int nRow) const
{
        if((nRow>-1)&&(nRow<_children.count()))
                return _children.at(nRow);
	else
		return NULL;
}

EIItem* EIItem::streamChild(int nRow)
{
	int iCurStream=-1;
	int curIndex=0;
        while((curIndex<_children.size())&&(iCurStream<nRow))
	{
                if(_children.at(curIndex)->getEIType()==EI::STREAM)
			iCurStream++;
	
		curIndex++;
	}

	if(iCurStream==nRow)
                return _children.at(curIndex-1);
	else
		return NULL;
}



EIItem* EIItem::groupChild(int nRow)
{
	int iCurGroup=-1;
	int curIndex=0;
        while((curIndex<_children.size())&&(iCurGroup<nRow))
	{
                if(_children.at(curIndex)->getEIType()==EI::GROUP)
			iCurGroup++;
	
		curIndex++;
	}

	if(iCurGroup==nRow)
                return _children.at(curIndex-1);
	else
		return NULL;
}

int EIItem::indexInParent()
{
    if(parent()==NULL)
        return -1;


    //looking for row number of child in parent
    int nbBrothers = parent()->childCount();
    bool found = false;
    int iC=0;

    while(!found && iC<nbBrothers)
    {
        found = (parent()->child(iC)==this);
        if(!found)
            iC++;
    }
    if(!found)
        return -1;
    else
        return iC;
}



void EIItem::removeChild(int i)
{
	if((i>-1)&&(i<childCount()))
	{
		delete child(i);
                _children.removeAt(i);
	}
}
void EIItem::removeChild(EIItem * item)
{
        int i=_children.indexOf(item);
	removeChild(i);
}


int EIItem::findChild(QVariant itemFieldValue, int iField)
{
        bool found = false;
        int i=0;
        int nbChildren=childCount();
        QVariant curFieldValue;

        while((!found)&&(i<nbChildren))
        {
                curFieldValue=child(i)->getFieldValue(iField);
                found=(itemFieldValue == curFieldValue);
                i++;
        }
        if(!found)
        {
                return -1;
        }
        else
        {
                return i-1;
        }
}


void EIItem::emitModified()
{
	emit modified();
}

int EIItem::depth()
{
	QString fullName=_name;
        EIItem *curParent = parent();

	if(curParent==NULL)
		return  0;
	else
		return curParent->depth()+1;
}


void EIItem::clear()
{
	clearDescendants();
	emit modified();
        _parent = NULL;
	_name = QString();
	emit cleared();
}

void EIItem::clearDescendants()
{
        while(_children.size()>0)
	{
                delete _children.at(0);
                _children.removeAt(0);
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
                for(int i=0;i<this->_children.size();i++)
                        brothersNames.push_back(this->_children.at(i)->name());

		child->checkUniqueItemName(brothersNames);
                _children.push_back(child);
		
		ok = true;

		emit addedChild(child);
		connect(child,SIGNAL(modified()),this,SIGNAL(modified()));
	}
	if(ok)
		emit modified();

	return ok;
}


void EIItem::setParent(EIItem *parent)
{
        if(_parent != parent)
	{
                _parent = parent;
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
        return _checked;
}

void EIItem::setChecked(bool checked)
{
        if(checked!=_checked)
	{
                setFieldValue(CHECKED,checked);

		// if false, uncheck children
                if(!checked)
		{
		for(int i=0;i<childCount();i++)
			child(i)->setChecked(false);
		}
		
		// if true, check parent
                if(_parent && checked)
                        _parent->setChecked(true),
		
		emit modified();
	}
}

void  EIItem::setModel(QString model)
{
    _model = model;
}

/**
  * \brief returns model it is involved in. If is empty, looks for first parent which got a non-empty modelname
  *
  */
QString  EIItem::model()
{
    if(!_model.isEmpty())
        return _model;
    else
    {
        if(parent()==NULL)
            return _model;
        else
            return parent()->model();
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
                EIItem *curParent = parent();

                while((curParent!=NULL)&&(curParent->parent()!=NULL))
		{
			fullName.insert(0,curParent->name(EI::SHORT)+".");
                        curParent = curParent->parent();
		}
		return fullName;
	}
}


QStringList EIItem::references()
{
    return QStringList();
}

