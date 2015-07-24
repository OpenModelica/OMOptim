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
        @version
*/

#include "Widgets/WidgetMooPointsList.h"
#include "ui_WidgetMooPointsList.h"
#include "MOParametersDlg.h"
#include "WidgetSelectVars.h"
#include "DlgSelectVars.h"

WidgetMooPointsList::WidgetMooPointsList(OptimResult* result,QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::WidgetMooPointsListClass)
{
    _ui->setupUi(this);


    _result = result;


    //***********
    //LIST
    //***********
    _listPoints = new WidgetList(this);
    _ui->layoutWidgetList->addWidget(_listPoints);
    //_ui->layoutWidgetList->setSizeConstraint(QLayout::SetMaximumSize);
    _listPoints->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred));

    if(_result->optObjectivesResults()->size()>0)
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
            this,SLOT(onIntSelectionChanged(QList<int>&)));

    //Shown points
    setOnlyPareto(_ui->pushPareto->isChecked());
    connect(_ui->pushPareto,SIGNAL(toggled(bool)),this,SLOT(setOnlyPareto(bool)));

    connect(_ui->pushCalcSelected,SIGNAL(clicked()),
            this,SLOT(recomputeSelectedPoints()));

    connect(_ui->pushExport,SIGNAL(clicked()),
            this,SLOT(exportSelectedPoints()));

    _selectedExportVariables = NULL;
}

WidgetMooPointsList::~WidgetMooPointsList()
{
    delete _ui;
}

void WidgetMooPointsList::onExtSelectionChanged(QList<int> & list)
{
    _listPoints->setSelectedIndexes(list);
}

void WidgetMooPointsList::onIntSelectionChanged(QList<int> & list)
{
    if(list.size()!=1)
        _result->setCurPoint(-1);
    else
        _result->setCurPoint(list.at(0));

    emit selectionChanged(list);
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
    if(_result->optObjectivesResults()->size()>0)
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


void WidgetMooPointsList::recomputeSelectedPoints()
{
    QList<int> pointsList;
    pointsList = this->_listPoints->getSelectedIndexes();


    // parameters
    MOParameters parameters;
    int index=0;
    parameters.addItem(new MOParameter("forceRecompute","Should we simulate again already simulated points ?",false,MOParameter::BOOL));

    MOParametersDlg dlg(&parameters);
    if(dlg.exec()==QDialog::Accepted)
    {
        bool forceRecompute = parameters.value("forceRecompute").toBool();
        _result->recomputePoints(pointsList,forceRecompute);
        emit pointsRecomputed();
    }
}

void WidgetMooPointsList::exportSelectedPoints()
{
    QList<int> listPoints = this->_listPoints->getSelectedIndexes();

    if(listPoints.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please select at least one point to export");
        msgBox.exec();
        return;
    }

    // first select points
    MOOptVector* selectedOptVars = DlgSelectVars::getSelectedOptVars(_result->recomputedVariables(),_selectedExportVariables);
    if(_selectedExportVariables)
        delete _selectedExportVariables;
    _selectedExportVariables = selectedOptVars;

    if(selectedOptVars)
    {
        // get file name
        QString csvPath = QFileDialog::getSaveFileName(
                    this,
                    "MO - Export optimum points",
                    QString::null,
                    "CSV file (*.csv)" );

        if(!csvPath.isNull())
        {

            QString csvText = _result->buildVarsFrontCSV(selectedOptVars,listPoints,",");

            QFile frontFile(csvPath);
            if(frontFile.exists())
                frontFile.remove();

            frontFile.open(QIODevice::WriteOnly);
            QTextStream tsfront( &frontFile );
            tsfront << csvText;
            frontFile.close();
        }
    }
}

