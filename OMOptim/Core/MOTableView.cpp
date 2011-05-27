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

 	@file MOTableView.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "MOTableView.h"


MOTableView::MOTableView(QWidget* parent):QTableView(parent)
{
	
	//_view->installEventFilter(this);
	installEventFilter(this);
	connect(this,SIGNAL(resize()),this,SLOT(adjustViewSize()));

	verticalHeader()->hide();
	
	//edit triggers
	setEditTriggers(QAbstractItemView::AllEditTriggers);
}

MOTableView::~MOTableView()
{
}

void MOTableView::adjustViewSize()
{
	//// Resize columns and rows.
	//resizeRowsToContents();
	//resizeColumnsToContents();
 //
	//// Stretched column index.
	//// This is the index of the column, which will have the dynamic width.
	//int stretchedColumnIndex = 2;
 //
	//// Complete width of view port.
	//int totalWidth = viewport()->width();
	//int availableWidth = totalWidth;

	//// Iterate all columns and give them the required size.
	//// The last column should receive the left size.
	//for( int i = 0; i < horizontalHeader()->count(); i++ )
	//{
	//		int reqColumnSize = columnWidth(i);
	//		availableWidth -= reqColumnSize;
	//}

	//if( availableWidth > 0 && availableWidth<totalWidth )
	//{
	//	float ratio = (float)totalWidth/(float)(totalWidth-availableWidth);
	//
	//
	//	for( int i = 0; i < horizontalHeader()->count(); i++ )
	//	{
	//		setColumnWidth(i,(int)(columnWidth(i)*ratio));
	//	}

	//}
		
}

void MOTableView::setModel(QAbstractItemModel *_model)
{
	QTableView::setModel(_model);

	connect(_model, SIGNAL(layoutChanged()), this, SLOT(adjustViewSize()));

	adjustViewSize();
}


bool MOTableView::eventFilter( QObject *obj, QEvent *ev )
{
	if( obj == this )
	{
		if( ev->type() == QEvent::Resize )
		{
			adjustViewSize();
			return false;
		}
	}
	return false;
}
