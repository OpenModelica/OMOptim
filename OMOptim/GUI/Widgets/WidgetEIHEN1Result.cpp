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

 	@file WidgetEIHEN1Result.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#include "WidgetEIHEN1Result.h"

namespace Ui
{
    class WidgetEIHEN1ResultClass;
}

WidgetEIHEN1Result::WidgetEIHEN1Result(EIHEN1Result *result,QWidget *parent)
    : QWidget(parent), _ui(new Ui::WidgetEIHEN1ResultClass)
{
    _ui->setupUi(this);
    _result = result;

    _ui->labelTotalCost->setText(QString::number(result->_totalCost));
    _ui->labelTotalArea->setText(QString::number(result->_totalArea));
    _ui->labelHENumber->setText(QString::number(result->_HENumber));
    _ui->labelCompTime->setText(result->_duration.toString());
    _ui->labelDate->setText(result->_date.toString());

    connect(_ui->pushLog,SIGNAL(clicked()),this,SLOT(onLogAsked()));
    connect(_ui->pushResult,SIGNAL(clicked()),this,SLOT(onResultAsked()));
    connect(_ui->pushSensitivity,SIGNAL(clicked()),this,SLOT(onSensitivityAsked()));
    connect(_ui->pushParameters,SIGNAL(clicked()),this,SLOT(onParametersAsked()));
}

WidgetEIHEN1Result::~WidgetEIHEN1Result()
{
    delete _ui;
}

void WidgetEIHEN1Result::onResultAsked()
{
    QString filePath = _result->saveFolder()+QDir::separator()+_result->_resFileName;
    QUrl fileUrl(QString("file:///").append(filePath));
    bool ok = QDesktopServices::openUrl(fileUrl);
}

void WidgetEIHEN1Result::onLogAsked()
{
    QString filePath = _result->saveFolder()+QDir::separator()+_result->_logFileName;
    QUrl fileUrl(QString("file:///").append(filePath));
    bool ok = QDesktopServices::openUrl(fileUrl);
}

void WidgetEIHEN1Result::onSensitivityAsked()
{
    QString filePath = _result->saveFolder()+QDir::separator()+_result->_sensFileName;
    QUrl fileUrl(QString("file:///").append(filePath));
    bool ok = QDesktopServices::openUrl(fileUrl);
}

void WidgetEIHEN1Result::onParametersAsked()
{
    if(_result->problem())
    {
        MOParametersDlg dlg(_result->problem()->parameters(),false);
        dlg.exec();
    }

}
