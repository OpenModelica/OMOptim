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

 	@file ModClassTree.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ModClassTree.h"

#include <QtGui>


ModClassTree::ModClassTree(ModReader* _modReader,ModClass* _rootElement,QObject *parent)
: QAbstractItemModel(parent)
{
	modReader = _modReader;
	rootElement = _rootElement;

	showOnlyComponents = false;

	// each change in rootElement descendants implicates all tree redraw
	// to optimize (more signals and index find function from ModClass*)
	connect(rootElement,SIGNAL(modified()),this,SLOT(allDataChanged()));
	connect(rootElement,SIGNAL(cleared()),this,SLOT(allDataCleared()));
}

ModClassTree::~ModClassTree()
{
}

int ModClassTree::columnCount(const QModelIndex &parent) const
{
		//return ModClass::nbFields;
	return 1;
}

QVariant ModClassTree::data(const QModelIndex &index, int role) const
{
		if (!index.isValid())
			return QVariant();

		if (role != Qt::DisplayRole && role !=Qt::ToolTipRole)
			return QVariant();

		if(index.column()<0 || index.column()>=ModClass::nbFields)
			return QVariant();

		ModClass *item = static_cast<ModClass*>(index.internalPointer());

		if(item)
		{
			if(!item->childrenReaden())
				modReader->readFromOmcV2(item,1);

			if (role == Qt::ToolTipRole)
			{
				QString tooltip = item->getStrToolTip();
				return tooltip;
			}

			return item->getFieldValue(index.column(),role);
		}
		else
		{
			return QVariant();
		}
}

Qt::ItemFlags ModClassTree::flags(const QModelIndex &index) const
{
		if (!index.isValid())
			return Qt::ItemIsEnabled;

		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

}

QVariant ModClassTree::headerData(int section, Qt::Orientation orientation,
									int role) const
{
		if(section<0 || section>=ModClass::nbFields)
			return QVariant();

		if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
			return ModClass::sFieldName(section,role);

	return QVariant();
}

 QStringList ModClassTree::mimeTypes() const
 {
     QStringList types;
     types << "application/vnd.text.list";
     return types;
 }

 QMimeData* ModClassTree::mimeData(const QModelIndexList &indexes) const
 {
     QMimeData *mimeData = new QMimeData();
     QByteArray encodedData;

     QDataStream stream(&encodedData, QIODevice::WriteOnly);
	 if(indexes.size()==1)
	 {
		 ModClass* _modEl = (ModClass*)indexes.at(0).internalPointer();
		 if(_modEl)
		 {
			 stream<< _modEl->name(Modelica::FULL);
		 }
	 }

     mimeData->setData("application/vnd.text.list", encodedData);
     return mimeData;
 }

QModelIndex ModClassTree::index(int row, int column, const QModelIndex &parent)
const
{
		if(!hasIndex(row,column,parent))
			return QModelIndex();

		ModClass *parentComponent;

		if (!parent.isValid())
			parentComponent = rootElement;
		else
			parentComponent = static_cast<ModClass*>(parent.internalPointer());

		// looking in children
		int nbPacks = parentComponent->packageChildCount();
		int nbModels = parentComponent->modelChildCount();
		int nbComps = parentComponent->compChildCount();

		if(row<0 || row>= nbPacks+nbModels+nbComps)
			return QModelIndex();


		ModClass *childElement = NULL;
		// start by packages
		if(row<nbPacks)
			childElement = parentComponent->packageChild(row);
		// then models
		else if (row<nbPacks+nbModels)
			childElement = parentComponent->modelChild(row - nbPacks);
		// then components
		else if (row < nbPacks+nbModels+nbComps)
			childElement = parentComponent->compChild(row - nbPacks - nbModels);

		if (childElement)
			return createIndex(row, column, childElement);
		else
			return QModelIndex();
}

QModelIndex ModClassTree::parent(const QModelIndex &index) const
{
		if (!index.isValid())
			return QModelIndex();

		ModClass *childElement = static_cast<ModClass*>(index.internalPointer());
		
		ModClass *parentElement  = NULL;
		parentElement = childElement->getParent();

		if (parentElement == rootElement)
			return QModelIndex();

		ModClass *grandParentElement = NULL;
		grandParentElement = parentElement->getParent();

		//looking for row number of child in parent
		int nbPacks = grandParentElement->packageChildCount();
		int nbModels = grandParentElement->modelChildCount();
		int nbComps = grandParentElement->compChildCount();

		int iC=0;
		bool found = false;
		//start by packages
		while(!found && iC<nbPacks)
		{
			found = (grandParentElement->packageChild(iC)==parentElement);	
			if(!found)
				iC++;
		}

		//then Models
		while(!found && iC<nbPacks+nbModels)
		{
			found = (grandParentElement->modelChild(iC-nbPacks)==parentElement);	
			if(!found)
				iC++;
		}

		// then components
		while(!found && iC<nbPacks + nbModels + nbComps)
		{
			found = (grandParentElement->compChild(iC - nbPacks - nbModels)==parentElement);	
			if(!found)
				iC++;
		}


		if(!found)
		{
			// ERROR
			return QModelIndex();
		}

		return createIndex(iC, 0, parentElement);
}

int ModClassTree::rowCount(const QModelIndex &parent) const
{
		ModClass *parentElement;

		if (parent.column() > 0)
         return 0;

		if (!parent.isValid())
			parentElement = rootElement;
		else
			parentElement = static_cast<ModClass*>(parent.internalPointer());

		return parentElement->childCount();
}



ModClass* ModClassTree::findItem(QString _fullName)
{
	return modReader->findInDescendants(rootElement,_fullName);
}



void ModClassTree::allDataChanged()
{
	emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
	emit layoutChanged();
}

void ModClassTree::allDataCleared()
{
	reset();
	//emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
	//emit layoutChanged();
}
