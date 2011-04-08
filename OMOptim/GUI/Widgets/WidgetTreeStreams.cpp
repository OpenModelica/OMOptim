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



WidgetTreeStreams::WidgetTreeStreams(EIItem* _rootEI,bool _showFields,bool _editable,EIReader* _eiReader,
                                     ModReader* _modReader,ModClass* _rootModClass, MOomc* _moomc,MOOptVector *_variables,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetTreeStreamsClass)
{
    ui->setupUi(this);
	
        modReader = _modReader;
        rootModClass = _rootModClass;
        moomc= _moomc;
	rootEI = _rootEI;
	variables = _variables;
	editable = _editable;
	showFields = _showFields;
	eiReader = _eiReader;

	//tree model
	treeEIStreams = new TreeEIStreams(rootEI,showFields,editable,eiReader);
	/*streamsProxyModel = new QSortFilterProxyModel();
	streamsProxyModel->setSourceModel(treeEIStreams);
	*/
	//buttons
	if(!editable)
	{
		ui->pushAddGroup->hide();
		ui->pushAddStream->hide();
		ui->pushRemove->hide();
	}
	

	treeView=new QTreeView(this);
	//treeView->setModel(streamsProxyModel);
	treeView->setModel(treeEIStreams);
	treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->layoutTreeStreams->addWidget(treeView);

	/*streamsProxyModel->setFilterKeyColumn(0);
	streamsProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	connect(ui->lineStreamsFilter,SIGNAL(textChanged(const QString&)),
	streamsProxyModel,SLOT(setFilterWildcard(const QString&)));
*/
	connect(ui->pushAddStream,SIGNAL(clicked()),this,SLOT(addEmptyStream()));
	connect(ui->pushAddGroup,SIGNAL(clicked()),this,SLOT(addEmptyGroup()));
	connect(ui->pushRemove,SIGNAL(clicked()),this,SLOT(removeItem()));
	connect(ui->pushTarget,SIGNAL(clicked()),this,SIGNAL(targetAsked()));
	connect(ui->pushMER,SIGNAL(clicked()),this,SLOT(onMERAsked()));
        connect(ui->pushLoadModel,SIGNAL(clicked()),this,SIGNAL(EILoadModelAsked()));

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
	widgetEIGroup = new WidgetEIGroup(NULL,true,this);
	widgetEIGroup->setVisible(false);
	ui->layoutWidgetItem->addWidget(widgetEIGroup);
	connect(treeView,SIGNAL(clicked(QModelIndex)),
		this,SLOT(onSelectItemChanged(QModelIndex)));
}

WidgetTreeStreams::~WidgetTreeStreams()
{
    delete ui;
}

EIItem* WidgetTreeStreams::getGroupParent(QModelIndex index)
{
	EIItem* item;
        EIItem* parentItem;
	if(index.isValid())
	{
		item = static_cast<EIItem*>(index.internalPointer());
	}
	else
		return rootEI;

        parentItem = item;
        while(parentItem!=rootEI && parentItem->getEIType()!=EI::GROUP)
                parentItem = parentItem->parent();

        return parentItem;
}

void WidgetTreeStreams::addEmptyStream()
{
	QModelIndex index = treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
        EIItem* parentItem = getGroupParent(index);
        eiReader->addEmptyStream(parentItem);
	//treeView->expand(index);
}



void WidgetTreeStreams::addEmptyGroup()
{
	QModelIndex index = treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
        EIItem* parentItem = getGroupParent(index);
        eiReader->addEmptyGroup(parentItem);
	treeView->expand(index);
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
	EIItem* eiItem,*eiParent;
	if(index.isValid())
	{
		eiItem = static_cast<EIItem*>(index.internalPointer());
                eiParent = eiItem->parent();
		treeEIStreams->publicBeginResetModel();
		if(eiParent)
			eiParent->removeChild(eiItem);
		treeEIStreams->publicEndResetModel();
	}
}

void WidgetTreeStreams::refreshTree()
{
	treeView->setModel(NULL);
	treeView->viewport()->update();
	//treeView->setModel(streamsProxyModel);
	treeView->setModel(treeEIStreams);

	for(int i=0;i<treeEIStreams->columnCount();i++)
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

void WidgetTreeStreams::onMERAsked()
{
	emit MERAsked(ui->checkUtilitiesMER->checkState()==Qt::Checked);
}
