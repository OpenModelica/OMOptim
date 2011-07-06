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

 	@file WidgetCCPlot.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
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
	plot1 = new MinCCPlot(result);
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

	//*****************
	//UNITS COMBOS
	//*****************
    ui->comboUnitMER->addItem("W",MEQflow::W);
    ui->comboUnitMER->addItem("kW",MEQflow::KW);
    ui->comboUnitMER->addItem("MW",MEQflow::MW);
    ui->comboUnitMERCold->addItem("W",MEQflow::W);
    ui->comboUnitMERCold->addItem("kW",MEQflow::KW);
    ui->comboUnitMERCold->addItem("MW",MEQflow::MW);
    ui->comboUnitT->addItem("°C",METemperature::C);
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
    plot1->clear();
}

void WidgetCCPlot::setViewCC(bool checked)
{
	if(checked)
    {
		plot1->setCCType(MinCCPlot::CC);
        if(!ui->radioCC->isChecked())
            ui->radioCC->setChecked(true);
}
}

void WidgetCCPlot::setViewGCC(bool checked)
{
	if(checked)
    {
		plot1->setCCType(MinCCPlot::GCC);
        if(!ui->radioGCC->isChecked())
            ui->radioGCC->setChecked(true);
}
}

void WidgetCCPlot::setMERResult(EIMERResult* result_)
{
    result = result_;
    connect(result,SIGNAL(updated()),this,SLOT(replot()));
    connect(result,SIGNAL(deleted()),this,SLOT(clear()));
    plot1->setResult(result);
    replot();
    setViewCC(true);
}


void WidgetCCPlot::replot()
{
	plot1->replot();
	unitChanged();
}

void WidgetCCPlot::unitChanged()
{
    if(result)
    {
        int unitT = ui->comboUnitT->itemData(ui->comboUnitT->currentIndex()).toInt();
        int unitMER =  ui->comboUnitMER->itemData(ui->comboUnitMER->currentIndex()).toInt();
        int unitMERCold =  ui->comboUnitMERCold->itemData(ui->comboUnitMERCold->currentIndex()).toInt();

        ui->labelTPinch->setText(QString::number(result->TPinch.value(unitT)));
        ui->labelMER->setText(QString::number(result->MER.value(unitMER)));
        ui->labelMERCold->setText(QString::number(result->MERCold.value(unitMERCold)));

    }
	else
    {
        ui->labelMER->setText("-");
        ui->labelMERCold->setText("-");
        ui->labelTPinch->setText("-");
}
}

