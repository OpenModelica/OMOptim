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

 	@file WidgetTableStreams.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#ifndef WidgetTableStreams_H
#define WidgetTableStreams_H

#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include<QtGui/QFileDialog>

#include "MyDelegates.h"
#include "TableEIItems.h"
#include "MOTableView.h"
#include "GuiTools.h"

namespace Ui {
    class WidgetTableStreamsClass;
}


class QErrorMessage;


class WidgetTableStreams : public QWidget {
    Q_OBJECT


public:
        explicit WidgetTableStreams(EIItem* _rootEI,bool _editable,QWidget *parent,MOVector<Variable> *_variables=NULL);
    virtual ~WidgetTableStreams();

	public slots:
		void onPushAdded();
		void updateCompleters();
		void removeStreams();
public:
    Ui::WidgetTableStreamsClass *ui;
	TableEIItems* tableEIStreams;
	QSortFilterProxyModel *streamsProxyModel;

	MOTableView* tableView;
	EIItem* rootEI;
	MOVector<Variable> *variables;
	VarCompleterDelegate *compltDlg1;
	VarCompleterDelegate *compltDlg2;
	VarCompleterDelegate *compltDlg3;

	GenericDelegate *tinUDlg;
	GenericDelegate *toutUDlg;

	bool editable;

};

#endif 
