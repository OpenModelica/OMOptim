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

  @file MOVector.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#if !defined(_MOVECTOR_H)
#define _MOVECTOR_H


#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>
#include <QtCore/QStringList>
#include <QMimeData>

#include "XMLTools.h"
#include "InfoSender.h"


/** MOVector :Container for MOElements (e.g. Variable, OptObjective...)
It uses template C++ functionality
It is therefore needed to keep in the same file functions declarations and definitions
*/

template<class ItemClass>
class MOVector : public QAbstractTableModel 
{

public:
    QList<ItemClass*> items;


    MOVector(bool owner);
    MOVector(const MOVector<ItemClass> &);
    MOVector<ItemClass>& operator=(const MOVector<ItemClass> &copied);
    MOVector(QString savedData,bool owner);
    MOVector(QDomElement & domList,bool owner);

    virtual ~MOVector();

    QStringList getItemNames();

    // void append(std::vector<ItemClass*>* toAppend);
    void setItems(QDomElement & domList);
    //virtual void append(const MOVector &,bool makeACopy);
    void update(const QDomElement & domList);

    int rowCount(const QModelIndex &parent ) const;
    int columnCount(const QModelIndex &parent ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual void addItem(ItemClass*);
    virtual void insertItem(ItemClass*,int);
    bool removeRow(int index,const QModelIndex &parent = QModelIndex());
    void removeRow(QString);
    bool removeRows(int index, int count,const QModelIndex &parent = QModelIndex());
    bool removeRows(QList<int> indexes);
    virtual int findItem(QString,Qt::CaseSensitivity=Qt::CaseInsensitive) const;
    int findItem(QVariant itemFieldValue,int iField) const;
    bool contains(ItemClass*);
    bool alreadyIn(QString);
    void replaceIn(MOVector<ItemClass> *);
    void addItems(MOVector<ItemClass> *,bool makeACopy);

    void cloneFromOtherVector(const MOVector*);
    MOVector<ItemClass>* clone() const;
    void clear();

    QModelIndex index(int row, int column, const QModelIndex &parent)const;

    // save and load functions
    virtual QString toSavingString();
    virtual QDomElement toXmlData(QDomDocument & doc,QString listTitle);

    // drag and drop functions
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action, int row, int column, const QModelIndex &parent);


    //read and access functions
    int size() const;
    ItemClass* at(int i) const;

protected :
    /**
        * is this vector the owner of the content.
        * If yes, content will be deleted with vector, or when an item replaces another one
        */
    bool _owner;

};


#include "MOVector.cpp"
#endif
