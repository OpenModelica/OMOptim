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



WidgetCCPlot::WidgetCCPlot(MERResult* _result,QWidget *parent) :
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
	connect(ui->pushZoom,SIGNAL(clicked()),plot1,SLOT(enableZoom()));
	ui->radioGCC->setChecked(true);

	//*****************
	//UNITS COMBOS
	//*****************
	ui->comboUnitMER->addItem("W",1);
	ui->comboUnitMER->addItem("kW",1E3);
	ui->comboUnitMER->addItem("MW",1E6);
	ui->comboUnitMERCold->addItem("W",1);
	ui->comboUnitMERCold->addItem("kW",1E3);
	ui->comboUnitMERCold->addItem("MW",1E6);
	ui->comboUnitT->addItem("°C","C");
	ui->comboUnitT->addItem("K","K");

	//Refresh
	connect(ui->comboUnitMER,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));
	connect(ui->comboUnitMERCold,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));
	connect(ui->comboUnitT,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));
	connect(result,SIGNAL(updated()),this,SLOT(replot()));
	//connect(dynamic_cast<ProblemTarget*>(result->problem())->_rootEI,SIGNAL(modified()),this,SLOT(relaunch()));

}

WidgetCCPlot::~WidgetCCPlot()
{
    delete ui;
}


void WidgetCCPlot::actualizeGui()
{
	unitChanged();
	plot1->replot();
}

void WidgetCCPlot::setViewCC(bool checked)
{
	if(checked)
		plot1->setCCType(MinCCPlot::CC);
}

void WidgetCCPlot::setViewGCC(bool checked)
{
	if(checked)
		plot1->setCCType(MinCCPlot::GCC);
}


void WidgetCCPlot::replot()
{
	plot1->replot();
	unitChanged();
}

void WidgetCCPlot::unitChanged()
{
	double merRatio = ui->comboUnitMER->itemData(ui->comboUnitMER->currentIndex()).toDouble();
	double merColdRatio = ui->comboUnitMERCold->itemData(ui->comboUnitMERCold->currentIndex()).toDouble();
	QString unitT = ui->comboUnitT->itemData(ui->comboUnitT->currentIndex()).toString();

	ui->labelMER->setText(QString::number(result->MER->finalValue(0,0)/merRatio));
	ui->labelMERCold->setText(QString::number(result->MERCold->finalValue(0,0)/merColdRatio));

	if(unitT=="C")
		ui->labelTPinch->setText(QString::number(result->TPinch->finalValue(0,0)-273.15));
	else
		ui->labelTPinch->setText(QString::number(result->TPinch->finalValue(0,0)));
}

