// $Id: MOItemViews.cpp 13878 2012-11-13 02:32:03Z hubert.thieriot $
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

     @file MOItemViews.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "MOItemViews.h"
#include <QDrag>
#include <QDragEnterEvent>
#include "InfoSender.h"
#include <QDebug>
#include "MOItemModel.h"
#include "MyDelegates.h"
#include "Tools/GuiTools.h"

MOItemTreeView::MOItemTreeView(QWidget* parent):QTreeView(parent)
{
    //editable ?
    setEditable(true);
    setAlternatingRowColors(true);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setSortingEnabled(true);

    //initDelegates();


    // frame

    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

}

MOItemTreeView::~MOItemTreeView()
{
}

void MOItemTreeView::setEditable(bool editable)
{
    if(editable)
    {
        this->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);
    }
    else
    {
        this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void MOItemTreeView::setModel ( QAbstractItemModel * model )
{
    QTreeView::reset();
    QTreeView::setModel(NULL);
    QTreeView::setModel(model);
    if(!model)
        QTreeView::reset();
    if(model)
    {
        initDelegates();
        //connect(model,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(onDataChanged(const QModelIndex&,const QModelIndex&)));
    }
}

void MOItemTreeView::clearDelegates()
{
    for(int i=0;i<_delegates.size();i++)
        delete _delegates.at(i);

    _delegates.clear();
}



void MOItemTreeView::initDelegates()
{
    clearDelegates();


    MOItemModel* itemModel = dynamic_cast<MOItemModel*>(model());
    DoubleSpinBoxDelegate* dblDelegate;
    if(itemModel)
    {
        MOItem* item = itemModel->item();
        if(item)
        {
            for(int i=0;i<item->getNbFields();i++)
            {
                if(item->isListField(i))
                {
                    QMap<int,QString> list = item->fieldList(i);
                    GenericDelegate* listDelegate = new GenericDelegate(list.keys(),list.values(),this);
                    this->setItemDelegateForRow(itemModel->iRow(i),listDelegate);
                    _delegates.push_back(listDelegate);
                }
                else
                {
                    switch(item->getFieldType(i))
                    {
                    case MOItem::BOOL :
                        break;
                    case MOItem::DOUBLE :
                        dblDelegate = new DoubleSpinBoxDelegate(this,10);
                        this->setItemDelegateForRow(itemModel->iRow(i),dblDelegate);
                        _delegates.push_back(dblDelegate);
                        break;
                    default:
                        QStyledItemDelegate* stdDelegate = new QStyledItemDelegate(this);
                        this->setItemDelegateForRow(itemModel->iRow(i),stdDelegate);
                        _delegates.push_back(stdDelegate);
                        break;
                    }
                }
            }
        }
    }
}






