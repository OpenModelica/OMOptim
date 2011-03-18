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

 	@file WidgetProgress.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#ifndef  WIDGETPROGRESS_H
#define WIDGETPROGRESS_H


#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include<QtGui/QFileDialog>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include "WidgetList.h"

#include "MOOptPlot.h"
#include "OptimResult.h"
#include "ui_WidgetMooPlot.h"

namespace Ui {
    class WidgetProgressClass;
}


class QErrorMessage;


class WidgetProgress : public QWidget {
    Q_OBJECT


public:
    explicit WidgetProgress(QWidget *parent = 0);
    virtual ~WidgetProgress();

	void updateCombos();

	public slots:
		void setProgress(float,int curEval,int totalEval);
		void setProgress(float);
		void newTask(QString);
		void endTask();
		void updateTimeLeft();
public:
    Ui::WidgetProgressClass *ui;

	
signals :
	void pushedStop();

private:
	float fraction;
	QString taskName;

	QTimer * timer;

	QList<double> lastSpeeds;
	int timeLeft;
	QTime lastTime;
	int nbSpeeds;
	int period;

};

#endif 
