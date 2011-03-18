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

 	@file tabresoptimization_Plot.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "tabResOptimization_Plot.h"
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QMenu>
#include "MOOptPlot.h"
#include <qwt_plot_picker.h>
#include <QContextMenuEvent>
#include "WidgetTableRecVar.h"



namespace Ui
{
	class TabResOptimization_Plot_Class;
}

TabResOptimization_Plot::TabResOptimization_Plot(Project *project,OptimResult *result, QWidget *parent, QToolBar* toolBar) :
    QWidget(parent), _ui(new Ui::TabResOptimization_Plot_Class)
{
    _ui->setupUi(this);

	_project = project;
	_result = result;
	_toolBar = toolBar;

	// create widgets
	_widgetMooPlot = new WidgetMooPlot(_result,this);
	_widgetTableRecVar = new WidgetTableRecVar(_result,this);
	_widgetBlocks = new WidgetBlocks(_project,_result,this);
	_widgetOptTable = new WidgetOptTable(_result,this);
	_widgetCalculate = new WidgetCalculateMooPoints(_result,_widgetMooPlot,this);
	
	_widgetCalculate->setMaximumSize(16777215,_widgetCalculate->sizeHint().height());

	// Splitter
	_splitter = new MOSplitter();
	_splitter->setOrientation(Qt::Vertical);
	_splitter->addWidget(_widgetMooPlot);
	_splitter->addWidget(_widgetOptTable);
	//splitter->addWidget(WidgetTableRecVar);
	_splitter->addWidget(_widgetBlocks);
	_splitter->addWidget(_widgetCalculate);
	_splitter->setChildrenCollapsible(false);
	_ui->layout->addWidget(_splitter);


	// create window for WidgetTableRecVar
	_tableVarsDlg = new QDialog(this);
	_tableVarsDlg->setAttribute(Qt::WA_DeleteOnClose,false);

	// connect
	connect(_widgetMooPlot,SIGNAL(selectionChanged(QList<int> &)),
		_widgetTableRecVar,SLOT(onSelectionChanged(QList<int> &)));
	connect(_result,SIGNAL(curScanChanged(int &)),_widgetTableRecVar,SLOT(onCurScanChanged(int &)));
	connect(_tableVarsDlg,SIGNAL(finished(int)),this,SLOT(onTableVarClosed()));

	
	// actions
	createToolbar();
}

TabResOptimization_Plot::~TabResOptimization_Plot()
{
    delete _ui;
}

void TabResOptimization_Plot::createToolbar()
{
	_toolBar->setStyleSheet("QToolBar {border: 0px;  }");

	_actShowPlot = new QAction("Plot", _toolBar);
	_actShowPlot->setCheckable(true);
	_actShowPlot->setChecked(true);
	showPlot(_actShowPlot->isChecked());
	connect(_actShowPlot,SIGNAL(toggled(bool)),this,SLOT(showPlot(bool)));

	_actShowVar = new QAction("All var.", _toolBar);
	_actShowVar->setCheckable(true);
	_actShowVar->setChecked(true);
	showVar(_actShowVar->isChecked());
	connect(_actShowVar,SIGNAL(toggled(bool)),this,SLOT(showVar(bool)));

	_actShowOpt = new QAction("Optimized var.", _toolBar);
	_actShowOpt->setCheckable(true);
	_actShowOpt->setChecked(false);
	showOpt(_actShowOpt->isChecked());
	connect(_actShowOpt,SIGNAL(toggled(bool)),this,SLOT(showOpt(bool)));

	bool doShowBlock = _result->_subBlocks.size()>1;
	if(doShowBlock)
	{
		_actShowBlock = new QAction("Blocks", _toolBar);
		_actShowBlock->setCheckable(true);
		_actShowBlock->setChecked(true);
		showBlock(_actShowBlock->isChecked());
		connect(_actShowBlock,SIGNAL(toggled(bool)),this,SLOT(showBlock(bool)));
	}
	else
	{
		showBlock(false);
	}

	_actParetoSet = new QAction("Pareto only.",_toolBar);
	_actParetoSet->setCheckable(true);
	_actParetoSet->setChecked(false);
	setOnlyPareto(_actParetoSet->isChecked());
	connect(_actParetoSet,SIGNAL(toggled(bool)),this,SLOT(setOnlyPareto(bool)));


	/*actShowCC = new QAction("Composite curves", this);
	actShowCC->setCheckable(true);
	actShowCC->setChecked(false);
	connect(actShowCC,SIGNAL(toggled(bool)),this,SLOT(showCC(bool)));*/



	_toolBar->addAction(_actShowPlot);
	_toolBar->addAction(_actShowOpt);
	_toolBar->addAction(_actShowVar);

	if(doShowBlock)
		_toolBar->addAction(_actShowBlock);

	_toolBar->addAction(_actParetoSet);

	

	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	_toolBar->setSizePolicy(sizePolicy);
	
	//_toolBar->addAction(actShowCC);     

	//plot1 = new MOPlot();
	//ui->layoutPlot->addWidget(plot1,5);
	//ui->layoutPlot->setContentsMargins(10,10,10,10);
	//ui->layoutPlot->setMargin(10);


}

