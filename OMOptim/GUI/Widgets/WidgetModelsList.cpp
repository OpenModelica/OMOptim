// $Id: WidgetModelsList.cpp 11231 2012-02-29 09:29:46Z hubert.thieriot $
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

    @file WidgetModelsList.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version
*/

#include "Widgets/WidgetModelsList.h"
#include "ui_WidgetModelsList.h"

#include "WidgetSelectModModel.h"
#include "Project.h"
#include "Optimization.h"

WidgetModelsList::WidgetModelsList(Project* project,Optimization* problem,QWidget *parent,bool editable):
    QWidget(parent),
    _ui(new Ui::WidgetModelsListClass)
{
    _ui->setupUi(this);
    _project= project;
    _problem = problem;


    refreshList();


    _editable = editable;
    if(!editable)
    {
        _ui->pushAdd->hide();
        _ui->pushRemove->hide();
    }

    connect(_ui->pushAdd,SIGNAL(clicked()),this,SLOT(addModels()));
    connect(_ui->pushRemove,SIGNAL(clicked()),this,SLOT(removeModels()));
}

WidgetModelsList::~WidgetModelsList()
{
    delete _ui;
}



void WidgetModelsList::addModels(QStringList list)
{
    for(int i=0;i<list.size();i++)
    {
        _problem->addModel(list.at(i));
    }
}

void WidgetModelsList::addModels()
{
    WidgetSelectModModel* widget = new WidgetSelectModModel(_project->modItemsTree(),ProblemInterface::SEVERALMODELS,this);
    QStringList modelsList;

    if(widget->exec()==QDialog::Accepted)
    {
        QList<ModItem*> models = widget->_selectedModels;
        for(int i=0;i<models.size();i++)
            modelsList.push_back(models.at(i)->name());

        addModels(modelsList);
    }
}

void WidgetModelsList::setInfos(const QString &infos)
{
    _ui->labelInfos->setText(infos);
    if(infos.isEmpty())
        _ui->labelInfos->hide();
    else
        _ui->labelInfos->show();
}

void WidgetModelsList::refreshList()
{
    _ui->filesList->clear();
    QStringList models = _problem->models();
    for(int i=0;i<models.size();i++)
        _ui->filesList->addItem(models.at(i));
}

void WidgetModelsList::removeModels()
{
    QList<QListWidgetItem*> itemsList = _ui->filesList->selectedItems();
    QStringList modelsToRemove;
    for(int i=0;i<itemsList.size();i++)
    {
        modelsToRemove.push_back(itemsList.at(i)->text());
    }

    for(int i=0;i<modelsToRemove.size();i++)
        _problem->removeModel(modelsToRemove.at(i));
}


