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

 	@file WidgetTableConnConstr.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetTableConnConstr.h"
#include "ui_WidgetTableConnConstr.h"
#include <QtGui/QErrorMessage>


WidgetTableConnConstr::WidgetTableConnConstr(EIConnConstrs *_constrs, EITree* _eiTree,bool _editable,QWidget *parent)
:QWidget(parent),
    ui(new Ui::WidgetTableConnConstrClass)
{
    ui->setupUi(this);
	
        eiTree = _eiTree;
	constrs = _constrs;
	editable = _editable;

	//table model
	tableView=new MOTableView(this);
	tableView->setModel(constrs);
	ui->layoutTable->addWidget(tableView);
	//GuiTools::minimizeTableSize(tableView);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(ui->pushAdd,SIGNAL(clicked()),this,SLOT(onPushAdd()));
	connect(ui->pushRemove,SIGNAL(clicked()),this,SLOT(onPushRemove()));

	// name completers delegates
	compltDlgA = NULL;
	compltDlgB = NULL;
	updateCompleters();

	// type delegate
	QStringList typeNames;
	QList<int> indexes;
	indexes.push_back(EIConnConstr::FORBIDDEN);
	typeNames.push_back(EIConnConstr::strType(EIConnConstr::FORBIDDEN));
	typeDlg = new GenericDelegate(indexes,typeNames,this);
	tableView->setItemDelegateForColumn(EIConnConstr::TYPE,typeDlg);

        // resize columns
        GuiTools::resizeTableViewColumns(tableView);
}

WidgetTableConnConstr::~WidgetTableConnConstr()
{
    delete ui;
}



void WidgetTableConnConstr::onPushAdd()
{
        constrs->addItem(new EIConnConstr(eiTree));
}

void WidgetTableConnConstr::updateCompleters()
{
	QStringList itemNames;
        if(eiTree)
	{
                itemNames = EIReader::getAllItemNames(eiTree->rootElement());
	}
	
	if(compltDlgA)
		delete compltDlgA;
	compltDlgA = new VarCompleterDelegate(itemNames,this);
	if(compltDlgB)
		delete compltDlgB;
	compltDlgB = new VarCompleterDelegate(itemNames,this);

	tableView->setItemDelegateForColumn(EIConnConstr::ITEMA,compltDlgA);
	tableView->setItemDelegateForColumn(EIConnConstr::ITEMB,compltDlgB);
}

void WidgetTableConnConstr::onPushRemove()
{
	QModelIndexList indexList = tableView->selectionModel()->selectedIndexes();
	QModelIndex curSourceIndex;

	QList<int> rows;
	for(int i=0;i<indexList.size();i++)
	{
		curSourceIndex = indexList.at(i);
		rows.push_back(curSourceIndex.row());
	}
	constrs->removeRows(rows);
}
