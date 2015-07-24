// $Id: OMOptimGuiTools.h -1   $
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
  @version
*/

#ifndef OMOPTIMGUITOOLS_H
#define OMOPTIMGUITOOLS_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
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
#endif

#include "Problem.h"
#include "OneSimulation.h"
#include "Optimization.h"
#include "OneSimResult.h"
#include "OptimResult.h"
#include "ModItemsTree.h"
#include "ModItem.h"
#include "MyTreeView.h"
#include "Project.h"



class OMOptimGuiTools: public QObject
{
    Q_OBJECT

public:


    static ModItemsTree* ModItemToTreeView(Project* project,ModLoader*,MOomc*,const ModItem &,QTreeView*,bool showComponent);

    //ModItem popup menus
    static QMenu* newModItemPopupMenu(Project*,const QPoint &,ModItem*);
    static void addModModelActions(QMenu*,Project*,const QPoint &,ModModel*);
    static void addModelicaActions(QMenu*,Project*,const QPoint &,ModItem*);
    static void addCommonActions(QMenu*,Project*,const QPoint &,ModItem*);


    // models path consolidation
    static void consolidateModelsPath(QString projectFile,QWidget* mainWindow);
    static void consolidateModelsPath(QStringList & modelsPath,QDir projectFolder,QWidget* mainWindow);
};



#endif
