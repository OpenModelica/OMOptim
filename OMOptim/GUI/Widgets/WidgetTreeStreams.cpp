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

 	@file WidgetTreeStreams.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetTreeStreams.h"
#include "ui_WidgetTreeStreams.h"
#include <QtGui/QErrorMessage>



WidgetTreeStreams::WidgetTreeStreams(EITree* _eiTree, bool _showFields,bool _editable,
                                     ModClassTree* _modClassTree, MOomc* _moomc,QWidget *parent,MOOptVector *_variables) :
    QWidget(parent),
    ui(new Ui::WidgetTreeStreamsClass)
{
    ui->setupUi(this);
	
    modClassTree = _modClassTree;
        moomc= _moomc;

	variables = _variables;
	editable = _editable;
	showFields = _showFields;

	//tree model
        //eiTree = new EITree(rootEI,showFields,editable);
        eiTree =_eiTree;
	/*streamsProxyModel = new QSortFilterProxyModel();
	streamsProxyModel->setSourceModel(eiTree);
	*/
	//buttons
	if(!editable)
	{
		ui->pushAddGroup->hide();
		ui->pushAddStream->hide();
		ui->pushRemove->hide();
        ui->pushLoadModel->hide();
	}
	


        treeView=new MyTreeView(this);
        GuiTools::ModelToView(eiTree,treeView);
	treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->layoutTreeStreams->addWidget(treeView);
        treeView->expandAll();

	connect(ui->pushAddStream,SIGNAL(clicked()),this,SLOT(addEmptyStream()));
	connect(ui->pushAddGroup,SIGNAL(clicked()),this,SLOT(addEmptyGroup()));
	connect(ui->pushRemove,SIGNAL(clicked()),this,SLOT(removeItem()));
        connect(ui->pushLoadModel,SIGNAL(clicked()),this,SIGNAL(EILoadModelAsked()));
        connect(ui->pushResize,SIGNAL(clicked()),this,SLOT(resizeColumns()));
	updateCompleters();


	// hide columns
	QList<int> varsColsToHide;
	varsColsToHide << EIStream::CHECKED;
	for(int i=0;i<varsColsToHide.size();i++)
		treeView->setColumnHidden(varsColsToHide.at(i),true);
	
	// unit delegates
	METemperature _temp;
	QStringList Tunits= _temp.units();
	QList<int> Tindexes;
	for(int i=0;i<Tunits.size();i++)
		Tindexes.push_back(i);

	MEQflow _qflow;
	QStringList Qunits= _qflow.units();
	QList<int> Qindexes;
	for(int i=0;i<Qunits.size();i++)
		Qindexes.push_back(i);

	tinUDlg = new GenericDelegate(Tindexes,Tunits,this);
	toutUDlg = new GenericDelegate(Tindexes,Tunits,this);
	qUDlg = new GenericDelegate(Qindexes,Qunits,this);
	treeView->setItemDelegateForColumn(EIStream::TIN_U,tinUDlg);
	treeView->setItemDelegateForColumn(EIStream::TOUT_U,toutUDlg);
	treeView->setItemDelegateForColumn(EIStream::QFLOW_U,qUDlg);

	//WidgetEIGroup
    widgetEIGroup = new WidgetEIGroup(NULL,this,true);
	widgetEIGroup->setVisible(false);
	ui->layoutWidgetItem->addWidget(widgetEIGroup);
	connect(treeView,SIGNAL(clicked(QModelIndex)),
		this,SLOT(onSelectItemChanged(QModelIndex)));

        //resize columns
        GuiTools::resizeTreeViewColumns(treeView);
}

WidgetTreeStreams::~WidgetTreeStreams()
{
    delete ui;
}

EIItem* WidgetTreeStreams::getContainer(QModelIndex index)
{
	EIItem* item;
        EIItem* parentItem;
	if(index.isValid())
	{
		item = static_cast<EIItem*>(index.internalPointer());
	}
	else
                return eiTree->rootElement();

        parentItem = item;
        while(parentItem!=eiTree->rootElement() && parentItem->getEIType()!=EI::GROUP && parentItem->getEIType()!=EI::GENERIC)
                parentItem = parentItem->parent();

        return parentItem;
}

void WidgetTreeStreams::addEmptyStream()
{
	QModelIndex index = treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
        EIItem* parentItem = getContainer(index);
         eiTree->addEmptyStream(parentItem);

        treeView->expand(eiTree->indexOf(parentItem));
        treeView->resizeColumnToContents(0);
}



void WidgetTreeStreams::addEmptyGroup()
{
	QModelIndex index = treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
        EIItem* parentItem = getContainer(index);
        eiTree->addEmptyGroup(parentItem);

	treeView->expand(index);
        treeView->resizeColumnToContents(0);
}

void WidgetTreeStreams::updateCompleters()
{
	if(variables)
	{
		QStringList varNames = variables->getItemNames();
		compltDlg1 = new VarCompleterDelegate(varNames,this);
		treeView->setItemDelegateForColumn(EIStream::TIN_V,compltDlg1);
		compltDlg2 = new VarCompleterDelegate(varNames,this);
		treeView->setItemDelegateForColumn(EIStream::TOUT_V,compltDlg2);
		compltDlg3 = new VarCompleterDelegate(varNames,this);
		treeView->setItemDelegateForColumn(EIStream::QFLOW_V,compltDlg3);
	}
}

void WidgetTreeStreams::removeItem()
{
	QModelIndex index = treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
        eiTree->removeItem(index);

	}

void WidgetTreeStreams::refreshTree()
{
	treeView->setModel(NULL);
	treeView->viewport()->update();
	//treeView->setModel(streamsProxyModel);
	treeView->setModel(eiTree);

	for(int i=0;i<eiTree->columnCount();i++)
	{
		treeView->resizeColumnToContents(i);
	}
}

void WidgetTreeStreams::setInputVars(MOOptVector* _variables)
{
	variables = _variables; //should change nothing, adress should stay similar

	if(compltDlg1)
	{
		delete compltDlg1;
		delete compltDlg2;
		delete compltDlg3;
	}

	updateCompleters();
}

void WidgetTreeStreams::onSelectItemChanged(QModelIndex _index)
{
	EIGroup* group = NULL;

	int oldC=_index.column();
	int oldR=_index.row();

	if(!_index.isValid())
		return;

	//_index = streamsProxyModel->mapToSource(_index);
	
	if(_index.isValid())
	{
		EIItem *item = static_cast<EIItem*>(_index.internalPointer());
		group = dynamic_cast<EIGroup*>(item);
	}
	
	if(group)
		onSelectGroupChanged(group);
	else
		onSelectGroupChanged(NULL);
}

void WidgetTreeStreams::onSelectGroupChanged(EIGroup* group)
{
	widgetEIGroup->setItem(group);

	if(!group)
		widgetEIGroup->hide();
	else
		widgetEIGroup->show();
}



void WidgetTreeStreams::resizeColumns()
{
    GuiTools::resizeTreeViewColumns(treeView);
}
