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

 	@file TreeEIStreams.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "TreeEIStreams.h"


TreeEIStreams::TreeEIStreams(EIItem * _rootElement,bool _showFields,bool _editable,EIReader* _eiReader)
:showFields(_showFields),editable(_editable)
{
	eiReader = _eiReader;
	rootElement = _rootElement;

	// each change in rootElement descendants implicates all tree redraw
	// to optimize (more signals and index find function from ModClass*)
	connect(rootElement,SIGNAL(modified()),this,SLOT(allDataChanged()));
	connect(rootElement,SIGNAL(cleared()),this,SLOT(allDataCleared()));
}

TreeEIStreams::~TreeEIStreams(void)
{
}

int TreeEIStreams::columnCount(const QModelIndex &parent) const
{
	return EIStream::nbFields;
}

QVariant TreeEIStreams::data(const QModelIndex &index, int role) const
{
	QVariant result;

	if (!index.isValid())
		return QVariant();

	if(index.column()<0 || index.column()>=columnCount())
		return QVariant();

	EIItem *item = static_cast<EIItem*>(index.internalPointer());
	if(!item)
		return 0;
	
	if((item->getEIType()==EI::GROUP)
		&&(index.column()>0)
		&&(role==Qt::DisplayRole))
		return QVariant();

	switch(role)
	{
			case Qt::DisplayRole :
			case Qt::EditRole :
				result = item->getFieldValue(index.column(),role);
				return result;
				break;
			case Qt::ToolTipRole :
				return item->getStrToolTip();
				break;
	}
	if(isCheckable(index)&&(role==Qt::CheckStateRole))
	{
		if(item->isChecked())
			return Qt::Checked;
		else
			return Qt::Unchecked;
	}				

	return QVariant();
}

bool TreeEIStreams::setData(const QModelIndex &index, const QVariant &value, int role)
{

	if (!index.isValid())
		return false;

	if(index.column()<0 || index.column()>=columnCount())
		return false;

	int checkState;
	EIItem *item = static_cast<EIItem*>(index.internalPointer());
	if(item)
	{
		switch(role)
		{
		case Qt::CheckStateRole :
			checkState = value.toInt();
			if((checkState == Qt::Unchecked) || (checkState == Qt::PartiallyChecked))
				item->setChecked(false);
			else
				item->setChecked(true);
			dataChanged(index,index);
			return true;
			break;
		case Qt::EditRole :
			item->setFieldValue(index.column(),value);
			dataChanged(index,index);
			return true;
			break;
		default:
			return false;
		}
	}

	dataChanged(QModelIndex(),QModelIndex());
	return true;
}

Qt::ItemFlags TreeEIStreams::flags(const QModelIndex &index) const
{
	Qt::ItemFlags _flags = Qt::NoItemFlags;
	

	if(!index.isValid())
		return _flags;

	EIItem* item = static_cast<EIItem*>(index.internalPointer());
	bool ok;
	switch(item->getEIType())
	{
	case EI::STREAM :
		ok=true;
		break;
	case EI::GROUP :
		ok=(index.column()==0);
		break;
	}

	if(ok)
	{
		_flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

		if(editable)
			_flags = _flags | Qt::ItemIsEditable;

		if(index.column()==0)
			_flags = _flags | Qt::ItemIsUserCheckable ;
	}
	return _flags;
}

QVariant TreeEIStreams::headerData(int section, Qt::Orientation orientation,
								   int role) const
{
	if((orientation==Qt::Horizontal)&&(showFields))
	{
		if (role == Qt::DisplayRole)
		{
			return EIStream::sFieldName(section,role);
		}
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex TreeEIStreams::index(int row, int column, const QModelIndex &parent)
const
{
	if(!hasIndex(row,column,parent))
			return QModelIndex();

		EIItem *parentComponent;

		if (!parent.isValid())
			parentComponent = rootElement;
		else
			parentComponent = static_cast<EIItem*>(parent.internalPointer());

		// looking in children
		int nbStreams = parentComponent->streamChildCount();
		int nbGroups = parentComponent->groupChildCount();
		
		if((row<0) || (row>= nbStreams+nbGroups))
			return QModelIndex();

		EIItem *childElement = NULL;
		// start by packages
		if(row<nbGroups)
			childElement = parentComponent->groupChild(row);
		// then models
		else if (row<nbStreams+nbGroups)
			childElement = parentComponent->streamChild(row - nbGroups);
		
		if (childElement)
			return createIndex(row, column, childElement);
		else
			return QModelIndex();
}

QModelIndex TreeEIStreams::parent(const QModelIndex &index) const
{
		if (!index.isValid())
			return QModelIndex();

		EIItem *childElement = static_cast<EIItem*>(index.internalPointer());
		
		EIItem *parentElement  = NULL;
		parentElement = childElement->getParent();

		if (parentElement == rootElement)
			return QModelIndex();

		EIItem *grandParentElement = NULL;
		grandParentElement = parentElement->getParent();

		//looking for row number of child in parent
		int nbGroups = grandParentElement->groupChildCount();
		int nbstreams = grandParentElement->streamChildCount();

		int iC=0;
		bool found = false;
		//start by groups
		while(!found && iC<nbGroups)
		{
			found = (grandParentElement->groupChild(iC)==parentElement);	
			if(!found)
				iC++;
		}

		//then streams
		while(!found && iC<nbGroups+nbstreams)
		{
			found = (grandParentElement->streamChild(iC-nbGroups)==parentElement);	
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

int TreeEIStreams::rowCount(const QModelIndex &parent) const
{
	EIItem *parentElement;

	if (!parent.isValid())
		parentElement = rootElement;
	else
		parentElement =static_cast<EIItem*>(parent.internalPointer());

	return parentElement->childCount();
}

EIItem* TreeEIStreams::findItem(QString _fullName)
{
	return eiReader->findInDescendants(rootElement,_fullName);
}


void TreeEIStreams::allDataChanged()
{
	emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
	emit layoutChanged();
}

void TreeEIStreams::allDataCleared()
{
	reset();
	//emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
	//emit layoutChanged();
}

bool TreeEIStreams::isCheckable(const QModelIndex _index) const
{
	return _index.column()==0;
}

void TreeEIStreams::publicBeginResetModel()
{
	beginResetModel();
}
void TreeEIStreams::publicEndResetModel()
{
	endResetModel();
}