// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

        @file WidgetEIConns.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version
*/

#include "WidgetEIConns.h"
#include "ui_WidgetEIConns.h"



WidgetEIConns::WidgetEIConns(EIConns *eiConns,QWidget *parent)
:QWidget(parent),
    _ui(new Ui::WidgetEIConnsClass)
{
    _ui->setupUi(this);

    _eiConns = eiConns;

        //table model
        _tableView=new MOTableView(this);
       _tableView->setModel(_eiConns);
        _ui->layoutTable->addWidget(_tableView);
        _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //        //*****************
    //        //UNITS COMBOS
    //        //*****************
    //        _ui->comboQUnit->addItem("W",MEQflow::W);
    //        _ui->comboQUnit->addItem("kW",MEQflow::KW);
    //        _ui->comboQUnit->addItem("MW",MEQflow::MW);
    //        _ui->comboTUnit->addItem("C",METemperature::C);
    //        _ui->comboTUnit->addItem("K",METemperature::K);

    //        //Connect actions
    //        connect(_ui->comboTUnit,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));
    //        connect(_ui->comboQUnit,SIGNAL(currentIndexChanged(int)),this,SLOT(unitChanged()));


        // resize columns
        GuiTools::resizeTableViewColumns(_tableView);
}

WidgetEIConns::~WidgetEIConns()
{
    delete _ui;
}

void WidgetEIConns::unitChanged()
{

}

