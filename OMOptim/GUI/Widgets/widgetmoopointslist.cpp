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

        @file WidgetMooPointsList.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9
*/

#include "widgetmoopointslist.h"
#include "ui_WidgetMooPointsList.h"
#include <QtGui/QErrorMessage>



        WidgetMooPointsList::WidgetMooPointsList(OptimResult* result,QWidget *parent) :
        QWidget(parent),
        _ui(new Ui::WidgetMooPointsListClass)
{
    _ui->setupUi(this);


    _result = result;


    //***********
    //LIST
    //***********
    _listPoints = new WidgetList();
    _ui->layoutWidgetList->addWidget(_listPoints);
    //_ui->layoutWidgetList->setSizeConstraint(QLayout::SetMaximumSize);
    _listPoints->setSizePolicy(QSizePolicy::QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred));

    if(_result->optObjectivesResults()->items.size()>0)
    {
        for (int i=0;i<_result->optObjectivesResults()->nbPoints();i++)
        {
            QListWidgetItem* newItem = new QListWidgetItem(QString::number(i));
            newItem->setData(Qt::UserRole,QVariant(i));
            _listPoints->addItem(newItem);
        }
    }
    _listPoints->setSelectionMode(QAbstractItemView::ExtendedSelection);


    // reemiting signal (other widgets in tab should catch it)
    connect(_listPoints,SIGNAL(selectionChanged(QList<int>&)),
            this,SIGNAL(selectionChanged(QList<int>&)));

    //Shown points
    setOnlyPareto(_ui->pushPareto->isChecked());
    connect(_ui->pushPareto,SIGNAL(toggled(bool)),this,SLOT(setOnlyPareto(bool)));
}

WidgetMooPointsList::~WidgetMooPointsList()
{
    delete _ui;
}

void WidgetMooPointsList::onExtSelectionChanged(QList<int> & list)
{
        _listPoints->setSelectedIndexes(list);
}

void WidgetMooPointsList::setShownPoints(QList<int> list)
{
    _listPoints->clear();

    for (int i=0;i<list.size();i++)
    {
        _listPoints->addItem(QString::number(list.at(i)));
    }

    emit shownPointsChanged(list);
}


void WidgetMooPointsList::showOnlyPoints(QList<int> _list)
{
    // unselect points which are not shown
    QList<int> selected = this->_listPoints->getSelectedIndexes();
    int i=0;
    while(i<selected.size())
    {
        if(!_list.contains(selected.at(i)))
            selected.removeAt(i);
        else
            i++;
    }
    this->_listPoints->setSelectedIndexes(selected);
    this->setShownPoints(_list);
}

void WidgetMooPointsList::showAllPoints()
{
    // set all points
    QList<int> list;
    int nbPoints=0;
    if(_result->optObjectivesResults()->items.size()>0)
        nbPoints = _result->optObjectivesResults()->nbPoints();

    for(int i =0;i<nbPoints;i++)
        list.push_back(i);

    showOnlyPoints(list);
}

void WidgetMooPointsList::showParetoPoints()
{
    // set all points
    QList<int> list = ParetoDominance::getParetoSet(((Optimization*)_result->problem())->objectives(),_result->optObjectivesResults());

    showOnlyPoints(list);
}

void WidgetMooPointsList::setOnlyPareto(bool onlyPareto)
{
    if(onlyPareto)
        showParetoPoints();
    else
        showAllPoints();
}

