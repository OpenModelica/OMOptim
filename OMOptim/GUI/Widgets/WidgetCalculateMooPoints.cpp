/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * @file WidgetCalculateMooPoints.cpp
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$


#include "Widgets/WidgetCalculateMooPoints.h"
#include "ui_WidgetCalculateMooPoints.h"

WidgetCalculateMooPoints::WidgetCalculateMooPoints(OptimResult* result,WidgetMooPointsList* widgetMooPointsList, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::WidgetCalculateMooPointsClass)
{
    _ui->setupUi(this);


    _result = result;
    _widgetMooPointsList = widgetMooPointsList;

    connect(_ui->pushCalcSelected,SIGNAL(clicked()),
            this,SLOT(recomputeSelectedPoints()));

    connect(_ui->pushExport,SIGNAL(clicked()),
            this,SLOT(exportSelectedPoints()));
}

WidgetCalculateMooPoints::~WidgetCalculateMooPoints()
{
    delete _ui;
}
void WidgetCalculateMooPoints::recomputeSelectedPoints()
{
    QList<int> pointsList;

    pointsList = _widgetMooPointsList->_listPoints->getSelectedIndexes();

    bool forceRecompute = _ui->checkForceRecompute->isChecked();
    _result->recomputePoints(pointsList,forceRecompute);

}

void WidgetCalculateMooPoints::exportSelectedPoints()
{

    // get file name
    QString csvPath = QFileDialog::getSaveFileName(
                this,
                "MO - Export optimum points",
                _result->saveFolder(),
                "CSV file (*.csv)" );

    if(!csvPath.isNull())
    {
        QList<int> listPoints = _widgetMooPointsList->_listPoints->getSelectedIndexes();
        QString csvText = _result->buildAllVarsFrontCSV(listPoints);

        QFile frontFile(csvPath);
        if(frontFile.exists())
            frontFile.remove();

        frontFile.open(QIODevice::WriteOnly);
        QTextStream tsfront( &frontFile );
        tsfront << csvText;
        frontFile.close();
    }
}

