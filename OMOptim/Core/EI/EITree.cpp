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

        @file EITree.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EITree.h"


EITree::EITree(bool showFields,bool editable,QString rootName)
    :_showFields(showFields),_editable(editable)
{
    _rootElement = new EIItem(NULL,rootName);
    _enabled = true;

    // each change in _rootElement descendants implicates all tree redraw
    // to optimize (more signals and index find function from ModClass*)
    connect(_rootElement,SIGNAL(modified()),this,SLOT(allDataChanged()));
    connect(_rootElement,SIGNAL(cleared()),this,SLOT(allDataCleared()));
    connect(_rootElement,SIGNAL(deleted()),this,SLOT(onRootElementDeleted()));
}

EITree::EITree(const EITree &tree)
{
    _showFields = tree._showFields;
    _editable = tree._editable;
    _enabled = tree._enabled;

    _rootElement = tree._rootElement->clone();

    // each change in rootElement descendants implicates all tree redraw
    // to optimize (more signals and index find function from ModClass*)
    connect(_rootElement,SIGNAL(modified()),this,SLOT(allDataChanged()));
    connect(_rootElement,SIGNAL(cleared()),this,SLOT(allDataCleared()));
    connect(_rootElement,SIGNAL(deleted()),this,SLOT(onRootElementDeleted()));
}

 EITree::EITree(QDomElement & domEl)
 {
     _rootElement = new EIItem(domEl);
     _enabled = true;

     // each change in _rootElement descendants implicates all tree redraw
     // to optimize (more signals and index find function from ModClass*)
     connect(_rootElement,SIGNAL(modified()),this,SLOT(allDataChanged()));
     connect(_rootElement,SIGNAL(cleared()),this,SLOT(allDataCleared()));
     connect(_rootElement,SIGNAL(deleted()),this,SLOT(onRootElementDeleted()));
 }

EITree::~EITree(void)
{
    delete _rootElement;
}



void EITree::addChild(EIItem* parent, EIItem* child)
{
    QModelIndex parentIndex = indexOf(parent);
    beginInsertRows(parentIndex,parent->childCount(),parent->childCount());
    parent->addChild(child);
    endInsertRows();
}

int EITree::columnCount(const QModelIndex &parent) const
{
    return EIStream::nbFields;
}

QVariant EITree::data(const QModelIndex &index, int role) const
{
    if(_enabled)
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
    else
        return QVariant();
}

bool EITree::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(_enabled)
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
    else
        return false;
}

Qt::ItemFlags EITree::flags(const QModelIndex &index) const
{
    Qt::ItemFlags _flags = Qt::NoItemFlags;
    if(!_enabled || !index.isValid())
        return _flags;


    EIItem* item = static_cast<EIItem*>(index.internalPointer());

    if(item==_rootElement)
        return  Qt::ItemIsEnabled;

    bool ok;
    switch(item->getEIType())
    {
    case EI::STREAM :
        ok=true;
        break;
    case EI::GROUP :
    case EI::MODELCONTAINER :
    case EI::GENERIC:
        ok=(index.column()==0);
        break;
    }


    if(ok)
    {
        _flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        if(_editable)
            _flags = _flags | Qt::ItemIsEditable;

        if(index.column()==0)
            _flags = _flags | Qt::ItemIsUserCheckable ;
    }
    return _flags;
}

QVariant EITree::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if((orientation==Qt::Horizontal)&&(_showFields))
    {
        if (role == Qt::DisplayRole)
        {
            return EIStream::sFieldName(section,role);
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex EITree::indexOf(EIItem* item,int column) const
{
    if(item==_rootElement)
        return createIndex(0,column,item);
    else
        return createIndex(item->indexInParent(),column,item);
}

QModelIndex EITree::index(int row, int column, const QModelIndex &parent)
        const
{
    if(_enabled)
    {
        if(!hasIndex(row,column,parent))
            return QModelIndex();

        EIItem *parentComponent;

        if (!parent.isValid())
            return indexOf(_rootElement,column);
        else
            parentComponent = static_cast<EIItem*>(parent.internalPointer());

        // looking in children
        int nbChildren = parentComponent->childCount();

        if((row<0) || (row>= nbChildren))
            return QModelIndex();

        EIItem* childElement = parentComponent->child(row);
        return createIndex(row, column, childElement);
    }
    else
        return QModelIndex();
}

QModelIndex EITree::parent(const QModelIndex &index) const
{
    if(_enabled)
    {
        if (!index.isValid())
            return QModelIndex();

        EIItem *childElement = static_cast<EIItem*>(index.internalPointer());

        if(childElement==_rootElement)
            return QModelIndex();

        EIItem *parentElement  = childElement->parent();
        return indexOf(parentElement);

    }
    else
        return QModelIndex();
}

int EITree::rowCount(const QModelIndex &parent) const
{
    if(_enabled)
    {
	EIItem *parentElement;

	if (!parent.isValid())
            return 1; // row corresponding to root element
	else
        {
            parentElement =static_cast<EIItem*>(parent.internalPointer());
            return parentElement->childCount();
        }
    }
    else
        return 0;
}

EIItem* EITree::findItem(QString _fullName)
{
    return EIReader::findInDescendants(_rootElement,_fullName);
}

EIItem* EITree::findItem(EI::Type eiType,QVariant itemFieldValue, int iField)
{
    return EIReader::findInDescendants(_rootElement,eiType,itemFieldValue,iField);
}




bool EITree::isCheckable(const QModelIndex _index) const
{
    return _index.column()==0;
}

bool EITree::removeItem(EIItem* item)
{
    EIItem* parent = item->parent();
    return removeChild(parent,item->indexInParent());
}

bool EITree::removeItem(const QModelIndex &index)
{
    if(!index.isValid())
        return false;

    EIItem* eiItem = static_cast<EIItem*>(index.internalPointer());
    EIItem* eiParent = eiItem->parent();

    return removeChild(eiParent,eiItem->indexInParent());
}

bool EITree::removeChild(EIItem* parent, int iChild)
{
    //create index
    QModelIndex parentIndex = indexOf(parent);
    return removeRow(iChild,parentIndex);
}

bool EITree::removeChildren(EIItem* parent)
{
    //create index
    QModelIndex parentIndex = indexOf(parent);
    int nbChildren = rowCount(parentIndex);
    return removeRows(0,nbChildren,parentIndex);
}

void EITree::removeUnchecked()
{
    beginResetModel();
    _rootElement->removeUncheckedDescendants();
    endResetModel();
}

bool EITree::removeRows ( int row, int count, const QModelIndex & parent )
{
    bool ok = true;
    if(!parent.isValid())
        return false;

    EIItem* parentItem = static_cast<EIItem*>(parent.internalPointer());
    int nbRemoved=0;

    beginRemoveRows(parent,row,row+count-1);
    while(nbRemoved<count)
    {
        ok = ok && parentItem->removeChild(row);
        nbRemoved++;
    }
    endRemoveRows();
    return ok;
}

EIItem* EITree::rootElement()
{
    return _rootElement;
}

void EITree::onRootElementDeleted()
{
    _enabled = false;
    //    this->revert();
    this->beginResetModel();
    this->reset();
    this->endResetModel();
}

void EITree::allDataChanged()
{
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    emit layoutChanged();
}

void EITree::allDataCleared()
{
    reset();
    //emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    //emit layoutChanged();
}

QDomElement EITree::toXmlData(QDomDocument & doc)
{
    return _rootElement->toXmlData(doc);
}
