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

 	@file GuiTools.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#ifndef GUITOOLS_H
#define GUITOOLS_H

#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTableView>
#include <QtGui/QTableWidget>
#include <QtGui/QListView>
#include <QtGui/QMenu>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QtGui/QHeaderView>
#include <QtGui/QScrollBar>

#include "Problem.h"
#include "OneSimulation.h"
#include "Optimization.h"
#include "OneSimResult.h"
#include "OptimResult.h"
#include "ModClassTree.h"
#include "ModClass.h"
#include "MyTreeView.h"




class GuiTools: public QObject
{
	Q_OBJECT

public:
	GuiTools(void);
	~GuiTools(void);

	

	static QSortFilterProxyModel * ModelToViewWithFilter(QAbstractItemModel *, QAbstractItemView *,QLineEdit*);
	static void ModelToView(QAbstractItemModel *, QAbstractItemView *);
	static void ModClassToTreeView(ModReader*,ModClass*,QTreeView* _treeView,ModClassTree* &_treeModel);

	//ModClass popup menus
	static QMenu* newModClassPopupMenu(Project*,const QPoint &,ModClass*); 
	static void addModModelActions(QMenu*,Project*,const QPoint &,ModModel*); 
	static void addCommonActions(QMenu*,Project*,const QPoint &,ModClass*); 
	
	
	static QMenu* createSolvedProblemPopupMenu(Project*,QWidget*,const QPoint &,Problem*,int numProblem); 
	static QMenu* createProblemPopupMenu(Project*,QWidget*,const QPoint &,Problem*,int numProblem); 
	

	static void minimizeTableSize(QTableView*);
	static void minimizeTableSize(QTableWidget*);

	static void resizeTable(QTableView*);
	
        static void resizeTreeViewColumns(MyTreeView*);
        static void resizeTableViewColumns(QTableView*);
};


#endif
