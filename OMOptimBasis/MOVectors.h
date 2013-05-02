// $Id: MOVectors.h 12027 2012-06-12 08:42:03Z hubert.thieriot $
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

  @file MOVectors.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#if !defined(_MOVectors_H)
#define _MOVectors_H


#include "MOVector.h"

/** MOVectors :Container for MOElements (e.g. Variable, OptObjective...)
  Actually, it contains a list of MOVector. It may be used when an integer changes
  the list of contents (e.g. iPoint in optimization results).
*/

template<class ItemClass>
class MOVectors : public QAbstractTableModel 
{

public :

    MOVectors();
    MOVectors(const MOVectors &);
    MOVectors(QDomElement & domList);
    virtual ~MOVectors();

    // MOVectors specific functions
    void setItems(QDomElement & domList);
    void setVector(MOVector<ItemClass>* vector, int iPoint);

    void pushBackVector(MOVector<ItemClass>* vector);
    void clear();


    void setIPoint(int);
    MOVector<ItemClass>* curVector() const;
    MOVector<ItemClass>* at(int iPoint) const;
    int size() const;
    int iPoint() const;


    // save and load functions
    virtual QDomElement toXmlData(QDomDocument & doc,QString listTitle);


    // Functions related to current iPoint
    int rowCount(const QModelIndex &parent ) const;
    int columnCount(const QModelIndex &parent ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QModelIndex index(int row, int column, const QModelIndex &parent)const;



    // drag and drop functions
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data,
                Qt::DropAction action, int row, int column, const QModelIndex &parent);



protected :
    /**
  * is this vector the owner of the content.
  * If yes, content will be deleted with vector, or when an item replaces another one
  */
    QList<MOVector<ItemClass>* > _items;
    void connectVector(int iPoint);
    void disconnectVector(int iPoint);
    int _iPoint;
};





template<class ItemClass>
MOVectors<ItemClass>::MOVectors() : QAbstractTableModel()
{

}



template<class ItemClass>
MOVectors<ItemClass>::MOVectors(const MOVectors & copied)
{
    int iv;
    for(iv=0;iv<copied.size();iv++)
    {
  pushBackVector(copied.at(iv)->clone());
    }
}



template<class ItemClass>
MOVectors<ItemClass>::MOVectors(QDomElement & domList)
{
    // Root element
    QDomNode n = domList.firstChild();
    while( !n.isNull() )
    {
  QDomElement domItem = n.toElement();
  if(domItem.tagName().contains("MOVector"))
  {
      int iPoint = domItem.attribute("iPoint","-1").toInt();
      if(iPoint>-1)
      {
          this->setVector(new MOVector<ItemClass>(domItem,true),iPoint);
      }
  }
  n = n.nextSibling();
    }
}



template<class ItemClass>
MOVectors<ItemClass>::~MOVectors() 
{
  //delete contents
  clear();
}

template<class ItemClass>
void MOVectors<ItemClass>::setVector(MOVector<ItemClass>* vector, int i)
{
    while(i>_items.size())
    {
  pushBackVector(new MOVector<ItemClass>(true));
    }

    if(iPoint()==i)
  beginResetModel();

    if(_items.size()>i)
    {
  // remove existing
  // change curPoint
  delete this->at(i);
  _items.replace(i,vector);
  connectVector(i);
    }
    else
    {
  pushBackVector(vector);
    }

    if(iPoint()==i)
  endResetModel();
}


template<class ItemClass>
void MOVectors<ItemClass>::pushBackVector(MOVector<ItemClass>* vector)
{
    _items.push_back(vector);
}


template<class ItemClass>
void MOVectors<ItemClass>::clear()
{
    setIPoint(-1);

    for(int i=0;i<size();i++)
    {
  delete this->at(i);
    }
    _items.clear();
}

template<class ItemClass>
void MOVectors<ItemClass>::setIPoint(int iPoint)
{
    emit beginResetModel();

    disconnectVector(_iPoint);
    _iPoint = iPoint;
    connectVector(iPoint);

    emit endResetModel();
}

template<class ItemClass>
MOVector<ItemClass>* MOVectors<ItemClass>::curVector() const
{
    if((_iPoint > -1)&&(_iPoint<size()))
  return _items.at(_iPoint);
    else
  return NULL;
}

