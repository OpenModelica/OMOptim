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

     @file MO2ColTab.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 
*/

#include "Tabs/MO2ColTab.h"

MO2ColTab::MO2ColTab(QString _projectName,MOItem* _mainItem,bool _closable,QWidget *parent)
:MOTabCplx(_projectName,_mainItem,parent)
{

    closable = _closable;

    if(closable)
    {
        dispTB = new QToolBar();
        dispTB->setOrientation(Qt::Vertical);
        addToolBar(Qt::LeftToolBarArea,dispTB);
    }


}

MO2ColTab::~MO2ColTab(void)
{

}

void MO2ColTab::addDockWidget(QString title,QWidget* widget,QWidget *tabifiedOn,Qt::DockWidgetArea dockWidgetArea)
{

    // Widget
    QDockWidget* dockWidget = new QDockWidget(title,this);
    dockWidget->setObjectName(title);
        dockWidget->setSizePolicy(widget->sizePolicy());

    if(closable)
        dockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    else
        dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        dockWidget->setLayout(new QGridLayout(this));
    dockWidget->setWidget(widget);
    dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

    //dockWidget->setStyleSheet("border-width: 1px;\n     border-style: solid;\n     border-color: gray;");
        QMainWindow::addDockWidget(dockWidgetArea,dockWidget);
    

    // Tabify
        if(tabifiedOn && mapDockWidgets.key(tabifiedOn,NULL))
                tabifyDockWidget(mapDockWidgets.key(tabifiedOn),dockWidget);


    dockWidget->show();
    
    // QAction
    if(closable)
    {
        QAction* action = dockWidget->toggleViewAction();
        dispTB->addAction(action);
    }

        mapDockWidgets.insert(dockWidget,widget);
    bool ok = restoreDockWidget(dockWidget);
}
void MO2ColTab::addFixedWidget(QString title,QWidget* widget,Qt::DockWidgetArea dockArea,Qt::Orientation orientation,bool showTitle)
{

    // Widget
    QDockWidget* dockWidget = new QDockWidget(title,this);
    dockWidget->setObjectName(title);
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidget->setLayout(new QGridLayout(this));
    dockWidget->setWidget(widget);
        dockWidget->setSizePolicy(widget->sizePolicy());
    

        if(!showTitle)
        {
            // to hide the title bar completely must replace the default widget with a generic one
            QWidget* titleWidget = new QWidget(dockWidget);
            dockWidget->setTitleBarWidget( titleWidget );
        }

        QMainWindow::addDockWidget(dockArea,dockWidget,orientation);
    widget->show();
    dockWidget->show();
        mapDockWidgets.insert(dockWidget,widget);
    bool ok = restoreDockWidget(dockWidget);
}

void MO2ColTab::setWidgetVisible(QWidget* _widget,bool _visible)
{
        QDockWidget* dock = mapDockWidgets.key(_widget,NULL);
    if(dock)
        dock->setVisible(_visible);
}
