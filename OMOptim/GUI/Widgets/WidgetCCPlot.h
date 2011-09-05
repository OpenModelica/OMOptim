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

 	@file WidgetCCPlot.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#ifndef  WIDGETCCPLOT_H
#define WIDGETCCPLOT_H


#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include<QtGui/QFileDialog>

#include "WidgetList.h"
#include "WidgetCCPlot.h"
#include "MOCCPlot.h"
#include "OptimResult.h"
#include "ui_WidgetCCPlot.h"
#include "WidgetTreeStreams.h"
#include "EITree.h"

namespace Ui {
    class WidgetCCPlotClass;
}


class QErrorMessage;


class WidgetCCPlot : public QWidget {
    Q_OBJECT


public:
        WidgetCCPlot(EIMERResult* _result,QWidget *parent);
    virtual ~WidgetCCPlot();
	 Ui::WidgetCCPlotClass *ui;

private :
	EIMERResult* result;
	WidgetTreeStreams* widgetTreeStreams;
	MOCCPlot *plot1;
        MEQflow::Units _qflowUnit;
        METemperature::Units _TUnit;



public slots:
	void replot();
	void unitChanged();
	void setViewCC(bool checked);
	void setViewGCC(bool checked);
	void actualizeGui();
        void setMERResult(EIMERResult*);
        void clear();

signals:
	void selectionChanged(QList<int>&);

	
};

#endif 
