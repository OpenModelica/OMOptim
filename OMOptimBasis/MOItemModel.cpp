// $Id: MOItemModel.cpp 13878 2012-11-13 02:32:03Z hubert.thieriot $
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

     @file MOItemModel.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "MOItemModel.h"
#include <QDrag>
#include <QDragEnterEvent>
#include "InfoSender.h"
#include <QDebug>


MOItemModel::MOItemModel(bool show,bool editable):QAbstractTableModel()
{
    _editable = editable;
    _item = NULL;
    _showDescription = show;
}

MOItemModel::~MOItemModel()
{
}

void MOItemModel::setMOItem(MOItem* item)
{
    this->disconnect(_item);
    _item->disconnect(this);

    beginResetModel();
    _item = item;
    endResetModel();

    if(_item)
        connect(_item,SIGNAL(deleted()),this,SLOT(onItemDeleted()));
}

void MOItemModel::setShowDescription(bool show)
{
    _showDescription = show;
}

int MOItemModel::columnCount(const QModelIndex &parent) const
{
    if(_showDescription)
        return 3;
    else
        return 2;
}

void MOItemModel::onItemDeleted()
{
    setMOItem(NULL);
}

QVariant MOItemModel::data(const QModelIndex &index, int role) const
{
    if(!_item)
        return QVariant();

    if (!index.isValid())
        return QVariant();

    if(index.column()<0 || index.column()>=columnCount())
        return QVariant();

    int iCol = index.column();
    int iRow = index.row();

    if(role!=Qt::DisplayRole)
        return QVariant();

    if(iCol == 0)
    {
        return _item->getFieldName(iRow,role);
    }
    if(iCol==1)
    {
        return _item->getFieldValue(iRow,role);
    }
    if(iCol==2)
    {
        return _item->getFieldDescription(iRow);
    }
    return QVariant();
}

bool MOItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok;
    if(!_editable || !_item)
        return false;

    if (!index.isValid())
        return false;

    if(index.column()!=1)
        return false;

    switch(role)
    {
    case Qt::EditRole :
        ok = _item->setFieldValue(index.row(),value);
        if(ok)
            dataChanged(index,index);
        return ok;
        break;
    default:
        return false;
    }
}

//QModelIndex MOItemModel::index(int row, int column, const QModelIndex &parent)
//const
//{
//    if((row>-1)&&(row < rowCount(parent)))
//    {
//        return createIndex(row,column);
//    }
//    else
//        return QModelIndex();
//}

//QModelIndex MOItemModel::parent(const QModelIndex &index) const
//{
//    return QModelIndex();
//}

Qt::ItemFlags MOItemModel::flags(const QModelIndex &index) const
{
    if (!_item || !index.isValid())
        return Qt::ItemIsEnabled;


    Qt::ItemFlags _flags;

    _flags = QAbstractItemModel::flags(index)| Qt::ItemIsSelectable ;

    if(_editable && index.column()==1 && !_item->isProtectedField(index.row()))
        _flags = _flags | Qt::ItemIsEditable;

    return _flags;
}


QVariant MOItemModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const
{
    if((orientation==Qt::Horizontal))
    {
        if (role == Qt::DisplayRole)
        {
            if(section==0)
                return "Parameter";
            if(section==1)
                return "Value";
            if(section==2)
                return "Description";
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}



int MOItemModel::rowCount(const QModelIndex &parent) const
{
    if(_item)
        return _item->getNbFields();
    else
        return 0;
}
