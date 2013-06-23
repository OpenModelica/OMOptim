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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

        @file OMCases.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "OMCases.h"

OMCases::OMCases(QString _name)
{
    name = _name;
}

OMCases::~OMCases(void)
{
    clear();
}

void OMCases::clear()
{
    beginResetModel();
    for(int i=0;i<items.size();i++)
    {
//        items.at(i)->dumpObjectInfo();
//        items.at(i)->dumpObjectTree();
        delete items.at(i);
    }
    items.clear();
    endResetModel();
}

int OMCases::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant OMCases::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    OMCase *omCase = items.at(index.row());

    return omCase->name();
}

Qt::ItemFlags OMCases::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant OMCases::headerData(int section, Qt::Orientation orientation,
                             int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return name;

    return QVariant();
}

QModelIndex OMCases::index(int row, int column, const QModelIndex &parent)
const
{
    return createIndex(row, column);
}

QModelIndex OMCases::parent(const QModelIndex &index) const
{
    return QModelIndex();

}

int OMCases::rowCount(const QModelIndex &parent) const
{
    
    if (!parent.isValid())
        return items.size();
    else
        return 0;
}

void OMCases::addCase(OMCase* omCase)
{
    int index=items.size();
    insertRow(index);//,createIndex(0,0));
    beginInsertRows(QModelIndex(),index,index);
    items.push_back(omCase);
    endInsertRows();
}

bool OMCases::removeRow(int row,const QModelIndex &parent)
{
    if((row>=0) &&(row<items.size()))
    {
        beginRemoveRows(parent,row,row);
//        items.at(row)->dumpObjectInfo();
//        items.at(row)->dumpObjectTree();
        delete items.at(row);
        items.remove(row);
        endRemoveRows();

        return true;
    }
    else
    {
        return false;
    }
}


OMCase* OMCases::findItem(QString itemName)
{
    bool found = false;
    int i=0;
    int nbItems=items.size();
    QString itemName2;

    while((!found)&&(i<nbItems))
    {
        itemName2=items.at(i)->name();
        found=!(itemName.compare(itemName2,Qt::CaseInsensitive));
        i++;
    }
    if(!found)
    {
        return NULL;
    }
    else
    {
        return items.at(i-1);
    }
}

/*******************************************************************/
OMCasesCombiner::OMCasesCombiner(OMCases* problems,OMCases* results,QObject* parent)
    :QAbstractItemModel(parent)
{
    _problems = problems;
    _results = results;
    _name = "Problems";

    connect(_problems,SIGNAL(rowsAboutToBeInserted ( const QModelIndex & , int , int  )),this,SLOT(rowsAboutToBeInsertedPb( const QModelIndex & , int , int  )));
    connect(_problems,SIGNAL(rowsAboutToBeRemoved ( const QModelIndex & , int , int  )),this,SLOT(rowsAboutToBeRemovedPb( const QModelIndex & , int , int  )));
    connect(_problems,SIGNAL(rowsInserted ( const QModelIndex & , int , int  )),this,SLOT(rowsInsertedPb( const QModelIndex & , int , int  )));
    connect(_problems,SIGNAL(rowsRemoved ( const QModelIndex & , int , int  )),this,SLOT(rowsRemovedPb( const QModelIndex & , int , int  )));
    connect(_problems,SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )),this,SLOT(dataChangedPb ( const QModelIndex & , const QModelIndex &  )));

    connect(_results,SIGNAL(rowsAboutToBeInserted ( const QModelIndex & , int , int  )),this,SLOT(rowsAboutToBeInsertedRes( const QModelIndex & , int , int  )));
    connect(_results,SIGNAL(rowsAboutToBeRemoved ( const QModelIndex & , int , int  )),this,SLOT(rowsAboutToBeRemovedRes( const QModelIndex & , int , int  )));
    connect(_results,SIGNAL(rowsInserted ( const QModelIndex & , int , int  )),this,SLOT(rowsInsertedRes( const QModelIndex & , int , int  )));
    connect(_results,SIGNAL(rowsRemoved ( const QModelIndex & , int , int  )),this,SLOT(rowsRemovedRes( const QModelIndex & , int , int  )));
    connect(_results,SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )),this,SLOT(dataChangedRes ( const QModelIndex & , const QModelIndex &  )));
}

