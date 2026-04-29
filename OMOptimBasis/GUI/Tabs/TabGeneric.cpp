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
 * @file TabGeneric.cpp
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$


#include "TabGeneric.h"


TabGeneric::TabGeneric(QString projectName,MOItem* mainItem):
MOTabSimple(projectName,mainItem),_ui(new Ui::TabGenericClass)
{
    _ui->setupUi(this);

}
void TabGeneric::setWidgets(QWidgetList widgets,QStringList titles)
{
    _widgets = widgets;
    _widgetsTitles = titles;



    for(int i=0;i<_widgetsTitles.size();i++)
    {
        _ui->mainLayout->addWidget(_widgets.at(i));
        _ui->comboDisp->addItem(_widgetsTitles.at(i),i);
    }

    connect(_ui->comboDisp,SIGNAL(activated(int)),
        this,SLOT(updateView(int)));

}

TabGeneric::~TabGeneric(void)
{
    delete _ui;
}

void TabGeneric::updateView(int view)
{
    int index = _ui->comboDisp->itemData(view).toInt();

    for(int i=0;i<_widgets.size();i++)
    {
        if(i==index)
            _widgets.at(i)->show();
        else
            _widgets.at(i)->hide();
    }
}
