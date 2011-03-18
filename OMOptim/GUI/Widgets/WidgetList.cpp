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

 	@file WidgetList.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetList.h"

WidgetList::WidgetList(QWidget *_parent):QListWidget(_parent)
{
	ignoreChange = false;
	connect(this,SIGNAL(itemSelectionChanged()),this,
		SLOT(onIntSelectionChanged()));
}


bool WidgetList::setSelectedIndexes(QList<int> _newIndexes)
{
	if(!ignoreChange)
	{
			QModelIndex curIndex;
			QListWidgetItem tmpItem;
			QList<QListWidgetItem*> tmpItems;
			QItemSelectionModel * model = selectionModel();


			// clear the selection
			ignoreChange = true;
			model->clearSelection();    // or QItemSelectionModel::clear()
		
			for(int i=0;i<_newIndexes.size();i++)
			{
				tmpItems = findItems(QString::number(_newIndexes.at(i)),Qt::MatchExactly);
				if(tmpItems.size()>0)
				{
					curIndex = indexFromItem(tmpItems.at(0));
					model->select(curIndex, QItemSelectionModel::SelectCurrent);	
				}
				else
				{
					//#TODO : send error msg in debug
				}
			}
			ignoreChange=false;

			return true;
	}
	else
		return false;
}


QList<int> WidgetList::getSelectedIndexes()
{
	QList<QListWidgetItem*> itemList = selectedItems();
	QList<int> indexes;
	for(int i=0;i<itemList.size();i++)
		indexes.push_back(itemList.at(i)->text().toInt());

	return indexes;
}


void WidgetList::onExtSelectionChanged(QList<int> & _extIndexes)
{
	setSelectedIndexes(_extIndexes);
	emit selectionChanged(_extIndexes);
}

void WidgetList::onIntSelectionChanged()
{
//	if(!ignoreChange)
//		setSelectedIndexes(getSelectedIndexes());

	if(!ignoreChange) {
    QList<int> indexes = getSelectedIndexes(); // You may not use this directly as temporaries cannot be used as reference...
		emit selectionChanged(indexes);
  }
}


