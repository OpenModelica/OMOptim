// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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
 	@version 
*/

#include "WidgetTreeStreams.h"
#include "ui_WidgetTreeStreams.h"
#include <QtGui/QErrorMessage>



WidgetTreeStreams::WidgetTreeStreams(EITree* eiTree, bool showFields,bool editable,
                                     ModClassTree* modClassTree, MOomc* moomc,QWidget *parent,MOOptVector *variables) :
    QWidget(parent),
    _ui(new Ui::WidgetTreeStreamsClass)
{
    _ui->setupUi(this);
	
    _modClassTree = modClassTree;
    _moomc= moomc;

    _variables = variables;
    _editable = editable;
    _showFields = showFields;

	//tree model
    _eiTree = eiTree;

	//buttons
    if(!_editable)
	{
        _ui->pushAddGroup->hide();
        _ui->pushAddStream->hide();
        _ui->pushRemove->hide();
        _ui->pushLoadModel->hide();
	}
	



    _treeView=new EITreeView(_eiTree,_editable,this,_variables);
    _ui->layoutTreeStreams->addWidget(_treeView);
    _treeView->expandAll();

    connect(_ui->pushAddStream,SIGNAL(clicked()),this,SLOT(addEmptyStream()));
    connect(_ui->pushAddGroup,SIGNAL(clicked()),this,SLOT(addEmptyGroup()));
    connect(_ui->pushRemove,SIGNAL(clicked()),this,SLOT(removeItem()));
    connect(_ui->pushLoadModel,SIGNAL(clicked()),this,SIGNAL(EILoadModelAsked()));

    connect(_treeView,SIGNAL(selectGroupChanged(EIGroup*)),this,SLOT(onSelectGroupChanged(EIGroup*)));
    connect(_treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(onSelectItemChanged(QModelIndex)));

	// hide columns
	QList<int> varsColsToHide;
	varsColsToHide << EIStream::CHECKED;
	for(int i=0;i<varsColsToHide.size();i++)
        _treeView->setColumnHidden(varsColsToHide.at(i),true);
	
	//WidgetEIGroup
    _widgetEIGroup = new WidgetEIGroup(NULL,this,true);
    _widgetEIGroup->setVisible(false);
    _ui->layoutWidgetItem->addWidget(_widgetEIGroup);

}

WidgetTreeStreams::~WidgetTreeStreams()
{
    delete _ui;
}

void WidgetTreeStreams::addEmptyStream()
{
    QModelIndex index = _treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
    EIItem* parentItem = _treeView->getContainer(index);
    _eiTree->addEmptyStream(parentItem);

    _treeView->expand(_eiTree->indexOf(parentItem));
    _treeView->resizeColumnToContents(0);
}



void WidgetTreeStreams::addEmptyGroup()
{
    QModelIndex index = _treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
    EIItem* parentItem = _treeView->getContainer(index);
    _eiTree->addEmptyGroup(parentItem);

    _treeView->expand(index);
    _treeView->resizeColumnToContents(0);
}


void WidgetTreeStreams::removeItem()
{
    QModelIndex index = _treeView->selectionModel()->currentIndex();
	//index = streamsProxyModel->mapToSource(index);
    _eiTree->removeItem(index);

	}

void WidgetTreeStreams::refreshTree()
{
    _treeView->refresh();
	}

void WidgetTreeStreams::setInputVars(MOOptVector* variables)
{
    _variables = variables; //should change nothing, adress should stay similar

    _treeView->updateCompleters(_variables);
	}

void WidgetTreeStreams::onSelectItemChanged(QModelIndex index)
{
	EIGroup* group = NULL;

    int oldC=index.column();
    int oldR=index.row();

    if(!index.isValid())
		return;

	
    if(index.isValid())
	{
        EIItem *item = static_cast<EIItem*>(index.internalPointer());
		group = dynamic_cast<EIGroup*>(item);
	}
	
	if(group)
		onSelectGroupChanged(group);
	else
		onSelectGroupChanged(NULL);
}

void WidgetTreeStreams::onSelectGroupChanged(EIGroup* group)
{
    _widgetEIGroup->setItem(group);

	if(!group)
        _widgetEIGroup->hide();
	else
        _widgetEIGroup->show();
}



