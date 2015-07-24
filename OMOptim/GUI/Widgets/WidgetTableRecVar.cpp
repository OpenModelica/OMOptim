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

  @file WidgetTableRecVar.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetTableRecVar.h"
#include "ui_WidgetTableRecVar.h"
#include "Optimization.h"


WidgetTableRecVar::WidgetTableRecVar(OptimResult* result,QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::WidgetTableRecVarClass)
{
    _ui->setupUi(this);

    _result = result;
    Optimization* problem = dynamic_cast<Optimization*>(_result->problem());

    // recomputed table
    _tableVariables = new MOTableView(this);
    _ui->layoutTableVar->addWidget(_tableVariables);
    GuiTools::ModelToViewWithFilter(_result->recomputedVariables(),_tableVariables,_ui->lineVariableFilter);
    GuiTools::minimizeTableSize(_tableVariables);


    //slide
    connect(_ui->spinScan,SIGNAL(valueChanged(int)),_ui->sliderScan,SLOT(setValue(int)));
    connect(_ui->sliderScan,SIGNAL(valueChanged(int)),_ui->spinScan,SLOT(setValue(int)));
    connect(_ui->spinScan,SIGNAL(valueChanged(int)),this,SLOT(onCurScanChanged(int)));

    updateFromNbScan();

    bool show = _result->recomputedVariables()->getUseScan();
    _ui->groupBox->setVisible(show);
}


WidgetTableRecVar::~WidgetTableRecVar()
{
    delete _tableVariables;
    delete _ui;
}



void WidgetTableRecVar::onExtSelectionChanged(QList<int>& list)
{
    if(list.size()==1)
    {
        _result->setCurPoint(list.at(0));
    }
    else
    {
        _result->setCurPoint(-1);
    }

    _tableVariables->viewport()->update();
}

void WidgetTableRecVar::onCurScanChanged(int _curScan )
{
    int newScan = _curScan;
    _result->setCurScan(newScan);

    _tableVariables->viewport()->update();
}

void WidgetTableRecVar::onPointsRecomputed()
{
    _tableVariables->viewport()->update();
    updateFromNbScan();
}

void WidgetTableRecVar::updateFromNbScan()
{
    _ui->sliderScan->setMinimum(0);
    _ui->sliderScan->setMaximum(_result->recomputedVariables()->nbScans()-1);
    _ui->spinScan->setMinimum(0);
    _ui->spinScan->setMaximum(_result->recomputedVariables()->nbScans()-1);
}
