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

 	@file WidgetTableStreams.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetTableStreams.h"
#include "ui_WidgetTableStreams.h"
#include <QtGui/QErrorMessage>



WidgetTableStreams::WidgetTableStreams(EIItem* _rootEI,bool _editable,MOVector<Variable> *_variables,QWidget *parent) :

    QWidget(parent),
    ui(new Ui::WidgetTableStreamsClass)
{
    ui->setupUi(this);
	
	rootEI = _rootEI;
	variables = _variables;
	editable = _editable;


	//table model
        tableEIStreams = new TableEIItems(rootEI,editable,EI::STREAM,true);
	tableView=new MOTableView(this);
	streamsProxyModel = GuiTools::ModelToViewWithFilter(tableEIStreams,tableView,ui->lineStreamsFilter);
	tableView->setModel(streamsProxyModel);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->layoutTableStreams->addWidget(tableView);
	//GuiTools::minimizeTableSize(tableView);

	connect(ui->pushAdd,SIGNAL(clicked()),this,SLOT(onPushAdded()));
	connect(ui->pushRemove,SIGNAL(clicked()),this,SLOT(removeStreams()));

	// completers delegates
	updateCompleters();

	// unit delegates
	METemperature _temp;
	QStringList units= _temp.units();
	QList<int> indexes;
	for(int i=0;i<units.size();i++)
		indexes.push_back(i);

	tinUDlg = new GenericDelegate(indexes,units,this);
	toutUDlg = new GenericDelegate(indexes,units,this);
	tableView->setItemDelegateForColumn(EIStream::TIN_U,tinUDlg);
	tableView->setItemDelegateForColumn(EIStream::TOUT_U,toutUDlg);

}

WidgetTableStreams::~WidgetTableStreams()
{
    delete ui;
}



void WidgetTableStreams::onPushAdded()
{
	//eiStreams->addItem(new EIStream());
}

void WidgetTableStreams::updateCompleters()
{
	if(variables)
	{
		QStringList varNames = variables->getItemNames();
		compltDlg1 = new VarCompleterDelegate(varNames,this);
		tableView->setItemDelegateForColumn(EIStream::TIN_V,compltDlg1);
		compltDlg2 = new VarCompleterDelegate(varNames,this);
		tableView->setItemDelegateForColumn(EIStream::TOUT_V,compltDlg2);
		compltDlg3 = new VarCompleterDelegate(varNames,this);
		tableView->setItemDelegateForColumn(EIStream::QFLOW_V,compltDlg3);
	}
}

void WidgetTableStreams::removeStreams()
{
	/*QModelIndexList indexList = tableView->selectionModel()->selectedRows();
	QModelIndex curSourceIndex;

	QList<int> rows;
	for(int i=0;i<indexList.size();i++)
	{
		curSourceIndex = streamsProxyModel->mapToSource(indexList.at(i));
		rows.push_back(curSourceIndex.row());
	}
	eiStreams->removeRows(rows);*/
}
