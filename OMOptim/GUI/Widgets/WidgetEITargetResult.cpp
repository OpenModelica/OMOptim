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

 	@file WidgetEITargetResult.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetEITargetResult.h"

namespace Ui
{
    class WidgetEITargetResultClass;
}

WidgetEITargetResult::WidgetEITargetResult(EITargetResult *result,QWidget *parent)
    : QWidget(parent), _ui(new Ui::WidgetEITargetResultClass)
{
    _ui->setupUi(this);
    _result = result;

    _ui->labelTotalCost->setText(QString::number(result->totalCost));

    connect(_ui->pushLog,SIGNAL(clicked()),this,SLOT(onLogAsked()));
    connect(_ui->pushResult,SIGNAL(clicked()),this,SLOT(onResultAsked()));
    connect(_ui->pushSensitivity,SIGNAL(clicked()),this,SLOT(onSensitivityAsked()));
}

WidgetEITargetResult::~WidgetEITargetResult()
{
    delete _ui;
}

void WidgetEITargetResult::onResultAsked()
{
    QString filePath = _result->saveFolder().absoluteFilePath(_result->_resFileName);
    QUrl fileUrl(QString("file:///").append(filePath));

    bool ok = QDesktopServices::openUrl(fileUrl);
}

void WidgetEITargetResult::onLogAsked()
{
    QDesktopServices::openUrl(_result->saveFolder().absoluteFilePath(_result->_logFileName));
}

void WidgetEITargetResult::onSensitivityAsked()
{
    QDesktopServices::openUrl(_result->saveFolder().absoluteFilePath(_result->_sensFileName));
}
