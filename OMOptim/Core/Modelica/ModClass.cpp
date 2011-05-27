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

 	@file ModClass.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ModClass.h"


ModClass::ModClass(MOomc* moomc)
{
	_moomc = moomc;
	_childrenReaden = false;
	_parent = NULL;
}


ModClass::ModClass(MOomc* moomc,ModClass* parent,QString name,QString filePath = "")
{
	_moomc = moomc;
	_parent = parent;
	_name = name;
	_filePath = filePath;
	_childrenReaden = false;
}

ModClass::~ModClass(void)
{
    emit deleted();
	clearDescendants();
}

QVariant ModClass::getFieldValue(int iField, int role) const
{
	switch(iField)
	{
	case NAME:
		return _name;
		break;
	case FILEPATH:
		return _filePath;
		break;
	default :
		return QVariant();
		break;
	}
}

bool ModClass::setFieldValue(int iField, QVariant value)
{
	try{
		switch(iField)
		{
		case NAME:
			_name = value.toString();
			break;
		case FILEPATH:
			_filePath = value.toString();
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

QString ModClass::sFieldName(int iField, int role)
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

ModClass* ModClass::parent()
{
		return _parent;
}

int ModClass::childCount()
{
	return _children.size();
}

QStringList ModClass::getChildrenNames()
{
	QStringList result;
	for(int i=0;i<this->childCount();i++)
	{
		result.push_back(this->child(i)->name());
	}
	return result;
}

bool ModClass::childrenReaden()
{
	return _childrenReaden;
}

void ModClass::setChildrenReaden(bool childrenReaden)
{
	_childrenReaden = childrenReaden;
}

int ModClass::compChildCount()
{
	int nbComp=0;
	for(int i=0;i<_children.size();i++)
		if(_children.at(i)->getClassRestr()==Modelica::COMPONENT)
			nbComp++;

	return nbComp;
}

int ModClass::modelChildCount()
{
	int nbModel=0;
	for(int i=0;i<_children.size();i++)
		if(_children.at(i)->getClassRestr()==Modelica::MODEL)
			nbModel++;

	return nbModel;
}

int ModClass::packageChildCount()
{
	int nbPackage=0;
	for(int i=0;i<_children.size();i++)
		if(_children.at(i)->getClassRestr()==Modelica::PACKAGE)
			nbPackage++;

	return nbPackage;
}

ModClass* ModClass::child(int nRow)
{
	if((nRow>-1)&&(nRow<_children.count()))
		return _children.at(nRow);
	else
		return NULL;
}

ModClass* ModClass::compChild(int nRow)
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

ModClass* ModClass::packageChild(int nRow)
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

ModClass* ModClass::modelChild(int nRow)
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

QString ModClass::filePath()
{
	if(_filePath.isEmpty()&& (_parent!=NULL))
		return _parent->filePath();
	else
		return _filePath;
}

QString ModClass::name(Modelica::NameFormat type)
{
	if(type == Modelica::SHORT)
		return _name;
	else
	{
		QString fullName = _name;
                ModClass *curParent = parent();

		while((curParent!=NULL)&&(curParent->name(Modelica::SHORT)!=""))
		{
			fullName.insert(0,curParent->name(Modelica::SHORT)+".");
                        curParent = curParent->parent();
		}

		QString dymolaName;
		switch(type)
		{
		case Modelica::DYMOLA:
			dymolaName = fullName.section(".",1,fullName.count(".")+1);
			return dymolaName;
		case Modelica::FULL:
			return fullName;
		default:
			return _name;
		}	
	}
}

QString ModClass::getModClassName()
{
	return name(Modelica::FULL);
}

void ModClass::emitModified()
{
	emit modified();
}

int ModClass::depth()
{
	QString fullName=_name;
        ModClass *curParent = parent();

	if(curParent==NULL)
		return  0;
	else
		return curParent->depth()+1;
}


void ModClass::clear()
{
	clearDescendants();
	emit modified();

	_parent = NULL;
	_name = QString();
	_filePath = QString();
	_childrenReaden = false;

	emit cleared();
}

void ModClass::clearDescendants()
{
	while(_children.size()>0)
	{
		delete _children.at(0);
		_children.removeAt(0);
	}
	_childrenReaden = false;
	emit modified();
}

bool ModClass::addChild(ModClass *child)
{
	bool ok=false;
	if(child)
	{
		child->setParent(this);
		_children.push_back(child);
		ok = true;
		emit addedChild(child);
		connect(child,SIGNAL(modified()),this,SIGNAL(modified()));
	}
	if(ok)
		emit modified();

	return ok;
}


void ModClass::setParent(ModClass *parent)
{
	if(_parent != parent)
	{
		_parent = parent;
		emit modified();
	}
}



QString ModClass::getStrToolTip()
{
	QString toolTip;
	toolTip += ("Generic Modelica Class : " + _name);
	return toolTip;
}

void ModClass::openMoFolder()
{
	QFileInfo fileInfo(filePath());
	LowTools::openFolder(fileInfo.absolutePath());
}

void ModClass::reloadInOMC()
{
	bool ok;
	QString error;
	_moomc->loadModel(filePath(),true,ok,error);
}
