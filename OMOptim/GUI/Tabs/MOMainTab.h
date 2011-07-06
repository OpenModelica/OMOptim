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

 	@file MOMainTab.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#ifndef MOMAINTAB_H
#define MOMAINTAB_H

#include <QtGui/QTabWidget>
#include <QContextMenuEvent>
#include "tabOneSim.h"
#include "tabOptimization.h"
#include "tabResOneSim.h"
#include "tabResOptimization.h"
#include "tabResOneSim.h"
#include "MOTab.h"
#include "Project.h"

#ifdef USEEI
#include "TabEITargetResult.h"
#include "tabEIHEN1Result.h"
#include "TabEIProblem.h"
#endif

class MainWindow;

class MOMainTab :public QTabWidget
{
	Q_OBJECT

public:
	MOMainTab(QWidget *_mainWindow,Project* _project);
        virtual ~MOMainTab(void);

	void addProblemTab(Project *project, Problem * problem);
        void addResultTab(Project *project, Result * result);
	void removeTab(MOTabBase::TabType,QString name);
	void removeTab(int);
	

public slots:
	void contextMenuEvent(QContextMenuEvent* pEvent);
	void onProblemRenamed(QString);
        void onTabCloseRequested(int index);

private :
	Project* project;
	QWidget* mainWindow;
};

#endif
