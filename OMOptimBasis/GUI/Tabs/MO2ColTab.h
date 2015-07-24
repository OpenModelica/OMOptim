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

     @file MO2ColTab.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#ifndef MO2ColTab_H
#define MO2ColTab_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtCore/QMap>
#include <QtGui/QPushButton>
#include <QtGui/QToolBar>
#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>
#endif

#include "MOTab.h"

class MO2ColTab : public MOTabCplx
{
    Q_OBJECT

public:
    MO2ColTab(QString _projectName,MOItem* _mainItem,bool closable=false,QWidget *parent = 0);
    virtual ~MO2ColTab(void);

    QVBoxLayout *buttonsLayout;
    QGridLayout *widgetsLayout;
    QToolBar *dispTB;

protected :
    void addDockWidget(QString title,QWidget*,QWidget *tabifiedOn=NULL,Qt::DockWidgetArea dockWidgetArea=Qt::TopDockWidgetArea);
    void addFixedWidget(QString title,QWidget*,Qt::DockWidgetArea dockWidgetArea=Qt::TopDockWidgetArea,Qt::Orientation orientation=Qt::Vertical,bool showTitle=true);
    void setWidgetVisible(QWidget*,bool _visible);

protected :
    bool closable;
    QMap<QDockWidget*,QWidget*> mapDockWidgets;

};

#endif