OMCasesCombiner::~OMCasesCombiner(void)
{

}

OMCase* OMCasesCombiner::item(const QModelIndex & index)
{
    if (!index.isValid() || ! index.parent().isValid())
        return NULL;


    int row = index.row();
    // if parent is problem
    if(index.parent().data()=="Problems")
    {
        if(row<_problems->rowCount())
            return _problems->at(row);
    }
    if(index.parent().data()=="Results")
    {
        if(row<_results->rowCount())
            return _results->at(row);
    }

    return NULL;
}

int OMCasesCombiner::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant OMCasesCombiner::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    // Problems or Result
    if(!index.parent().isValid())
    {
        if(index.row()==0)
            return "Problems";
        if(index.row()==1)
            return "Results";
    }

    // get parent
    if(index.parent().isValid())
    {
        int row = index.row();
        // if parent is problem
        if(index.parent().data()=="Problems")
        {
            if(row<_problems->rowCount())
                return _problems->at(row)->name();
        }
        if(index.parent().data()=="Results")
        {
            if(row<_results->rowCount())
                return _results->at(row)->name();
        }
    }

    return QVariant();
}

Qt::ItemFlags OMCasesCombiner::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant OMCasesCombiner::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    return QVariant();
}

QModelIndex OMCasesCombiner::index(int row, int column, const QModelIndex &parent)
const
{
    if(!hasIndex(row,column,parent))
        return QModelIndex();


    if (!parent.isValid())
    {
        if(row==0)
            return problemsRootIndex();
        else if(row==1)
            return resultsRootIndex();
        else
            return QModelIndex();
    }
    else
    {
        if((parent.internalPointer() == _problems) && (row < _problems->rowCount()))
        {
            return createIndex(row, column,_problems->at(row));
        }
        else if((parent.internalPointer()== _results) && (row < _results->rowCount()))
        {
            return createIndex(row, column,_results->at(row));
        }
        else
            return QModelIndex();
    }

}
QModelIndex OMCasesCombiner::problemsRootIndex() const
{
    return createIndex(0,0,_problems);
}

QModelIndex OMCasesCombiner::resultsRootIndex() const
{
    return createIndex(1,0,_results);
}


QModelIndex OMCasesCombiner::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    void *data = index.internalPointer();

    if((data==_problems)||(data==_results))
        return QModelIndex();
    else
    {
        if(_problems->contains((OMCase*)data))
            return problemsRootIndex();
        else if(_results->contains((OMCase*)data))
            return resultsRootIndex();
    }

    return QModelIndex();
}

int OMCasesCombiner::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 2;
    else
    {
        if(parent.internalPointer()==_problems)
            return _problems->rowCount();
        else if(parent.internalPointer()==_results)
            return _results->rowCount();
        else
            return 0;
    }
}

void OMCasesCombiner::rowsAboutToBeInsertedPb( const QModelIndex & parent, int start, int end )
{
    beginInsertRows(problemsRootIndex(),start,end);
}

void OMCasesCombiner::rowsAboutToBeRemovedPb( const QModelIndex & parent, int start, int end )
{
    beginRemoveRows(problemsRootIndex(),start,end);
}

void OMCasesCombiner::rowsInsertedPb( const QModelIndex & parent, int start, int end )
{
    endInsertRows();
}

void OMCasesCombiner::rowsRemovedPb( const QModelIndex & parent, int start, int end )
{
    endRemoveRows();
}

void OMCasesCombiner::dataChangedPb( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
    dataChanged(topLeft,bottomRight);
}

void OMCasesCombiner::rowsAboutToBeInsertedRes( const QModelIndex & parent, int start, int end )
{
    beginInsertRows(resultsRootIndex(),start,end);
}

void OMCasesCombiner::rowsAboutToBeRemovedRes( const QModelIndex & parent, int start, int end )
{
    beginRemoveRows(resultsRootIndex(),start,end);
}

void OMCasesCombiner::rowsInsertedRes( const QModelIndex & parent, int start, int end )
{
    endInsertRows();
}

void OMCasesCombiner::rowsRemovedRes( const QModelIndex & parent, int start, int end )
{
    endRemoveRows();
}

void OMCasesCombiner::dataChangedRes( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
    dataChanged(topLeft,bottomRight);
}