template<class ItemClass>
MOVector<ItemClass>* MOVectors<ItemClass>::at(int iPoint) const
{
    if((iPoint > -1)&&(iPoint<size()))
    {
  MOVector<ItemClass>* result = _items.at(iPoint);
  return result;
    }
    else
  return NULL;
}

template<class ItemClass>
int MOVectors<ItemClass>::iPoint() const
{
    return _iPoint;
}


template<class ItemClass>
int MOVectors<ItemClass>::size() const
{
    return _items.size();
}


template<class ItemClass>
QDomElement MOVectors<ItemClass>::toXmlData(QDomDocument & doc,QString listTitle)
{
    // Root element
    QDomElement cList = doc.createElement(listTitle);

    QString movectorName = "MOVector";
    for(int i=0;i<_items.size();i++)
    {
  QDomElement cItem = _items.at(i)->toXmlData(doc,movectorName);
  cItem.setAttribute("iPoint",i);
  cList.appendChild(cItem);
    }
    return cList;
}



template<class ItemClass>
int MOVectors<ItemClass>::rowCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);

    if(curVector())
  return curVector()->rowCount(parent);
    else
  return 0;
}

template<class ItemClass>
int MOVectors<ItemClass>::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ItemClass::nbFields;
}

template<class ItemClass>
QVariant MOVectors<ItemClass>::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal)
    {
  if (role == Qt::DisplayRole)
  {
      return ItemClass::sFieldName(section,role);
  }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

template<class ItemClass>
QVariant MOVectors<ItemClass>::data(const QModelIndex &index, int role) const
{
    if(curVector())
  return curVector()->data(index,role);
    else
  return QVariant();

}

template<class ItemClass>
Qt::ItemFlags MOVectors<ItemClass>::flags(const QModelIndex &index) const
{
    if(curVector())
  return curVector()->flags(index);
    else
  return Qt::NoItemFlags;
}

template<class ItemClass>
bool MOVectors<ItemClass>::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(curVector())
  return curVector()->setData(index,value,role);
    else
  return false;
}

template<class ItemClass>
QModelIndex MOVectors<ItemClass>::index(int row, int column, const QModelIndex &parent)const
{
    if(curVector())
  return curVector()->index(row,column,parent);
    else
  return QModelIndex();
}

template<class ItemClass>
QStringList MOVectors<ItemClass>::mimeTypes () const
{
    if(curVector())
  return curVector()->mimeTypes();
    else
  return QStringList();
}

template<class ItemClass>
QMimeData* MOVectors<ItemClass>::mimeData(const QModelIndexList &indexes) const
{
    if(curVector())
  return curVector()->mimeData(indexes);
    else
  return NULL;
}

template<class ItemClass>
Qt::DropActions MOVectors<ItemClass>::supportedDropActions() const{
    if(curVector())
  return curVector()->supportedDropActions();
    else
  return Qt::IgnoreAction;
}

template<class ItemClass>
bool MOVectors<ItemClass>::dropMimeData(const QMimeData *data,
                                  Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if(curVector())
  return curVector()->dropMimeData(data,action,row,column,parent);
    else
  return false;
}




template<class ItemClass>
void MOVectors<ItemClass>::connectVector(int iPoint)
{
    MOVector<ItemClass> *vector = at(iPoint);

    if(vector)
    {
  connect(vector,SIGNAL(rowsAboutToBeInserted ( const QModelIndex & , int , int  )),this,SLOT(rowsAboutToBeInsertedPb( const QModelIndex & , int , int  )));
  connect(vector,SIGNAL(rowsAboutToBeRemoved ( const QModelIndex & , int , int  )),this,SLOT(rowsAboutToBeRemovedPb( const QModelIndex & , int , int  )));
  connect(vector,SIGNAL(rowsInserted ( const QModelIndex & , int , int  )),this,SLOT(rowsInsertedPb( const QModelIndex & , int , int  )));
  connect(vector,SIGNAL(rowsRemoved ( const QModelIndex & , int , int  )),this,SLOT(rowsRemovedPb( const QModelIndex & , int , int  )));
  connect(vector,SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )),this,SLOT(dataChangedPb ( const QModelIndex & , const QModelIndex &  )));
    }
}

template<class ItemClass>
void MOVectors<ItemClass>::disconnectVector(int iPoint)
{
    MOVector<ItemClass> *vector = at(iPoint);

    if(vector)
  disconnect(vector,0,this,0);
}



#endif
