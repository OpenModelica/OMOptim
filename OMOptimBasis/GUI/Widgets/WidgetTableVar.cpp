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

  @file WidgetTableVar.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetTableVar.h"
#include "MOOptVector.h"


WidgetTableVar::WidgetTableVar(MOVector<Variable> *variables,QWidget *parent,bool addFilter)
    :QWidget(parent)
{

    _variables = variables;

    this->setLocale(QLocale::C);
    // Layouts
    _allLayout = new QGridLayout();
    this->setLayout(_allLayout);

    // variables table
    _tableVariables = new MOTableView(this);

    _allLayout->addWidget(_tableVariables,1,0);
    if(addFilter)
    {
        QLineEdit *newLine = new QLineEdit(this);
        _allLayout->addWidget(newLine,0,0);
        GuiTools::ModelToViewWithFilter(_variables,_tableVariables,newLine);
        GuiTools::minimizeTableSize(_tableVariables);
    }
}

void WidgetTableVar::refresh()
{
    _tableVariables->setModel(NULL);
    _tableVariables->setModel(_variables);
}


WidgetTableOptVar::WidgetTableOptVar(MOOptVector *variables,QWidget *parent, bool addFilter)
    :QWidget(parent)
{
    _variables = variables;
    this->setLocale(QLocale::C);

    // Layouts
    _allLayout = new QGridLayout();
    this->setLayout(_allLayout);

    // variables table
    _tableVariables = new MOTableView(this);


    if(addFilter)
    {
        QLineEdit *newLine = new QLineEdit(this);
        _allLayout->addWidget(newLine,0,0);
        GuiTools::ModelToViewWithFilter(_variables,_tableVariables,newLine);
        GuiTools::minimizeTableSize(_tableVariables);
    }

    QPushButton* pushExport = new QPushButton("Export...",this);
    connect(pushExport,SIGNAL(clicked()),this,SLOT(exportCSV()));
    _allLayout->addWidget(pushExport,0,1);

    _allLayout->addWidget(_tableVariables,1,0,1,2);




    connect(_variables,SIGNAL(curScanChanged()),this,SLOT(refresh()));
}


void WidgetTableOptVar::exportCSV()
{
    // get file name
    QString csvPath = QFileDialog::getSaveFileName(
                this,
                "MO - Export variables",
                QString::null,
                "CSV file (*.csv)" );

    if(!csvPath.isNull())
    {
        QString separator = "\t";
        QString csvText = _variables->toCSV(separator,QList<int>() << _variables->curPoint());

        QFile frontFile(csvPath);
        if(frontFile.exists())
            frontFile.remove();

        frontFile.open(QIODevice::WriteOnly);
        QTextStream tsfront( &frontFile );
        tsfront << csvText;
        frontFile.close();
    }
}

void WidgetTableOptVar::refresh()
{
//    _tableVariables->setModel(NULL);
//    _tableVariables->setModel(_variables);
}

