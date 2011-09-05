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

 	@file WidgetEIInputVars.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#ifndef WidgetEIInputVars_H
#define WidgetEIInputVars_H

#include <QtGui/QWidget>
#include "OneSimulation.h"
#include "ui_WidgetEIInputVars.h"
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QHeaderView>
#include "Project.h"
#include "MOTab.h"
#include "WidgetTableStreams.h"
#include "EIValueFiller.h"

namespace Ui {
    class WidgetEIInputVarsClass;
}

class WidgetEIInputVars : public QWidget {
    Q_OBJECT

public:
    WidgetEIInputVars(Project *project, MOOptVector *, EIItem* _rootEI, QWidget *parent);
    virtual ~WidgetEIInputVars();

	Ui::WidgetEIInputVarsClass *ui;

	Project *project;
	MOOptVector *inputVars;
        EIItem* rootEI;

	QSortFilterProxyModel *loadedVarsProxyModel;

public slots :
	void appendInputVars();
	void clearInputVars();  
	void fillList();

        void dispReferences();
        void dispMissingReferences();

signals :
	void inputVarsModified();
private:
	
	QMap<QListWidgetItem*,MOOptVector*> map;
};

#endif // WidgetEIInputVarsCLASS_H
