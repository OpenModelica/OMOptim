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


// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#include <QAbstractTableModel>
#include <QTextStream>
#define toAscii toLatin1
#else
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>
#include <QtCore/QStringList>
#include <QMimeData>
#endif

#include "XMLTools.h"
#include "InfoSender.h"


/** MOAVector :Container for MOElements (e.g. Variable, OptObjective...)
It uses template C++ functionality
It is therefore needed to keep in the same file functions declarations and definitions
MOAVector : can be used with abstract classes
MOVector : cannot but offer more functions
*/

template<class ItemClass>
class MOAVector : public QAbstractTableModel
{

public:



    MOAVector(bool owner);
    MOAVector(const QList<ItemClass*> ,bool owner,bool makeACopy);
    MOAVector(const MOAVector<ItemClass> &);
    MOAVector<ItemClass>& operator=(const MOAVector<ItemClass> &copied);


    virtual ~MOAVector();

    QStringList getItemNames();

    void setEditable(bool);
    void setEditableFields(QList<int> indexes,bool editable=true);
    virtual int rowCount(const QModelIndex &parent ) const;
    virtual int columnCount(const QModelIndex &parent ) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void addItem(ItemClass*);
    virtual bool insertItem(ItemClass*,int);
    virtual bool removeRow(int index,const QModelIndex &parent = QModelIndex());
    virtual void removeRow(QString);
    virtual bool removeRows(int index, int count,const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(QList<int> indexes);
    virtual bool removeItems(QList<ItemClass*> indexes);
    ItemClass* findItem(QString) const;
    ItemClass* findItem(QVariant itemFieldValue,int iField) const;
    int indexOf(QVariant itemFieldValue,int iField) const;
    int indexOf(ItemClass*);
    bool contains(ItemClass*);
    bool alreadyIn(QString);
    void replaceIn(MOAVector<ItemClass> *);
    void replaceAt(int index, ItemClass *newItem);
    void addItems(MOAVector<ItemClass> *,bool makeACopy);
    virtual bool isEmpty() const;

    void cloneFromOtherVector(const MOAVector*);
    MOAVector<ItemClass>* clone() const;
    virtual void clear();



    virtual QModelIndex index(int row, int column, const QModelIndex &parent)const;

    // save and load functions
    virtual QString toSavingString();
    virtual QDomElement toXmlData(QDomDocument & doc,QString listTitle);

    // drag and drop functions
    virtual QStringList mimeTypes() const;
    virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
    virtual Qt::DropActions supportedDropActions() const;



    //read and access functions
    int size() const;
    ItemClass* at(int i) const;

protected :
    /**
        * is this vector the owner of the content.
        * If yes, content will be deleted with vector, or when an item replaces another one
        */
    bool _owner;

    QList<ItemClass*> _items;
    bool _editable; // overal editable propriety

    // items modification direclty
    virtual void addInItems(ItemClass *);
    virtual void removeFromItems(int index);

};

template<class ItemClass>
class MOVector : public MOAVector<ItemClass>
{
public:
    MOVector(bool owner,QObject* parent=NULL); // must have parent=NULL, otherwise MOVector(true) is considered as MOVector(QObject*) (strange behavior though)
    MOVector(const QList<ItemClass*> ,bool owner,bool makeACopy, QObject *parent);
    MOVector(const MOAVector<ItemClass> &);
    MOVector<ItemClass>& operator=(const MOAVector<ItemClass> &copied);

    MOVector(QDomElement & domList,bool owner);

public :
    // void append(std::vector<ItemClass*>* toAppend);
    virtual void setItems(QDomElement & domList);
    //virtual void append(const MOAVector &,bool makeACopy);
    virtual void update(const QDomElement & domList);
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action, int row, int column, const QModelIndex &parent);

    MOVector<ItemClass>* clone() const;

};

#include "MOVector.cpp"
#endif
