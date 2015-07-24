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

     @file WidgetOptTable.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "Widgets/WidgetOptTable.h"
#include "ui_WidgetOptTable.h"
#include "Tools/GuiTools.h"


WidgetOptTable::WidgetOptTable(OptimResult* _result,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WidgetOptTableClass)
{
    ui->setupUi(this);

    result = _result;

    connect(result,SIGNAL(curPointChanged()),this,SLOT(onResultCurPointChanged()));

    fillOptTable();
    GuiTools::minimizeTableSize(ui->tableOpt);
}

WidgetOptTable::~WidgetOptTable()
{
    delete ui;
}



void WidgetOptTable::fillOptTable()
{

    int nbObj = result->optObjectivesResults()->size();
    int nbOpt = result->optVariablesResults()->size();
    int nbPoints = result->optObjectivesResults()->nbPoints();
    // headers
    QStringList colHeaders;
    //colHeaders << "Variable" ;
    for (int i=0;i< nbPoints;i++)
        colHeaders << QString::number(i);



    for(int i = 0; i < nbPoints; i++)
        ui->tableOpt->insertColumn(i);

    ui->tableOpt->setHorizontalHeaderLabels(colHeaders);

    QStringList rowHeaders;
    // fill data
    int curRow=0;
    QStringList objNames;
    for (int i=0;i<nbObj;i++)
    {
        ui->tableOpt->insertRow(curRow);
        rowHeaders << result->optObjectivesResults()->at(i)->name();

        objNames << result->optObjectivesResults()->at(i)->name();
        for(int j=0;j<nbPoints;j++)
        {
            ui->tableOpt->setItem(curRow,j,new QTableWidgetItem(QString::number(result->optObjectivesResults()->at(i)->finalValue(0,j))));
        }
        curRow++;
    }

    for (int i=0;i<nbOpt;i++)
    {
        if (!objNames.contains(result->optVariablesResults()->at(i)->name()))
        {
            ui->tableOpt->insertRow(curRow);
            rowHeaders << result->optVariablesResults()->at(i)->name();

            if(result->optVariablesResults()->at(i)->finalValuesAtScan(0).size()>0)
            {
                for(int j=0;j<nbPoints;j++)
                {
                    QString msg = "Widget Opt Table Row : "+QString::number(i)+", column : "+QString::number(j) + "\n";
                    InfoSender::instance()->debug(msg);
                    ui->tableOpt->setItem(curRow,j,new QTableWidgetItem(QString::number(result->optVariablesResults()->at(i)->finalValue(0,j))));
                }
            }
            curRow++;
        }
    }

ui->tableOpt->setVerticalHeaderLabels(rowHeaders);

}

void WidgetOptTable::onResultCurPointChanged()
{
    int curPoint = result->curPoint();
    ui->tableOpt->setCurrentCell(0,curPoint,QItemSelectionModel::Rows);
}

