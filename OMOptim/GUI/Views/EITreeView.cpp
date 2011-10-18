// $Id: EITreeView.cpp 9677 2011-08-24 13:09:00Z hubert.thieriot $
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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

  @file EITreeView.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "EITreeView.h"
#include "Project.h"

EITreeView::EITreeView(EITree* eiTree,bool editable,QWidget *parent,MOOptVector* variables) :
    MyTreeView(parent)
{

    _eiTree = eiTree;
    _editable = editable;
    _variables = variables;

    GuiTools::ModelToView(eiTree,this);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->expandAll();

    // hide columns
    QList<int> varsColsToHide;
    varsColsToHide << EIStream::CHECKED;
    for(int i=0;i<varsColsToHide.size();i++)
        this->setColumnHidden(varsColsToHide.at(i),true);

    // unit delegates
    METemperature temp;
    QStringList Tunits= temp.units();
    QList<int> Tindexes;
    for(int i=0;i<Tunits.size();i++)
        Tindexes.push_back(i);

    MEQflow qflow;
    QStringList Qunits= qflow.units();
    QList<int> Qindexes;
    for(int i=0;i<Qunits.size();i++)
        Qindexes.push_back(i);

    MEMassFlow mflow;
    QStringList Munits= mflow.units();
    QList<int> Mindexes;
    for(int i=0;i<Munits.size();i++)
        Mindexes.push_back(i);

    MEHTCoeff htcoeff;
    QStringList HTunits= htcoeff.units();
    QList<int> HTindexes;
    for(int i=0;i<HTunits.size();i++)
        HTindexes.push_back(i);

    // boolean delegate
    QStringList ToutVarLabels;
    ToutVarLabels << "No" << "Yes";
    QList<int> ToutVarIndexes;
    ToutVarIndexes << 0 << 1;


    _tinUDlg = new GenericDelegate(Tindexes,Tunits,this);
    _toutUDlg = new GenericDelegate(Tindexes,Tunits,this);
    _qUDlg = new GenericDelegate(Qindexes,Qunits,this);
    _mUDlg = new GenericDelegate(Mindexes,Munits,this);
    _htUDlg = new GenericDelegate(HTindexes,HTunits,this);


    this->setItemDelegateForColumn(EIStream::TIN_U,_tinUDlg);
    this->setItemDelegateForColumn(EIStream::TOUT_U,_toutUDlg);
    this->setItemDelegateForColumn(EIStream::QFLOW_U,_qUDlg);
    this->setItemDelegateForColumn(EIStream::MASSFLOW_U,_mUDlg);
    this->setItemDelegateForColumn(EIStream::HTCOEFF_U,_htUDlg);

     // variable names completer
     _compltDlg1 = NULL;
     _compltDlg2 = NULL;
     _compltDlg3 = NULL;

    updateCompleters(_variables);

    //resize columns
    GuiTools::resizeTreeViewColumns(this);

    setEditable(_editable);



}

EITreeView::~EITreeView()
{
}

EIItem* EITreeView::getContainer(QModelIndex index)
{
    EIItem* item;
    EIItem* parentItem;
    if(index.isValid())
    {
        item = static_cast<EIItem*>(index.internalPointer());
    }
    else
        return _eiTree->rootElement();

    parentItem = item;
    while(parentItem!=_eiTree->rootElement() && parentItem->getEIType()!=EI::GROUP && parentItem->getEIType()!=EI::GENERIC)
        parentItem = parentItem->parent();

    return parentItem;
}



void EITreeView::updateCompleters(MOOptVector* variables)
{
    if(_compltDlg1)
    {
        delete _compltDlg1;
        delete _compltDlg2;
        delete _compltDlg3;
    }

    if(variables)
    {
        QStringList varNames = variables->getItemNames();
        _compltDlg1 = new VarCompleterDelegate(varNames,this);
        this->setItemDelegateForColumn(EIStream::TIN_V,_compltDlg1);
        _compltDlg2 = new VarCompleterDelegate(varNames,this);
        this->setItemDelegateForColumn(EIStream::TOUT_V,_compltDlg2);
        _compltDlg3 = new VarCompleterDelegate(varNames,this);
        this->setItemDelegateForColumn(EIStream::QFLOW_V,_compltDlg3);
    }
}

void EITreeView::refresh()
{
    this->setModel(NULL);
    this->viewport()->update();
    //treeView->setModel(streamsProxyModel);
    this->setModel(_eiTree);

    for(int i=0;i<_eiTree->columnCount();i++)
    {
        this->resizeColumnToContents(i);
    }
}


void EITreeView::onSelectItemChanged(QModelIndex _index)
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
        emit selectGroupChanged(group);
    else
        emit selectGroupChanged(NULL);
}

void EITreeView::setEditable(bool editable)
{

    // drag and drop
    this->setDragEnabled(editable);
    this->setAcceptDrops(editable);
    this->setDropIndicatorShown(editable);
    this->viewport()->setAcceptDrops(editable);

    if(editable)
    {
        this->setDragDropMode(QAbstractItemView::DragDrop);
        this->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);

    }
    else
    {
        /// \todo find a way to avoid checking/unchecking
        this->setDragDropMode(QAbstractItemView::NoDragDrop);
        this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}