void TabResOptimization_Plot::showPlot(bool show)
{
	if(show)
		_widgetMooPlot->show();
	else
		_widgetMooPlot->hide();
}
void TabResOptimization_Plot::onTableVarClosed()
{
	_actShowVar->setChecked(false);
}
void TabResOptimization_Plot::showVar(bool show)
{
	if(show)
	{
		_tableVarsDlg->setLayout(new QGridLayout());
		_tableVarsDlg->layout()->addWidget(_widgetTableRecVar);
		_tableVarsDlg->show();
	}
	else
		_tableVarsDlg->close();
}

void TabResOptimization_Plot::showBlock(bool show)
{
	if(show)
		_widgetBlocks->show();
	else
		_widgetBlocks->hide();
}


void TabResOptimization_Plot::showOpt(bool show)
{
	if(show)
		_widgetOptTable->show();
	else
		_widgetOptTable->hide();
}



void TabResOptimization_Plot::setOnlyPareto(bool onlyPareto)
{
	if(onlyPareto)
		showParetoPoints();
	else
		showAllPoints();
}










void TabResOptimization_Plot::contextMenuEvent(QContextMenuEvent *pEvent)
{

	//QMenu menu(this);

	////Show variables
	//QAction* showOptVariablesAct = ui->dockOptimizedVariables->toggleViewAction();
	//showOptVariablesAct->setText("Show optimized variables");
	//menu.addAction(showOptVariablesAct);
	//menu.exec(this->mapToGlobal(pEvent->pos()));


	////Show variables
	//QAction* showVariablesAct = ui->dockVariables->toggleViewAction();
	//showVariablesAct->setText("Show Variables");
	//menu.addAction(showVariablesAct);
	//menu.exec(this->mapToGlobal(pEvent->pos()));

}

void TabResOptimization_Plot::showAllPoints()
{
	// set all points
	QList<int> _list;
	int nbPoints=0;
	if(_result->optObjectivesResults()->items.size()>0)
		nbPoints = _result->optObjectivesResults()->items.at(0)->nbPoints();

	for(int i =0;i<nbPoints;i++)
		_list.push_back(i);
	
	showOnlyPoints(_list);
}


void TabResOptimization_Plot::showParetoPoints()
{
	// set all points
	QList<int> _list = ParetoDominance::getParetoSet(((Optimization*)_result->problem())->objectives(),_result->optObjectivesResults());
	
	showOnlyPoints(_list);
}

void TabResOptimization_Plot::showOnlyPoints(QList<int> _list)
{
	// unselect points which are not shown
	QList<int> selected = _widgetMooPlot->_listPoints->getSelectedIndexes();
	int i=0;
	while(i<selected.size())
	{
		if(!_list.contains(selected.at(i)))
			selected.removeAt(i);
		else
			i++;
	}
	_widgetMooPlot->_listPoints->setSelectedIndexes(selected);

	_widgetMooPlot->setShownPoints(_list);
	//WidgetTableRecVar->showOnlyPoints(_list);
	//widgetOptTable->showOnlyPoints(_list);
}
