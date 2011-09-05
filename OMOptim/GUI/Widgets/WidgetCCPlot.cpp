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

  @file WidgetCCPlot.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "WidgetCCPlot.h"
#include "ui_WidgetCCPlot.h"
#include <QtGui/QErrorMessage>



WidgetCCPlot::WidgetCCPlot(EIMERResult* _result,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetCCPlotClass)
{
    ui->setupUi(this);

    result = _result;

    //***********
    //PLOT
    //***********
    plot1 = new MOCCPlot(result);
    ui->layoutPlot->addWidget(plot1);
    ui->layoutPlot->setSizeConstraint(QLayout::SetMaximumSize);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plot1->setSizePolicy(sizePolicy);

    //*****************
    //BUTTONS
    //*****************
    connect(ui->radioCC,SIGNAL(toggled(bool)),this,SLOT(setViewCC(bool)));
    connect(ui->radioGCC,SIGNAL(toggled(bool)),this,SLOT(setViewGCC(bool)));
    connect(ui->pushZoom,SIGNAL(clicked(bool)),plot1,SLOT(enableZoom(bool)));
    ui->radioGCC->setChecked(true);
    setViewGCC(true);

    //*****************
    //UNITS COMBOS
    //*****************
    ui->comboUnitMER->addItem("W",MEQflow::W);
    ui->comboUnitMER->addItem("kW",MEQflow::KW);
    ui->comboUnitMER->addItem("MW",MEQflow::MW);
    ui->comboUnitMERCold->addItem("W",MEQflow::W);
    ui->comboUnitMERCold->addItem("kW",MEQflow::KW);
    ui->comboUnitMERCold->addItem("MW",MEQflow::MW);
    ui->comboUnitT->addItem("C",METemperature::C);
    ui->comboUnitT->addItem("K",METemperature::K);

    //Connect actions
    connect(ui->comboUnitMER,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));
    connect(ui->comboUnitMERCold,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));
    connect(ui->comboUnitT,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));
    connect(result,SIGNAL(updated()),this,SLOT(replot()));
    connect(result,SIGNAL(deleted()),this,SLOT(clear()));

    // gui
    actualizeGui();
}

WidgetCCPlot::~WidgetCCPlot()
{
    delete ui;
}


void WidgetCCPlot::actualizeGui()
{
    plot1->replot();
    unitChanged();
}

void WidgetCCPlot::clear()
{
    QString msg = "WidgetCCPlot::clear";
    qDebug(msg.toLatin1().data());
    plot1->clear();
}

void WidgetCCPlot::setViewCC(bool checked)
{
    QString msg = "WidgetCCPlot::setViewCC";
    qDebug(msg.toLatin1().data());
    if(checked)
    {
        plot1->setCCType(MOCCPlot::CC);
        if(!ui->radioCC->isChecked())
            ui->radioCC->setChecked(true);
    }
}

void WidgetCCPlot::setViewGCC(bool checked)
{
    QString msg = "WidgetCCPlot::setViewGCC";
    qDebug(msg.toLatin1().data());

    if(checked)
    {
        plot1->setCCType(MOCCPlot::GCC);
        if(!ui->radioGCC->isChecked())
            ui->radioGCC->setChecked(true);
    }
}

void WidgetCCPlot::setMERResult(EIMERResult* result_)
{
    QString msg = "WidgetCCPlot::setMERResult";
    qDebug(msg.toLatin1().data());

    result = result_;
    plot1->setResult(result);
    //    replot();
    //    setViewCC(true);
    actualizeGui();

}


void WidgetCCPlot::replot()
{
    QString msg = "WidgetCCPlot::replot";
    qDebug(msg.toLatin1().data());

    plot1->replot();
    unitChanged();
}

void WidgetCCPlot::unitChanged()
{
    QString msg = "WidgetCCPlot::unitChanged";
    qDebug(msg.toLatin1().data());


    if(result)
    {

        _TUnit = (METemperature::Units)ui->comboUnitT->itemData(ui->comboUnitT->currentIndex()).toInt();
        _qflowUnit =  (MEQflow::Units)ui->comboUnitMER->itemData(ui->comboUnitMER->currentIndex()).toInt();
        int unitMERCold =  ui->comboUnitMERCold->itemData(ui->comboUnitMERCold->currentIndex()).toInt();

        ui->labelTPinch->setText(QString::number(result->TPinch.value(_TUnit)));
        ui->labelMER->setText(QString::number(result->MER.value(_qflowUnit)));
        ui->labelMERCold->setText(QString::number(result->MERCold.value(unitMERCold)));

        plot1->setTUnit(_TUnit);
        plot1->setQUnit(_qflowUnit);

    }
    else
    {
        ui->labelMER->setText("-");
        ui->labelMERCold->setText("-");
        ui->labelTPinch->setText("-");
    }
}

