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

    @file WidgetSelectModModel.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version
*/

#include "Widgets/WidgetSelectModModel.h"
#include "ui_WidgetSelectModModel.h"

WidgetSelectModModel::WidgetSelectModModel(ModItemsTree* modItemsTree,ProblemInterface::ModelNeeds needs,QWidget *parent ):
    QDialog(parent),
    ui(new Ui::WidgetSelectModModelClass)
{
    ui->setupUi(this);
    _modItemsTree = modItemsTree;
    _needs = needs;

    ui->treeView->setModel(modItemsTree);

    switch(_needs)
    {
    case ProblemInterface::ONEMODEL :
        ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
        break;
    case ProblemInterface::SEVERALMODELS :
        ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        break;
    }

    connect(ui->treeView, SIGNAL(clicked(QModelIndex)),this, SLOT(onSelectedModItem(QModelIndex)));
//    connect(ui->treeView2, SIGNAL(clicked()),this, SLOT(onSelectedExeItem()));
    connect(ui->pushValidate,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->pushCancel,SIGNAL(clicked()),this,SLOT(reject()));
    //setSelectedModModel(NULL);
}

void WidgetSelectModModel::onSelectedModItem(QModelIndex index)
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    QList<ModItem*> modClasses;
    for(int i=0;i<indexes.size();i++)
    {
        if(indexes.at(i).isValid())
        {
            ModItem* modClass = static_cast<ModItem*>(indexes.at(i).internalPointer());
            modClasses.push_back(modClass);

        }
    }
    setSelectedModModel(modClasses);

}

WidgetSelectModModel::~WidgetSelectModModel()
{
    delete ui;
}

void WidgetSelectModModel::setSelectedModModel(QList<ModItem*> modClasses)
{

    _selectedModels.clear();
    ui->pushValidate->setEnabled(false);
    for(int i=0;i<modClasses.size();i++)
    {
    if((dynamic_cast<ModModel*>(modClasses.at(i)))|| (dynamic_cast<ExeModel*>(modClasses.at(i))) )

        {
            _selectedModels.push_back(modClasses.at(i));
            ui->pushValidate->setEnabled(true);
            ui->pushValidate->setFocus();
     //   selectedModel = modClass;
        }



//        selectedModel = dynamic_cast<ModModel*>(modClass);

//            if(selectedModel)
//        {
//                ui->pushValidate->setEnabled(true);
//                ui->pushValidate->setFocus();
//        }
//            else
//                ui->pushValidate->setEnabled(false);
    }
}


