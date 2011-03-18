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

 	@file BlockSubstitutions.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "BlockSubstitutions.h"
#include "LowTools.h"

BlockSubstitutions::BlockSubstitutions(void)
{
}

BlockSubstitutions::BlockSubstitutions(Project* _project,ModModelPlus* _model,QDomElement _el,ModClass* _modelRoot,ModReader* _modReader)
{
	QDomElement e2 = _el.firstChildElement();	
	while( !e2.isNull() )
	{


		if(e2.tagName()=="BlockSubstitution")
		{
			BlockSubstitution* _newBlock = new BlockSubstitution(_project,_model,_model->connections(),e2,_modelRoot,_modReader);
			add(_newBlock);
		}
		e2 = e2.nextSiblingElement();
	}
}

BlockSubstitutions::~BlockSubstitutions(void)
{
}

BlockSubstitutions* BlockSubstitutions::clone()
{
	BlockSubstitutions* newVector = new BlockSubstitutions();

	int i;
	for(i=0;i<size();i++)
	{
		newVector->push_back(this->at(i)->clone());
	}
	return newVector;

}

QStringList BlockSubstitutions::getReplacedComponentsNames()
{
	QStringList list;

	for(int i=0;i<size();i++)
	{
		list << this->at(i)->orgComponent;
	}

	LowTools::removeDuplicates(list);

	return list;
}
bool BlockSubstitutions::isReplacedComponent(QString _name)
{

	return (getReplacedComponentsNames().indexOf(_name)>-1);
}

QList<int> BlockSubstitutions::getReplacedComponentIndexes(QString _name)
{
	QList<int> indexes;

	for(int i=0;i<size();i++)
	{
		if(at(i)->orgComponent==_name)
			indexes << i;
	}

	return indexes;
}

QStringList BlockSubstitutions::getReplacingComponentNames(QString _replaced)
{
	QList<int> indexes = getReplacedComponentIndexes(_replaced);
	QStringList replacingNames;

	for(int i=0; i< indexes.size(); i++)
	{
		if(!at(indexes.at(i))->subComponent.isEmpty())
			replacingNames << at(indexes.at(i))->subComponent;
	}
	return replacingNames;
}

QStringList BlockSubstitutions::getReplacingComponentNames(ModClass* _replacedComp)
{
	return getReplacingComponentNames(_replacedComp->name(Modelica::FULL));
}

BlockSubstitution* BlockSubstitutions::find(QString replacedName,QString replacingName)
{


	bool found=false;
	int i=0;
	while(!found && i<size())
	{
		if((at(i)->orgComponent == replacedName) && (at(i)->subComponent == replacingName) )
			return at(i);
		i++;
	}
	return NULL;
}

void BlockSubstitutions::add(BlockSubstitution* _added)
{
	push_back(_added);
	emit changed();
}

int BlockSubstitutions::getSize()
{
	return size();
}
BlockSubstitution* BlockSubstitutions::getAt(int i)
{
	return at(i);
}



bool BlockSubstitutions::removeBlock(QString replacedName,QString replacingName)
{
	bool found=false;
	int i=0;
	while(i<size())
	{
		if((at(i)->orgComponent == replacedName) && (at(i)->subComponent == replacingName) )
		{
			found = true;
			remove(i);
		}
		else
			i++;
	}

	emit changed();
	return found;
}

bool BlockSubstitutions::removeBlocks(QString replacedName)
{

	bool found=false;
	int i=0;
	while(i<size())
	{
		if(at(i)->orgComponent == replacedName)
		{
			found = true;
			remove(i);
		}
		else
			i++;
	}
	emit changed();
	return found;
}



QDomElement BlockSubstitutions::toXMLData(QDomDocument & _doc)
{
	QDomElement cBlocks = _doc.createElement("BlockSubstitutions");
	for(int i=0;i<size();i++)
	{
		QDomElement cBlock = at(i)->toXMLData(_doc);
		cBlocks.appendChild(cBlock);
	}

	return cBlocks;
}
