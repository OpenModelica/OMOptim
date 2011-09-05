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

 	@file TableEIItems.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
        @version

  */
#ifndef _TABLEEISTREAMS_H
#define _TABLEEISTREAMS_H

#include "EIStream.h"
#include "EIReader.h"
#include "MOCCCurve.h"
#include "MOVector.h"
#include "CCTools.h"

using namespace EI;
class TableEIItems : public QAbstractItemModel
{

	Q_OBJECT

public:
        TableEIItems(EI::EIItem *_rootElement,bool _editable,EI::Type _filter,bool _recursive);
	~TableEIItems(void);

private :
        EI::EIItem* rootElement;
	bool editable;
	EI::Type filter;
	bool recursive;
        QList<EI::EIItem*> eiItems;
        bool enabled;


	// abstract model functions
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:
	void hasChanged();

public slots :
        void updateList();

};


#endif
