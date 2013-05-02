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

     @file WidgetLaunchOptim.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 
*/

#ifndef WidgetOptimActions_H
#define WidgetOptimActions_H

#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include <QDesktopServices>

#include "Optimization.h"
#include "ui_WidgetOptimActions.h"
#include "Project.h"



namespace Ui {
    class WidgetOptimActionsClass;
}


class WidgetOptimActions : public QWidget
{

    Q_OBJECT
public:
    WidgetOptimActions(ProjectBase* project,Optimization* problem,bool isResult,Result* result,QWidget *parent);
    virtual ~WidgetOptimActions();


    Ui::WidgetOptimActionsClass *_ui;
    Optimization* _problem;
    Result* _result;
    ProjectBase* _project;
    bool _isResult;

    void actualizeGui();

    QPushButton * addButton(QString text, bool before);

public slots :
  void launch();
  void restoreProblem();
  void onShowModelAsked();

signals:
  void showModclass(ModItem*);


};

#endif 
