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

 	@file TableEIItems.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "TableEIItems.h"

TableEIItems::TableEIItems(EIItem * _rootElement,bool _editable,EI::Type _filter,bool _recursive)
:editable(_editable)
{
	rootElement = _rootElement;
	filter = _filter;
	recursive = _recursive;
    enabled = true;

	updateList();

    connect(rootElement,SIGNAL(deleted()),this,SLOT(onRootElementDeleted()));
	connect(rootElement,SIGNAL(modified()),this,SLOT(updateList()));
}

TableEIItems::~TableEIItems(void)
{
}


void TableEIItems::updateList()
{
    if(enabled)
    {
	eiItems.clear();
    eiItems = EIReader::getItems(rootElement,recursive,filter);
}
}

int TableEIItems::columnCount(const QModelIndex &parent) const
{
	switch(filter)
	{
	case EI::STREAM :
		return EIStream::nbFields;
	case EI::GROUP :
		return EIGroup::nbFields;
	default :
		return 0;
	}
}

QVariant TableEIItems::data(const QModelIndex &index, int role) const
{
    if(enabled)
    {
	QVariant result;

	if (!index.isValid())
		return QVariant();

	if(index.column()<0 || index.column()>=columnCount())
		return QVariant();

	EIItem *item = static_cast<EIItem*>(index.internalPointer());
	if(!item)
        return QVariant();
	
    if((role==Qt::CheckStateRole) && (index.column()>0))
        return QVariant();

	switch(role)
	{
			case Qt::DisplayRole :
			case Qt::EditRole :
				result = item->getFieldValue(index.column(),role);
				return result;
				break;
			case Qt::CheckStateRole :
					if(item->isChecked())
						return Qt::Checked;
					else
						return Qt::Unchecked;
			case Qt::ToolTipRole :
				return item->getStrToolTip();
				break;

	}
}

	return QVariant();
}

bool TableEIItems::setData(const QModelIndex &index, const QVariant &value, int role)
{

if (!index.isValid())
		return false;

	if(index.column()<0 || index.column()>=columnCount())
		return false;

	if(index.parent().isValid())
	{
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
	}
	dataChanged(QModelIndex(),QModelIndex());
	return true;
}


Qt::ItemFlags TableEIItems::flags(const QModelIndex &index) const
{
	Qt::ItemFlags _flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	
	if(editable)
		_flags = _flags | Qt::ItemIsEditable;

	if(index.column()==0)
		_flags = _flags | Qt::ItemIsUserCheckable ;

	return _flags;
}

QVariant TableEIItems::headerData(int section, Qt::Orientation orientation,
									int role) const
{
	if((orientation==Qt::Horizontal))
	{
		if (role == Qt::DisplayRole)
		{
			switch(filter)
			{
			case EI::STREAM:
				return EIStream::sFieldName(section,role);
			case EI::GROUP:
				return EIGroup::sFieldName(section,role);
			}
		}
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex TableEIItems::index(int row, int column, const QModelIndex &parent)
const
{
	if((row>-1)&&(row < eiItems.size()))
	{
		return createIndex(row,column,eiItems.at(row));
	}
    else
        return QModelIndex();
}

QModelIndex TableEIItems::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int TableEIItems::rowCount(const QModelIndex &parent) const
{
    if(enabled)
    {
	return eiItems.size();
}
    else
        return 0;
}

void TableEIItems::onRootElementDeleted()
{
    enabled = false;
    this->reset();
}
