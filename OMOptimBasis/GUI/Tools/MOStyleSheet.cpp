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
 * @file MOStyleSheet.cpp
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$


#include "Tools/MOStyleSheet.h"

MOStyleSheet::MOStyleSheet(void)
{
}

MOStyleSheet::~MOStyleSheet(void)
{
}
void MOStyleSheet::initialize(QApplication * _qApp)
{

    QString toolBoxSS = "QToolBox::tab {\n      background: #B8B8B8;\n\n         border-radius: 3px;\n        color: #212A5E;\n    font: bold;\n}";
    QString mainWindowSS = "QMainWindow::separator { width: 1px; height: 1px; }";
    QString tabWidget = "QTabWidget{ border : 0px;}";

    QString toolBar = "QToolBar{ border-style: none; border : 0px;}";

    QString allSS = toolBoxSS + mainWindowSS + tabWidget + toolBar;

    _qApp->setStyleSheet(allSS);
}

QString MOStyleSheet::launchBarStyleSheet()
{
    return "QFrame{background:qlineargradient(spread:repeat, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(65, 72, 89, 255), stop:0.466019 rgba(94, 105, 129, 255), stop:0.961165 rgba(65, 72, 89, 255));}";
}

QString MOStyleSheet::toolBarStyleSheet()
{
    return "QToolBar{background:qlineargradient(spread:repeat, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(94, 105, 129, 150), stop:0.466019 rgba(94, 105, 129, 255), stop:0.961165 rgba(94, 105, 129, 150));}";
}
