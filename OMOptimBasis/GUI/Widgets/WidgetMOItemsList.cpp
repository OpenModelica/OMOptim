// $Id: WidgetMOItemsList.cpp 11891 2012-05-11 07:16:22Z hubert.thieriot $
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

     @file WidgetMOItemsList.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "Widgets/WidgetMOItemsList.h"
#include "ui_WidgetMOItemsList.h"


WidgetMOItemsList::WidgetMOItemsList(QAbstractItemModel *itemsList, QWidget *parent, bool showDescription,bool editable):
    QWidget(parent),
    _ui(new Ui::WidgetMOItemsListClass)
{
    _ui->setupUi(this);
    _itemsList = itemsList;

    _ui->listItems->setModelColumn(0);
    _ui->listItems->setModel(_itemsList);

    _itemView = new MOItemTreeView(this);
    _ui->layoutItemView->addWidget(_itemView);

    _itemModel = new MOItemModel(showDescription,editable);
    _itemView->setModel(_itemModel);
    _itemView->setProperty("frameShape",QFrame::Box);
    connect(_ui->listItems->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(onIndexChanged()));
}

WidgetMOItemsList::~WidgetMOItemsList()
{
    delete _ui;
    delete _itemModel;
}

void WidgetMOItemsList::onIndexChanged()
{
    QModelIndexList indexes = _ui->listItems->selectionModel()->selectedIndexes();
    QModelIndex index = _ui->listItems->currentIndex();

    if(!index.isValid())
        _itemModel->setMOItem(NULL);


    MOItem* item = static_cast<MOItem*>(index.internalPointer());
    if(item)
        _itemModel->setMOItem(item);
    else
        _itemModel->setMOItem(NULL);

    _itemView->setModel(NULL);
    _itemView->setModel(_itemModel);

}


