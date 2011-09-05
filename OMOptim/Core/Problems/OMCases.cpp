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
    qDebug("deleting OMCases");
	for(int i=0;i<items.size();i++)
	{
                delete items.at(i);
	}
}

void OMCases::reset()
{
	for(int i=0;i<items.size();i++)
	{
                delete items.at(i);
	}

	items.clear();
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
		 items.erase(items.begin()+row);
		 endRemoveRows();

		 return true;
	 }
	 else
	 {
		 return false;
	 }
 }


int OMCases::findItem(QString itemName)
{
	bool found = false;
	int i=0;
	int nbItems=items.size();
	QString itemName2;

	while((!found)&&(i<nbItems))
	{
		itemName2=items.at(i)->name();
		found=(itemName==itemName2);
		i++;
	}
	if(!found)
	{
		return -1;
	}
	else
	{
		return i-1;
	}
}
