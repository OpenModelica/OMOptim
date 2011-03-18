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

 	@file EIReader.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIReader.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>
#include <vector>
#include <algorithm>
#include <functional>
#include "assert.h"

EIReader::EIReader()
{
}



bool EIReader::isInDescendants(EIItem* parent, QString fullName)
{
	EIItem* foundClass = findInDescendants(parent,fullName);
	return (bool)(foundClass);
}

EIItem* EIReader::findInDescendants(EIItem* parent,QString fullName)
{
	EIItem* curChild;
	QString curFullName = parent->name();

	int curDepth = parent->depth();
	int lookingDepth = fullName.split(".").size()-1;

	// check if it is this component
	if(curFullName == fullName)
		return parent;

	//first check name compatibility
	if(fullName.indexOf(curFullName)!=0)
		return NULL;

	//get child name to visit
	QString childShortName = fullName.section(".",curDepth,curDepth);

	// looking in children
	for(int iChild=0;iChild<parent->childCount();iChild++)
	{
		curChild = parent->child(iChild);
		if(curChild->name(EI::SHORT)==childShortName)
			return findInDescendants(curChild,fullName);
	}
	return NULL;

}

void EIReader::addEmptyStream(EIItem* parent)
{
	EIStream* newStream = new EIStream(parent);
	bool ok = parent->addChild(newStream);
	if(!ok)
		delete newStream;
}

void EIReader::addEmptyGroup(EIItem* parent)
{
	EIGroup* newGroup = new EIGroup();
	bool ok = parent->addChild(newGroup);
	if(!ok)
		delete newGroup;
}

void EIReader::removeItem(EIItem* _item)
{
	EIItem* parent = _item->getParent();
	if(parent)
	{
		int i=0;
		while((i<parent->childCount())&&(parent->child(i)!=_item))
			i++;
		if(i<parent->childCount())
		{
			delete parent->child(i);
			parent->removeChild(i);
		}
	}
}
void EIReader::setItems(QDomElement & domEl,EIItem* rootEI)
{
	rootEI->clearDescendants();

	int curDepth=0;
	QDomNode n;
	QDomElement domItem;
	QString parentName;
	bool curDepthFound = true;
	EIItem* newEIitem;

	n = domEl.firstChild();
	while( !n.isNull() )
	{
		// curLevel elements
		domItem = n.toElement();

		QString node = domItem.nodeName();
		
		if(node.compare("EIStream",Qt::CaseInsensitive)==0)
		{
			newEIitem = new EIStream(domItem);
			rootEI->addChild(newEIitem);
		}

		if(node.compare("EIGroup",Qt::CaseInsensitive)==0)
		{
			newEIitem = new EIGroup(domItem);
			rootEI->addChild(newEIitem);
		}

		//fill children
		setItems(domItem,newEIitem);

		n = n.nextSibling();
	}
}

QList<EIStream*> EIReader::getStreams(EIItem* parent)
{
	QList<EIStream*> result;
	
	for(int i=0;i<parent->streamChildCount();i++)
		result.push_back(dynamic_cast<EIStream*>(parent->streamChild(i)));

	for(int i=0;i<parent->childCount();i++)
	{
		if(parent->child(i)->getEIType()!=EI::STREAM)
			result.append(getStreams(parent->child(i)));
	}
	return result;
}

QList<EIItem*> EIReader::getItems(EIItem* parent,bool recursive,EI::Type filter)
{
	QList<EIItem*> result;
	
	for(int i=0;i<parent->childCount();i++)
	{
		if((filter == EI::GENERIC)||(parent->child(i)->getEIType()==filter))
			result.push_back(parent->child(i));
	}

	if(recursive)
	{
		for(int i=0;i<parent->childCount();i++)
		{
			if(parent->child(i)->getEIType()!=EI::STREAM)
				result.append(getItems(parent->child(i),recursive,filter));
		}
	}

	return result;
}

QList<EIStream*> EIReader::getValidStreams(EIItem*parent,MOOptVector *variables,bool onlyChecked)
{
	QList<EIStream*> result = getStreams(parent);
	
	EIStream* curStream;
	for(int i=0;i<result.size();i++)
	{
		curStream = result.at(i);
		if(!curStream->isValid(variables,QString()))
			result.removeAt(i);

		if(onlyChecked && !curStream->isChecked())
			result.removeAt(i);			
	}
	return result;
}

void EIReader::getValidTk(EIItem* parent, QList<METemperature> & Tk,MOOptVector *variables)
{
	QList<EIStream*> streams = getValidStreams(parent,variables,true);

	EIStream* curStream;
	double TinProv,ToutProv,DTmin2prov; // Temperature in Kelvin
	QList<double> allT; // in Kelvin
	
	bool ok1,ok2;
	
	for(int i=0;i<streams.size();i++)
	{
		curStream = streams.at(i);
		TinProv = curStream->Tin.getNumValue(variables,METemperature::K,ok1);
		ToutProv = curStream->Tout.getNumValue(variables,METemperature::K,ok2);
		DTmin2prov = curStream->getFieldValue(EIStream::DTMIN2).toDouble();
		assert(TinProv!=ToutProv);
		if(TinProv<ToutProv)
		{
			//Cold stream
			TinProv += DTmin2prov;
			ToutProv += DTmin2prov;
		}
		else
		{
			//Hot stream
			TinProv += -DTmin2prov;
			ToutProv += -DTmin2prov;
		}

		allT.push_back(TinProv);
		allT.push_back(ToutProv);
	}

	//sort
	qSort(allT.begin(),allT.end());
	//remove duplicates
	std::unique(allT.begin(),allT.end());
	
	//fill result with dimensional values
	Tk.clear();
	for(int i=0;i<allT.size();i++)
	{
		Tk.push_back(METemperature(allT.at(i),METemperature::K));
	}
}

void EIReader::getFirstGroupFact(EIItem* item,EIGroupFact* &fact,EIGroup* &group)
{
	fact = NULL;
	group = NULL;
	EIItem* parent=item;
	while(fact==NULL && parent!=NULL)
	{
		parent = parent->getParent();
		EIGroup* _group = dynamic_cast<EIGroup*>(parent);
		if(_group)
		{
			if(_group->isFactVariable())
			{
				fact = _group->getFact();
				group = _group;
			}
		}
	}
}


QStringList EIReader::getAllItemNames(EIItem* item,EI::Type filter)
{
	QStringList itemNames;
	EIItem* curChild;
	for(int i=0;i<item->childCount();i++)
	{
		curChild = item->child(i);
		
		if((filter==EI::GENERIC)||(curChild->getEIType()==filter))
			itemNames.push_back(curChild->name(EI::FULL));

		itemNames.append(getAllItemNames(curChild,filter));
	}
	return itemNames;
}