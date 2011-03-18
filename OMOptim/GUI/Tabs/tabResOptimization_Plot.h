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

 	@file tabResOptimization_Plot.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#ifndef TABRESOPTIMIZATION_PLOT_CLASS_H
#define TABRESOPTIMIZATION_PLOT_CLASS_H

#include <QtGui/QWidget>
#include <QtGui/QToolBar>
#include "OptimResult.h"
#include "ui_tabResOptimization_plot.h"
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QHeaderView>
#include "Project.h"
#include "MOOptPlot.h"
#include "MOPlot.h"
#include "MyDelegates.h"
#include "MOTableView.h"
#include "GuiTools.h"
#include "BlockSubsScene.h"
#include "MOSplitter.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_plot_picker.h>


#include "WidgetBlocks.h"
#include "WidgetMooPlot.h"
#include "WidgetOptTable.h"
#include "WidgetTableRecVar.h"
#include "WidgetToolBar.h"
#include "WidgetCalculateMooPoints.h"
#include "ParetoDominance.h"

namespace Ui {
    class TabResOptimization_Plot_Class;
}

class TabResOptimization_Plot : public QWidget {
    Q_OBJECT

public:
	TabResOptimization_Plot(Project *project, OptimResult *result_, QWidget *parent, QToolBar *_toolBar);
    ~TabResOptimization_Plot();

	

	Project *_project;
	OptimResult *_result;

	//UI
	Ui::TabResOptimization_Plot_Class *_ui;



	// main widgets
	WidgetTableRecVar* _widgetTableRecVar;
	WidgetBlocks* _widgetBlocks;
	WidgetMooPlot* _widgetMooPlot;
	WidgetOptTable* _widgetOptTable;
	WidgetCalculateMooPoints *_widgetCalculate;


	QDialog* _tableVarsDlg;
	
	
	//Splitter
	MOSplitter *_splitter;
	

	void updateCombos();

	void fillOptTable();

	// Actions
	QToolBar *_toolBar;

	QAction *_actShowPlot;
	QAction *_actShowOpt;
	QAction *_actShowVar;
	QAction *_actShowBlock;
	QAction *_actParetoSet;

	

	void showOnlyPoints(QList<int>);
	void showAllPoints();
	void showParetoPoints();
	 

public slots :
	void createToolbar();


	void showPlot(bool);
	void showVar(bool);
	void showOpt(bool);
	void showBlock(bool);
	void setOnlyPareto(bool);
//	void showCC(bool);
	void onTableVarClosed();

	void contextMenuEvent(QContextMenuEvent* pEvent);



  
};




#endif // TABRESOPTIMIZATION_PLOT_CLASS_H
