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

 	@file tabresoptimization_config.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "tabResOptimization_Config.h"
#include <QtGui/QSortFilterProxyModel>
#include "MOOptPlot.h"
#include "OptimAlgoUtils.h"
#include "GuiTools.h"
#include "MOTableView.h"

namespace Ui
{
	class TabResOptimization_Config_Class;
}

TabResOptimization_Config::TabResOptimization_Config(Project *project,OptimResult *result, QWidget *parent) :
    QWidget(parent), _ui(new Ui::TabResOptimization_Config_Class)
{
    _ui->setupUi(this);

	_project = project;
	_result = result;

	// AlgoParameters table
	_tableAlgoParameters = new MOTableView(this);
	_ui->layoutTableEAParams->addWidget(_tableAlgoParameters);

	actualizeGui();
}

TabResOptimization_Config::~TabResOptimization_Config()
{
    delete _ui;
}

void TabResOptimization_Config::actualizeGui()
{

	//_ui->labelEAName->setText(EAUtils::getEAName(result->iEA));
        _ui->labelCompTime->setText(_result->_duration.toString());
        _ui->labelDate->setText(_result->_date.toString());
	}
