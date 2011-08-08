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

 	@file TreeEIStreams.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _TreeEIStreams_H
#define _TreeEIStreams_H

#include "TableEIItems.h"
#include "MOCCCurve.h"
#include "MOVector.h"
#include "CCTools.h"
#include "EIReader.h"
#include "EIItem.h"



        /**
        * \brief EITree is a container for eiItem hierarchy.
        *
        * EITree is a container for eiItem hierarchy. It provides managing function
        * and inherits QAbstractItemModel in order to provide easy display and editing.
        */
        class EITree : public QAbstractItemModel
{

    Q_OBJECT

public:

            EITree(bool showFields=true,bool editable=true);

    EITree(const EITree &);
    EITree(QDomElement & domEl);

    ~EITree(void);

    void clear();

    // remove functions
    bool removeItem(EIItem* item);
    bool removeItem(const QModelIndex &index);
    bool removeChild(EIItem* parent, int iChild);
    bool removeChildren(EIItem* parent);
    void removeUnchecked();

    //edit function
    void uncheckUtilities();

    // add functions
    bool addChild(EIItem* parent, EIItem* child);
    void addEmptyGroup(EIItem* parent);
    void addEmptyStream(EIItem* parent);

    // find functions
    EIItem* findItem(QString fullName);
    EIItem* findItem(EI::Type eiType, QVariant itemFieldValue, int iField);

    // abstract model functions
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex indexOf(EIItem*,int column=0) const;

    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool isCheckable(const QModelIndex) const;
    bool removeRows (int row,int count,const QModelIndex & parent);

    bool isValid(QString &msg,MOOptVector* variables=NULL,const QModelIndex = QModelIndex(),bool recursive=true);

    QDomElement toXmlData(QDomDocument & doc);


    EIItem* rootElement();


    private :
        EIItem* _rootElement;
        bool _showFields;
        bool _editable;
        bool _enabled;

        static QColor utilityTextColor(){return QColor(139,69,0);}
        static QColor processTextColor(){return QColor(85,107,47);}




};


#endif
